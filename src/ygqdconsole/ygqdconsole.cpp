/*
Copyright (C) 2017  YLoader.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma warning(disable : 4482)

#include "stdafx.h"

using yloader::operator<<;

class ConsoleException : public std::exception {
 public:
  ConsoleException(const std::string& message) : exception(message.c_str()) {}
};

class ConsoleEventSink : public YahooEventSink {
 public:
  void event(const YahooEvent& event) { printLine(event.toString()); }
};

inline unsigned int getConnections(CmdLinePtr cmdLine) {
  return cmdLine->connections(DEFAULT_THREADS);
}

void validateCmdLine(CmdLinePtr cmdLine) {
  unsigned int connections(::getConnections(cmdLine));
  if (connections == 0 || connections > MAX_THREADS) {
    throw ConsoleException("The number of connection must be between 1 and 20");
  }

  COleDateTime startDate(cmdLine->startDateAsOleDateTime());
  COleDateTime endDate(cmdLine->endDateAsOleDateTime());

  if (startDate.GetStatus() == COleDateTime::DateTimeStatus::valid &&
      endDate.GetStatus() == COleDateTime::DateTimeStatus::valid &&
      startDate > endDate) {
    throw ConsoleException("Start date must be before end date");
  }

  // check the data path
  if (((std::wstring)cmdLine->dataPath()).empty()) {
    throw CmdLineException(_T( "No output data path specified" ));
  }

  if (!yloader::DirectoryExists(cmdLine->dataPath())) {
    throw ConsoleException(std::string("Output path \"") +
                           yloader::ws2s(cmdLine->dataPath().getValue()) +
                           "\" could not be found");
  }

  const std::wstring symbolsFileName(cmdLine->symbolsFileName());
  if (symbolsFileName.empty()) {
    throw CmdLineException(_T( "No symbols file specified" ));
  }

  if (!yloader::fileExists(symbolsFileName)) {
    throw ConsoleException(std::string("Symbols file \"") +
                           yloader::ws2s(symbolsFileName) +
                           "\" could not be found");
  }
}

YDataSourcePluginPtr getAndValidateDataSourcePlugin(CmdLinePtr cmdLine) {
  // check the datasource plugin
  if (!cmdLine->dataSourcePlugin().isSet() ||
      ((std::wstring)cmdLine->dataSourcePlugin()).empty()) {
    throw CmdLineException(_T( "No datasource plugin specified" ));
  }

  YPlugins _plugins;
  _plugins.load(yloader::getModulePath());
  YDataSourcePluginVectorPtr dataSources(
      _plugins.getMatchingDataSourcePlugins(cmdLine->dataSourcePlugin()));

  if (dataSources->empty()) {
    throw ConsoleException(
        std::string("Could not find a datasource plugin \"") +
        yloader::ws2s(cmdLine->dataSourcePlugin().getValue()) + "\"");
  }

  if (dataSources->size() > 1) {
    throw ConsoleException(
        std::string("Ambiguous datasource plugin \"") +
        yloader::ws2s(cmdLine->dataSourcePlugin().getValue()) + "\"");
  }

  return (*dataSources)[0];
}

class ConsoleStatistics : public Statistics {
  unsigned __int64 _eventCount;
  unsigned __int64 _progress;

  yloader::Mutex _mx;

 public:
  // progress indicates how often to show progress - 0 for no progress, N for
  // every N lines
  ConsoleStatistics(unsigned int progress)
      : _eventCount(0), _progress(progress) {}

  virtual void event(const YahooEvent& e) {
    yloader::Lock lock(_mx);

    ++_eventCount;

    __super::event(e);

    if (_progress > 0 && _eventCount % _progress == 0) {
      printLine(__super::toGraphicString());
    }
  }
};

class ConsoleUpdaterHandler : public UpdaterHandler {
 private:
  yloader::Condition condition;
  yloader::NonRecursiveMutex mutex;
  Updater m_updater;

 private:
  void wait() {
    NonRecursiveLock lock(mutex);
    condition.wait(lock);
  }

 public:
  ConsoleUpdaterHandler(const StringSettable& alternateHost,
                        const StringSettable& alternateUpdateDirectory)
      : m_updater(*this, _T("en_us"), alternateHost, alternateUpdateDirectory) {
  }

  virtual void checkingForUpdate() {
    printLine(_T("------ Checking for update ------"));
  }

  virtual void updateAvailable() {
    printLine(std::wstring(_T("Your version: ")) + Version::CURRENT.toString() +
              _T(", released on ") +
              VersionInfo().getReleaseDate()->to_simple_string());
    printLine(std::wstring(_T("New version available: ")) +
              m_updater.getVersion().toString() + _T( ", released on " ) +
              m_updater.getUpdateInfo()->getReleaseDate()->to_simple_string());
    printLine(std::wstring(_T("Go to ")) +
              *m_updater.getUpdateInfo()->getDownloadUrl() +
              _T( " to download new version" ));

    condition.notify_one();
  }

  virtual void noUpdateAvailable() {
    printLine(_T("No new version available"));
    condition.notify_one();
  }

  virtual void updateError(const std::wstring& error) {
    printLine(std::wstring(_T("Update error: ")) + error);
    condition.notify_one();
  }

  void start() {
    m_updater.start();

    wait();
  }
};

int _tmain(int argc, _TCHAR* argv[]) {
  int ret = 0;
  // return 0 for no error, 1 for error

  CmdLinePtr cmdLine;
  try {
    printDashedLine(yloader::getCaption(STRPRODUCT_NAME));
    printLine();

    cmdLine = ysession();

    if (cmdLine) {
      if (!cmdLine->doNotCheckForUpdate()) {
        ConsoleUpdaterHandler handler(cmdLine->alternateUpdateHostName(),
                                      cmdLine->alternateUpdateDirectory());

        handler.start();
      }

      validateCmdLine(cmdLine);

      YDataSourcePluginPtr dateSourcePlugin(
          getAndValidateDataSourcePlugin(cmdLine));

      COleDateTime startDate(cmdLine->startDateAsOleDateTime());
      COleDateTime endDate(cmdLine->endDateAsOleDateTime());

      unsigned int connections(::getConnections(cmdLine));

      const std::wstring symbolsFileName(cmdLine->symbolsFileName());

      CharactersMapping cm(cmdLine->mapCharacters().getValue());

      YahooEventDelegator* yed(new YahooEventDelegator());
      YahooEventDelegatorPtr yes(yed);
      ConsoleEventSink ces;
      ConsoleStatistics stats(cmdLine->progress(DEFAULT_PROGRESS));
      ;
      yed->addSink(&ces);
      yed->addSink(&stats);

      StrListPtr symbolsList(yloader::getSymbols(symbolsFileName, true));

      if (!symbolsList) {
        throw ConsoleException("Error generating the symbols list");
      }

      SymbolsListIteratorPtr si(new SymbolsListIterator(symbolsList.get()));

      std::wstring logFile(cmdLine->logFile());

      bool appendToLog(cmdLine->appendToLog(DEFAULT_APPEND_TO_LOG));
      bool logOnlyErrors(cmdLine->logOnlyErrors(DEFAULT_LOG_ONLY_ERRORS));

      DataParams dataParams(
          cmdLine->errorSymbolsListFileName(DEFAULT_ERROR_SYMBOLS_LIST),
          cmdLine->appendToErrorSymbolsList(
              DEFAULT_APPEND_TO_ERROR_SYMBOLS_LIST),
          cmdLine->ignoreErrorSymbolsList(DEFAULT_IGNORE_ERROR_SYMBOLS_LIST),
          cmdLine->ignoreSymbolsList(DEFAULT_IGNORE_SYMBOLS_LIST),
          dateSourcePlugin, cmdLine->dataPath(),
          cmdLine->period(DEFAULT_PERIOD), cmdLine->adjust(DEFAULT_ADJUST),
          cmdLine->allAvailable(DEFAULT_ALL_AVAILABLE),
          cmdLine->update(DEFAULT_UPDATE), startDate, endDate,
          cmdLine->validatePrices(DEFAULT_VALIDATE_PRICES),
          cmdLine->sortBarsAscending(DEFAULT_SORT_BARS_ASCENDING),
          cmdLine->dateFormat(DEFAULT_DATE_FORMAT),
          cmdLine->addSymbol(DEFAULT_ADD_SYMBOL), cmdLine->symbolColumn(),
          cmdLine->prependToDataFileNameAsString(),
          cmdLine->appendToDataFileNameAsString(),
          cmdLine->padDateFields(DEFAULT_PAD_DATE_FIELDS),
          DEFAULT_SYMBOL_TIMEOUT,
          cmdLine->createSubdirs(DEFAULT_CREATE_SUBDIRS),
          cmdLine->handlingInvalidData(DEFAULT_HANDLING_INVALID_DATA),
          cmdLine->handlingVolume0(DEFAULT_VOLUME0_INVALID), connections,
          DEFAULT_AUTO_SELECT_OUTPUT_PATH,  // cmdLine->autoSelectOutputPath(
                                            // DEFAULT_AUTO_SELECT_OUTPUT_PATH ),
          _T( "" ), cm, cmdLine->fieldSeparator(DEFAULT_FIELD_SEPARATOR),
          cmdLine->volumeMultiplier(DEFAULT_VOLUME_MULTIPLIER),
          cmdLine->dataFileHeader(),
          cmdLine->dateSeparator(DEFAULT_DATE_SEPARATOR), cmdLine->outputFile(),
          symbolsFileName, cmdLine->extension(DEFAULT_EXTENSION),
          cmdLine->noReloadOldData(DEFAULT_DONT_RELOAD_DATA_IN_UPDATE_MODE),
          logFile, appendToLog, logOnlyErrors,
          cmdLine->enableRegexFomratting(DEFAULT_ENABLE_REGEX_FORMATTING),
          cmdLine->regexMatch(DEFAULT_MATCH_REGEX),
          cmdLine->regexFormat(DEFAULT_FORMAT_STRING),
          cmdLine->regexFlags(DEFAULT_REGEX_FLAGS));

      // this needs to be done after creating the DataParams, as the log class
      // needs it
      YahooSessionLogPtr logSink;

      if (!logFile.empty()) {
        logSink = YahooSessionLogPtr(new YahooSessionLog(dataParams));

        yed->addSink(logSink.get());
      }

      if (cmdLine->showSettings()) {
        printDashedLine(_T( "Session settings"));
        printLine(dataParams.toString());
      }

      std::wstring errorSymbolsFileName(cmdLine->errorSymbolsListFileName());
      std::wstring ignoreSymbolsList(cmdLine->ignoreSymbolsList());

      WriteFileSymbolsListPtr errorSymbolsList(
          cmdLine->hasErrorSymbolsListFileName()
              ? boost::make_shared<WriteFileSymbolsList>(errorSymbolsFileName)
              : WriteFileSymbolsListPtr());

      if (!cmdLine->dontRun()) {
        yloader::UniqueSymbolsSetPtr ignoreSymbolsSet(
            dataParams.getIgnoreSymbols(yes));

        yloader::ManagedPtr<CDownloading> downloading(
            new CDownloading(dataParams, errorSymbolsList.get(), symbolsList,
                             ignoreSymbolsSet, si, yes));

        stats.start(symbolsList->size());

        printLine();
        printDashedLine(_T( "Downloading..." ));
        printLine(stats.toString());
        downloading->startSync();

        printLine(stats.toGraphicString());
        printLine();
        printDashedLine(_T( "Session statistics"));
        printLine(stats.toString());

        printLine(_T( "Downloading session completed successfully" ));

        if (errorSymbolsList) {
          printDashedLine(_T("Writing error symbols list file" ));
          if (!errorSymbolsList->write()) {
            std::wstring errorMessage =
                _T("Error: Failed to write to the symbols list file \"" );
            errorMessage +=
                (std::wstring)cmdLine->errorSymbolsListFileName() + _T( "\"" );
            printLine(errorMessage);
          }
        }
      } else {
        printLine(_T( "Session halted" ));
      }

      ret = 0;
    } else {
      // this is when the user asked for --help, it's not an error, but don't do
      // anything
      ret = 0;
    }
  } catch (const LogFileException& e) {
    printLine(std::wstring(_T( "Could not open log file \"" )) + e.fileName() +
              _T( "\"" ));
    ret = 1;
  } catch (const CmdLineException& e) {
    // if command line error, ignore cmd line params
    printLine(_T("Command line error"));
    printLine(yloader::s2ws(e.what()));
    ret = 1;
  } catch (const ConsoleException& e) {
    printLine(_T("Command line error"));
    printLine(yloader::s2ws(e.what()));
    ret = 1;
  } catch (const FileSymbolsParserException& e) {
    // problem with the symbols file
    printLine(_T("Symbols file error"));
    printLine(e.message());
    ret = 1;
  } catch (const boost::bad_expression& e) {
    printLine(yloader::s2ws(std::string("Match regex error: ") + e.what()));
    ret = 1;
  } catch (const std::exception& e) {
    printLine(_T("Error"));
    printLine(yloader::s2ws(e.what()));
    ret = 1;
  } catch (...) {
    printLine(_T( "Unknown error" ));
    ret = 1;
  }

  switch (ret) {
    case 0:
      break;
    case 1:
      printLine(_T( "Downloading session failed" ));
      break;
    default:
      printLine(_T( "Downloading session failed" ));
      break;
  }

  return ret;
}

unsigned int _create(unsigned int mode) {
  return 1;  // return create( mode );
}

bool _close() {
  return true;  // return close( downloaderId );
}

bool _isRunning(bool* isRunning) {
  return true;  // return isRunning( downloaderId, isRunning );
}

bool _getLastError(TCHAR* error, unsigned int errorMessageLength) {
  if (error != 0) {
    *error = 0;
  }

  return false;
}

bool _loadSettings(const TCHAR* settingsName) {
  return true;  // return loadSettings( downloaderId, settingsName );
}

void _setSymbolsFileName(const TCHAR* symbolsFileName) {}

void _setAutoStart(bool autoStart) {}

void _setAutoExit(bool autoExit) {}

void _setDataPath(const TCHAR* dataPath) {}

void _setStartDate(const TCHAR* startDate) {}

void _setEndDate(const TCHAR* endDate) {}

void _setDateSeparator(const TCHAR* dateSeparator) {}

void _setAddSymbolToColumn(bool add, unsigned int colum) {}

void _setSortBarsAscending(bool sortBarsAscending) {}

void _setPadDateFields(bool padDateFields) {}

void _setDataFileHeader(const TCHAR* dataFileHeader) {}

void _setInvalidDataHandling(unsigned int invalidDataHandling) {}

void _setVolume0Handling(bool volume0Handling) {}

void _setPrependToDataFileName(const TCHAR* prepend) {}

void _setAppendToDataFileName(const TCHAR* append) {}

void _setExtension(const TCHAR* extension) {}

void _setConnections(unsigned int connections) {}

void _setOutputFile(const TCHAR* outputFile) {}

void _setDateFormat(unsigned int dateFormat) {}

void _setAdjust(bool adjust) {}

void _setValidatePrices(bool validate) {}

void _setPeriod(unsigned int period) {}

void _setAllAvailable(bool allAvailable) {}

void _setUpdate(bool update) {}

void _setDataSourcePlugin(const TCHAR* dataSource) {}

void _setNoReloadOldData(bool noReloadOldData) {}

void _setCreateSubdirs(bool createSubdirs) {}

void _setAppendToLog(bool appendToLog) {}

void _setLogOnlyErrors(bool logOnlyErrors) {}

void _setFieldSeparator(const TCHAR* fieldSeparator) {}

void _setVolumeMultiplier(double volumeMultiplier) {}

void _setLogFile(const TCHAR* logFile) {}

void _setProgress(unsigned int progress) {}

void _setRegexMatch(const TCHAR* regexMatch) {}

void _setRegexFormat(const TCHAR* regexFormat) {}

void _setRegexFlags(unsigned long flags) {}

void _setEnableRegexFormatting(bool enable) {}

void _setErrorSymbolsListFileName(const TCHAR* errorSymbolsListFileName) {}

void _setAppendToErrorSymbolsList(bool append) {}

void _setIgnoreErrorSymbolsList(bool ignore) {}

void _setIgnoreSymbolsList(const TCHAR* symbolsList) {}

void _setMapCharacters(const std::vector<std::string>& mapCharacters) {}

void _setDoNotCheckForUpdate(bool doNotCheckForUpdate) {}

void _showError(const TCHAR* text, const TCHAR* title) { printLine(text); }

void _showInfo(const TCHAR* text, const TCHAR* title) { printLine(text); }

bool _setUpdater(const StringSettable&, const StringSettable&) { return true; }

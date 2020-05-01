/*
Copyright (C) 2020  YLoader.com

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

#include "stdafx.h"

class ConsoleException : public std::exception {
 public:
  ConsoleException(const std::string& message) : exception(message.c_str()) {}
};

class ConsoleEventSink : public YahooEventSink {
 public:
  void event(const YahooEvent& event) { printLine(event.toString()); }
};

inline unsigned int getConnections(CmdLinePtr cmdLine) {
  return cmdLine->get< unsigned int >(&CmdLine::connections, DEFAULT_THREADS);
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
    throw CmdLineException(L"No output data path specified");
  }

  if (!yloader::DirectoryExists(cmdLine->dataPath())) {
    throw ConsoleException("Output path \"" + yloader::ws2s(cmdLine->dataPath().getValue()) + "\" could not be found");
  }

  const std::wstring symbolsFileName(cmdLine->symbolsFileName());
  if (symbolsFileName.empty()) {
    throw CmdLineException(L"No symbols file specified");
  }

  if (!yloader::fileExists(symbolsFileName)) {
    throw ConsoleException("Symbols file \"" + yloader::ws2s(symbolsFileName) + "\" could not be found");
  }
}

YDataSourcePluginPtr getAndValidateDataSourcePlugin(CmdLinePtr cmdLine) {
  // check the datasource plugin
  if (!cmdLine->dataSourcePlugin().isSet() || cmdLine->dataSourcePlugin().getValue().empty()) {
    throw CmdLineException(L"No datasource plugin specified");
  }

  YPlugins m_plugins;
  m_plugins.load(yloader::getModulePath());
  YDataSourcePluginVectorPtr dataSources(m_plugins.getMatchingDataSourcePlugins(cmdLine->dataSourcePlugin()));

  if (dataSources->empty()) {
    throw ConsoleException("Could not find a datasource plugin \"" + yloader::ws2s(cmdLine->dataSourcePlugin().getValue()) + "\"");
  }

  if (dataSources->size() > 1) {
    throw ConsoleException("Ambiguous datasource plugin \"" + yloader::ws2s(cmdLine->dataSourcePlugin().getValue()) + "\"");
  }

  return (*dataSources)[0];
}

class ConsoleStatistics : public Statistics {
  unsigned __int64 m_eventCount;
  unsigned __int64 m_progress;

  std::mutex m_mx;

 public:
  // progress indicates how often to show progress - 0 for no progress, N for
  // every N lines
  ConsoleStatistics(unsigned int progress)
      : m_eventCount(0), m_progress(progress) {}

  virtual void event(const YahooEvent& e) {
    std::scoped_lock lock(m_mx);

    ++m_eventCount;

    __super::event(e);

    if (m_progress > 0 && m_eventCount % m_progress == 0) {
      printLine(__super::toGraphicString());
    }
  }
};

class ConsoleUpdaterHandler : public UpdaterHandler {
 private:
  std::condition_variable condition;
  std::mutex mutex;
  Updater m_updater;

 private:
  void wait() {
    std::unique_lock< std::mutex > lock(mutex);
    condition.wait(lock);
  }

 public:
  ConsoleUpdaterHandler(const StringSettable& alternateHost, const StringSettable& alternateUpdateDirectory)
      : m_updater(*this, L"en_us", alternateHost, alternateUpdateDirectory) {
  }

  virtual void checkingForUpdate() {
    printLine(L"------ Checking for update ------");
  }

  virtual void updateAvailable() {
    printLine(L"Your version: " + Version::CURRENT.toString() + L", released on " + VersionInfo().getReleaseDate()->to_simple_string());
    printLine(L"New version available: " + m_updater.getVersion().toString() + L", released on " + m_updater.getUpdateInfo()->getReleaseDate()->to_simple_string());
    printLine(L"Go to " + *m_updater.getUpdateInfo()->getDownloadUrl() + L" to download new version");

    condition.notify_one();
  }

  virtual void noUpdateAvailable() {
    printLine(L"No new version available");
    condition.notify_one();
  }

  virtual void updateError(const std::wstring& error) {
    printLine(L"Update error: " + error);
    condition.notify_one();
  }

  void start() {
    m_updater.start();

    wait();
  }
};


int _tmain(int argc, _TCHAR* argv[]) {
  InitUninit initUninit;
  int ret = 0;
  // return 0 for no error, 1 for error

  CmdLinePtr cmdLine;
  try {
    printDashedLine(yloader::getCaption(STRPRODUCT_NAME));
    printLine();
    CmdLinePtr cmdLine(std::make_shared<CmdLine>());

    if (!cmdLine->help()) {
      if (!cmdLine->doNotCheckForUpdate()) {
        ConsoleUpdaterHandler handler(cmdLine->alternateUpdateHostName(), cmdLine->alternateUpdateDirectory());

        handler.start();
      }

      validateCmdLine(cmdLine);

      YDataSourcePluginPtr dateSourcePlugin(getAndValidateDataSourcePlugin(cmdLine));

      COleDateTime startDate(cmdLine->startDateAsOleDateTime());
      COleDateTime endDate(cmdLine->endDateAsOleDateTime());

      unsigned int connections(::getConnections(cmdLine));

      const std::wstring symbolsFileName(cmdLine->symbolsFileName());

      CharactersMapping cm(cmdLine->mapCharacters().getValue());

      YahooEventDelegator* yed(new YahooEventDelegator());
      YahooEventDelegatorPtr yes(yed);
      ConsoleEventSink ces;
      ConsoleStatistics stats(cmdLine->get< unsigned int >(&CmdLine::progress, DEFAULT_PROGRESS));

      yed->addSink(&ces);
      yed->addSink(&stats);

      StrListPtr symbolsList(yloader::getSymbols(symbolsFileName, true));

      if (!symbolsList) {
        throw ConsoleException("Error generating the symbols list");
      }

      SymbolsListIteratorPtr si(new SymbolsListIterator(symbolsList.get()));

      std::wstring logFile(cmdLine->logFile());

      DataParams dataParams(
        cmdLine->get< std::wstring >(&CmdLine::errorSymbolsListFileName, DEFAULT_ERROR_SYMBOLS_LIST),
        cmdLine->get< bool >(&CmdLine::appendToErrorSymbolsList, DEFAULT_APPEND_TO_ERROR_SYMBOLS_LIST),
        cmdLine->get< bool >(&CmdLine::ignoreErrorSymbolsList, DEFAULT_IGNORE_ERROR_SYMBOLS_LIST),
        cmdLine->get< std::wstring>(&CmdLine::ignoreSymbolsList, DEFAULT_IGNORE_SYMBOLS_LIST),
        dateSourcePlugin,
        cmdLine->dataPath(),
        static_cast< Period >(cmdLine->get< unsigned int >(&CmdLine::period, DEFAULT_PERIOD)),
        cmdLine->get< bool >( &CmdLine::adjust, DEFAULT_ADJUST),
        cmdLine->get< bool >(&CmdLine::allAvailable, DEFAULT_ALL_AVAILABLE),
        cmdLine->get< bool >(&CmdLine::update, DEFAULT_UPDATE),
        startDate,
        endDate,
        cmdLine->get< bool >(&CmdLine::validatePrices, DEFAULT_VALIDATE_PRICES),
        cmdLine->get< bool >(&CmdLine::sortBarsAscending, DEFAULT_SORT_BARS_ASCENDING),
        static_cast< yloader::DateFormat>(cmdLine->get< unsigned int >(&CmdLine::dateFormat, DEFAULT_DATE_FORMAT)),
        cmdLine->get< bool >(&CmdLine::addSymbol, DEFAULT_ADD_SYMBOL),
        cmdLine->symbolColumn(),
        cmdLine->prependToDataFileNameAsString(),
        cmdLine->appendToDataFileNameAsString(),
        cmdLine->get< bool >(&CmdLine::padDateFields, DEFAULT_PAD_DATE_FIELDS),
        DEFAULT_SYMBOL_TIMEOUT,
        cmdLine->get< bool >(&CmdLine::createSubdirs, DEFAULT_CREATE_SUBDIRS),
        cmdLine->get< unsigned int >(&CmdLine::handlingInvalidData, DEFAULT_HANDLING_INVALID_DATA),
        cmdLine->get< bool >(&CmdLine::handlingVolume0, DEFAULT_VOLUME0_INVALID),
        connections,
        DEFAULT_AUTO_SELECT_OUTPUT_PATH,
        L"",
        cm,
        cmdLine->get< std::wstring>(&CmdLine::fieldSeparator, DEFAULT_FIELD_SEPARATOR),
        cmdLine->get< double >(&CmdLine::volumeMultiplier, DEFAULT_VOLUME_MULTIPLIER),
        cmdLine->dataFileHeader(),
        cmdLine->get< std::wstring>( &CmdLine::dateSeparator, DEFAULT_DATE_SEPARATOR),
        cmdLine->outputFile(),
        symbolsFileName,
        cmdLine->get< std::wstring>(&CmdLine::extension, DEFAULT_EXTENSION),
        cmdLine->get< bool >(&CmdLine::noReloadOldData, DEFAULT_DONT_RELOAD_DATA_IN_UPDATE_MODE),
        logFile,
        cmdLine->get< bool >(&CmdLine::appendToLog, DEFAULT_APPEND_TO_LOG),
        cmdLine->get< bool >(&CmdLine::logOnlyErrors, DEFAULT_LOG_ONLY_ERRORS),
        cmdLine->get< int >(&CmdLine::fixedDecimalsCount, DEFAULT_FIXED_DECIMALS_COUNT),
        cmdLine->get< std::wstring>(&CmdLine::decimalSeparator, DEFAULT_DECIMAL_SEPARATOR),
        cmdLine->get< bool >(&CmdLine::enableRegexFormatting, DEFAULT_ENABLE_REGEX_FORMATTING),
        boost::wregex(cmdLine->get< std::wstring >(&CmdLine::regexMatch, DEFAULT_MATCH_REGEX)),
        cmdLine->get< std::wstring >(&CmdLine::regexFormat, DEFAULT_FORMAT_STRING),
        cmdLine->get< unsigned long >(&CmdLine::regexFlags, DEFAULT_REGEX_FLAGS)
      );

      // this needs to be done after creating the DataParams, as the log class
      // needs it
      YahooSessionLogPtr logSink;

      if (!logFile.empty()) {
        logSink = YahooSessionLogPtr(new YahooSessionLog(dataParams));

        yed->addSink(logSink.get());
      }

      if (cmdLine->showSettings()) {
        printDashedLine(L"Session settings");
        printLine(dataParams.toString());
      }

      std::wstring errorSymbolsFileName(cmdLine->errorSymbolsListFileName());
      std::wstring ignoreSymbolsList(cmdLine->ignoreSymbolsList());

      WriteFileSymbolsListPtr errorSymbolsList(
          cmdLine->hasErrorSymbolsListFileName()
              ? std::make_shared<WriteFileSymbolsList>(errorSymbolsFileName)
              : nullptr);

      if (!cmdLine->dontRun()) {
        yloader::UniqueSymbolsSetPtr ignoreSymbolsSet(dataParams.getIgnoreSymbols(yes));

        std::shared_ptr<CDownloading> downloading(std::make_shared< CDownloading >(dataParams, errorSymbolsList.get(), symbolsList,
                             ignoreSymbolsSet, si, yes));

        stats.start(symbolsList->size());

        printLine();
        printDashedLine(L"Downloading...");
        printLine(stats.toString());
        downloading->startSync();

        printLine(stats.toGraphicString());
        printLine();
        printDashedLine(L"Session statistics");
        printLine(stats.toString());

        printLine(L"Downloading session completed successfully");

        if (errorSymbolsList) {
          printDashedLine(L"Writing error symbols list file" );
          if (!errorSymbolsList->write()) {
            std::wstring errorMessage = L"Error: Failed to write to the symbols list file \"";
            errorMessage += cmdLine->errorSymbolsListFileName().getValue() + L"\"";
            printLine(errorMessage);
          }
        }
      }
      else {
        printLine(L"Session halted");
      }

      ret = 0;
    }
    else {
      // this is when the user asked for --help, it's not an error, but don't do
      // anything
      printLine( cmdLine->getUsage());
      ret = 0;
    }
  }
  catch (const LogFileException& e) {
    printLine(L"Could not open log file \"" + e.fileName() + L"\"");
    ret = 1;
  }
  catch (const CmdLineException& e) {
    // if command line error, ignore cmd line params
    printLine(L"Command line error");
    printLine(yloader::s2ws(e.what()));
    ret = 2;
  }
  catch (const ConsoleException& e) {
    printLine(L"Command line error");
    printLine(yloader::s2ws(e.what()));
    ret = 3;
  }
  catch (const FileSymbolsParserException& e) {
    // problem with the symbols file
    printLine(L"Symbols file error");
    printLine(e.message());
    ret = 4;
  }
  catch (const boost::bad_expression& e) {
    printLine(yloader::s2ws("Match regex error: "s + e.what()));
    ret = 5;
  }
  catch (const std::exception& e) {
    printLine(L"Error");
    printLine(yloader::s2ws(e.what()));
    ret = 6;
  }
  catch (...) {
    printLine(L"Unknown error");
    ret = 7;
  }

  if(0 != ret) {
      printLine(L"Downloading session failed");
  }

  return ret;
}



void showError(const TCHAR* text, const TCHAR* title) { printLine(text); }

void showInfo(const TCHAR* text, const TCHAR* title) { printLine(text); }

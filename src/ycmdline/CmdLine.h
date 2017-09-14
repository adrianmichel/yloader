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

#pragma once

#pragma warning(disable : 4482)

#include <atlcomtime.h>
#include <datetime.h>
#include <misc.h>
#include <sharedptr.h>
#include <strings.h>
#include <yloader.h>
#include <boost/regex.hpp>

class CmdLine {
 private:
  std::wstring _description;

  BoolSettable _addSymbol;
  BoolSettable _adjust;
  BoolSettable _allAvailable;
  StringSettable _appendToDataFileName;
  BoolSettable _autoExit;
  BoolSettable _autoStart;
  UIntSettable _connections;
  StringSettable _dataFileHeader;
  StringSettable _dataPath;
  StringSettable _dataSourcePlugin;
  UIntSettable _dateFormat;
  StringSettable _dateSeparator;
  StringSettable _endDate;
  StringSettable _extension;
  UIntSettable _handlingInvalidData;
  BoolSettable _handlingVolume0;
  BoolSettable _noReloadOldData;
  StringSettable _outputFile;
  BoolSettable _padDateFields;
  UIntSettable _period;
  StringSettable _prependToDataFileName;
  BoolSettable _sortBarsAscending;
  StringSettable _startDate;
  unsigned int _symbolColumn;
  StringSettable _symbolsFileName;
  BoolSettable _update;
  BoolSettable _validatePrices;
  BoolSettable _createSubdirs;
  BoolSettable _showSettings;
  BoolSettable _dontRun;
  StringSettable _fieldSeparator;
  DoubleSettable _volumeMultiplier;
  StringSettable _logFile;
  BoolSettable _appendToLog;
  BoolSettable _logOnlyErrors;
  UIntSettable _progress;
  StringSettable _regexMatch;
  StringSettable _regexFormat;
  ULongSettable _regexFlags;
  BoolSettable _enableRegexFormatting;
  StringSettable _errorSymbolsFileName;
  BoolSettable _appendToErrorSymbolsList;
  BoolSettable _ignoreErrorSymbolsList;
  StringSettable _ignoreSymbolsList;
  bool _doNotCheckForUpdate;
  StringSettable _alternateUpdateHost;
  StringSettable _alternateUpdateDirectory;
  StrVectorSettable _mapCharacters;
  IntSettable _fixedDecimalsCount;
  StringSettable _decimalSeparator;

 public:
  CmdLine();
  bool process();

  std::wstring getUsage() const;

  void showUsage();
  void showError(const std::wstring& error);

  const BoolSettable& autoExit() const { return _autoExit; }
  bool autoExit(bool def) const { return _autoExit.isSet() ? _autoExit : def; }
  const BoolSettable& autoStart() const { return _autoStart; }
  const StringSettable& symbolsFileName() const { return _symbolsFileName; }
  const StringSettable& dataPath() const { return _dataPath; }
  const StringSettable& startDate() const { return _startDate; }
  ATL::COleDateTime startDateAsOleDateTime() const {
    ATL::COleDateTime sd;

    if (_startDate.isSet())
      sd.ParseDateTime(((std::wstring)_startDate).c_str());
    else
      sd.SetStatus(COleDateTime::DateTimeStatus::null);

    return sd;
  }

  const StringSettable& endDate() const { return _endDate; }
  COleDateTime endDateAsOleDateTime() const {
    ATL::COleDateTime ed;

    if (_endDate.isSet())
      ed.ParseDateTime(((std::wstring)_endDate).c_str());
    else
      ed.SetStatus(COleDateTime::DateTimeStatus::null);

    return ed;
  }
  const BoolSettable& adjust() const { return _adjust; }
  bool adjust(bool def) const { return _adjust.isSet() ? _adjust : def; }
  const BoolSettable& validatePrices() const { return _validatePrices; }
  bool validatePrices(bool def) const {
    return _validatePrices.isSet() ? _validatePrices : def;
  }
  const BoolSettable& allAvailable() const { return _allAvailable; }
  bool allAvailable(bool def) const {
    return _allAvailable.isSet() ? _allAvailable : def;
  }
  const UIntSettable& period() const { return _period; }
  Period period(Period def) const {
    return _period.isSet() ? (Period)(unsigned int)_period : def;
  }
  const UIntSettable& connections() const { return _connections; }
  unsigned int connections(unsigned int def) const {
    return _connections.isSet() ? _connections : def;
  }
  const BoolSettable& update() const { return _update; }
  bool update(bool def) const { return _update.isSet() ? _update : def; }
  const UIntSettable& dateFormat() const { return _dateFormat; }
  yloader::DateFormat dateFormat(yloader::DateFormat def) const {
    return _dateFormat.isSet() ? (yloader::DateFormat)(unsigned int)_dateFormat
                               : def;
  }
  const BoolSettable& addSymbol() const { return _addSymbol; }
  bool addSymbol(bool def) const {
    return _addSymbol.isSet() ? _addSymbol : def;
  }
  unsigned int symbolColumn() const { return _symbolColumn; }
  const StringSettable& dateSeparator() const { return _dateSeparator; }
  std::wstring dateSeparator(const std::wstring& def) const {
    return _dateSeparator.isSet() ? _dateSeparator : def;
  }
  const BoolSettable& sortBarsAscending() const { return _sortBarsAscending; }
  bool sortBarsAscending(bool def) const {
    return _sortBarsAscending.isSet() ? _sortBarsAscending : def;
  }
  const BoolSettable& padDateFields() const { return _padDateFields; }
  bool padDateFields(bool def) const {
    return _padDateFields.isSet() ? _padDateFields : def;
  }
  const StringSettable& dataFileHeader() const { return _dataFileHeader; }
  const UIntSettable& handlingInvalidData() const {
    return _handlingInvalidData;
  }
  unsigned int handlingInvalidData(unsigned int def) const {
    return _handlingInvalidData.isSet() ? _handlingInvalidData : def;
  }
  const BoolSettable& handlingVolume0() const { return _handlingVolume0; }
  bool handlingVolume0(bool def) const {
    return _handlingVolume0.isSet() ? _handlingVolume0 : def;
  }
  const StringSettable& prependToDataFileName() const {
    return _prependToDataFileName;
  }
  std::wstring prependToDataFileNameAsString() const {
    return _prependToDataFileName.isSet() ? _prependToDataFileName
                                          : std::wstring();
  }
  std::wstring appendToDataFileNameAsString() const {
    return _appendToDataFileName.isSet() ? _appendToDataFileName
                                         : std::wstring();
  }
  const StringSettable& appendToDataFileName() const {
    return _appendToDataFileName;
  }
  const StringSettable& extension() const { return _extension; }
  std::wstring extension(std::wstring def) const {
    return _extension.isSet() ? _extension : def;
  }
  const StringSettable& dataSourcePlugin() const { return _dataSourcePlugin; }
  const StringSettable& outputFile() const { return _outputFile; }
  const BoolSettable& noReloadOldData() const { return _noReloadOldData; }
  bool noReloadOldData(bool def) const {
    return _noReloadOldData.isSet() ? _noReloadOldData : def;
  }
  const BoolSettable& createSubdirs() const { return _createSubdirs; }
  bool createSubdirs(bool def) const {
    return _createSubdirs.isSet() ? _createSubdirs : def;
  }
  bool showSettings() const { return _showSettings.isSet(); }
  bool dontRun() const { return _dontRun.isSet(); }
  const StringSettable& fieldSeparator() const { return _fieldSeparator; }
  std::wstring fieldSeparator(const std::wstring& def) const {
    return _fieldSeparator.isSet() ? _fieldSeparator : def;
  }
  const DoubleSettable& volumeMultiplier() const { return _volumeMultiplier; }
  double volumeMultiplier(double def) const {
    return _volumeMultiplier.isSet() ? _volumeMultiplier : def;
  }
  const StringSettable& logFile() const { return _logFile; }
  std::wstring logFile(const std::wstring& def) const {
    return _logFile.isSet() ? _logFile : def;
  }
  const BoolSettable& appendToLog() const { return _appendToLog; }
  bool appendToLog(bool def) const {
    return _appendToLog.isSet() ? _appendToLog : def;
  }
  const BoolSettable& logOnlyErrors() const { return _logOnlyErrors; }
  bool logOnlyErrors(bool def) const {
    return _logOnlyErrors.isSet() ? _logOnlyErrors : def;
  }
  const UIntSettable& progress() const { return _progress; }
  unsigned int progress(unsigned int def) const {
    return _progress.isSet() ? _progress : def;
  }

  const StringSettable& regexMatch() const { return _regexMatch; }
  const StringSettable& regexFormat() const { return _regexFormat; }
  boost::wregex regexMatch(const std::wstring& def) const {
    return hasRegexMatch() ? boost::wregex((const std::wstring&)_regexMatch)
                           : boost::wregex(def);
  }
  std::wstring regexFormat(const std::wstring& def) const {
    return _regexFormat.isSet() ? _regexFormat : def;
  }
  bool hasRegexMatch() const {
    return _regexMatch.isSet() && !((const std::wstring&)_regexMatch).empty();
  }
  const ULongSettable& regexFlags() const { return _regexFlags; }
  unsigned long regexFlags(unsigned long def) const {
    return _regexFlags.isSet() ? _regexFlags : def;
  }
  const BoolSettable& enableRegexFormatting() const {
    return _enableRegexFormatting;
  }
  bool enableRegexFomratting(bool def) const {
    return _enableRegexFormatting.isSet() ? _enableRegexFormatting : def;
  }
  const StringSettable& errorSymbolsListFileName() const {
    return _errorSymbolsFileName;
  }
  std::wstring errorSymbolsListFileName(const std::wstring& def) const {
    return _errorSymbolsFileName.isSet() ? _errorSymbolsFileName : def;
  }
  bool hasErrorSymbolsListFileName() const {
    return _errorSymbolsFileName.isSet();
  }

  const BoolSettable& appendToErrorSymbolsList() const {
    return _appendToErrorSymbolsList;
  }
  bool appendToErrorSymbolsList(bool def) const {
    return _appendToErrorSymbolsList.isSet() ? _appendToErrorSymbolsList : def;
  }

  const BoolSettable& ignoreErrorSymbolsList() const {
    return _ignoreErrorSymbolsList;
  }
  bool ignoreErrorSymbolsList(bool def) const {
    return _ignoreErrorSymbolsList.isSet() ? _ignoreErrorSymbolsList : def;
  }

  const StringSettable& ignoreSymbolsList() const { return _ignoreSymbolsList; }
  std::wstring ignoreSymbolsList(const std::wstring& def) const {
    return _ignoreSymbolsList.isSet() ? _ignoreSymbolsList : def;
  }

  const StringSettable& alternateUpdateHostName() const {
    return _alternateUpdateHost;
  }
  const StringSettable& alternateUpdateDirectory() const {
    return _alternateUpdateDirectory;
  }

  bool doNotCheckForUpdate() const { return _doNotCheckForUpdate; }
  const StrVectorSettable& mapCharacters() const { return _mapCharacters; }
  const IntSettable fixedDecimalsCount() const { return _fixedDecimalsCount; }
  const StringSettable& decimalSeparator() const { return _decimalSeparator; }
  int fixedDecimalsCount(int def) const {
    return _fixedDecimalsCount.isSet() ? _fixedDecimalsCount.getValue() : def;
  }
  const std::wstring decimalSeparator(const std::wstring& def) const {
    return _decimalSeparator.isSet() ? _decimalSeparator.getValue() : def;
  }
};

class CmdLineException : public std::exception {
 public:
  CmdLineException(const std::wstring& message)
      : exception(yloader::ws2s(message).c_str()) {}
};

typedef yloader::ManagedPtr<CmdLine> CmdLinePtr;

void notice(yloader::t_ostream& os);

CmdLinePtr ysession();

unsigned int _create(unsigned int mode);
bool _start(StartMode startMode, ExitMode exitMode);
bool _setUpdater(const StringSettable& alternateUpdateHostName,
                 const StringSettable& alternateUpdateDirectory);
bool _close();
bool _isRunning(bool* isRunning);
bool _getLastError(TCHAR* error, unsigned int errorMessageLength);
bool _loadSettings(const TCHAR* settingsName);

void _setSymbolsFileName(const TCHAR* symbolsFileName);
void _setDataPath(const TCHAR* dataPath);
void _setStartDate(const TCHAR* startDate);
void _setEndDate(const TCHAR* endDate);
void _setDateSeparator(const TCHAR* dateSeparator);
void _setAddSymbolToColumn(bool add, unsigned int colum);
void _setSortBarsAscending(bool sortBarsAscending);
void _setPadDateFields(bool padDateFields);
void _setDataFileHeader(const TCHAR* dataFileHeader);
void _setInvalidDataHandling(unsigned int invalidDataHandling);
void _setVolume0Handling(bool volume0Handling);
void _setPrependToDataFileName(const TCHAR* prepend);
void _setAppendToDataFileName(const TCHAR* append);
void _setExtension(const TCHAR* extension);
void _setConnections(unsigned int connections);
void _setOutputFile(const TCHAR* outputFile);
void _setDateFormat(unsigned int dateFormat);
void _setAdjust(bool adjust);
void _setValidatePrices(bool validate);
void _setPeriod(unsigned int period);
void _setAllAvailable(bool allAvailable);
void _setUpdate(bool update);
void _setDataSourcePlugin(const TCHAR* dataSource);
void _setNoReloadOldData(bool noReloadOldData);
void _setCreateSubdirs(bool createSubdirs);
void _setFieldSeparator(const TCHAR* fieldSeparator);
void _setVolumeMultiplier(double volumeMultiplier);
void _setLogFile(const TCHAR* logFile);
void _setAppendToLog(bool appendToLog);
void _setLogOnlyErrors(bool logOnlyErrors);
void _setProgress(unsigned int progress);
void _setRegexMatch(const TCHAR* regexMatch);
void _setRegexFormat(const TCHAR* regexFormat);
void _setRegexFlags(unsigned long flags);
void _setEnableRegexFormatting(bool enable);
void _setErrorSymbolsListFileName(const TCHAR* errorSymbolsListFileName);
void _setAppendToErrorSymbolsList(bool append);
void _setIgnoreErrorSymbolsList(bool ignore);
void _setAutoStart(bool autoStart);
void _setAutoExit(bool autoExit);
void _setIgnoreSymbolsList(const TCHAR* symbolsList);
void _setDoNotCheckForUpdate(bool doNotCheckForUpdate);
void _setMapCharacters(const std::vector<std::string>& mapCharacters);
void _setFixedDecimalsCount(unsigned int count);
void _setDecimalSeparator(const TCHAR* decimalSeparator);

void _showError(const TCHAR* text, const TCHAR* title);
void _showInfo(const TCHAR* text, const TCHAR* title);

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

#pragma once

#pragma warning(disable : 4482)

#include <atlcomtime.h>
#include <datetime.h>
#include <misc.h>
#include <sharedptr.h>
#include <strings.h>
#include <yloader.h>
#include <functional>

using namespace std::string_literals;

#define SINGLE_OUTPUT_FILE_SHORT L"b"
#define ADD_SYMBOL_SHORT L"y"
#define NO_DATE_SEPARATOR_SHORT L"z"
#define SYMBOL_COLUMN_SHORT L"m"
#define DATE_SEPARATOR_SHORT L"r"

// abcdefghijklmnopqrstuvwxyz
// **************************
//
// ABCDEFGHIJKLMNOPQRSTUVWXYZ
// ********************** ***

constexpr const wchar_t* COMMAND_LINE_HELP[] = { L"help", L"?", L"show this help message" };
constexpr const wchar_t* AUTO_START[] = { L"autostart", L"a", L"start downloading automatically at startup (only in GUI version)" };
constexpr const wchar_t* SINGLE_OUTPUT_FILE[] = { L"singleoutputfile", SINGLE_OUTPUT_FILE_SHORT, L"single output file name, which will contain the data for all symbols. If this argument is not specified, no single file will be generated" };
constexpr const wchar_t* CONNECTIONS[] = { L"connections", L"c", L"number of simultaneous connections - between 1 and 20" };
constexpr const wchar_t* DATA_PATH[] = { L"datapath", L"d", L"data directory" };
constexpr const wchar_t* END_DATE[] = { L"enddate", L"e", L"end of date range as \"mm/dd/yyyy\" or a quoted empty string (\"\") for the latest available" };
constexpr const wchar_t* DATE_FORMAT[] = { L"dateformat", L"f", L"output date format: 0 for m/d/y, 1 for d/m/y, 2 for y/m/d" };
constexpr const wchar_t* LOG_FILE[] = { L"logfile", L"g", L"log file name" };
constexpr const wchar_t* DATA_FILE_HEADER[] = { L"datafileheader", L"h", L"string that will be placed at the top of each data file" };
constexpr const wchar_t* HANDLING_INVALID_DATA[] = { L"invaliddatahandling", L"i", L"data error handling mode - 0 to treat as warnings, 1 to treat as errors" };
constexpr const wchar_t* ADJUST[] = { L"adjust", L"j", L"dividend and split adjust mode - 0 for adjust off, 1 for adjust on" };
constexpr const wchar_t* NO_SINGLE_OUTPUT_FILE[] = { L"nosingleoutputfile", L"k", L"create a single output file containing data for all symbols. Takes precedence over \"-" SINGLE_OUTPUT_FILE_SHORT "\"" };
constexpr const wchar_t* ALL_AVAILABLE[] = { L"allavailable", L"l", L"download all available data - 0 to disable and downlod by range, 1 to download all" };
constexpr const wchar_t* SYMBOL_COLUMN[] = { L"symbolcolumn", SYMBOL_COLUMN_SHORT, L"the column to which the symbol will be added in the data file if the option \"-" ADD_SYMBOL_SHORT "\" is also present with argument 1, otherwise will have no effect" };
constexpr const wchar_t* DATA_SOURCE_PLUGIN[] = { L"datasource", L"n", L"data source - can be the first character(s) of the name of the datasource (case insensitive), for example \"y\" for Yahoo" };
constexpr const wchar_t* SHOW_SETTINGS[] = { L"showsettings", L"o", L"show current settings, applies to console version only" };
constexpr const wchar_t* PERIOD[] = { L"period", L"p", L"period - 0 for daily, 1 for weekly, 2 for monthly, if supported by the data source" };
constexpr const wchar_t* CREATE_SUBDIRS[] = { L"createsubdirs", L"q", L"save data files under nested directories" };
constexpr const wchar_t* DATE_SEPARATOR[] = { L"dateseparator", DATE_SEPARATOR_SHORT, L"date separator character. To disable the date separator, use the option \"-" NO_DATE_SEPARATOR_SHORT "\"" };
constexpr const wchar_t* SYMBOLS_FILE[] = { L"symbolsfile", L"s", L"symbols file name" };
constexpr const wchar_t* START_DATE[] = { L"startdate", L"t", L"start of date range as \"mm/dd/yyyy\" or a quoted empty string (\"\") for the oldest available" };
constexpr const wchar_t* UPDATE[] = { L"update", L"u", L"update mode - 0 for update off, 1 for update on" };
constexpr const wchar_t* VALIDATE_PRICES[] = { L"validateprices", L"v", L"price validation mode - 0 for validation off, 1 for validation on" };
constexpr const wchar_t* NO_RELOAD_OLD_DATA[] = { L"noreloadolddata", L"w", L"no reload of old data in Update mode, even if old data is inconsistent with new data" };
constexpr const wchar_t* AUTO_EXIT[] = { L"autoexit", L"x", L"exit after completing the downloading session (only in GUI version" };
constexpr const wchar_t* ADD_SYMBOL[] = { L"addsymbol", ADD_SYMBOL_SHORT, L"add symbol to data file -  0 to not add symbols to the data file, 1 to add symbol to data file to column specified by option \"-" SYMBOL_COLUMN_SHORT "\" if set, or first column if not" };
constexpr const wchar_t* NO_DATE_SEPARATOR[] = { L"nodateseparator", NO_DATE_SEPARATOR_SHORT, L"disable date separator - see option \"-" DATE_SEPARATOR_SHORT "\"" };
constexpr const wchar_t* APPEND_TO_DATA_FILE_NAME[] = { L"appendtofilename", L"A", L"append string to data file name" };
constexpr const wchar_t* ERROR_SYMBOLS_FILE_NAME[] = { L"errorsymbolsfilename", L"B", L"error symbols file name - if not set no error symbols file is generated, if set to a valid file name, symbols for which there is no data will be saved to that file" };
constexpr const wchar_t* CONFIG_FILE[] = { L"configfile", L"C", L"get command line options from config file (can be specified with @name)" };
constexpr const wchar_t* PAD_DATE_FIELDS[] = { L"paddatefields", L"D", L"date fields padding - 0 for off, 1 for on" };
constexpr const wchar_t* LOG_ONLY_ERRORS[] = { L"logonlyerrors", L"E", L"log only errors - 0 to log all errors, 1 to log only errors" };
constexpr const wchar_t* FIELD_SEPARATOR[] = { L"fieldseparator", L"F", L"field separator string" };
constexpr const wchar_t* REGEX_FLAGS[] = { L"regexflags", L"G", L"regular expression formatting parameters. This argument is optional and not needed in most cases. See the documentation for more details." };
constexpr const wchar_t* APPEND_TO_ERROR_SYMBOLS_LIST[] = { L"appendtoerrorsymbolslist", L"H", L"append vs overwrite for the error symbols list file -  0 to overwrite the existing file, if any, 1 to append new symbols to the existing file if any." };
constexpr const wchar_t* IGNORE_ERROR_SYMBOLS_LIST[] = { L"ignoreerrorsymbolslist", L"I", L"ignore mode symbols in error symbols list - 0 to attempt downloading data for all symbols in the current symbols list, 1 to ignore symbols present in the error symbols list." };
constexpr const wchar_t* IGNORE_SYMBOLS_LIST[] = { L"ignoresymbolslist", L"J", L"ignore symbols from the specified file" };
constexpr const wchar_t* MAP_CHARACTER[] = { L"mapcharacter", L"K", L"if argument contains two characters, change first character to second character in data file name. If argument contains only on character, remove it from the data file name. See full documentation for more details" };
constexpr const wchar_t* VOLUME_MULTIPLIER[] = { L"volumemultiplier", L"M", L"volume multiplier" };
constexpr const wchar_t* DONT_RUN[] = { L"dontrun", L"N", L"don't start session (only in console version)" };
constexpr const wchar_t* ENABLE_REGEX_FORMATTING[] = { L"enableregexformatting", L"O", L"enables regex formatting and disables settings formating" };
constexpr const wchar_t* PREPEND_TO_DATA_FILE_NAME[] = { L"prependtofilename", L"P", L"prepended string to the data file name" };
constexpr const wchar_t* PROGRESS[] = { L"progress", L"R", L"download progress message frequency - 0 for no progress, positive integer N to show progress every N symbols (only in console version)" };
constexpr const wchar_t* SORT_BARS_ASCENDING[] = { L"sortascending", L"S", L"sort bars - 0 descending (recent bars at the top), 1 ascending (recent bars at the bottom)" };
constexpr const wchar_t* APPEND_TO_LOG[] = { L"appendtolog", L"T", L"append new log content to the existing log file - 0 to clear log each time, 1 to append to existing log" };
constexpr const wchar_t* DO_NOT_CHECK_FOR_UPDATE[] = { L"donotcheckforupdate", L"Q", L"do not check for YLoader update at startup, by default will check if there is an update (only in console version)" };
constexpr const wchar_t* HANDLING_VOLUME_0[] = { L"volume0handling", L"V", L"zero volume handling mode - 0 to treat as valid data, 1 to treat as invalid data" };
constexpr const wchar_t* EXTENSION[] = { L"extension", L"X", L"data file extension" };
constexpr const wchar_t* REGEX_MATCH[] = { L"matchregex", L"Y", L"match string for regex formatting" };
constexpr const wchar_t* REGEX_FORMAT[] = { L"formatstring", L"Z", L"format string for regex formatting" };
constexpr const wchar_t* ALTERNATE_UPDATE_HOST[] = { L"altupdatehost", L"", L"" };
constexpr const wchar_t* ALTERNATE_UPDATE_DIRECTORY[] = { L"altupdatedir", L"", L"" };
constexpr const wchar_t* FIXED_DECIMALS_COUNT[] = { L"fixeddecimalscount", L"L", L"forces price values to display with fixed number of decimals specified by the argument" };
constexpr const wchar_t* DECIMAL_SEPARATOR[] = { L"decimalseparator", L"U", L"sets the character used as decimal separator, by default \".\"" };

inline std::wstring wLongName(const LPCWSTR* option) {
  assert(L""s != option[0]);
  return option[0];
}
inline std::string longName(const LPCWSTR* option) {
  assert(L""s != option[0]);
  return yloader::ws2s(option[0]);
}

inline std::optional< std::string > shortName(const LPCWSTR* option) {
  if (L""s == option[1]) {
    return std::nullopt;
  }
  else {
    return yloader::ws2s(option[1]);
  }
}

inline std::optional< std::wstring > wShortName(const LPCWSTR* option) {
  if (L""s == option[1]) {
    return std::nullopt;
  }
  else {
    return option[1];
  }
}

inline std::string fullName(const LPCWSTR* option) {
  return longName(option) + (shortName(option) ? (","s + *shortName(option)) : "");
}

inline std::string description(const LPCWSTR* option) {
  return yloader::ws2s(option[2]);
}




class CmdLine {
 private:
  std::wstring m_description;

  BoolSettable m_help;
  BoolSettable m_addSymbol;
  BoolSettable m_adjust;
  BoolSettable m_allAvailable;
  StringSettable m_appendToDataFileName;
  BoolSettable m_autoExit;
  BoolSettable m_autoStart;
  UIntSettable m_connections;
  StringSettable m_dataFileHeader;
  StringSettable m_dataPath;
  StringSettable m_dataSourcePlugin;
  UIntSettable m_dateFormat;
  StringSettable m_dateSeparator;
  StringSettable m_endDate;
  StringSettable m_extension;
  UIntSettable m_handlingInvalidData;
  BoolSettable m_handlingVolume0;
  BoolSettable m_noReloadOldData;
  StringSettable m_outputFile;
  BoolSettable m_padDateFields;
  UIntSettable m_period;
  StringSettable m_prependToDataFileName;
  BoolSettable m_sortBarsAscending;
  StringSettable m_startDate;
  UIntSettable m_symbolColumn;
  StringSettable m_symbolsFileName;
  BoolSettable m_update;
  BoolSettable m_validatePrices;
  BoolSettable m_createSubdirs;
  BoolSettable m_showSettings;
  BoolSettable m_dontRun;
  StringSettable m_fieldSeparator;
  DoubleSettable m_volumeMultiplier;
  StringSettable m_logFile;
  BoolSettable m_appendToLog;
  BoolSettable m_logOnlyErrors;
  UIntSettable m_progress;
  StringSettable m_regexMatch;
  StringSettable m_regexFormat;
  ULongSettable m_regexFlags;
  BoolSettable m_enableRegexFormatting;
  StringSettable m_errorSymbolsFileName;
  BoolSettable m_appendToErrorSymbolsList;
  BoolSettable m_ignoreErrorSymbolsList;
  StringSettable m_ignoreSymbolsList;
  BoolSettable m_doNotCheckForUpdate;
  StringSettable m_alternateUpdateHost;
  StringSettable m_alternateUpdateDirectory;
  StrVectorSettable m_mapCharacters;
  IntSettable m_fixedDecimalsCount;
  StringSettable m_decimalSeparator;

private:
  static ATL::COleDateTime dateAsOleDateTime(const StringSettable& date) {
    ATL::COleDateTime d;

    if (date.isSet()) {
      d.ParseDateTime(date.getValue().c_str());
    }
    else {
      d.SetStatus(COleDateTime::DateTimeStatus::null);
    }

    return d;
  }

  void process(std::vector<std::string>& args);

public:
  CmdLine();
  CmdLine(const std::wstring& cmdLineStr);

  bool help() const {
    return m_help;
  }

  std::wstring getUsage() const;

  const BoolSettable& autoExit() const { return m_autoExit; }
  const BoolSettable& autoStart() const { return m_autoStart; }
  const StringSettable& symbolsFileName() const { return m_symbolsFileName; }
  const StringSettable& dataPath() const { return m_dataPath; }
  const StringSettable& startDate() const { return m_startDate; }

  ATL::COleDateTime startDateAsOleDateTime() const {
    return dateAsOleDateTime(m_startDate);
  }

  const StringSettable& endDate() const { return m_endDate; }
  COleDateTime endDateAsOleDateTime() const {
    return dateAsOleDateTime(m_endDate);
  }
  const BoolSettable& adjust() const { return m_adjust; }
  const BoolSettable& validatePrices() const { return m_validatePrices; }
  const BoolSettable& allAvailable() const { return m_allAvailable; }
  const UIntSettable& period() const { return m_period; }
  const UIntSettable& connections() const { return m_connections; }

  const BoolSettable& update() const { return m_update; }
  const UIntSettable& dateFormat() const { return m_dateFormat; }
  const BoolSettable& addSymbol() const { return m_addSymbol; }
  template< typename T > T get(std::function< const Settable< T >&(CmdLine*)> f, const T& def) {
    const Settable< T >& s = std::invoke(f, this);
    return s.isSet() ? s.getValue() : def;
  }
  unsigned int symbolColumn() const { return m_symbolColumn; }
  const StringSettable& dateSeparator() const { return m_dateSeparator; }
  const BoolSettable& sortBarsAscending() const { return m_sortBarsAscending; }
  const BoolSettable& padDateFields() const { return m_padDateFields; }
  const StringSettable& dataFileHeader() const { return m_dataFileHeader; }
  const UIntSettable& handlingInvalidData() const {return m_handlingInvalidData;}
  const BoolSettable& handlingVolume0() const { return m_handlingVolume0; }
  const StringSettable& prependToDataFileName() const { return m_prependToDataFileName; }
  std::wstring prependToDataFileNameAsString() const {
    return m_prependToDataFileName.isSet() ? m_prependToDataFileName.getValue(): std::wstring();
  }
  std::wstring appendToDataFileNameAsString() const {
    return m_appendToDataFileName.isSet() ? m_appendToDataFileName.getValue() : std::wstring();
  }
  const StringSettable& appendToDataFileName() const {return m_appendToDataFileName; }
  const StringSettable& extension() const { return m_extension; }
  const StringSettable& dataSourcePlugin() const { return m_dataSourcePlugin; }
  const StringSettable& outputFile() const { return m_outputFile; }
  const BoolSettable& noReloadOldData() const { return m_noReloadOldData; }
  const BoolSettable& createSubdirs() const { return m_createSubdirs; }
  bool showSettings() const { return m_showSettings.isSet(); }
  bool dontRun() const { return m_dontRun.isSet(); }
  const StringSettable& fieldSeparator() const { return m_fieldSeparator; }
  const DoubleSettable& volumeMultiplier() const { return m_volumeMultiplier; }
  const StringSettable& logFile() const { return m_logFile; }
  const BoolSettable& appendToLog() const { return m_appendToLog; }
  const BoolSettable& logOnlyErrors() const { return m_logOnlyErrors; }
  const UIntSettable& progress() const { return m_progress; }
  const StringSettable& regexMatch() const { return m_regexMatch; }
  const StringSettable& regexFormat() const { return m_regexFormat; }
  boost::wregex getRegexMatch(const std::wstring& def) const {
    return hasRegexMatch() ? boost::wregex((const std::wstring&)m_regexMatch)
                           : boost::wregex(def);
  }
  bool hasRegexMatch() const {
    return m_regexMatch.isSet() && !((const std::wstring&)m_regexMatch).empty();
  }
  const ULongSettable& regexFlags() const { return m_regexFlags; }
  const BoolSettable& enableRegexFormatting() const { return m_enableRegexFormatting; }
  const StringSettable& errorSymbolsListFileName() const { return m_errorSymbolsFileName; }
  bool hasErrorSymbolsListFileName() const { return m_errorSymbolsFileName.isSet(); }
  const BoolSettable& appendToErrorSymbolsList() const { return m_appendToErrorSymbolsList; }
  const BoolSettable& ignoreErrorSymbolsList() const { return m_ignoreErrorSymbolsList; }
  const StringSettable& ignoreSymbolsList() const { return m_ignoreSymbolsList; }
  const StringSettable& alternateUpdateHostName() const { return m_alternateUpdateHost; }
  const StringSettable& alternateUpdateDirectory() const { return m_alternateUpdateDirectory; }

  bool doNotCheckForUpdate() const { return m_doNotCheckForUpdate; }
  const StrVectorSettable& mapCharacters() const { return m_mapCharacters; }
  const IntSettable& fixedDecimalsCount() const { return m_fixedDecimalsCount; }
  const StringSettable& decimalSeparator() const { return m_decimalSeparator; }
};

class CmdLineException : public std::exception {
 public:
  CmdLineException(const std::wstring& message)
      : exception(yloader::ws2s(message).c_str()) {}
};

using CmdLinePtr = std::shared_ptr<CmdLine> ;

void notice(std::wostream& os);

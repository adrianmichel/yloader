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

// TODO
// add:
//  - volume multiplier
//	- log file
// 	- hidden diagnostic log
//  - symbol timeout
//  - autoselect output path

#include "stdafx.h"
#include "CmdLine.h"

using yloader::operator<<;

// abcdefghijklmnopqrstuvwxyz
// **************************
//
// ABCDEFGHIJKLMNOPQRSTUVWXYZ
// *********** ** ***** * ***

#define COMMAND_LINE_HELP_SHORT _T( "?" )
#define COMMAND_LINE_HELP_LONG _T( "help" )

#define AUTO_START_SHORT _T( "a" )
#define AUTO_START_LONG _T( "autostart" )

#define SINGLE_OUTPUT_FILE_SHORT _T( "b" )
#define SINGLE_OUTPUT_FILE_LONG _T( "singleoutputfile" )

#define CONNECTIONS_SHORT _T( "c" )
#define CONNECTIONS_LONG _T( "connections" )

#define DATA_PATH_SHORT _T( "d" )
#define DATA_PATH_LONG _T( "datapath" )

#define END_DATE_SHORT _T( "e" )
#define END_DATE_LONG _T( "enddate" )

#define DATE_FORMAT_SHORT _T( "f" )
#define DATE_FORMAT_LONG _T( "dateformat" )

#define LOG_FILE_SHORT _T( "g" )
#define LOG_FILE_LONG _T( "logfile" )

#define DATA_FILE_HEADER_SHORT _T( "h" )
#define DATA_FILE_HEADER_LONG _T( "datafileheader" )

#define HANDLING_INVALID_DATA_SHORT _T( "i" )
#define HANDLING_INVALID_DATA_LONG _T( "invaliddatahandling" )

#define ADJUST_SHORT _T( "j" )
#define ADJUST_LONG _T( "adjust" )

#define NO_SINGLE_OUTPUT_FILE_SHORT _T( "k" )
#define NO_SINGLE_OUTPUT_FILE_LONG _T( "nosingleoutputfile" )

#define ALL_AVAILABLE_SHORT _T( "l" )
#define ALL_AVAILABLE_LONG _T( "allavailable" )

#define SYMBOL_COLUMN_SHORT _T( "m" )
#define SYMBOL_COLUMN_LONG _T( "symbolcolumn" )

#define DATA_SOURCE_PLUGIN_SHORT _T( "n" )
#define DATA_SOURCE_PLUGIN_LONG _T( "datasource" )

#define SHOW_SETTINGS_SHORT _T( "o" )
#define SHOW_SETTINGS_LONG _T( "showsettings" )

#define PERIOD_SHORT _T( "p" )
#define PERIOD_LONG _T( "period" )

#define CREATE_SUBDIRS_SHORT _T( "q" )
#define CREATE_SUBDIRS_LONG _T( "createsubdirs" )

#define DATE_SEPARATOR_SHORT _T( "r" )
#define DATE_SEPARATOR_LONG _T( "dateseparator" )

#define SYMBOLS_FILE_SHORT _T( "s" )
#define SYMBOLS_FILE_LONG _T( "symbolsfile" )

#define START_DATE_SHORT _T( "t" )
#define START_DATE_LONG _T( "startdate" )

#define UPDATE_SHORT _T( "u" )
#define UPDATE_LONG _T( "update" )

#define VALIDATE_PRICES_SHORT _T( "v" )
#define VALIDATE_PRICES_LONG _T( "validateprices" )

#define NO_RELOAD_OLD_DATA_SHORT _T( "w" )
#define NO_RELOAD_OLD_DATA_LONG _T( "noreloadolddata" )

#define AUTO_EXIT_SHORT _T( "x" )
#define AUTO_EXIT_LONG _T( "autoexit" )

#define ADD_SYMBOL_SHORT _T( "y" )
#define ADD_SYMBOL_LONG _T( "addsymbol" )

#define NO_DATE_SEPARATOR_SHORT _T( "z" )
#define NO_DATE_SEPARATOR_LONG _T( "nodateseparator" )

#define APPEND_TO_DATA_FILE_NAME_SHORT _T( "A" )
#define APPEND_TO_DATA_FILE_NAME_LONG _T( "appendtofilename" )

#define ERROR_SYMBOLS_FILE_NAME_SHORT _T( "B" )
#define ERROR_SYMBOLS_FILE_NAME_LONG _T( "errorsymbolsfilename" )

#define CONFIG_FILE_SHORT _T( "C" )
#define CONFIG_FILE_LONG _T( "configfile" )

#define PAD_DATE_FIELDS_SHORT _T( "D" )
#define PAD_DATE_FIELDS_LONG _T( "paddatefields" )

#define LOG_ONLY_ERRORS_SHORT _T( "E" )
#define LOG_ONLY_ERRORS_LONG _T( "logonlyerrors" )

#define FIELD_SEPARATOR_SHORT _T( "F" )
#define FIELD_SEPARATOR_LONG _T( "fieldseparator" )

#define REGEX_FLAGS_SHORT _T( "G" )
#define REGEX_FLAGS_LONG _T( "regexflags" )

#define APPEND_TO_ERROR_SYMBOLS_LIST_SHORT _T( "H" )
#define APPEND_TO_ERROR_SYMBOLS_LIST_LONG _T( "appendtoerrorsymbolslist" )

#define IGNORE_ERROR_SYMBOLS_LIST_SHORT _T( "I" )
#define IGNORE_ERROR_SYMBOLS_LIST_LONG _T( "ignoreerrorsymbolslist" )

#define IGNORE_SYMBOLS_LIST_SHORT _T( "J" )
#define IGNORE_SYMBOLS_LIST_LONG _T( "ignoresymbolslist" )

#define MAP_CHARACTER_SHORT _T( "K" )
#define MAP_CHARACTER_LONG _T( "mapcharacter" )

#define VOLUME_MULTIPLIER_SHORT _T( "M" )
#define VOLUME_MULTIPLIER_LONG _T( "volumemultiplier" )

#define DONT_RUN_SHORT _T( "N" )
#define DONT_RUN_LONG _T( "dontrun" )

#define ENABLE_REGEX_FORMATTING_SHORT _T( "O" )
#define ENABLE_REGEX_FORMATTING_LONG _T( "enableregexformatting" )

#define PREPEND_TO_DATA_FILE_NAME_SHORT _T( "P" )
#define PREPEND_TO_DATA_FILE_NAME_LONG _T( "prependtofilename" )

#define PROGRESS_SHORT _T( "R" )
#define PROGRESS_LONG _T( "progress" )

#define SORT_BARS_ASCENDING_SHORT _T( "S" )
#define SORT_BARS_ASCENDING_LONG _T( "sortascending" )

#define APPEND_TO_LOG_SHORT _T( "T" )
#define APPEND_TO_LOG_LONG _T( "appendtolog" )

#define DO_NOT_CHECK_FOR_UPDATE_SHORT _T( "Q" )
#define DO_NOT_CHECK_FOR_UPDATE_LONG _T( "donotcheckforupdate" )

#define HANDLING_VOLUME_0_SHORT _T( "V" )
#define HANDLING_VOLUME_0_LONG _T( "volume0handling" )

#define EXTENSION_SHORT _T( "X" )
#define EXTENSION_LONG _T( "extension" )

#define REGEX_MATCH_SHORT _T( "Y" )
#define REGEX_MATCH_LONG _T( "matchregex" )

#define REGEX_FORMAT_SHORT _T( "Z" )
#define REGEX_FORMAT_LONG _T( "formatstring" )

#define ALTERNATE_UPDATE_HOST _T( "altupdatehost" )

#define ALTERNATE_UPDATE_DIRECTORY _T( "altupdatedir" )

typedef std::map<std::string, unsigned int> CmdLineInfoBase;

class CmdLineInfo : public CmdLineInfoBase {
  static TCHAR* x[][3];
  static const size_t x_size;

 public:
  CmdLineInfo() {
    for (unsigned int n = 0; n < x_size; ++n) {
      std::wstring shortName(x[n][0]);

      __super::insert(CmdLineInfoBase::value_type(yloader::ws2s(shortName), n));
    }
  }

  std::string fullName(const std::wstring& shortName) {
    CmdLineInfoBase::const_iterator i = __super::find(yloader::ws2s(shortName));

    if (i != __super::end())
      return std::string(yloader::ws2s(x[i->second][1])) + "," +
             yloader::ws2s(x[i->second][0]);
    else
      return std::string();
  }

  std::wstring longName(const std::wstring& shortName) {
    CmdLineInfoBase::const_iterator i = __super::find(yloader::ws2s(shortName));

    if (i != __super::end())
      return x[i->second][1];
    else
      return std::wstring();
  }

  std::string description(const std::wstring& shortName) {
    CmdLineInfoBase::const_iterator i = __super::find(yloader::ws2s(shortName));

    if (i != __super::end())
      return yloader::ws2s(x[i->second][2]);
    else
      return std::string();
  }
};

#define ARGS(x) x##_SHORT, x##_LONG,

TCHAR* CmdLineInfo::x
    []
    [3] = {{ARGS(COMMAND_LINE_HELP) _T( "show this help message")},
           {ARGS(
               AUTO_START) _T( "start downloding automatically at startup (only in GUI version)")},
           {ARGS(
               SINGLE_OUTPUT_FILE) _T( "single output file name, which will contain the data for all symbols. If this argument is not specified, no single file will be generated")},
           {ARGS(DATA_PATH) _T( "data directory")},
           {ARGS(
               CONNECTIONS) _T( "number of simultaneous connections - between 1 and 20")},
           {ARGS(
               END_DATE) _T( "end of date range as \"mm/dd/yyyy\" or a quoted empty string (\"\") for the latest available")},
           {ARGS(
               DATE_FORMAT) _T( "output date format: 0 for m/d/y, 1 for d/m/y, 2 for y/m/d")},
           {ARGS(LOG_FILE) _T( "log file name")},
           {ARGS(
               DATA_FILE_HEADER) _T( "string that will be places at the top of the data file")},
           {ARGS(
               HANDLING_INVALID_DATA) _T( "data error handling mode - 0 to treat as warnings, 1 to treat as errors")},
           {ARGS(
               ADJUST) _T( "dividend and split adjust mode - 0 for adjust off, 1 for adjust on")},
           {ARGS(NO_SINGLE_OUTPUT_FILE) _T(
               "create a single output file containing data for all symbols. "
               "Takes precendence over \"-" SINGLE_OUTPUT_FILE_SHORT "\"")},
           {ARGS(
               ALL_AVAILABLE) _T( "download all available data - 0 to disable and downlod by range, 1 to download all")},
           {ARGS(SYMBOL_COLUMN)
                _T("the column to which the symbol will be added in the data "
                   "file if the option \"-" ADD_SYMBOL_SHORT
                   "\" is also present with argument 1, otherwise will have no "
                   "effect")},
           {ARGS(
               DATA_SOURCE_PLUGIN) _T( "data source - can be the first character(s) of the name of the datasource (case insensitive), for example \"y\" for Yahoo")},
           {ARGS(
               SHOW_SETTINGS) _T( "show current settings, applies to console version only")},
           {ARGS(
               PERIOD) _T( "period - 0 for daily, 1 for weekly, 2 for monthly")},
           {ARGS(CREATE_SUBDIRS) _T( "save data files in nested directories")},
           {ARGS(DATE_SEPARATOR)
                _T("date separator character. To disable the date separator, "
                   "use the option \"-" NO_DATE_SEPARATOR_SHORT "\"")},
           {ARGS(SYMBOLS_FILE) _T( "symbols file name")},
           {ARGS(
               START_DATE) _T( "start of date range as \"mm/dd/yyyy\" or a quoted empty string (\"\") for the oldest available")},
           {ARGS(
               UPDATE) _T( "update mode - 0 for update off, 1 for update on")},
           {ARGS(
               VALIDATE_PRICES) _T( "price validation mode - 0 for validation off, 1 for validation on")},
           {ARGS(
               NO_RELOAD_OLD_DATA) _T( "no reload of old data in Update mode, even if old data is inconsistent with new data")},
           {ARGS(AUTO_EXIT) _T( "exit after downloading (only in GUI version")},
           {ARGS(ADD_SYMBOL)
                _T("add symbol to data file -  0 to not add symbols to the "
                   "data file, 1 to add symbol to data file to column "
                   "specified by option \"-" SYMBOL_COLUMN_SHORT
                   "\" if set, or first column if not")},
           {ARGS(NO_DATE_SEPARATOR) _T(
               "disable date separator - see option \"-" DATE_SEPARATOR_SHORT
               "\"")},
           {ARGS(
               APPEND_TO_DATA_FILE_NAME) _T( "append string to data file name")},
           {ARGS(
               CONFIG_FILE) _T( "get command line options from config file (can be specified with @name)")},
           {ARGS(
               PAD_DATE_FIELDS) _T( "date fields padding - 0 for off, 1 for on")},
           {ARGS(
               LOG_ONLY_ERRORS) _T( "log only errors - 0 to log all errors, 1 to log only errors")},
           {ARGS(FIELD_SEPARATOR) _T( "field separator string")},
           {ARGS(
               REGEX_FLAGS) _T( "regular expression formatting parameters. This argument is optional and not needed in most cases. See the documentation for more details.")},
           {ARGS(VOLUME_MULTIPLIER) _T( "volume multiplier")},
           {ARGS(
               DONT_RUN) _T( "don't start session (only in console version)")},
           {ARGS(
               PREPEND_TO_DATA_FILE_NAME) _T( "prepended string to the data file name")},
           {ARGS(
               PROGRESS) _T( "download progress message frequency - 0 for no progress, positive integer N to show progress every N symbols (only in console version)")},
           {ARGS(
               SORT_BARS_ASCENDING) _T( "sort bars - 0 descending (recent bars at the top), 1 ascending (recent bars at the bottom)")},
           {ARGS(
               APPEND_TO_LOG) _T( "append new log content to the existing log file - 0 to clear log each time, 1 to append to existing log")},
           {ARGS(
               HANDLING_VOLUME_0) _T( "zero volume handling mode - 0 to treat as valid data, 1 to treat as invalid data")},
           {ARGS(EXTENSION) _T( "data file extension")},
           {ARGS(
               ENABLE_REGEX_FORMATTING) _T( "enables regex formatting and disables settings formating")},
           {ARGS(REGEX_MATCH) _T( "match string for regex formatting")},
           {ARGS(REGEX_FORMAT) _T( "format string for regex formatting")},
           {ARGS(
               ERROR_SYMBOLS_FILE_NAME) _T( "error symbols file name - if not set no error symbols file is generated, if set to a valid file name, symbols for which there is no data will be saved to that file")},
           {ARGS(
               APPEND_TO_ERROR_SYMBOLS_LIST) _T( "append vs overwrite for the error symbols list file -  0 to overwrite the existing file, if any, 1 to append new symbols to the existing file if any.")},
           {ARGS(
               IGNORE_ERROR_SYMBOLS_LIST) _T( "ignore mode symbols in error symbols list - 0 to attempt downloading data for all symbols in the current symbols list, 1 to ignore symbols present in the error symbols list.")},
           {ARGS(
               IGNORE_SYMBOLS_LIST) _T( "ignore symbols from the specified file")},
           {ARGS(
               MAP_CHARACTER) _T( "if argument contains two characters, change first character to second character in data file name. If argument contains only on character, remove it from the data file name. See full documentation for more details")},
           {ARGS(
               DO_NOT_CHECK_FOR_UPDATE) _T( "do not check for YLoader update at startup, by default will check if there is an update (only in console version)")}};

const size_t CmdLineInfo::x_size(sizeof(CmdLineInfo::x) /
                                 sizeof(CmdLineInfo::x[0]));

namespace po = boost::program_options;
using namespace std;

CmdLine::CmdLine() : _symbolColumn(0) {}

std::wstring CmdLine::getUsage() const {
  std::wostringstream o;
  o << _T( "Command line usage: " ) << std::endl << _description << std::endl;
  return o.str();
}

void CmdLine::showUsage() {
  _showInfo(getUsage().c_str(), _T( "YLoader command line usage" ));
}

void CmdLine::showError(const std::wstring& error) {
  std::wostringstream o;
  o << error << std::endl << std::endl << getUsage();
  _showInfo(o.str().c_str(), _T( "YLoader command line error" ));
}

// Additional command line parser which interprets '@something' as a
// option "config-file" with the value "something"
pair<std::string, std::string> at_option_parser(std::string const& s) {
  if ('@' == s[0])
    return std::make_pair(std::string(yloader::ws2s(CONFIG_FILE_LONG)),
                          s.substr(1));
  else
    return pair<std::string, std::string>();
}

typedef boost::shared_ptr<yloader::StrVector> StrVectorPtr;

typedef Tokenizer Tok;

std::wstring parse_config_file(const std::wstring& configFile) {
  yloader::t_ifstream ifs(configFile.c_str());
  if (!ifs)
    throw CmdLineException(
        std::wstring(_T( "Could not open the configuration file " ))
        << configFile);

  std::wstringstream ss;

  ss << ifs.rdbuf();

  std::wstring sep(_T( "\n\r"));
  Tok tok(ss.str(), sep);

  std::wstring s;

  for (Tok::const_iterator i = tok.begin(); i != tok.end(); ++i) {
    std::wstring token(yloader::trim(*i));

    if (!token.empty() && token[0] != TCHAR('#')) {
      std::wstring::size_type n = token.find_first_of(TCHAR('='));

      token[n] = TCHAR(' ');
      s += _T( "--" ) + token + _T( " " );
    }
  }

  return s;
}

struct EscapedString : std::string {
 public:
  EscapedString() {}
  EscapedString(const std::string& str) : std::string(str) {}
};

void validate(boost::any& v, const std::vector<std::wstring>& values,
              EscapedString*, int) {
  using namespace boost::program_options;

  // Make sure no previous assignment to 'a' was made.
  validators::check_first_occurrence(v);
  // Extract the first string from 'values'. If there is more than
  // one string, it's an error, and exception will be thrown.
  const std::wstring& s = validators::get_single_string(values);

  v = boost::any(EscapedString(yloader::ws2s(yloader::unescape(s))));
}

std::wstring& getHelp(std::wstring& message) {
  message << _T("\n\nrun the application with the -? or --help argument for ")
             _T("complete information on command line parameters\n");

  return message;
}

bool CmdLine::process() {
  CmdLineInfo cmdLineInfo;
  //	::MessageBox(0,  AUTO_START_LONG.c_str(), 0, 0 );
  po::options_description desc;
  desc.add_options()

      (cmdLineInfo.fullName(COMMAND_LINE_HELP_SHORT).c_str(),
       cmdLineInfo.description(COMMAND_LINE_HELP_SHORT).c_str())(
          cmdLineInfo.fullName(ADD_SYMBOL_SHORT).c_str(), po::value<bool>(),
          cmdLineInfo.description(ADD_SYMBOL_SHORT).c_str())(
          cmdLineInfo.fullName(ADJUST_SHORT).c_str(), po::value<bool>(),
          cmdLineInfo.description(ADJUST_SHORT).c_str())(
          cmdLineInfo.fullName(ALL_AVAILABLE_SHORT).c_str(), po::value<bool>(),
          cmdLineInfo.description(ALL_AVAILABLE_SHORT).c_str())(
          cmdLineInfo.fullName(APPEND_TO_DATA_FILE_NAME_SHORT).c_str(),
          po::value<string>(),
          cmdLineInfo.description(APPEND_TO_DATA_FILE_NAME_SHORT).c_str())(
          cmdLineInfo.fullName(APPEND_TO_LOG_SHORT).c_str(), po::value<bool>(),
          cmdLineInfo.description(APPEND_TO_LOG_SHORT).c_str())(
          cmdLineInfo.fullName(AUTO_EXIT_SHORT).c_str(),
          cmdLineInfo.description(AUTO_EXIT_SHORT).c_str())(
          cmdLineInfo.fullName(AUTO_START_SHORT).c_str(),
          cmdLineInfo.description(AUTO_START_SHORT).c_str())(
          cmdLineInfo.fullName(CONFIG_FILE_SHORT).c_str(),
          po::value<std::vector<string> >(),
          cmdLineInfo.description(CONFIG_FILE_SHORT).c_str())(
          cmdLineInfo.fullName(CONNECTIONS_SHORT).c_str(),
          po::value<unsigned int>(),
          cmdLineInfo.description(CONNECTIONS_SHORT).c_str())(
          cmdLineInfo.fullName(CREATE_SUBDIRS_SHORT).c_str(), po::value<bool>(),
          cmdLineInfo.description(CREATE_SUBDIRS_SHORT).c_str())(
          cmdLineInfo.fullName(DATA_FILE_HEADER_SHORT).c_str(),
          po::value<string>(),
          cmdLineInfo.description(DATA_FILE_HEADER_SHORT).c_str())(
          cmdLineInfo.fullName(DATA_PATH_SHORT).c_str(), po::value<string>(),
          cmdLineInfo.description(DATA_PATH_SHORT).c_str())(
          cmdLineInfo.fullName(DATA_SOURCE_PLUGIN_SHORT).c_str(),
          po::value<string>(),
          cmdLineInfo.description(DATA_SOURCE_PLUGIN_SHORT).c_str())(
          cmdLineInfo.fullName(DATE_FORMAT_SHORT).c_str(),
          po::value<unsigned int>(),
          cmdLineInfo.description(DATE_FORMAT_SHORT).c_str())(
          cmdLineInfo.fullName(DATE_SEPARATOR_SHORT).c_str(),
          po::value<string>(),
          cmdLineInfo.description(DATE_SEPARATOR_SHORT).c_str())(
          cmdLineInfo.fullName(DONT_RUN_SHORT).c_str(),
          cmdLineInfo.description(DONT_RUN_SHORT).c_str())(
          cmdLineInfo.fullName(END_DATE_SHORT).c_str(), po::value<string>(),
          cmdLineInfo.description(END_DATE_SHORT).c_str())(
          cmdLineInfo.fullName(EXTENSION_SHORT).c_str(), po::value<string>(),
          cmdLineInfo.description(EXTENSION_SHORT).c_str())(
          cmdLineInfo.fullName(FIELD_SEPARATOR_SHORT).c_str(),
          po::value<string>(),
          cmdLineInfo.description(FIELD_SEPARATOR_SHORT).c_str())(
          cmdLineInfo.fullName(HANDLING_INVALID_DATA_SHORT).c_str(),
          po::value<unsigned int>(),
          cmdLineInfo.description(HANDLING_INVALID_DATA_SHORT).c_str())(
          cmdLineInfo.fullName(LOG_FILE_SHORT).c_str(), po::value<string>(),
          cmdLineInfo.description(LOG_FILE_SHORT).c_str())(
          cmdLineInfo.fullName(LOG_ONLY_ERRORS_SHORT).c_str(),
          po::value<bool>(),
          cmdLineInfo.description(LOG_ONLY_ERRORS_SHORT).c_str())(
          cmdLineInfo.fullName(NO_DATE_SEPARATOR_SHORT).c_str(),
          cmdLineInfo.description(NO_DATE_SEPARATOR_SHORT).c_str())(
          cmdLineInfo.fullName(ENABLE_REGEX_FORMATTING_SHORT).c_str(),
          po::value<bool>(),
          cmdLineInfo.description(ENABLE_REGEX_FORMATTING_SHORT).c_str())(
          cmdLineInfo.fullName(NO_SINGLE_OUTPUT_FILE_SHORT).c_str(),
          cmdLineInfo.description(NO_SINGLE_OUTPUT_FILE_SHORT).c_str())(
          cmdLineInfo.fullName(NO_RELOAD_OLD_DATA_SHORT).c_str(),
          po::value<bool>(),
          cmdLineInfo.description(NO_RELOAD_OLD_DATA_SHORT).c_str())(
          cmdLineInfo.fullName(SINGLE_OUTPUT_FILE_SHORT).c_str(),
          po::value<string>(),
          cmdLineInfo.description(SINGLE_OUTPUT_FILE_SHORT).c_str())(
          cmdLineInfo.fullName(REGEX_MATCH_SHORT).c_str(), po::value<string>(),
          cmdLineInfo.description(REGEX_MATCH_SHORT).c_str())(
          cmdLineInfo.fullName(REGEX_FLAGS_SHORT).c_str(),
          po::value<unsigned long>(),
          cmdLineInfo.description(REGEX_FLAGS_SHORT).c_str())(
          cmdLineInfo.fullName(REGEX_FORMAT_SHORT).c_str(), po::value<string>(),
          cmdLineInfo.description(REGEX_FORMAT_SHORT).c_str())(
          cmdLineInfo.fullName(PAD_DATE_FIELDS_SHORT).c_str(),
          po::value<bool>(),
          cmdLineInfo.description(PAD_DATE_FIELDS_SHORT).c_str())(
          cmdLineInfo.fullName(PERIOD_SHORT).c_str(), po::value<unsigned int>(),
          cmdLineInfo.description(PERIOD_SHORT).c_str())(
          cmdLineInfo.fullName(PREPEND_TO_DATA_FILE_NAME_SHORT).c_str(),
          po::value<string>(),
          cmdLineInfo.description(PREPEND_TO_DATA_FILE_NAME_SHORT).c_str())(
          cmdLineInfo.fullName(PROGRESS_SHORT).c_str(),
          po::value<unsigned int>(),
          cmdLineInfo.description(PROGRESS_SHORT).c_str())(
          cmdLineInfo.fullName(SHOW_SETTINGS_SHORT).c_str(),
          cmdLineInfo.description(SHOW_SETTINGS_SHORT).c_str())(
          cmdLineInfo.fullName(SORT_BARS_ASCENDING_SHORT).c_str(),
          po::value<bool>(),
          cmdLineInfo.description(SORT_BARS_ASCENDING_SHORT).c_str())(
          cmdLineInfo.fullName(START_DATE_SHORT).c_str(), po::value<string>(),
          cmdLineInfo.description(START_DATE_SHORT).c_str())(
          cmdLineInfo.fullName(SYMBOL_COLUMN_SHORT).c_str(),
          po::value<unsigned int>(),
          cmdLineInfo.description(SYMBOL_COLUMN_SHORT).c_str())(
          cmdLineInfo.fullName(SYMBOLS_FILE_SHORT).c_str(), po::value<string>(),
          cmdLineInfo.description(SYMBOLS_FILE_SHORT).c_str())(
          cmdLineInfo.fullName(UPDATE_SHORT).c_str(), po::value<bool>(),
          cmdLineInfo.description(UPDATE_SHORT).c_str())(
          cmdLineInfo.fullName(VALIDATE_PRICES_SHORT).c_str(),
          po::value<bool>(),
          cmdLineInfo.description(VALIDATE_PRICES_SHORT).c_str())(
          cmdLineInfo.fullName(HANDLING_VOLUME_0_SHORT).c_str(),
          po::value<bool>(),
          cmdLineInfo.description(HANDLING_VOLUME_0_SHORT).c_str())(
          cmdLineInfo.fullName(VOLUME_MULTIPLIER_SHORT).c_str(),
          po::value<double>(),
          cmdLineInfo.description(VOLUME_MULTIPLIER_SHORT).c_str())(
          cmdLineInfo.fullName(ERROR_SYMBOLS_FILE_NAME_SHORT).c_str(),
          po::value<string>(),
          cmdLineInfo.description(ERROR_SYMBOLS_FILE_NAME_SHORT).c_str())(
          cmdLineInfo.fullName(APPEND_TO_ERROR_SYMBOLS_LIST_SHORT).c_str(),
          po::value<bool>(),
          cmdLineInfo.description(APPEND_TO_ERROR_SYMBOLS_LIST_SHORT).c_str())(
          cmdLineInfo.fullName(IGNORE_ERROR_SYMBOLS_LIST_SHORT).c_str(),
          po::value<bool>(),
          cmdLineInfo.description(IGNORE_ERROR_SYMBOLS_LIST_SHORT).c_str())(
          cmdLineInfo.fullName(IGNORE_SYMBOLS_LIST_SHORT).c_str(),
          po::value<string>(),
          cmdLineInfo.description(IGNORE_SYMBOLS_LIST_SHORT).c_str())(
          cmdLineInfo.fullName(MAP_CHARACTER_SHORT).c_str(),
          po::value<std::vector<string> >(),
          cmdLineInfo.description(MAP_CHARACTER_SHORT).c_str())(
          cmdLineInfo.fullName(DO_NOT_CHECK_FOR_UPDATE_SHORT).c_str(),
          po::value<bool>(&_doNotCheckForUpdate)
              ->default_value(DEFAULT_DO_NOT_CHECK_FOR_UPDATE)
              ->implicit_value(true),
          cmdLineInfo.description(DO_NOT_CHECK_FOR_UPDATE_SHORT).c_str())(
          yloader::ws2s(ALTERNATE_UPDATE_HOST).c_str(), po::value<string>())(
          yloader::ws2s(ALTERNATE_UPDATE_DIRECTORY).c_str(),
          po::value<string>());

  try {
    std::stringstream ss;
    ss << desc;

    _description += yloader::s2ws(ss.str());

    LPCTSTR cmdLineStr = ::GetCommandLine();

    std::vector<std::string> args = yloader::cmdLineSplitter(cmdLineStr);

    // remove the executable name
    args.erase(args.begin());

    po::variables_map vm;
    po::store(po::command_line_parser(args)
                  .options(desc)
                  ./*extra_parser( at_option_parser).*/ run(),
              vm);
    po::notify(vm);

    if (vm.count(yloader::ws2s(COMMAND_LINE_HELP_LONG))) {
      showUsage();
      return false;
    } else {
      // now loading the arguments from the configuration file (if any)
      if (vm.count(yloader::ws2s(CONFIG_FILE_LONG)) > 0) {
        std::vector<std::string> configFiles(
            vm[yloader::ws2s(CONFIG_FILE_LONG)]
                .as<std::vector<std::string> >());

        for (std::vector<std::string>::size_type n = 0; n < configFiles.size();
             ++n) {
          std::wstring s(
              parse_config_file(yloader::s2ws(configFiles[n]).c_str()));
          po::store(po::command_line_parser(yloader::cmdLineSplitter(s))
                        .options(desc)
                        .run(),
                    vm);
        }
      }

      if (vm.count(yloader::ws2s(AUTO_START_LONG)) > 0) _autoStart.set(true);
      if (vm.count(yloader::ws2s(AUTO_EXIT_LONG)) > 0) _autoExit.set(true);
      if (vm.count(yloader::ws2s(SYMBOLS_FILE_LONG)) > 0)
        _symbolsFileName.set(
            yloader::s2ws(vm[yloader::ws2s(SYMBOLS_FILE_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(DATA_PATH_LONG)) > 0)
        _dataPath.set(
            yloader::s2ws(vm[yloader::ws2s(DATA_PATH_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(START_DATE_LONG)) > 0)
        _startDate.set(
            yloader::s2ws(vm[yloader::ws2s(START_DATE_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(END_DATE_LONG)) > 0)
        _endDate.set(
            yloader::s2ws(vm[yloader::ws2s(END_DATE_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(ADJUST_LONG)) > 0)
        _adjust.set(vm[yloader::ws2s(ADJUST_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(VALIDATE_PRICES_LONG)) > 0)
        _validatePrices.set(vm[yloader::ws2s(VALIDATE_PRICES_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(ALL_AVAILABLE_LONG)) > 0)
        _allAvailable.set(vm[yloader::ws2s(ALL_AVAILABLE_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(UPDATE_LONG)) > 0)
        _update.set(vm[yloader::ws2s(UPDATE_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(PERIOD_LONG)) > 0)
        _period.set(vm[yloader::ws2s(PERIOD_LONG)].as<unsigned int>());
      if (vm.count(yloader::ws2s(CONNECTIONS_LONG)) > 0)
        _connections.set(
            vm[yloader::ws2s(CONNECTIONS_LONG)].as<unsigned int>());
      if (vm.count(yloader::ws2s(DATE_FORMAT_LONG)) > 0)
        _dateFormat.set(vm[yloader::ws2s(DATE_FORMAT_LONG)].as<unsigned int>());
      if (vm.count(yloader::ws2s(ADD_SYMBOL_LONG)) > 0)
        _addSymbol.set(vm[yloader::ws2s(ADD_SYMBOL_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(SYMBOL_COLUMN_LONG)) > 0)
        _symbolColumn =
            vm[yloader::ws2s(SYMBOL_COLUMN_LONG)].as<unsigned int>();
      if (vm.count(yloader::ws2s(DATE_SEPARATOR_LONG)) > 0)
        _dateSeparator.set(
            yloader::s2ws(vm[yloader::ws2s(DATE_SEPARATOR_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(NO_DATE_SEPARATOR_LONG)) > 0)
        _dateSeparator.set(_T( "" ));
      if (vm.count(yloader::ws2s(REGEX_FLAGS_LONG)) > 0)
        _regexFlags.set(
            vm[yloader::ws2s(REGEX_FLAGS_LONG)].as<unsigned long>());
      if (vm.count(yloader::ws2s(SORT_BARS_ASCENDING_LONG)) > 0)
        _sortBarsAscending.set(
            vm[yloader::ws2s(SORT_BARS_ASCENDING_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(PAD_DATE_FIELDS_LONG)) > 0)
        _padDateFields.set(vm[yloader::ws2s(PAD_DATE_FIELDS_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(DATA_FILE_HEADER_LONG)) > 0)
        _dataFileHeader.set(yloader::s2ws(
            vm[yloader::ws2s(DATA_FILE_HEADER_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(HANDLING_INVALID_DATA_LONG)) > 0)
        _handlingInvalidData.set(
            vm[yloader::ws2s(HANDLING_INVALID_DATA_LONG)].as<unsigned int>());
      if (vm.count(yloader::ws2s(HANDLING_VOLUME_0_LONG)) > 0)
        _handlingVolume0.set(
            vm[yloader::ws2s(HANDLING_VOLUME_0_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(PREPEND_TO_DATA_FILE_NAME_LONG)) > 0)
        _prependToDataFileName.set(yloader::s2ws(
            vm[yloader::ws2s(PREPEND_TO_DATA_FILE_NAME_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(APPEND_TO_DATA_FILE_NAME_LONG)) > 0)
        _appendToDataFileName.set(yloader::s2ws(
            vm[yloader::ws2s(APPEND_TO_DATA_FILE_NAME_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(EXTENSION_LONG)) > 0)
        _extension.set(
            yloader::s2ws(vm[yloader::ws2s(EXTENSION_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(SINGLE_OUTPUT_FILE_LONG)) > 0)
        _outputFile.set(yloader::s2ws(
            vm[yloader::ws2s(SINGLE_OUTPUT_FILE_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(NO_SINGLE_OUTPUT_FILE_LONG)) > 0)
        _outputFile.set(_T( "" ));
      if (vm.count(yloader::ws2s(NO_RELOAD_OLD_DATA_LONG)) > 0)
        _noReloadOldData.set(
            vm[yloader::ws2s(NO_RELOAD_OLD_DATA_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(CREATE_SUBDIRS_LONG)) > 0)
        _createSubdirs.set(vm[yloader::ws2s(CREATE_SUBDIRS_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(ENABLE_REGEX_FORMATTING_LONG)) > 0)
        _enableRegexFormatting.set(
            vm[yloader::ws2s(ENABLE_REGEX_FORMATTING_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(DATA_SOURCE_PLUGIN_LONG)) > 0)
        _dataSourcePlugin.set(yloader::s2ws(
            vm[yloader::ws2s(DATA_SOURCE_PLUGIN_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(SHOW_SETTINGS_LONG)) > 0)
        _showSettings.set(true);
      if (vm.count(yloader::ws2s(DONT_RUN_LONG)) > 0) _dontRun.set(true);
      if (vm.count(yloader::ws2s(FIELD_SEPARATOR_LONG)) > 0)
        _fieldSeparator.set(yloader::s2ws(
            vm[yloader::ws2s(FIELD_SEPARATOR_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(VOLUME_MULTIPLIER_LONG)) > 0)
        _volumeMultiplier.set(
            vm[yloader::ws2s(VOLUME_MULTIPLIER_LONG)].as<double>());
      if (vm.count(yloader::ws2s(LOG_FILE_LONG)) > 0)
        _logFile.set(
            yloader::s2ws(vm[yloader::ws2s(LOG_FILE_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(APPEND_TO_LOG_LONG)) > 0)
        _appendToLog.set(vm[yloader::ws2s(APPEND_TO_LOG_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(LOG_ONLY_ERRORS_LONG)) > 0)
        _logOnlyErrors.set(vm[yloader::ws2s(LOG_ONLY_ERRORS_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(PROGRESS_LONG)) > 0)
        _progress.set(vm[yloader::ws2s(PROGRESS_LONG)].as<unsigned int>());
      if (vm.count(yloader::ws2s(REGEX_MATCH_LONG)) > 0)
        _regexMatch.set(
            yloader::s2ws(vm[yloader::ws2s(REGEX_MATCH_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(REGEX_FORMAT_LONG)) > 0)
        _regexFormat.set(
            yloader::s2ws(vm[yloader::ws2s(REGEX_FORMAT_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(ERROR_SYMBOLS_FILE_NAME_LONG)) > 0)
        _errorSymbolsFileName.set(yloader::s2ws(
            vm[yloader::ws2s(ERROR_SYMBOLS_FILE_NAME_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(APPEND_TO_ERROR_SYMBOLS_LIST_LONG)) > 0)
        _appendToErrorSymbolsList.set(
            vm[yloader::ws2s(APPEND_TO_ERROR_SYMBOLS_LIST_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(IGNORE_ERROR_SYMBOLS_LIST_LONG)) > 0)
        _ignoreErrorSymbolsList.set(
            vm[yloader::ws2s(IGNORE_ERROR_SYMBOLS_LIST_LONG)].as<bool>());
      if (vm.count(yloader::ws2s(IGNORE_SYMBOLS_LIST_LONG)) > 0)
        _ignoreSymbolsList.set(yloader::s2ws(
            vm[yloader::ws2s(IGNORE_SYMBOLS_LIST_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(IGNORE_SYMBOLS_LIST_LONG)) > 0)
        _ignoreSymbolsList.set(yloader::s2ws(
            vm[yloader::ws2s(IGNORE_SYMBOLS_LIST_LONG)].as<string>()));
      if (vm.count(yloader::ws2s(ALTERNATE_UPDATE_HOST)) > 0)
        _alternateUpdateHost.set(yloader::s2ws(
            vm[yloader::ws2s(ALTERNATE_UPDATE_HOST)].as<string>()));
      if (vm.count(yloader::ws2s(ALTERNATE_UPDATE_DIRECTORY)) > 0)
        _alternateUpdateDirectory.set(yloader::s2ws(
            vm[yloader::ws2s(ALTERNATE_UPDATE_DIRECTORY)].as<string>()));
      if (vm.count(yloader::ws2s(MAP_CHARACTER_LONG)) > 0)
        _mapCharacters.set(vm[yloader::ws2s(MAP_CHARACTER_LONG)]
                               .as<std::vector<std::string> >());

      return true;
    }
  } catch (const CmdLineException& e) {
    std::wstring newMessage(yloader::s2ws(e.what()));

    throw CmdLineException(getHelp(newMessage));
  } catch (const std::exception& e) {
    std::wstring newMessage(yloader::s2ws(e.what()));

    throw CmdLineException(getHelp(newMessage));
  }
}

#define SET_STR(name, setName)                       \
  if (cmdLine->##name().isSet()) {                   \
    ##setName(cmdLine->##name().getValue().c_str()); \
  }

#define SET_NON_STR(name, setName)           \
  if (cmdLine->##name().isSet()) {           \
    ##setName(cmdLine->##name().getValue()); \
  }

CmdLinePtr ysession() {
  CmdLinePtr cmdLine(new CmdLine());

  if (cmdLine->process()) {
    _loadSettings(_T( "" ));
    SET_STR(symbolsFileName, _setSymbolsFileName)
    SET_STR(dataPath, _setDataPath)
    SET_STR(startDate, _setStartDate)
    SET_STR(endDate, _setEndDate)
    SET_STR(dateSeparator, _setDateSeparator)
    SET_NON_STR(adjust, _setAdjust)
    SET_NON_STR(validatePrices, _setValidatePrices)
    SET_NON_STR(allAvailable, _setAllAvailable)
    SET_NON_STR(period, _setPeriod)
    SET_NON_STR(connections, _setConnections)
    SET_NON_STR(update, _setUpdate)
    SET_NON_STR(dateFormat, _setDateFormat)
    SET_NON_STR(sortBarsAscending, _setSortBarsAscending)
    SET_NON_STR(padDateFields, _setPadDateFields)
    SET_STR(dataFileHeader, _setDataFileHeader)
    SET_NON_STR(handlingInvalidData, _setInvalidDataHandling)
    SET_NON_STR(handlingVolume0, _setVolume0Handling)
    SET_STR(prependToDataFileName, _setPrependToDataFileName)
    SET_STR(appendToDataFileName, _setAppendToDataFileName)
    SET_STR(extension, _setExtension)
    SET_STR(dataSourcePlugin, _setDataSourcePlugin)
    SET_STR(outputFile, _setOutputFile)
    SET_NON_STR(noReloadOldData, _setNoReloadOldData)
    SET_NON_STR(createSubdirs, _setCreateSubdirs)
    SET_STR(fieldSeparator, _setFieldSeparator)
    SET_NON_STR(volumeMultiplier, _setVolumeMultiplier)
    SET_STR(logFile, _setLogFile)
    SET_NON_STR(appendToLog, _setAppendToLog)
    SET_NON_STR(logOnlyErrors, _setLogOnlyErrors)
    SET_NON_STR(progress, _setProgress)
    SET_STR(regexMatch, _setRegexMatch)
    SET_STR(regexFormat, _setRegexFormat)
    SET_NON_STR(regexFlags, _setRegexFlags)
    SET_NON_STR(enableRegexFormatting, _setEnableRegexFormatting)
    SET_STR(errorSymbolsListFileName, _setErrorSymbolsListFileName)
    SET_NON_STR(appendToErrorSymbolsList, _setAppendToErrorSymbolsList)
    SET_NON_STR(ignoreErrorSymbolsList, _setIgnoreErrorSymbolsList)
    SET_NON_STR(autoStart, _setAutoStart)
    SET_NON_STR(autoExit, _setAutoExit)
    SET_STR(ignoreSymbolsList, _setIgnoreSymbolsList)
    SET_NON_STR(mapCharacters, _setMapCharacters)
    _setUpdater(cmdLine->alternateUpdateHostName(),
                cmdLine->alternateUpdateDirectory());
    if (cmdLine->addSymbol().isSet()) {
      _setAddSymbolToColumn(cmdLine->addSymbol().getValue(),
                            cmdLine->symbolColumn());
    }

    return cmdLine;
  } else
    return CmdLinePtr();
}

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

// TODO
// add:
//  - volume multiplier
//	- log file
// 	- hidden diagnostic log
//  - symbol timeout
//  - autoselect output path

#include "stdafx.h"
#include "CmdLine.h"

namespace po = boost::program_options;
using namespace std;

CmdLine::CmdLine()
  : CmdLine(::GetCommandLine()){
}

CmdLine::CmdLine(const std::wstring& cmdLineStr)
: m_symbolColumn(0){

  std::vector<std::string> args = yloader::cmdLineSplitter(cmdLineStr);

  // remove the executable name
  // assume the exe is the first element only if it doesn't start with "-"
  if (!args.empty() && !args.front().empty() && args.front().front() != '-') {
    args.erase(args.begin());
  }
  process(args);
}

std::wstring CmdLine::getUsage() const {
  std::wostringstream o;
  o << L"Command line usage: " << std::endl << m_description << std::endl;
  return o.str();
}

// Additional command line parser which interprets '@something' as a
// option "config-file" with the value "something"
pair<std::string, std::string> at_option_parser(std::string const& s) {
  if ('@' == s[0])
    return std::make_pair(longName(CONFIG_FILE),s.substr(1));
  else
    return pair<std::string, std::string>();
}

using StrVectorPtr = std::shared_ptr<yloader::StrVector>;

using Tok = Tokenizer;

std::wstring parse_config_file(const std::wstring& configFile) {
  std::wifstream ifs(configFile.c_str());
  if (!ifs) {
    throw CmdLineException(L"Could not open the configuration file "s + configFile);
  }

  std::wstringstream ss;

  ss << ifs.rdbuf();

  std::wstring sep(L"\n\r");
  Tok tok(ss.str(), sep);

  std::wstring s;

  for (Tok::const_iterator i = tok.begin(); i != tok.end(); ++i) {
    std::wstring token(yloader::trim(*i));

    if (!token.empty() && token[0] != L'#') {
      std::wstring::size_type n = token.find_first_of(L'=');

      token[n] = L' ';
      s += L"--" + token + L" ";
    }
  }

  return s;
}

struct EscapedString : std::string {
 public:
  EscapedString() {}
  EscapedString(const std::string& str) : std::string(str) {}
};

void validate(boost::any& v, const std::vector<std::wstring>& values, EscapedString*, int) {
  using namespace boost::program_options;

  // Make sure no previous assignment to 'a' was made.
  validators::check_first_occurrence(v);
  // Extract the first string from 'values'. If there is more than
  // one string, it's an error, and exception will be thrown.
  const std::wstring& s = validators::get_single_string(values);

  v = boost::any(EscapedString(yloader::ws2s(yloader::unescape(s))));
}

std::wstring getHelp(std::wstring& message) {
  return L"\n\nrun the application with the -? or --help argument for complete information on command line parameters\n";
}

class SetOption {
private:
  const po::variables_map& m_vm;

public:
  SetOption(const po::variables_map& vm)
    : m_vm(vm) {
  }

  template< typename T > void set(const LPCWSTR* name, Settable< T >& settable) {
    if (m_vm.count(longName(name))) {
      settable.set(m_vm[longName(name)].as<T>());
    }
  }

  template<> void set< std::wstring >(const LPCWSTR* name, Settable< std::wstring >& settable) {
    if (m_vm.count(longName(name))) {
      settable.set(yloader::s2ws(m_vm[longName(name)].as<std::string>()));
    }
  }

};

void CmdLine::process(std::vector<std::string>& args){
  po::options_description desc;
  desc.add_options()
    (fullName(COMMAND_LINE_HELP).c_str(), po::value<bool>()->implicit_value(true)->default_value(false)->zero_tokens(), description(COMMAND_LINE_HELP).c_str())
    (fullName(ADD_SYMBOL).c_str(), po::value<bool>(), description(ADD_SYMBOL).c_str())
    (fullName(ADJUST).c_str(), po::value<bool>(), description(ADJUST).c_str())
    (fullName(ALL_AVAILABLE).c_str(), po::value<bool>(), description(ALL_AVAILABLE).c_str())
    (fullName(APPEND_TO_DATA_FILE_NAME).c_str(), po::value<string>(), description(APPEND_TO_DATA_FILE_NAME).c_str())
    (fullName(APPEND_TO_LOG).c_str(), po::value<bool>(), description(APPEND_TO_LOG).c_str())
    (fullName(AUTO_EXIT).c_str(), po::value<bool>()->implicit_value(true)->default_value(false)->zero_tokens(), description(AUTO_EXIT).c_str())
    (fullName(AUTO_START).c_str(), po::value<bool>()->implicit_value(true)->zero_tokens(), description(AUTO_START).c_str())
    (fullName(CONFIG_FILE).c_str(), po::value<std::vector<string> >(), description(CONFIG_FILE).c_str())
    (fullName(CONNECTIONS).c_str(), po::value<unsigned int>(), description(CONNECTIONS).c_str())
    (fullName(CREATE_SUBDIRS).c_str(), po::value<bool>(), description(CREATE_SUBDIRS).c_str())
    (fullName(DATA_FILE_HEADER).c_str(), po::value<string>(), description(DATA_FILE_HEADER).c_str())
    (fullName(DATA_PATH).c_str(), po::value<string>(), description(DATA_PATH).c_str())
    (fullName(DATA_SOURCE_PLUGIN).c_str(), po::value<string>(), description(DATA_SOURCE_PLUGIN).c_str())
    (fullName(DATE_FORMAT).c_str(), po::value<unsigned int>(), description(DATE_FORMAT).c_str())
    (fullName(DATE_SEPARATOR).c_str(), po::value<string>(), description(DATE_SEPARATOR).c_str())
    (fullName(DONT_RUN).c_str(), description(DONT_RUN).c_str())
    (fullName(END_DATE).c_str(), po::value<string>(), description(END_DATE).c_str())
    (fullName(EXTENSION).c_str(), po::value<string>(), description(EXTENSION).c_str())
    (fullName(FIELD_SEPARATOR).c_str(), po::value<string>(), description(FIELD_SEPARATOR).c_str())
    (fullName(HANDLING_INVALID_DATA).c_str(), po::value<unsigned int>(), description(HANDLING_INVALID_DATA).c_str())
    (fullName(LOG_FILE).c_str(), po::value<string>(), description(LOG_FILE).c_str())
    (fullName(LOG_ONLY_ERRORS).c_str(), po::value<bool>(), description(LOG_ONLY_ERRORS).c_str())
    (fullName(NO_DATE_SEPARATOR).c_str(), description(NO_DATE_SEPARATOR).c_str())
    (fullName(ENABLE_REGEX_FORMATTING).c_str(), po::value<bool>(), description(ENABLE_REGEX_FORMATTING).c_str())
    (fullName(NO_SINGLE_OUTPUT_FILE).c_str(), description(NO_SINGLE_OUTPUT_FILE).c_str())
    (fullName(NO_RELOAD_OLD_DATA).c_str(), po::value<bool>(), description(NO_RELOAD_OLD_DATA).c_str())
    (fullName(SINGLE_OUTPUT_FILE).c_str(), po::value<string>(), description(SINGLE_OUTPUT_FILE).c_str())
    (fullName(REGEX_MATCH).c_str(), po::value<string>(), description(REGEX_MATCH).c_str())
    (fullName(REGEX_FLAGS).c_str(), po::value<unsigned long>(), description(REGEX_FLAGS).c_str())
    (fullName(REGEX_FORMAT).c_str(), po::value<string>(), description(REGEX_FORMAT).c_str())
    (fullName(PAD_DATE_FIELDS).c_str(), po::value<bool>(), description(PAD_DATE_FIELDS).c_str())
    (fullName(PERIOD).c_str(), po::value<unsigned int>(), description(PERIOD).c_str())
    (fullName(PREPEND_TO_DATA_FILE_NAME).c_str(), po::value<string>(), description(PREPEND_TO_DATA_FILE_NAME).c_str())
    (fullName(PROGRESS).c_str(), po::value<unsigned int>(), description(PROGRESS).c_str())
    (fullName(SHOW_SETTINGS).c_str(), description(SHOW_SETTINGS).c_str())
    (fullName(SORT_BARS_ASCENDING).c_str(), po::value<bool>(), description(SORT_BARS_ASCENDING).c_str())
    (fullName(START_DATE).c_str(), po::value<string>(), description(START_DATE).c_str())
    (fullName(SYMBOL_COLUMN).c_str(), po::value<unsigned int>(), description(SYMBOL_COLUMN).c_str())
    (fullName(SYMBOLS_FILE).c_str(), po::value<string>(), description(SYMBOLS_FILE).c_str())
    (fullName(UPDATE).c_str(), po::value<bool>(), description(UPDATE).c_str())
    (fullName(VALIDATE_PRICES).c_str(), po::value<bool>(), description(VALIDATE_PRICES).c_str())
    (fullName(HANDLING_VOLUME_0).c_str(), po::value<bool>(), description(HANDLING_VOLUME_0).c_str())
    (fullName(VOLUME_MULTIPLIER).c_str(), po::value<double>(), description(VOLUME_MULTIPLIER).c_str())
    (fullName(ERROR_SYMBOLS_FILE_NAME).c_str(), po::value<string>(), description(ERROR_SYMBOLS_FILE_NAME).c_str())
    (fullName(APPEND_TO_ERROR_SYMBOLS_LIST).c_str(), po::value<bool>(), description(APPEND_TO_ERROR_SYMBOLS_LIST).c_str())
    (fullName(IGNORE_ERROR_SYMBOLS_LIST).c_str(), po::value<bool>(), description(IGNORE_ERROR_SYMBOLS_LIST).c_str())
    (fullName(IGNORE_SYMBOLS_LIST).c_str(), po::value<string>(), description(IGNORE_SYMBOLS_LIST).c_str())
    (fullName(MAP_CHARACTER).c_str(), po::value<std::vector<string> >(), description(MAP_CHARACTER).c_str())
    (fullName(DECIMAL_SEPARATOR).c_str(), po::value<string>(), description(DECIMAL_SEPARATOR).c_str())
    (fullName(FIXED_DECIMALS_COUNT).c_str(), po::value<int>(), description(FIXED_DECIMALS_COUNT).c_str())
    (fullName(DO_NOT_CHECK_FOR_UPDATE).c_str(), po::value<bool>()->default_value(DEFAULT_DO_NOT_CHECK_FOR_UPDATE)->implicit_value(true), description(DO_NOT_CHECK_FOR_UPDATE).c_str())
    (fullName(ALTERNATE_UPDATE_HOST).c_str(), po::value<string>())
    (fullName(ALTERNATE_UPDATE_DIRECTORY).c_str(),po::value<string>());

  try {
    std::stringstream ss;
    ss << desc;

    m_description += yloader::s2ws(ss.str());

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(desc).run(), vm);
    po::notify(vm);
    SetOption so(vm);

    // now loading the arguments from the configuration file (if any)
    if (vm.count(longName(CONFIG_FILE)) > 0) {
      std::vector<std::string> configFiles(vm[longName(CONFIG_FILE)].as<std::vector<std::string> >());

      for(const auto& configFile : configFiles) {
        std::wstring s(parse_config_file(yloader::s2ws(configFile.c_str())));
        po::store(po::command_line_parser(yloader::cmdLineSplitter(s)).options(desc).run(), vm);
      }
    }

    so.set(COMMAND_LINE_HELP, m_help);
    so.set(AUTO_START, m_autoStart);
    so.set(AUTO_EXIT, m_autoExit);
    so.set(SYMBOLS_FILE, m_symbolsFileName);
    so.set(DATA_PATH, m_dataPath);
    so.set(START_DATE, m_startDate);
    so.set(END_DATE, m_endDate);
    so.set(ADJUST, m_adjust);
    so.set(VALIDATE_PRICES, m_validatePrices);
    so.set(ALL_AVAILABLE, m_allAvailable);
    so.set(UPDATE, m_update);
    so.set(PERIOD, m_period);
    so.set(CONNECTIONS, m_connections);
    so.set(DATE_FORMAT, m_dateFormat);
    so.set(ADD_SYMBOL, m_addSymbol);
    so.set(SYMBOL_COLUMN, m_symbolColumn);
    so.set(DATE_SEPARATOR, m_dateSeparator);
    if (vm.count(longName(NO_DATE_SEPARATOR)) > 0)
      m_dateSeparator.set(L"");
    so.set(REGEX_FLAGS, m_regexFlags);
    so.set(SORT_BARS_ASCENDING, m_sortBarsAscending);
    so.set(PAD_DATE_FIELDS, m_padDateFields);
    so.set(DATA_FILE_HEADER, m_dataFileHeader);
    so.set(HANDLING_INVALID_DATA, m_handlingInvalidData);
    so.set(HANDLING_VOLUME_0, m_handlingVolume0);
    so.set(PREPEND_TO_DATA_FILE_NAME, m_prependToDataFileName);
    so.set(APPEND_TO_DATA_FILE_NAME, m_appendToDataFileName);
    so.set(EXTENSION, m_extension);
    so.set(SINGLE_OUTPUT_FILE, m_outputFile);
    if (vm.count(longName(NO_SINGLE_OUTPUT_FILE)) > 0)
      m_outputFile.set(L"");
    so.set(NO_RELOAD_OLD_DATA, m_noReloadOldData);
    so.set(CREATE_SUBDIRS, m_createSubdirs);
    so.set(ENABLE_REGEX_FORMATTING, m_enableRegexFormatting);
    so.set(DATA_SOURCE_PLUGIN, m_dataSourcePlugin);
    so.set(SHOW_SETTINGS, m_showSettings);
    so.set(DONT_RUN, m_dontRun);
    so.set(FIELD_SEPARATOR, m_fieldSeparator);
    so.set(VOLUME_MULTIPLIER, m_volumeMultiplier);
    so.set(LOG_FILE, m_logFile);
    so.set(APPEND_TO_LOG, m_appendToLog);
    so.set(LOG_ONLY_ERRORS, m_logOnlyErrors);
    so.set(PROGRESS, m_progress);
    so.set(REGEX_MATCH, m_regexMatch);
    so.set(REGEX_FORMAT, m_regexFormat);
    so.set(ERROR_SYMBOLS_FILE_NAME, m_errorSymbolsFileName);
    so.set(APPEND_TO_ERROR_SYMBOLS_LIST, m_appendToErrorSymbolsList);
    so.set(IGNORE_ERROR_SYMBOLS_LIST, m_ignoreErrorSymbolsList);
    so.set(IGNORE_SYMBOLS_LIST, m_ignoreSymbolsList);
    so.set(ALTERNATE_UPDATE_HOST, m_alternateUpdateHost);
    so.set(ALTERNATE_UPDATE_DIRECTORY, m_alternateUpdateDirectory);
    so.set(MAP_CHARACTER, m_mapCharacters);
    so.set(FIXED_DECIMALS_COUNT, m_fixedDecimalsCount);
    so.set(DECIMAL_SEPARATOR, m_decimalSeparator);
    so.set(DO_NOT_CHECK_FOR_UPDATE, m_doNotCheckForUpdate);
  }
  catch (const CmdLineException& e) {
    std::wstring newMessage(yloader::s2ws(e.what()));

    throw CmdLineException(getHelp(newMessage));
  }
  catch (const std::exception& e) {
    std::wstring newMessage(yloader::s2ws(e.what()));

    throw CmdLineException(getHelp(newMessage));
  }
}


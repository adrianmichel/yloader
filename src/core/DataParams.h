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

#include <datetime.h>
#include <filesymbols.h>
#include <miscfile.h>
#include <boost/make_shared.hpp>
#include <boost/regex.hpp>
#include "charactermapping.h"
#include "defaults.h"
#include "plugin.h"
#include "yahooeventsink.h"
#include "yplugin.h"

inline std::wstring boolToString(bool b) {
  return b ? L"on" : L"off";
}
inline std::wstring quotedString(const std::wstring& str) {
  return L"\""s + str + L"\"";
}

inline yloader::Date oleDateTimeToDate(const COleDateTime& date) {
  COleDateTime::DateTimeStatus status = date.GetStatus();
  return status == COleDateTime::DateTimeStatus::valid ? yloader::Date(date.GetYear(), date.GetMonth(), date.GetDay()) : yloader::Date();
}

class DataFormatSettings {
 private:
  const bool m_addSymbol;
  const unsigned int m_symbolColNumber;
  const yloader::DateFormat m_dateFormat;
  const std::wstring m_dateSeparator;
  const std::wstring m_fieldSeparator;
  const bool m_padDate;
  const int m_fixedDecimalsCount;
  const std::wstring m_decimalSeparator;

 public:
  DataFormatSettings(bool addSymbol, unsigned int symbolColNumber, yloader::DateFormat dateFormat, const std::wstring& dateSeparator,
                     const std::wstring& fieldSeparator, bool padDate, int fixedDecimalCount, const std::wstring& decimalSeparator)
      : m_addSymbol(addSymbol),
        m_symbolColNumber(symbolColNumber),
        m_dateFormat(dateFormat),
        m_dateSeparator(dateSeparator),
        m_padDate(padDate),
        m_fieldSeparator(fieldSeparator),
        m_fixedDecimalsCount(fixedDecimalCount),
        m_decimalSeparator(decimalSeparator) {}

  yloader::DateFormat dateFormat() const { return m_dateFormat; }
  bool addSymbol() const { return m_addSymbol; }
  bool padDate() const { return m_padDate; }
  const std::wstring& dateSeparator() const { return m_dateSeparator; }
  unsigned int symbolColNumber() const { return m_symbolColNumber; }
  const std::wstring& fieldSeparator() const { return m_fieldSeparator; }
  int fixedDecimalsCount() const { return m_fixedDecimalsCount; }
  const std::wstring& decimalSeparator() const { return m_decimalSeparator; }

  inline std::wstring pad0(int n, bool pad) const {
    std::wstring o;
    if (pad && n < 10) {
      o += L"0";
    }
    o += std::to_wstring( n );
    return o;
  }

  // stringForNAD - string for not a date (what should be printed for this case)
  const std::wstring dateToString(const yloader::Date& date, const std::wstring& stringForNAD = std::wstring{}) const {
    if (date.isNotADate()) {
      return stringForNAD;
    }
    std::wostringstream o;

    switch (dateFormat()) {
      case yloader::us:
        o << pad0(date.month(), padDate()) << dateSeparator() << pad0(date.day(), padDate()) << dateSeparator() << date.year();
        break;
      case yloader::european:
        o << pad0(date.day(), padDate()) << dateSeparator() << pad0(date.month(), padDate()) << dateSeparator() << date.year();
        break;
      case yloader::iso:
        o << date.year() << dateSeparator() << pad0(date.month(), padDate()) << dateSeparator() << pad0(date.day(), padDate());
        break;
      default:
        assert(false);
        return L"Unknown date format";
    }

    return o.str();
  }

  std::wstring toString() const {
    std::wostringstream o;
    o << L"Date separator:" << TRIPLE_TAB << quotedString(m_dateSeparator) << std::endl;
    o << L"Date format:" << TRIPLE_TAB << dateFormatToString(m_dateFormat, m_dateSeparator) << std::endl;
    o << L"Pad date:" << TRIPLE_TAB << boolToString(m_padDate) << std::endl;
    o << L"Field separator:" << DOUBLE_TAB << quotedString(m_fieldSeparator)<< std::endl;
    o << L"Add symbol to file:" << DOUBLE_TAB;
    if (m_addSymbol) {
      o << L"column " << m_symbolColNumber;
    }
    else {
      o << L"off";
    }

    o << std::endl;

    return o.str();
  }
};

class RegexDataFormatSettings {
 private:
  bool m_regexFormattingEnabled;
  boost::wregex m_regexMatch;
  const std::wstring m_regexFormat;
  unsigned long m_regexFlags;

 public:
  RegexDataFormatSettings(bool regexFormattingEnabled, const boost::wregex& regexMatch, const std::wstring& regexFormat, unsigned long regexFlags)
      : m_regexFormattingEnabled(regexFormattingEnabled), m_regexMatch(regexMatch),
        m_regexFormat(regexFormat), m_regexFlags(regexFlags) {}

  bool regexFormattingEnabled() const { return m_regexFormattingEnabled; }
  const boost::wregex& regexMatch() const { return m_regexMatch; }
  const std::wstring& regexFormat() const { return m_regexFormat; }
  unsigned long regexFlags() const { return m_regexFlags; }

  std::wstring toString() const {
    std::wostringstream o;

    o << L"Regex formatting:" << DOUBLE_TAB << boolToString(m_regexFormattingEnabled) << std::endl;
    o << L"Match regex:" << TRIPLE_TAB << quotedString(m_regexMatch.str()) << std::endl;
    o << L"Format string:" << TRIPLE_TAB << quotedString(m_regexFormat) << std::endl;
    o << L"Regex flags:" << TRIPLE_TAB << m_regexFlags << std::endl;

    return o.str();
  }
};

class DataParams : public DataFormatSettings, public RegexDataFormatSettings {
 private:
  const bool m_adjust;
  const bool m_allData;
  const std::wstring m_appendToFileName;
  const bool m_autoSelectOutputPath;
  const std::wstring m_autoSelectOutputPathName;
  const CharactersMapping m_cm;
  const bool m_createSubdirs;
  const bool m_dontReloadOldDataInUpdateMode;
  const YDataSourcePluginPtr m_dsp;
  const yloader::Date m_end;
  const std::wstring m_ext;
  const std::wstring m_fileHeader;
  const int m_invalidDataHandling;
  const bool m_lastBarsLast;
  const Period m_period;
  const std::wstring m_prependToFileName;
  const std::wstring m_saveDir;
  const std::wstring m_saveFileAll;
  const yloader::Date m_start;
  const std::wstring m_symbolsFileName;
  const unsigned int m_symbolTimeout;
  const unsigned int m_threads;
  const bool m_update;
  const bool m_validatePrices;
  const bool m_volume0invalid;
  const double m_volumeMultiplier;
  const std::wstring m_logFile;
  bool m_appendToLogFile;
  bool m_logOnlyErrors;
  const std::wstring m_errorSymbolsList;
  bool m_appendToErrorSymbolsList;
  bool m_ignoreErrorSymbolsList;
  const std::wstring m_ignoreSymbolsList;

 public:
  DataParams(
      const std::wstring& errorSymbolsList, bool appendToErrorSymbolsList,
      bool ignoreErrorSymbolsList, const std::wstring& ignoreSymbolsList,
      const YDataSourcePluginPtr dsp, const std::wstring& saveDir,
      Period period, bool adjust, bool allData, bool update,
      const COleDateTime& start, const COleDateTime& end, bool validatePrices,
      bool lastBarsLast, yloader::DateFormat dateFormat, bool addSymbol,
      unsigned int symbolColNumber, const std::wstring& prependToFileName,
      const std::wstring& appendToFileName, bool padDate,
      unsigned int symbolTimeout, bool createSubdirs, int invalidDataHandling,
      bool volume0invalid, unsigned int threads, bool autoSelectOutputPath,
      const std::wstring& autoSelectOutputPathName, const CharactersMapping& cm,
      const std::wstring& fieldSeparator, double volumeMultiplier,
      const std::wstring& fileHeader, const std::wstring& dateSeparator,
      const std::wstring& saveFileAll, const std::wstring& symbolsFileName,
      const std::wstring& ext, bool dontReloadOldDataInUpdateMode,
      const std::wstring& logFile, bool appendToLogFile, bool logOnlyErrors,
      int fixedDecimalsCount, const std::wstring& decimalSeparator,
      bool regex = false, const boost::wregex& regexMatch = boost::wregex(),
      const std::wstring& regexFormat = std::wstring(),
      unsigned long regexFlags = DEFAULT_REGEX_FLAGS)
      : m_saveDir(saveDir),
        m_period(period),
        m_start(oleDateTimeToDate(start)),
        m_end(oleDateTimeToDate(end)),
        m_allData(allData),
        m_adjust(adjust),
        m_update(update),
        m_validatePrices(validatePrices),
        m_lastBarsLast(lastBarsLast),
        m_prependToFileName(prependToFileName),
        m_appendToFileName(appendToFileName),
        m_symbolTimeout(symbolTimeout),
        m_createSubdirs(createSubdirs),
        m_volume0invalid(volume0invalid),
        m_invalidDataHandling(invalidDataHandling),
        m_threads(threads),
        m_autoSelectOutputPath(autoSelectOutputPath),
        m_autoSelectOutputPathName(autoSelectOutputPathName),
        m_cm(cm),
        m_volumeMultiplier(volumeMultiplier),
        m_fileHeader(fileHeader),
        m_dsp(dsp),
        m_saveFileAll(saveFileAll),
        m_symbolsFileName(symbolsFileName),
        m_ext(ext),
        m_dontReloadOldDataInUpdateMode(dontReloadOldDataInUpdateMode),
        m_logFile(logFile),
        m_appendToLogFile(appendToLogFile),
        m_logOnlyErrors(logOnlyErrors),
        DataFormatSettings(addSymbol, symbolColNumber, dateFormat, dateSeparator, fieldSeparator, padDate, fixedDecimalsCount, decimalSeparator),
        RegexDataFormatSettings(regex, regexMatch, regexFormat, regexFlags),
        m_errorSymbolsList(errorSymbolsList),
        m_appendToErrorSymbolsList(appendToErrorSymbolsList),
        m_ignoreErrorSymbolsList(ignoreErrorSymbolsList),
        m_ignoreSymbolsList(ignoreSymbolsList) {}

  yloader::UniqueSymbolsSetPtr getIgnoreSymbols( YahooEventDelegatorPtr yes) const {
    assert(yes);
    yloader::UniqueSymbolsSetPtr symbols(new yloader::UniqueSymbolsSet());

    if (m_ignoreErrorSymbolsList && !m_errorSymbolsList.empty()) {
      try {
        yloader::UniqueSymbolsSetPtr symbols1(yloader::getUniqueSymbols(m_errorSymbolsList));
        if (symbols1) {
          *symbols += *symbols1;
        }
      }
      catch (const yloader::FileSymbolsParserException& e) {
        yes->event(YahooEvent(L"", e.message(), event_warning));
      }
    }

    if (!m_ignoreSymbolsList.empty()) {
      try {
        yloader::UniqueSymbolsSetPtr symbols1(yloader::getUniqueSymbols(m_ignoreSymbolsList));
        if (symbols1) {
          *symbols += *symbols1;
        }
      }
      catch (const yloader::FileSymbolsParserException& e) {
        yes->event(YahooEvent(L"", e.message(), event_warning));
      }
    }

    return symbols;
  }

  const std::wstring& symbolsFileName() const { return m_symbolsFileName; }
  const std::wstring& saveDir() const { return m_saveDir; }
  const Period period() const { return m_period; }
  bool adjust() const { return m_adjust; }
  bool allData() const { return m_allData; }
  bool update() const { return m_update; }
  const yloader::Date& start() const { return m_start; }
  const yloader::Date& end() const { return m_end; }
  bool verify() const { return m_validatePrices; }
  bool lastBarsLast() const { return m_lastBarsLast; }
  const std::wstring& prependToFileName() const { return m_prependToFileName; }
  const std::wstring& appendToFileName() const { return m_appendToFileName; }
  unsigned int symbolTimeout() const { return m_symbolTimeout; }
  bool createSubdirs() const { return m_createSubdirs; }
  bool volume0invalid() const { return m_volume0invalid; }
  int invalidDataHandling() const { return m_invalidDataHandling; }
  unsigned int threads() const { return m_threads; }
  bool autoSelectOutputPath() const { return m_autoSelectOutputPath; }
  const CharactersMapping& charactersMapping() const { return m_cm; }
  const std::wstring& fileHeader() const { return m_fileHeader; }
  double volumeMultiplier() const { return m_volumeMultiplier; }
  const YDataSourcePluginPtr dataSourcePlugin() const { return m_dsp; }
  const std::wstring& saveFileAll() const { return m_saveFileAll; }
  bool dontReloadOldDataInUpdateMode() const {
    return m_dontReloadOldDataInUpdateMode;
  }
  const std::wstring outputPath() const {
    return m_saveDir.empty() ? yloader::addBackSlash(yloader::getFilePath(m_symbolsFileName)) +
      (m_autoSelectOutputPath ? m_autoSelectOutputPathName : L"") : m_saveDir;
  }

  const std::wstring& logFile() const { return m_logFile; }
  bool appendToLogFile() const { return m_appendToLogFile; }
  bool logOnlyErrors() const { return m_logOnlyErrors; }

  // this gets just the file name and extension, without the directory
  // (prefix_symbol_postfix.ext)
  std::wstring getFileName(const std::wstring& symbol) const {
    return yloader::addExtension(prependToFileName() + charactersMapping().map(symbol) + appendToFileName(), m_ext);
  }

  const std::wstring toString() const {
    std::wostringstream o;

    o << L"Data source:" << TRIPLE_TAB << quotedString(m_dsp->name()) << std::endl;
    o << L"Symbols file:" << TRIPLE_TAB << quotedString(m_symbolsFileName) << std::endl;
    o << L"Output path:" << TRIPLE_TAB << quotedString(outputPath()) << std::endl;
    o << L"Auto select output path: " << TAB << boolToString(m_autoSelectOutputPath) << std::endl;

    o << L"Period:" << QUAD_TAB;
    switch (m_period) {
      case daily:
        o << L"daily";
        break;
      case weekly:
        o << "weekly";
        break;
      case monthly:
        o << L"monthly";
        break;
        /*
      case TA_QUARTERLY:
        o << L"quarterly";
        break;
      case TA_YEARLY:
        o << L"yearly";
        break;
        */
      default:
        assert(false);
        break;
    }

    o << std::endl;

    o << L"Connections:" << TRIPLE_TAB << m_threads << std::endl;
    o << L"Adjust:" << QUAD_TAB << boolToString(m_adjust) << std::endl;
    o << L"All data:" << TRIPLE_TAB << boolToString(m_allData) << std::endl;
    o << "Range:" << QUAD_TAB;
    o << ((m_allData || m_start.isNotADate() && m_end.isNotADate()) ? L"all available data" : dateToString(m_start, L"earliest date") + L" - " + dateToString(m_end, L"most recent date")) << std::endl;
    o << L"Update:" << QUAD_TAB << boolToString(m_update) << std::endl;

    o << DataFormatSettings::toString();

    o << L"Validate prices:" << DOUBLE_TAB << boolToString(m_validatePrices)<< std::endl;
    o << L"Volume 0 invalid:" << DOUBLE_TAB << boolToString(m_volume0invalid) << std::endl;
    o << L"Invalid data handling:" << DOUBLE_TAB << (m_invalidDataHandling == 0 ? L"as warning" : L"as error") << std::endl;

    o << L"Sort data:" << TRIPLE_TAB << (m_lastBarsLast ? L"ascending" : L"descending") << std::endl;
    o << L"Volume multiplier:" << DOUBLE_TAB << m_volumeMultiplier << std::endl;

    o << L"Prepend to file name:" << DOUBLE_TAB << (m_prependToFileName.empty() ? L"off": m_prependToFileName) << std::endl;
    o << L"Append to file name:" << DOUBLE_TAB << (m_appendToFileName.empty() ? L"off" : m_appendToFileName) << std::endl;
    o << L"File name extension:" << DOUBLE_TAB << m_ext << std::endl;
    o << L"Create subidrs:" << TRIPLE_TAB << boolToString(m_createSubdirs) << std::endl;
    o << L"Save to single file:" << DOUBLE_TAB << (m_saveFileAll.empty() ? L"off" : m_saveFileAll) << std::endl;
    o << L"Symbol timeout:" << TRIPLE_TAB << m_symbolTimeout << (m_symbolTimeout == 1 ? L" second" : L" seconds") << std::endl;
    o << L"File header: " << TRIPLE_TAB << quotedString(m_fileHeader) << std::endl;
    o << L"Log file: \"" << TRIPLE_TAB << quotedString(m_logFile) << std::endl;
    o << L"Append to log file: " << DOUBLE_TAB << boolToString(m_appendToLogFile) << std::endl;
    o << L"Log only errors:" << DOUBLE_TAB << boolToString(m_logOnlyErrors) << std::endl;
    o << RegexDataFormatSettings::toString();
    o << L"Error symbols list:" << DOUBLE_TAB << quotedString(m_errorSymbolsList) << std::endl;
    o << L"Append to error symbols list:" << TAB << boolToString(m_appendToErrorSymbolsList) << std::endl;
    o << L"Ignore error symbols list:" << TAB << boolToString(m_ignoreErrorSymbolsList) << std::endl;
    o << L"Ignore symbols list:" << DOUBLE_TAB << quotedString(m_ignoreSymbolsList) << std::endl;

    return o.str();
  }
};

using DataParamsPtr = std::shared_ptr<DataParams>;

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

#pragma warning( disable: 4482 )


#include "charactermapping.h"
#include "plugin.h"
#include <miscfile.h>
#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/make_shared.hpp>
#include <datetime.h>
#include "yplugin.h"
#include "defaults.h"
#include <filesymbols.h>
#include "yahooeventsink.h"

#define TAB _T( "\t" )
#define DOUBLE_TAB TAB TAB
#define TRIPLE_TAB DOUBLE_TAB TAB
#define QUAD_TAB TRIPLE_TAB TAB

inline std::wstring boolToString(bool b) {
  return b ? _T( "on" ) : _T( "off" );
}
inline std::wstring quotedString(const std::wstring& str) {
  return std::wstring(_T( "\"")) + str + _T("\"" );
}

inline yloader::Date oleDateTimeToDate(const COleDateTime& date) {
  COleDateTime::DateTimeStatus status = date.GetStatus();
  return status == COleDateTime::DateTimeStatus::valid
             ? yloader::Date(date.GetYear(), date.GetMonth(), date.GetDay())
             : yloader::Date();
}

class DataFormatSettings {
 private:
  const bool _addSymbol;
  const unsigned int _symbolColNumber;
  const yloader::DateFormat _dateFormat;
  const std::wstring _dateSeparator;
  const std::wstring _fieldSeparator;
  const bool _padDate;

 public:
  DataFormatSettings(bool addSymbol, unsigned int symbolColNumber,
                     yloader::DateFormat dateFormat,
                     const std::wstring& dateSeparator,
                     const std::wstring& fieldSeparator, bool padDate)
      : _addSymbol(addSymbol),
        _symbolColNumber(symbolColNumber),
        _dateFormat(dateFormat),
        _dateSeparator(dateSeparator),
        _padDate(padDate),
        _fieldSeparator(fieldSeparator) {}

  yloader::DateFormat dateFormat() const { return _dateFormat; }
  bool addSymbol() const { return _addSymbol; }
  bool padDate() const { return _padDate; }
  const std::wstring& dateSeparator() const { return _dateSeparator; }
  unsigned int symbolColNumber() const { return _symbolColNumber; }
  const std::wstring& fieldSeparator() const { return _fieldSeparator; }

  inline std::wstring pad0(int n, bool pad) const {
    std::wstring o;
    if (pad && n < 10) o << _T( "0" );
    o << n;
    return o;
  }

  // stringForNAD - string for not a date (what should be printed for this case)
  const std::wstring dateToString(
      const yloader::Date& date,
      const std::wstring& stringForNAD = std::wstring()) const {
    if (date.isNotADate()) return stringForNAD;
    std::wostringstream o;

    switch (dateFormat()) {
      case yloader::us:
        o << pad0(date.month(), padDate()) << dateSeparator()
          << pad0(date.day(), padDate()) << dateSeparator() << date.year();
        break;
      case yloader::european:
        o << pad0(date.day(), padDate()) << dateSeparator()
          << pad0(date.month(), padDate()) << dateSeparator() << date.year();
        break;
      case yloader::iso:
        o << date.year() << dateSeparator() << pad0(date.month(), padDate())
          << dateSeparator() << pad0(date.day(), padDate());
        break;
      default:
        assert(false);
        return _T("Unknown date format");
    }

    return o.str();
  }

  std::wstring toString() const {
    std::wostringstream o;
    o << _T( "Date separator:") << TRIPLE_TAB << quotedString(_dateSeparator)
      << std::endl;
    o << _T( "Date format:" ) << TRIPLE_TAB
      << dateFormatToString(_dateFormat, _dateSeparator) << std::endl;
    o << _T( "Pad date:" ) << TRIPLE_TAB << boolToString(_padDate) << std::endl;

    o << _T( "Field separator:" ) << DOUBLE_TAB << quotedString(_fieldSeparator)
      << std::endl;
    o << _T( "Add symbol to file:" ) << DOUBLE_TAB;
    if (_addSymbol)
      o << _T( "column " ) << _symbolColNumber;
    else
      o << _T( "off" );

    o << std::endl;

    return o.str();
  }
};

class RegexDataFormatSettings {
 private:
  bool _regexFormattingEnabled;
  boost::wregex _regexMatch;
  const std::wstring _regexFormat;
  unsigned long _regexFlags;

 public:
  RegexDataFormatSettings(bool regexFormattingEnabled,
                          const boost::wregex& regexMatch,
                          const std::wstring& regexFormat,
                          unsigned long regexFlags)
      : _regexFormattingEnabled(regexFormattingEnabled),
        _regexMatch(regexMatch),
        _regexFormat(regexFormat),
        _regexFlags(regexFlags) {}

  bool regexFormattingEnabled() const { return _regexFormattingEnabled; }
  const boost::wregex& regexMatch() const { return _regexMatch; }
  const std::wstring& regexFormat() const { return _regexFormat; }
  unsigned long regexFlags() const { return _regexFlags; }

  std::wstring toString() const {
    std::wostringstream o;

    o << _T( "Regex formatting:") << DOUBLE_TAB
      << boolToString(_regexFormattingEnabled) << std::endl;
    o << _T( "Match regex:") << TRIPLE_TAB << quotedString(_regexMatch.str())
      << std::endl;
    o << _T( "Format string:") << TRIPLE_TAB << quotedString(_regexFormat)
      << std::endl;
    o << _T( "Regex flags:" ) << TRIPLE_TAB << _regexFlags << std::endl;

    return o.str();
  }
};

class DataParams : public DataFormatSettings, public RegexDataFormatSettings {
 private:
  const bool _adjust;
  const bool _allData;
  const std::wstring _appendToFileName;
  const bool _autoSelectOutputPath;
  const std::wstring _autoSelectOutputPathName;
  const CharactersMapping _cm;
  const bool _createSubdirs;
  const bool _dontReloadOldDataInUpdateMode;
  const YDataSourcePluginPtr _dsp;
  const yloader::Date _end;
  const std::wstring _ext;
  const std::wstring _fileHeader;
  const int _invalidDataHandling;
  const bool _lastBarsLast;
  const Period _period;
  const std::wstring _prependToFileName;
  const std::wstring _saveDir;
  const std::wstring _saveFileAll;
  const yloader::Date _start;
  const std::wstring _symbolsFileName;
  const unsigned int _symbolTimeout;
  const unsigned int _threads;
  const bool _update;
  const bool _validatePrices;
  const bool _volume0invalid;
  const double _volumeMultiplier;
  const std::wstring _logFile;
  bool _appendToLogFile;
  bool _logOnlyErrors;
  const std::wstring _errorSymbolsList;
  bool _appendToErrorSymbolsList;
  bool _ignoreErrorSymbolsList;
  const std::wstring _ignoreSymbolsList;

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
      bool regex = false, const boost::wregex& regexMatch = boost::wregex(),
      const std::wstring& regexFormat = std::wstring(),
      unsigned long regexFlags = DEFAULT_REGEX_FLAGS)
      : _saveDir(saveDir),
        _period(period),
        _start(oleDateTimeToDate(start)),
        _end(oleDateTimeToDate(end)),
        _allData(allData),
        _adjust(adjust),
        _update(update),
        _validatePrices(validatePrices),
        _lastBarsLast(lastBarsLast),
        _prependToFileName(prependToFileName),
        _appendToFileName(appendToFileName),
        _symbolTimeout(symbolTimeout),
        _createSubdirs(createSubdirs),
        _volume0invalid(volume0invalid),
        _invalidDataHandling(invalidDataHandling),
        _threads(threads),
        _autoSelectOutputPath(autoSelectOutputPath),
        _autoSelectOutputPathName(autoSelectOutputPathName),
        _cm(cm),
        _volumeMultiplier(volumeMultiplier),
        _fileHeader(fileHeader),
        _dsp(dsp),
        _saveFileAll(saveFileAll),
        _symbolsFileName(symbolsFileName),
        _ext(ext),
        _dontReloadOldDataInUpdateMode(dontReloadOldDataInUpdateMode),
        _logFile(logFile),
        _appendToLogFile(appendToLogFile),
        _logOnlyErrors(logOnlyErrors),
        DataFormatSettings(addSymbol, symbolColNumber, dateFormat,
                           dateSeparator, fieldSeparator, padDate),
        RegexDataFormatSettings(regex, regexMatch, regexFormat, regexFlags),
        _errorSymbolsList(errorSymbolsList),
        _appendToErrorSymbolsList(appendToErrorSymbolsList),
        _ignoreErrorSymbolsList(ignoreErrorSymbolsList),
        _ignoreSymbolsList(ignoreSymbolsList) {}

  yloader::UniqueSymbolsSetPtr getIgnoreSymbols(
      YahooEventDelegatorPtr yes) const {
    assert(yes);
    yloader::UniqueSymbolsSetPtr symbols(new yloader::UniqueSymbolsSet());

    if (_ignoreErrorSymbolsList && !_errorSymbolsList.empty()) {
      try {
        yloader::UniqueSymbolsSetPtr symbols1(
            yloader::getUniqueSymbols(_errorSymbolsList));
        if (symbols1) *symbols += *symbols1;
      } catch (const yloader::FileSymbolsParserException& e) {
        yes->event(YahooEvent(_T(""), e.message(), event_warning));
      }
    }

    if (!_ignoreSymbolsList.empty()) {
      try {
        yloader::UniqueSymbolsSetPtr symbols1(
            yloader::getUniqueSymbols(_ignoreSymbolsList));
        if (symbols1) *symbols += *symbols1;
      } catch (const yloader::FileSymbolsParserException& e) {
        yes->event(YahooEvent(_T(""), e.message(), event_warning));
      }
    }

    return symbols;
  }

  const std::wstring& symbolsFileName() const { return _symbolsFileName; }
  const std::wstring& saveDir() const { return _saveDir; }
  const Period period() const { return _period; }
  bool adjust() const { return _adjust; }
  bool allData() const { return _allData; }
  bool update() const { return _update; }
  const yloader::Date& start() const { return _start; }
  const yloader::Date& end() const { return _end; }
  bool verify() const { return _validatePrices; }
  bool lastBarsLast() const { return _lastBarsLast; }
  const std::wstring& prependToFileName() const { return _prependToFileName; }
  const std::wstring& appendToFileName() const { return _appendToFileName; }
  unsigned int symbolTimeout() const { return _symbolTimeout; }
  bool createSubdirs() const { return _createSubdirs; }
  bool volume0invalid() const { return _volume0invalid; }
  int invalidDataHandling() const { return _invalidDataHandling; }
  unsigned int threads() const { return _threads; }
  bool autoSelectOutputPath() const { return _autoSelectOutputPath; }
  const CharactersMapping& charactersMapping() const { return _cm; }
  const std::wstring& fileHeader() const { return _fileHeader; }
  double volumeMultiplier() const { return _volumeMultiplier; }
  const YDataSourcePluginPtr dataSourcePlugin() const { return _dsp; }
  const std::wstring& saveFileAll() const { return _saveFileAll; }
  bool dontReloadOldDataInUpdateMode() const {
    return _dontReloadOldDataInUpdateMode;
  }
  const std::wstring outputPath() const {
    return _saveDir.empty()
               ? yloader::addBackSlash(yloader::getFilePath(_symbolsFileName)) +
                     (_autoSelectOutputPath ? _autoSelectOutputPathName
                                            : _T(""))
               : _saveDir;
  }

  const std::wstring& logFile() const { return _logFile; }
  bool appendToLogFile() const { return _appendToLogFile; }
  bool logOnlyErrors() const { return _logOnlyErrors; }

  // this gets just the file name and extension, without the directory
  // (prefix_symbol_postfix.ext)
  std::wstring getFileName(const std::wstring& symbol) const {
    return yloader::addExtension(prependToFileName() +
                                     charactersMapping().map(symbol) +
                                     appendToFileName(),
                                 _ext);
  }

  const std::wstring toString() const {
    std::wostringstream o;

    o << _T( "Data source:") << TRIPLE_TAB << quotedString(_dsp->name())
      << std::endl;
    o << _T( "Symbols file:") << TRIPLE_TAB << quotedString(_symbolsFileName)
      << std::endl;
    o << _T( "Output path:" ) << TRIPLE_TAB << quotedString(outputPath())
      << std::endl;
    o << _T( "Auto select output path: " ) << TAB
      << boolToString(_autoSelectOutputPath) << std::endl;

    o << _T( "Period:" ) << QUAD_TAB;
    switch (_period) {
      case daily:
        o << _T( "daily" );
        break;
      case weekly:
        o << _T( "weekly" );
        break;
      case monthly:
        o << _T( "monthly" );
        break;
        /*
      case TA_QUARTERLY:
        o << _T( "quarterly" );
        break;
      case TA_YEARLY:
        o << _T( "yearly" );
        break;
        */
      default:
        assert(false);
        break;
    }

    o << std::endl;

    o << _T( "Connections:") << TRIPLE_TAB << _threads << std::endl;
    o << _T( "Adjust:" ) << QUAD_TAB << boolToString(_adjust) << std::endl;
    o << _T( "All data:" ) << TRIPLE_TAB << boolToString(_allData) << std::endl;
    o << _T( "Range:" ) << QUAD_TAB;
    o << ((_allData || _start.isNotADate() && _end.isNotADate())
              ? _T( "all available data" )
              : dateToString(_start, _T("earliest date" )) + _T( " - " ) +
                    dateToString(_end, _T("most recent date" )))
      << std::endl;
    o << _T( "Update:" ) << QUAD_TAB << boolToString(_update) << std::endl;

    o << DataFormatSettings::toString();

    o << _T( "Validate prices:" ) << DOUBLE_TAB << boolToString(_validatePrices)
      << std::endl;
    o << _T( "Volume 0 invalid:" ) << DOUBLE_TAB
      << boolToString(_volume0invalid) << std::endl;
    o << _T( "Invalid data handling:") << DOUBLE_TAB
      << (_invalidDataHandling == 0 ? "as warning" : "as error") << std::endl;

    o << _T( "Sort data:" ) << TRIPLE_TAB
      << (_lastBarsLast ? _T( "ascending" ) : _T( "descending" )) << std::endl;
    o << _T( "Volume multiplier:" ) << DOUBLE_TAB << _volumeMultiplier
      << std::endl;

    o << _T( "Prepend to file name:" ) << DOUBLE_TAB
      << (_prependToFileName.empty() ? _T( "off" ) : _prependToFileName)
      << std::endl;
    o << _T( "Append to file name:" ) << DOUBLE_TAB
      << (_appendToFileName.empty() ? _T( "off" ) : _appendToFileName)
      << std::endl;
    o << _T( "File name extension:" ) << DOUBLE_TAB << _ext << std::endl;
    o << _T( "Create subidrs:" ) << TRIPLE_TAB << boolToString(_createSubdirs)
      << std::endl;
    o << _T( "Save to single file:" ) << DOUBLE_TAB
      << (_saveFileAll.empty() ? _T("off") : _saveFileAll) << std::endl;
    o << _T( "Symbol timeout:" ) << TRIPLE_TAB << _symbolTimeout
      << (_symbolTimeout == 1 ? _T( " second" ) : _T( " seconds" ))
      << std::endl;
    o << _T( "File header: " ) << TRIPLE_TAB << quotedString(_fileHeader)
      << std::endl;
    o << _T( "Log file: \"" ) << TRIPLE_TAB << quotedString(_logFile)
      << std::endl;
    o << _T( "Append to log file: ") << DOUBLE_TAB
      << boolToString(_appendToLogFile) << std::endl;
    o << _T( "Log only errors:" ) << DOUBLE_TAB << boolToString(_logOnlyErrors)
      << std::endl;
    o << RegexDataFormatSettings::toString();
    o << _T( "Error symbols list:") << DOUBLE_TAB
      << quotedString(_errorSymbolsList) << std::endl;
    o << _T( "Append to error symbols list:" ) << TAB
      << boolToString(_appendToErrorSymbolsList) << std::endl;
    o << _T( "Ignore error symbols list:" ) << TAB
      << boolToString(_ignoreErrorSymbolsList) << std::endl;
    o << _T( "Ignore symbols list:") << DOUBLE_TAB
      << quotedString(_ignoreSymbolsList) << std::endl;

    return o.str();
  }
};

typedef boost::shared_ptr<DataParams> DataParamsPtr;

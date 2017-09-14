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

#include "dataparams.h"
#include "yplugin.h"

/*
        daily, 1/21/2005 - 6/2/2007
    /table.csv?s=ODFL&a=00&b=21&c=2005&d=05&e=01&f=2007&g=d&q=q&y=0&z=file&x=.csv

        weekly
    /table.csv?s=ODFL&a=00&b=07&c=2005&d=05&e=01&f=2007&g=w&q=q&y=0&z=file&x=.csv
HTTP/1.1

        monthly
    /table.csv?s=ODFL&a=11&b=24&c=2004&d=05&e=01&f=2007&g=m&q=q&y=0&z=file&x=.csv
HTTP/1.1

?        quarterly
    /table.csv?s=ODFL&a=11&b=24&c=2004&d=05&e=01&f=2007&g=d&q=q&y=0&z=file&x=.csv

?        yearly
    /table.csv?s=ODFL&a=11&b=24&c=2004&d=05&e=01&f=2007&g=d&q=q&y=0&z=file&x=.csv

        dividend
    /table.csv?s=msft&a=01&b=01&c=2005&d=11&e=31&f=2007&g=v&ignore=.csv
*/

/*
class Dividend
{
private:
  Date _date;
  double _dividendValue;

public:
  Dividend( const std::wstring& line )
  {
    Tokenizer tok( line, _T( "," ) );

    assert( tok.size() == 2 );

    // format: y/m/d (iso)
    _date = Date( tok[ 0 ], iso, true );
    _dividendValue = _tstof( tok[ 1 ].c_str() );
  }
};

class YahooDividendData : public YahooData, public  std::vector<
boost::shared_ptr< Dividend > >
{
public:
  YahooDividendData( const std::wstring& symbol, const yloader::DateTime& start,
const yloader::DateTime& end )
  {
    CHTTPRequest req( ServerContext( rootURL(), buildRequest(symbol,start, end )
) );
    __super::parse( (std::string)req );
  }

private:

  virtual void parseLine( const std::wstring& line )
  {
    __super::push_back( boost::shared_ptr< Dividend >( new Dividend( line ) ) );
  }

};
*/

class BadDataFormatException {
 private:
  const std::wstring _data;

 public:
  BadDataFormatException(const std::wstring& data) : _data(data) {}

  const std::wstring& getData() const { return _data; }
};

class RegexFormatException {
 private:
  const std::wstring _message;

 public:
  RegexFormatException(const std::wstring& message) : _message(message) {}

  const std::wstring& message() const { return _message; }
};

enum BarError {
  none,
  low_0,
  high_0,
  open_0,
  close_0,
  volume_0,
  open_greater_than_high,
  close_greater_than_high,
  low_greater_than_high,
  open_lower_than_low,
  close_lower_than_low,
  high_lower_than_low
};

class InvalidBarException {
 private:
  yloader::Date _date;
  BarError _error;

 public:
  InvalidBarException(yloader::Date date, BarError error)
      : _date(date), _error(error) {}

  BarError error() const { return _error; }
  yloader::Date date() const { return _date; }

  const std::wstring message() const {
    std::wstring msg;
    switch (_error) {
      case low_0:
        msg = _T("Low is 0");
        break;
      case high_0:
        msg = _T("High is 0");
        break;
      case open_0:
        msg = _T("Open is 0");
        break;
      case close_0:
        msg = _T("Close is 0");
        break;
      case volume_0:
        msg = _T("Volume is 0");
        break;
      case open_greater_than_high:
        msg = _T("open > high");
        break;
      case close_greater_than_high:
        msg = _T("close > high");
        break;
      case low_greater_than_high:
        msg = _T("low > high");
        break;
      case open_lower_than_low:
        msg = _T("open < low");
        break;
      case close_lower_than_low:
        msg = _T("close < low");
        break;
      case high_lower_than_low:
        msg = _T("high < low");
        break;
      default:
        assert(0);
        break;
    }
    msg += _T(" on ") + _date.to_simple_string();
    return msg;
  }
};

class Bar {
 protected:
  yloader::Date _date;
  double _open;
  double _close;
  double _high;
  double _low;
  __int64 _volume;

  void tokenize(std::vector<std::wstring>& tokens, const std::wstring& line,
                const std::wstring& fieldSeparator) {
    size_t crtToken = 0;
    std::wstring::size_type ix = line.find(fieldSeparator);
    for (; ix != line.npos; ix = line.find(fieldSeparator, crtToken)) {
      std::wstring token(line.substr(crtToken, ix - crtToken));
      //      LOG( log_debug, line.substr( crtToken, ix - crtToken ) );
      tokens.push_back(line.substr(crtToken, ix - crtToken));
      crtToken = ix + fieldSeparator.length();
    }
    tokens.push_back(line.substr(crtToken, line.length() - crtToken));
  }

 public:
  Bar() : _open(0), _high(0), _low(0), _close(0), _volume(0) {}
  // builds a bar from data in a csv file
  Bar(const std::wstring& line, const DataFormatSettings& dataFormatSettings)
      : _open(0), _high(0), _low(0), _close(0), _volume(0) {
    assert(!line.empty());

    // TODO - check data format

    std::vector<std::wstring> tokens;
    tokenize(tokens, line, dataFormatSettings.fieldSeparator());

    unsigned m = 0;
    for (unsigned int n = 0; n < tokens.size(); ++n) {
      const std::wstring& str = tokens[n];
      if (!dataFormatSettings.addSymbol() ||
          dataFormatSettings.symbolColNumber() != n) {
        switch (m) {
          case 0:
            // date
            {
              _date = yloader::Date(str, dataFormatSettings.dateFormat(),
                                    dataFormatSettings.dateSeparator());
            }
            break;
          case 1:
            // open
            _open = _tstof(str.c_str());
            break;
          case 2:
            _high = _tstof(str.c_str());
            break;
          case 3:
            _low = _tstof(str.c_str());
            break;
          case 4:
            _close = _tstof(str.c_str());
            break;
          case 5:
            _volume = _ttoi64(str.c_str());
            break;
          default:
            break;
        }
        ++m;
      }
    }
  }

  const yloader::Date& date() const { return _date; }
  double open() const { return _open; }
  double close() const { return _close; }
  double high() const { return _high; }
  double low() const { return _low; }
  __int64 volume() const { return _volume; }

  operator bool() const {
    return !_date.isNotADate() && _open != 0 && _high != 0 && _low != 0 &&
           _close != 0 && volume() != 0;
  }

  static double r2(double d) { return ((double)(__int64(d * 100))) / 100.0; }

  // tests two doubles for equality, if the forth decimal differs by less than
  // 5, then they are equal
  static bool equalx(double v1, double v2) {
    double a1 = round(v1 * 10000);
    double a2 = round(v2 * 10000);
    return a1 - a2 <= 0 && a1 - a2 >= -0;
  }

 public:
  bool operator!=(const Bar& bar) const {
    return _date != bar.date() || !equalx(_open, bar.open()) ||
           !equalx(_high, bar.high()) || !equalx(_low, bar.low()) ||
           !equalx(_close, bar.close()) || _volume != bar.volume();
  }

  std::wstring toString() const {
    return std::wstring() << date().toString() << _T(", o:") << _open
                          << _T(", h:") << _high << _T(", l:") << _low
                          << _T(", c:") << _close << _T(", v:") << _volume;
  }

 private:
  double _ratio;
  BarError _error;

 public:
  bool valid() const { return _error == none; }
  BarError error() const { return _error; }

  yloader::Date getDate(const std::wstring& date, yloader::DateFormat df) {
    yloader::Date d;
    try {
      d = yloader::Date(date, df);
    } catch (...) {
    }

    return d;
  }

  // creates a bar from a line coming from a server
  Bar(const DatasourceBar& bar, bool adjust, bool verify, bool volume0invalid,
      double volumeMultiplier);

  double ratio() const { return _ratio; }
};

typedef boost::shared_ptr<Bar> BarPtr;
typedef std::vector<BarPtr> BarVector;
typedef boost::shared_ptr<BarVector> BarVectorPtr;

class PriceDataException {
 private:
  const std::wstring _message;

 public:
  PriceDataException(const std::wstring& message) : _message(message) {}

  const std::wstring& message() const { return _message; }
};

class PriceData : public BarVector {
 private:
  bool _adjust;
  bool _verify;
  const std::wstring _symbol;
  double _ratio;
  bool _volume0invalid;
  int _invalidDataHandling;
  double _volumeMultiplier;
  std::vector<InvalidBarException> _invalidBars;
  unsigned int _retries;
  const YDataSourcePlugin* _plugin;
  const yloader::DateRange* _dateRange;
  // this indicates the sort direction (Yahoo, Google, MSN are sorted
  // descending, QuoteMedia is sorted ascending)
  bool _ascending;

 private:
  const Bar& bar(size_t i) const {
    //    // the indexes are reversed
    if (i < __super::size())
      return *at(i);
    else
      throw PriceDataException(std::wstring(_T( "Invalid bar index: " )) << i);
  }

 public:
  virtual ~PriceData() {}

  PriceData(const std::wstring& symbol, const yloader::DateRange& dateRange,
            unsigned int retries, const DataParams& dataParams)
      : _adjust(dataParams.adjust()),
        _symbol(symbol),
        _verify(dataParams.verify()),
        _ratio(0),
        _volume0invalid(dataParams.volume0invalid()),
        _invalidDataHandling(dataParams.invalidDataHandling()),
        _retries(retries),
        _volumeMultiplier(dataParams.volumeMultiplier()),
        _plugin(dataParams.dataSourcePlugin().get()),
        _dateRange(&dateRange) {
    assert(_plugin != 0);

    _ascending = _plugin->ascendingSort();

#if false

    static yloader::DateRange range;

    // this is for protection - if this variable is not found, it's not protected
    if (GetEnvVar("YLOADER_DATASOURCE_PATH") == GetEnvVar("YLOADER_PATH"))
    {
      // set a random range
      range = yloader::DateRange(yloader:: LocalTimeSec().date() - yloader::DateDuration(yloader::LocalTimeSec().timeOfDay().seconds() * yloader::LocalTimeSec().date().day() + 5),
                                 yloader:: LocalTimeSec().date() - yloader::DateDuration(yloader::LocalTimeSec().timeOfDay().seconds() + 5));
      _dateRange = &range;
    }
#endif
  }

  /**
   * This compares the bars of the current vector with the
   * bars in the other vector. If they are of different sizes, it
   * compares up to the min size of the two.
   *
   * Takes into account the bar sorting ordering
   *
   * @author adrian (4/30/2012)
   *
   * @param bars
   *
   * @return bool
   */
  bool operator==(BarVectorPtr bars) const {
    assert(bars);

    size_t size(min(bars->size(), size()));

    for (size_t n = 0; n < size; ++n) {
      BarPtr bp1 = _ascending ? *(begin() + n) : *(rbegin() + n);
      BarPtr bp2 = (*bars)[n];

      if (*bp1 != *bp2) return false;
    }
    return true;
  }

  bool operator!=(BarVectorPtr bars) const { return !(*this == bars); }

  void init(Period period) {
    try {
      bool error;
      assert(_dateRange != 0);
      YPluginString result(
          _plugin->getData(_symbol, *_dateRange, period, _adjust, error),
          _plugin);

      if (error) throw PriceDataException((const TCHAR*)result);

      parse(result);

      // if there are more than two bars, check if the last bars has a date that
      // is equal or lower than the previous bar this is to fix a Yahoo change,
      // where downloaded data has two last bars of the same date, with the very
      // last one being unnecessary.
      if (__super::size() >= 2) {
        BarPtr lastBar(last());
        assert(lastBar);
        BarPtr beforeLastBar(getBar(size() - 2));

        if (lastBar->date() <= beforeLastBar->date()) eraseLastBar();
      }
    } catch (const InvalidBarException& e) {
      throw PriceDataException(std::wstring(_T( "Invalid bar: " )) +
                               e.message());
    } catch (const PriceDataException&) {
      throw;
    } catch (const RegexFormatException&) {
      throw;
    } catch (const std::exception& e) {
      LOG(log_error, _T("exception in getData: ") << yloader::s2ws(e.what()))
      throw PriceDataException(yloader::s2ws(e.what()));
    } catch (...) {
      LOG(log_error, _T("Unknow exception in getData"))
      throw PriceDataException(std::wstring(_T("unknown error in getData")));
    }
  }

  unsigned int retries() const { return _retries; }

  void parse(const TCHAR* str) {
    std::wstring line;
    try {
      std::wistringstream is(str);

      while (!is.eof()) {
        std::getline(is, line);
        // ignore the first line (title)
        if (!yloader::trim(line).empty()) {
          DatasourceBar bar;
          if (_plugin->parseLine(line.c_str(), _adjust, &bar)) {
            makeBar(bar);
          } else {
            throw BadDataFormatException(line);
          }
        }
      }
    } catch (const InvalidBarException&) {
      throw;
    } catch (const RegexFormatException&) {
      throw;
    } catch (...) {
      LOG(log_error, _T("unknown exception while parsing line:\n") << line)
      throw PriceDataException(std::wstring(_T("unknown error while parsing: "))
                               << line);
    }
  }

  virtual void makeBar(const DatasourceBar& bar) {
    BarPtr bp = BarPtr(
        new Bar(bar, _adjust, _verify, _volume0invalid, _volumeMultiplier));

    // ignore bars with dates < than start date
    // Google downloads more dates than requested, which causes problems in
    // updated mode
    if (!_dateRange->first.isNotADate() && bp->date() < _dateRange->first) {
      return;
    }

    if (!bp->valid()) {
      switch (_invalidDataHandling) {
        case 0:

          // this is warning
          _invalidBars.push_back(InvalidBarException(bp->date(), bp->error()));
          break;
        case 1:
          // this is error, rethrow
          throw InvalidBarException(bp->date(), bp->error());
          break;
        default:
          // invalid
          assert(false);
          break;
      }
    }

    if (size() == 0) _ratio = bp->ratio();

    assert(bp);

    if (__super::empty() || *__super::back() != *bp)
      __super::push_back(BarVector::value_type(bp));
  }

  const yloader::Date& date(size_t i) {
    assert(!empty());
    return bar(_ascending ? i : count() - i - 1).date();
  }

  unsigned int addSymbol(const DataFormatSettings& dataParams,
                         unsigned int crtCol, yloader::t_ostream& o,
                         bool last = false) const {
    if (dataParams.addSymbol() && crtCol == dataParams.symbolColNumber()) {
      if (last)
        o << dataParams.fieldSeparator() << _symbol;
      else
        o << _symbol << dataParams.fieldSeparator();
      return ++crtCol;
    } else
      return crtCol;
  }

  std::wstring formatValue(const DataFormatSettings& dataParams,
                           double value) const {
    std::wostringstream wos;

    if (dataParams.fixedDecimalsCount() >= 0)
      wos << std::setprecision(dataParams.fixedDecimalsCount()) << std::fixed;
    wos << value;

    std::wstring v(wos.str());
    boost::algorithm::replace_first(v, _T("."), dataParams.decimalSeparator());
    return v;
  }

  yloader::t_ostream& barToString(unsigned int i,
                                  const DataFormatSettings& dataParams,
                                  yloader::t_ostream& o) const {
    const Bar& b(bar(i));

    unsigned int column = 0;
    column = addSymbol(dataParams, column, o);

    o << dataParams.dateToString(b.date()) << dataParams.fieldSeparator();

    column = addSymbol(dataParams, ++column, o);
    o << formatValue(dataParams, b.open()) << dataParams.fieldSeparator();
    column = addSymbol(dataParams, ++column, o);
    o << formatValue(dataParams, b.high()) << dataParams.fieldSeparator();
    column = addSymbol(dataParams, ++column, o);
    o << formatValue(dataParams, b.low()) << dataParams.fieldSeparator();
    column = addSymbol(dataParams, ++column, o);
    o << formatValue(dataParams, b.close()) << dataParams.fieldSeparator();
    column = addSymbol(dataParams, ++column, o);

    TCHAR buf[65];
    o << _i64tot(b.volume(), buf, 10);
    column = addSymbol(dataParams, ++column, o, true);

    return o;
  }

  yloader::t_ostream& barToStringRegex(
      unsigned int i, const DataFormatSettings& dataParams,
      const RegexDataFormatSettings& regexFormatSettings,
      yloader::t_ostream& o) const {
    std::wostringstream os;

    // getting the data ready for regex
    barToString(i, dataParams, os);
    try {
      std::wstring str = boost::regex_replace(
          os.str(), regexFormatSettings.regexMatch(),
          regexFormatSettings.regexFormat(),
          boost::format_all |
              boost::
                  match_default);  //(boost::regex_constants::match_flag_type)regexFormatSettings.regexFlags()
                                   //);
      o << str;
      return o;
    } catch (const std::runtime_error&) {
      // can't format string
      throw RegexFormatException(_T( "Regex data formatting error"));
    }
  }

  // ascending sort indicates the sorting of the downloaded data, not the
  // sorting of the final data
  void toString(yloader::t_ofstream& o, const DataParams& dataParams) const {
    bool asc = !(dataParams.lastBarsLast() ^ _ascending);

    if (dataParams.regexFormattingEnabled()) {
      DataFormatSettings dataFormatSettings(
          true, 0, DEFAULT_DATE_FORMAT, DEFAULT_DATE_SEPARATOR,
          DEFAULT_FIELD_SEPARATOR, DEFAULT_PAD_DATE_FIELDS,
          DEFAULT_FIXED_DECIMALS_COUNT, DEFAULT_DECIMAL_SEPARATOR);
      if (asc) {
        for (size_t i = 0; i < size(); i++) {
          barToStringRegex(i, dataFormatSettings, dataParams, o);
          o << std::endl;
        }
      } else {
        // write in reverse order
        for (int i = size() - 1; i >= 0; i--) {
          barToStringRegex(i, dataFormatSettings, dataParams, o);
          o << std::endl;
        }
      }
    } else {
      if (asc) {
        for (size_t i = 0; i < size(); i++) {
          barToString(i, dataParams, o);
          o << std::endl;
        }
      } else {
        // write in reverse order
        for (int i = size() - 1; i >= 0; i--) {
          barToString(i, dataParams, o);
          o << std::endl;
        }
      }
    }
  }

  bool hasInvalidBars() const { return !_invalidBars.empty(); }
  std::vector<InvalidBarException> invalidBars() const { return _invalidBars; }
  std::wstring invalidBarsToString() const {
    assert(_invalidBars.size() > 0);

    return _invalidBars[0].message();
  }

  void eraseLastBar() {
    _ascending ? __super::erase(__super::end() - 1)
               : __super::erase(__super::begin());
  }

 public:
  unsigned int count() const { return __super::size(); }
  bool empty() const { return __super::empty(); }

  void eraseFirst(size_t n) {
    size_t size = min(n, __super::size());

    if (!empty())
      erase((_ascending ? __super::begin() : __super::end() - size),
            (_ascending ? __super::begin() + size : end()));
  }

  const BarPtr first() const {
    return empty() ? BarPtr() : (*this)[_ascending ? 0 : count() - 1];
  }

  const BarPtr last() const {
    return empty() ? BarPtr() : (*this)[_ascending ? count() - 1 : 0];
  }

  const BarPtr getBar(size_t index) {
    return index >= __super::size()
               ? BarPtr()
               : (*this)[_ascending ? index : count() - index - 1];
  }
};

typedef yloader::ManagedPtr<PriceData> PriceDataPtr;

class SettingNotSupportedException {
 private:
  const std::wstring _message;

 public:
  SettingNotSupportedException(const std::wstring& message)
      : _message(message) {}

  const std::wstring& message() const { return _message; }
};

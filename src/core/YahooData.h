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
    Tokenizer tok( line, L"," ) );

    assert( tok.size() == 2 );

    // format: y/m/d (iso)
    _date = Date( tok[ 0 ], iso, true );
    _dividendValue = _tstof( tok[ 1 ].c_str() );
  }
};

class YahooDividendData : public YahooData, public  std::vector<
std::shared_ptr< Dividend > >
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
    __super::push_back( std::shared_ptr< Dividend >( new Dividend( line ) ) );
  }

};
*/

class BadDataFormatException {
 private:
  const std::wstring m_data;

 public:
  BadDataFormatException(const std::wstring& data) : m_data(data) {}

  const std::wstring& getData() const { return m_data; }
};

class RegexFormatException {
 private:
  const std::wstring m_message;

 public:
  RegexFormatException(const std::wstring& message) : m_message(message) {}

  const std::wstring& message() const { return m_message; }
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
  yloader::Date m_date;
  BarError m_error;

 public:
  InvalidBarException(yloader::Date date, BarError error)
      : m_date(date), m_error(error) {}

  BarError error() const { return m_error; }
  yloader::Date date() const { return m_date; }

  const std::wstring message() const {
    std::wstring msg;
    switch (m_error) {
      case low_0:
        msg = L"Low is 0";
        break;
      case high_0:
        msg = L"High is 0";
        break;
      case open_0:
        msg = L"Open is 0";
        break;
      case close_0:
        msg = L"Close is 0";
        break;
      case volume_0:
        msg = L"Volume is 0";
        break;
      case open_greater_than_high:
        msg = L"open > high";
        break;
      case close_greater_than_high:
        msg = L"close > high";
        break;
      case low_greater_than_high:
        msg = L"low > high";
        break;
      case open_lower_than_low:
        msg = L"open < low";
        break;
      case close_lower_than_low:
        msg = L"close < low";
        break;
      case high_lower_than_low:
        msg = L"high < low";
        break;
      default:
        assert(0);
        break;
    }
    msg += L" on " + m_date.to_simple_string();
    return msg;
  }
};

class Bar {
 protected:
  yloader::Date m_date;
  double m_open;
  double m_close;
  double m_high;
  double m_low;
  __int64 m_volume;

  void tokenize(std::vector<std::wstring>& tokens, const std::wstring& line, const std::wstring& fieldSeparator) {
    size_t crtToken = 0;
    std::wstring::size_type ix = line.find(fieldSeparator);
    for (; ix != line.npos; ix = line.find(fieldSeparator, crtToken)) {
      tokens.push_back(line.substr(crtToken, ix - crtToken));
      crtToken = ix + fieldSeparator.length();
    }
    tokens.push_back(line.substr(crtToken, line.length() - crtToken));
  }

 public:
  Bar() : m_open(0), m_high(0), m_low(0), m_close(0), m_volume(0) {}
  // builds a bar from data in a csv file
  Bar(const std::wstring& line, const DataFormatSettings& dataFormatSettings)
      : m_open(0), m_high(0), m_low(0), m_close(0), m_volume(0) {
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
            m_date = yloader::Date(str, dataFormatSettings.dateFormat(), dataFormatSettings.dateSeparator());
            break;
          case 1:
            // open
            m_open = _tstof(str.c_str());
            break;
          case 2:
            m_high = _tstof(str.c_str());
            break;
          case 3:
            m_low = _tstof(str.c_str());
            break;
          case 4:
            m_close = _tstof(str.c_str());
            break;
          case 5:
            m_volume = _ttoi64(str.c_str());
            break;
          default:
            break;
        }
        ++m;
      }
    }
  }

  const yloader::Date& date() const { return m_date; }
  double open() const { return m_open; }
  double close() const { return m_close; }
  double high() const { return m_high; }
  double low() const { return m_low; }
  __int64 volume() const { return m_volume; }

  operator bool() const {
    return !m_date.isNotADate() && m_open != 0 && m_high != 0 && m_low != 0 && m_close != 0 && volume() != 0;
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
    return m_date != bar.date() || !equalx(m_open, bar.open()) || !equalx(m_high, bar.high()) || !equalx(m_low, bar.low()) || !equalx(m_close, bar.close()) || m_volume != bar.volume();
  }

  std::wstring toString() const {
    std::wostringstream os;
    os << date().toString() << L", o:" << m_open << L", h:" << m_high << L", l:" << m_low << L", c:" << m_close << L", v:" << m_volume;
    return os.str();
  }

 private:
  double m_ratio;
  BarError m_error;

 public:
  bool valid() const { return m_error == none; }
  BarError error() const { return m_error; }

  yloader::Date getDate(const std::wstring& date, yloader::DateFormat df) {
    yloader::Date d;
    try {
      d = yloader::Date(date, df);
    }
    catch (...) {
    }

    return d;
  }

  // creates a bar from a line coming from a server
  Bar(const DatasourceBar& bar, bool adjust, bool verify, bool volume0invalid,
      double volumeMultiplier);

  double ratio() const { return m_ratio; }
};

using BarPtr = std::shared_ptr<Bar>;
using BarVector = std::vector<BarPtr>;
using BarVectorPtr = std::shared_ptr<BarVector>;

class PriceDataException {
 private:
  const std::wstring m_message;

 public:
  PriceDataException(const std::wstring& message) : m_message(message) {}

  const std::wstring& message() const { return m_message; }
};

class PriceData : public BarVector {
 private:
  bool m_adjust;
  bool m_verify;
  const std::wstring m_symbol;
  double m_ratio;
  bool m_volume0invalid;
  int m_invalidDataHandling;
  double m_volumeMultiplier;
  std::vector<InvalidBarException> m_invalidBars;
  unsigned int m_retries;
  const YDataSourcePlugin* m_plugin;
  const yloader::DateRange* m_dateRange;
  // this indicates the sort direction (Yahoo, Google, MSN are sorted
  // descending, QuoteMedia is sorted ascending)
  bool m_ascending;

 private:
  const Bar& bar(size_t i) const {
    //    // the indexes are reversed
    if (i < __super::size()) {
      return *at(i);
    }
    else {
      throw PriceDataException(L"Invalid bar index: "s + std::to_wstring( i ));
    }
  }

 public:
  virtual ~PriceData() {}

  PriceData(const std::wstring& symbol, const yloader::DateRange& dateRange, unsigned int retries, const DataParams& dataParams)
      : m_adjust(dataParams.adjust()),
        m_symbol(symbol),
        m_verify(dataParams.verify()),
        m_ratio(0),
        m_volume0invalid(dataParams.volume0invalid()),
        m_invalidDataHandling(dataParams.invalidDataHandling()),
        m_retries(retries),
        m_volumeMultiplier(dataParams.volumeMultiplier()),
        m_plugin(dataParams.dataSourcePlugin().get()),
        m_dateRange(&dateRange) {
    assert(m_plugin != 0);

    m_ascending = m_plugin->ascendingSort();

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
      BarPtr bp1 = m_ascending ? *(begin() + n) : *(rbegin() + n);
      BarPtr bp2 = (*bars)[n];

      if (*bp1 != *bp2) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(BarVectorPtr bars) const { return !(*this == bars); }

  void init(Period period) {
    try {
      bool error;
      assert(m_dateRange != 0);
      LOG(log_info, L"Calling plugin getData");
      YPluginString result(m_plugin->getData(m_symbol, *m_dateRange, period, m_adjust, error), m_plugin);
      LOG(log_info, L"Plugin getData returned result");

      if (error) {
        LOG(log_error, L"Error signaled by plugin getData, throwing PriceDataException: ", result.to_wstring());

        throw PriceDataException(result.to_wstring());
      }

      LOG(log_info, L"Parsing result");
      parse(result.to_wstring());
      LOG(log_info, L"Successfully parsed result");
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
      LOG(log_info, L"Exiting init");
    }
    catch (const InvalidBarException& e) {
      LOG(log_error, L"InvalidBarException: ", e.message());
      LOG(log_error, L"Re-throwing as PriceDataException");

      throw PriceDataException(L"Invalid bar: "s + e.message());
    }
    catch (const PriceDataException& e) {
      LOG(log_error, L"PriceDataException: ", e.message(), L", re-throwing");
      throw;
    }
    catch (const RegexFormatException& e) {
      LOG(log_error, L"RegexFormatException: ", e.message(), L", re-throwing");
      throw;
    }
    catch (const std::exception& e) {
      LOG(log_error, L"std::exception in getData: ", yloader::s2ws(e.what()), L", re-throwing as PriceDataException");
      throw PriceDataException(yloader::s2ws(e.what()));
    }
    catch (...) {
      LOG(log_error, L"Unknown exception in getData, re-throwing as PriceDataException");
      throw PriceDataException(L"unknown error in getData");
    }
  }

  unsigned int retries() const { return m_retries; }

  void parse(const TCHAR* str) {
    std::wstring line;
    try {
      std::wistringstream is(str);

      while (!is.eof()) {
        std::getline(is, line);
        // ignore the first line (title)
        if (!yloader::trim(line).empty()) {
          DatasourceBar bar;
          if (m_plugin->parseLine(line.c_str(), m_adjust, &bar)) {
            makeBar(bar);
          }
          else {
            LOG(log_error, L"Throwing BadDataFormatException: ", line);

            throw BadDataFormatException(line);
          }
        }
      }
    }
    catch (const InvalidBarException& e) {
      LOG(log_error, L"InvalidBarException: ", e.message(), L", re-throwing");
      throw;
    }
    catch (const RegexFormatException& e) {
      LOG(log_error, L"RegexFormatException: ", e.message(), L", re-throwing");
      throw;
    }
    catch (...) {
      LOG(log_error, L"Unknown exception while parsing line:\n", line);
      LOG(log_error, L"Re-throwing as PriceDataException");
      throw PriceDataException(L"unknown error while parsing: "s + line);
    }
  }

  virtual void makeBar(const DatasourceBar& bar) {
    BarPtr bp = std::make_shared< Bar >(bar, m_adjust, m_verify, m_volume0invalid, m_volumeMultiplier);

    // ignore bars with dates < than start date
    // Google downloads more dates than requested, which causes problems in
    // updated mode
    if (!m_dateRange->first.isNotADate() && bp->date() < m_dateRange->first) {
      return;
    }

    if (!bp->valid()) {
      switch (m_invalidDataHandling) {
        case 0:

          // this is warning
          m_invalidBars.push_back(InvalidBarException(bp->date(), bp->error()));
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

    if (size() == 0) m_ratio = bp->ratio();

    assert(bp);

    if (__super::empty() || *__super::back() != *bp) {
      __super::push_back(BarVector::value_type(bp));
    }
  }

  const yloader::Date& date(size_t i) {
    assert(!empty());
    return bar(m_ascending ? i : count() - i - 1).date();
  }

  unsigned int addSymbol(const DataFormatSettings& dataParams, unsigned int crtCol, std::wostream& o, bool last = false) const {
    if (dataParams.addSymbol() && crtCol == dataParams.symbolColNumber()) {
      if (last) {
        o << dataParams.fieldSeparator() << m_symbol;
      }
      else {
        o << m_symbol << dataParams.fieldSeparator();
      }
      return ++crtCol;
    }
    else {
      return crtCol;
    }
  }

  std::wstring formatValue(const DataFormatSettings& dataParams, double value) const {
    std::wostringstream wos;

    if (dataParams.fixedDecimalsCount() >= 0) {
      wos << std::setprecision(dataParams.fixedDecimalsCount()) << std::fixed;
    }
    wos << value;

    std::wstring v(wos.str());
    boost::algorithm::replace_first(v, L".", dataParams.decimalSeparator());
    return v;
  }

  std::wostream& barToString(size_t i, const DataFormatSettings& dataParams, std::wostream& o) const {
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

    o << std::to_wstring(b.volume());
    column = addSymbol(dataParams, ++column, o, true);

    return o;
  }

  std::wostream& barToStringRegex(size_t i, const DataFormatSettings& dataParams, const RegexDataFormatSettings& regexFormatSettings, std::wostream& o) const {
    std::wostringstream os;

    // getting the data ready for regex
    barToString(i, dataParams, os);
    try {
      std::wstring str = boost::regex_replace(os.str(), regexFormatSettings.regexMatch(), regexFormatSettings.regexFormat(), boost::format_all | boost::match_default);
      o << str;
      return o;
    }
    catch (const std::runtime_error&) {
      // can't format string
      throw RegexFormatException(L"Regex data formatting error");
    }
  }

  // ascending sort indicates the sorting of the downloaded data, not the
  // sorting of the final data
  void toString(std::wofstream& o, const DataParams& dataParams) const {
    LOG(log_info, L"Getting sorting order");
    bool asc = !(dataParams.lastBarsLast() ^ m_ascending);

    LOG(log_info, L"Checking regex enabled status");
    if (dataParams.regexFormattingEnabled()) {
      LOG(log_info, L"Creating DataFormatSettings object");
      DataFormatSettings dataFormatSettings(true, 0, DEFAULT_DATE_FORMAT, DEFAULT_DATE_SEPARATOR,
          DEFAULT_FIELD_SEPARATOR, DEFAULT_PAD_DATE_FIELDS, DEFAULT_FIXED_DECIMALS_COUNT, DEFAULT_DECIMAL_SEPARATOR);
      if (asc) {
        LOG(log_info, L"Writing data in ascending order, regex");
        for (size_t i = 0; i < size(); i++) {
          barToStringRegex(i, dataFormatSettings, dataParams, o);
          o << std::endl;
        }
        LOG(log_info, L"Done writing data in ascending order, regex");
      }
      else {
        LOG(log_info, L"Writing data in descending order, regex");
        // write in reverse order
        for (size_t i = size() - 1; i >= 0; i--) {
          barToStringRegex(i, dataFormatSettings, dataParams, o);
          o << std::endl;
        }
        LOG(log_info, L"Done writing data in descending order, regex");
      }
    }
    else {
      if (asc) {
        LOG(log_info, L"Writing data in ascending order, normal");
        for (size_t i = 0; i < size(); i++) {
          barToString(i, dataParams, o);
          o << std::endl;
        }
        LOG(log_info, L"Done writing data in ascending order, normal");
      }
      else {
        // write in reverse order
        LOG(log_info, L"Writing data in descending order, normal");
        for (size_t i = size() - 1; i >= 0; i--) {
          barToString(i, dataParams, o);
          o << std::endl;
        }
        LOG(log_info, L"Done writing data in descending order, normal");
      }
    }
    LOG(log_info, L"Exiting");
  }

  bool hasInvalidBars() const { return !m_invalidBars.empty(); }
  std::vector<InvalidBarException> invalidBars() const { return m_invalidBars; }
  std::wstring invalidBarsToString() const {
    assert(m_invalidBars.size() > 0);

    return m_invalidBars[0].message();
  }

  void eraseLastBar() {
    m_ascending ? __super::erase(__super::end() - 1) : __super::erase(__super::begin());
  }

 public:
  size_t count() const { return __super::size(); }
  bool empty() const { return __super::empty(); }

  void eraseFirst(size_t n) {
    size_t size = min(n, __super::size());

    if (!empty()) {
      erase((m_ascending ? __super::begin() : __super::end() - size), (m_ascending ? __super::begin() + size : end()));
    }
  }

  const BarPtr first() const {
    return empty() ? BarPtr() : (*this)[m_ascending ? 0 : count() - 1];
  }

  const BarPtr last() const {
    return empty() ? BarPtr() : (*this)[m_ascending ? count() - 1 : 0];
  }

  const BarPtr getBar(size_t index) {
    return index >= __super::size() ? BarPtr() : (*this)[m_ascending ? index : count() - index - 1];
  }
};

using PriceDataPtr = std::shared_ptr<PriceData>;

class SettingNotSupportedException {
 private:
  const std::wstring m_message;

 public:
  SettingNotSupportedException(const std::wstring& message)
      : m_message(message) {}

  const std::wstring& message() const { return m_message; }
};

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


#include "stdafx.h"
#include <misc.h>
#include <tokenizer.h>

yloader::StrPtrMap strings;

bool validated(false);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}

// This is an example of an exported function.
YDSPLUGIN_API const TCHAR* getYDataSourcePluginInfo() {
  //	const char* pluginInfo = "name: Yahoo Data Source\ndescription: Gets
  //free quotes from Yahoo\nmaxconnections: 20\n";
  return 0;
}

YDSPLUGIN_API unsigned int getMaxConnections() { return 20; }
YDSPLUGIN_API bool getCanAdjust() { return true; }

YDSPLUGIN_API bool getCanUnAdjust() { return true; }

YDSPLUGIN_API const TCHAR* getName() { return strings.add(_T( "QuoteMedia" )); }
YDSPLUGIN_API const TCHAR* getDescription() {
  return strings.add(_T( "Get free quotes from QuoteMedia" ));
}

YDSPLUGIN_API const TCHAR* getId() {
  return strings.add(_T( "B5024556-1905-4e49-8081-6183A944794A" ));
}

YDSPLUGIN_API const TCHAR* getError() { return strings.add(_T( "" )); }

YDSPLUGIN_API const TCHAR* getDeveloperName() {
  return strings.add(_T( "yloader.com" ));
}
YDSPLUGIN_API const TCHAR* getDeveloperURL() {
  return strings.add(_T( "http://www.yloader.com" ));
}
YDSPLUGIN_API const TCHAR* getDeveloperEmail() {
  return strings.add(_T( "info@yloader.com" ));
}
YDSPLUGIN_API const TCHAR* getVersion() { return strings.add(WSTRPRODUCTVER); }

YDSPLUGIN_API const TCHAR* getUpdateInfoURL() { return strings.add(_T( "" )); }
YDSPLUGIN_API const TCHAR* getHelpURL() { return strings.add(_T("")); }

YDSPLUGIN_API const TCHAR* getAPIVersion() {
  return strings.add(WSTRPRODUCTVER);
}

YDSPLUGIN_API void showSettingsDialog() {}

#define FIRST_QM_DATE_STR _T( "1/1/1960" )
#define FIRST_QM_DATE yloader::Date(FIRST_QM_DATE_STR)

// http://app.quotemedia.com/quotetools/getHistoryDownload.csv?&webmasterId=501&symbol=QMCI&startDay=28&startMonth=1&startYear=2009&endDay=28&endMonth=2&endYear=2009&isRanged=yes

YDSPLUGIN_API const TCHAR* getFirstDate() {
  return strings.add(FIRST_QM_DATE_STR);
}

class QMData : public yloader::ReqBuilder {
 public:
  QMData() {}
  virtual ~QMData(void) {}

 private:
  static std::wstring buildRequestDatePart(
      const std::wstring& symbol, const yloader::DateRange& dateRange) {
    yloader::Date start =
        dateRange.first.isNotADate() ? FIRST_QM_DATE : dateRange.first;
    yloader::Date end = dateRange.second.isNotADate()
                            ? yloader::LocalTimeSec().date()
                            : dateRange.second;
    std::wstring req =
        std::wstring(_T( "?webmasterId=501"))
        << _T( "&symbol=" ) << urlencode(symbol) << _T( "&startDay=" )
        << (start.day()) << _T( "&startMonth=" ) << (start.month() - 1)
        << _T( "&startYear=" ) << start.year() << _T( "&endDay=" )
        << (end.day()) << _T( "&endMonth=" ) << (end.month() - 1)
        << _T( "&endYear=" ) << end.year() << _T( "&isRanged=true" );

    return req;
  }

 public:
  virtual std::wstring buildRequest(const std::wstring& symbol,
                                    const yloader::DateRange& dateRange,
                                    Period period) const {
    std::wstring req = buildRequestDatePart(symbol, dateRange);
    /*
        switch( period )
        {
        case daily:
          req += _T( "&g=d" );
          break;
        case weekly:
          req += _T( "&g=w" );
          break;
        case monthly:
          req += _T( "&g=m" );
          break;
        default:
          break;
        }

        req += _T( "&q=q&y=0&z=file&x=.csv" );
    */
    return req;
  }

  std::wstring rootURL() const {
    return _T( "app.quotemedia.com/quotetools/getHistoryDownload.csv" );
  }
};

class QuoteMediaHTTPErrorHandler : public yloader::HTTPErrorHandler {
 public:
  virtual yloader::HTTPErrorType handler(unsigned int error) const {
    switch (error) {
      case 400:
        return yloader::no_data_for_symbol_or_range;
      case 404:
        return yloader::no_data_for_symbol;
      case 302:
        return yloader::retry;
      case 500:
        return yloader::retry;
      default:
        return yloader::unknown_http_error;
    }
  }
};

std::string proxyServerAddress;
std::string proxyServerUserName;
std::string proxyServerPassword;
unsigned int httpRequestTimeout = 0;

yloader::StringPtr format(yloader::StringPtr str) {
  bool valid = true;
  size_t sepCount = 0;
  for (size_t i = 0; i < str->size(); ++i) {
    if ((*str)[i] == '\n') {
      valid = true;
      sepCount = 0;
    } else if (valid) {
      if ((*str)[i] == ',') ++sepCount;
      if (sepCount == 6) valid = false;
    }

    if (!valid) (*str)[i] = ' ';
  }

  return str;
}

YDSPLUGIN_API const TCHAR* getData(const TCHAR* symbol, const TCHAR* startDate,
                                   const TCHAR* endDate, Period period,
                                   bool adjusted, bool& error, int retries) {
  if (!validated) return 0;

  QMData qmd;
  QuoteMediaHTTPErrorHandler qmeh;
  yloader::Request req(qmd, qmeh, symbol,
                       yloader::DateRange(startDate, endDate), period, true);

  bool result = req.run();

  yloader::StringPtr str;

  if (!result) {
    str = yloader::StringPtr(new std::wstring(req.error()));
    error = true;
  } else {
    str = yloader::removeLines(*req.response(), 1, _T( "\n" ));

    error = false;
  }

  strings.add(str);
  return str.get()->c_str();
}

YDSPLUGIN_API void freeString(const TCHAR* str) { strings.remove(str); }

YDSPLUGIN_API const TCHAR* getShortName() {
  return strings.add(_T( "QuoteMedia" ));
}

YDSPLUGIN_API const TCHAR* getSupportedPeriods() {
  return strings.add(_T( "0" ));
}

YDSPLUGIN_API void cancelData() { yloader::Request::cancel(); }

YDSPLUGIN_API void setProxyServerAddress(const TCHAR* address) {
  yloader::Request::setProxyServerAddress(address);
}
YDSPLUGIN_API void setProxyServerUserName(const TCHAR* userName) {
  yloader::Request::setProxyServerUserName(userName);
}
YDSPLUGIN_API void setProxyServerPassword(const TCHAR* password) {
  yloader::Request::setProxyServerPassword(password);
}
YDSPLUGIN_API void setHTTPRequestTimeout(unsigned int timeout) {
  yloader::Request::setHTTPRequestTimeout(timeout);
}

YDSPLUGIN_API bool getAscendingSort() { return false; }

#include <crypt.h>

const uint32_t VERSION_KEY[4] = {0x3, 0x1, 0x0, 0x0};

YDSPLUGIN_API bool validation(const TCHAR* str) {
  yloader::Crypt crypt(VERSION_KEY, true, true);

  boost::shared_ptr<std::string> s(crypt.de(yloader::ws2s(str)));

  validated = s && yloader::s2ws(*s) == getVersion();

  return validated;
}

YDSPLUGIN_API int getRetries() { return 0; }

// date, open, high, low, close, volume, changed, changep, adjclose, tradeval,
// tradevol 0     1     2     3    4      5       6        7        8         9
// 10
YDSPLUGIN_API bool parseLine(const TCHAR* line, bool adjusted,
                             DatasourceBar* bar) {
  if (line == 0 || _tcslen(line) == 0) {
    return false;
  }
  Tokenizer tokens(line, _T(","));

  if (tokens.size() < 9) {
    return false;
  }

  _tcscpy(bar->date, tokens[0].c_str());
  bar->open = _tstof(tokens[1].c_str());
  bar->high = _tstof(tokens[2].c_str());
  bar->low = _tstof(tokens[3].c_str());
  bar->close = _tstof(tokens[4].c_str());
  bar->volume = (__int64)(_tstof(tokens[5].c_str()));

  if (adjusted) {
    double adjClose = _tstof(tokens[8].c_str());
    double ratio = bar->close > 0 ? adjClose / bar->close : 1;

    bar->open *= ratio;
    bar->high *= ratio;
    bar->low *= ratio;
    bar->close = adjClose;
    bar->volume /= ratio;
  }

  return true;
}

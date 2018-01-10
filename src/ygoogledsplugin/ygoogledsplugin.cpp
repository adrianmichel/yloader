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

#include "ygoogledsplugin.h"
#include <crypt.h>
#include <misc.h>

using yloader::operator<<;

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

YDSPLUGIN_API const TCHAR* getYDataSourcePluginInfo() {
  //	const char* pluginInfo = "name: Yahoo Data Source\ndescription: Gets
  //free quotes from Yahoo\nmaxconnections: 20\n";
  return 0;
}

YDSPLUGIN_API unsigned int getMaxConnections() { return 20; }
YDSPLUGIN_API bool getCanAdjust() { return true; }

YDSPLUGIN_API bool getCanUnAdjust() { return false; }

YDSPLUGIN_API const TCHAR* getName() { return strings.add(_T( "Google" )); }
YDSPLUGIN_API const TCHAR* getDescription() {
  return strings.add(_T( "Get free quotes from Google Finance" ));
}

YDSPLUGIN_API const TCHAR* getId() {
  return strings.add(_T( "4389DDF8-91D8-4c8b-894C-60E033AE83C7" ));
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

YDSPLUGIN_API const TCHAR* getAPIVersion() {
  return strings.add(WSTRPRODUCTVER);
}

YDSPLUGIN_API const TCHAR* getUpdateInfoURL() { return strings.add(_T( "" )); }
YDSPLUGIN_API const TCHAR* getHelpURL() { return strings.add(_T("")); }

YDSPLUGIN_API void showSettingsDialog() {}

class SettingNotSupportedException {
 private:
  const std::wstring _message;

 public:
  SettingNotSupportedException(const std::wstring& message)
      : _message(message) {}

  const std::wstring& message() const { return _message; }
};

#define FIRST_GOOGLE_DATE_STR _T( "1/2/1970" )
#define FIRST_GOOGLE_DATE yloader::Date(FIRST_GOOGLE_DATE_STR)

YDSPLUGIN_API const TCHAR* getFirstDate() {
  return strings.add(FIRST_GOOGLE_DATE_STR);
}

class GoogleHTTPErrorHandler : public yloader::HTTPErrorHandler {
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

class GoogleData : public yloader::ReqBuilder {
 public:
  GoogleData() {
    /*
if( !adjust )
{
throw SettingNotSupportedException( "Unadjusted data is not supported with
Google data source" );
}
    */
  }
  virtual ~GoogleData(void) {}

 protected:
  static const std::wstring makeDate(const std::wstring& name,
                                     yloader::Date d) {
    static std::wstring months[] = {
        _T( "" ),   _T( "Jan"), _T( "Feb"), _T( "Mar"), _T( "Apr"),
        _T( "May"), _T( "Jun"), _T( "Jul"), _T( "Aug"), _T( "Sep"),
        _T( "Oct"), _T( "Nov"), _T( "Dec")};

    return std::wstring(_T( "&" ))
           << name << _T( "=" ) << months[d.month()] << _T( "+" ) << d.day()
           << _T( "%2C+" ) << d.year();
  }

  virtual std::wstring buildRequestDatePart(
      const std::wstring& symbol, const yloader::DateRange& dateRange) const {
    // daily, all data
    // http://finance.google.com/finance/historical?q=NASDAQ:GOOG&histperiod=daily&output=csv
    // weekly, all data
    // http://finance.google.com/finance/historical?q=NASDAQ:GOOG&histperiod=weekly&output=csv
    // daily, between dates
    // http://finance.google.com/finance/historical?cid=694653&startdate=Sep+15%2C+2006&enddate=Sep+15%2C+2007&output=csv

    yloader::Date s =
        dateRange.first.isNotADate() || dateRange.first.isNegInfinity()
            ? FIRST_GOOGLE_DATE
            : dateRange.first;
    yloader::Date e = dateRange.second;

    std::wstring req = std::wstring(_T( "/finance/historical?q=" ))
                       << urlencode(symbol) << makeDate(_T( "startdate" ), s)
                       << makeDate(_T( "enddate" ), e);

    return req;
  }

 public:
  virtual std::wstring buildRequest(const std::wstring& symbol,
                                    const yloader::DateRange& dateRange,
                                    Period period) const {
    if (!dateRange.first.isNotADate() && !dateRange.first.isNegInfinity() &&
        dateRange.first < FIRST_GOOGLE_DATE) {
      throw SettingNotSupportedException(
          _T( "Dates earlier than January 2, 1970 are not supported with Google data source" ));
    }
    std::wstring req = buildRequestDatePart(symbol, dateRange);

    switch (period) {
      case daily:
        req += _T( "&histperiod=daily" );
        break;
      case weekly:
        req += _T( "&histperiod=weekly" );
        break;
      case monthly:
        throw SettingNotSupportedException(
            _T( "Monthly data not supported with Google data source" ));
        break;
      default:
        break;
    }

    req += _T( "&output=csv" );

    return req;
  }

  std::wstring rootURL() const { return _T( "www.google.com" ); }
};

class GoogleMultiDataLoader : public yloader::MultiDataLoader {
 public:
  GoogleMultiDataLoader()
      : MultiDataLoader(
            yloader::ReqBuilderPtr(new GoogleData()),
            yloader::HTTPErrorHandlerPtr(new GoogleHTTPErrorHandler()), strings,
            250) {}

  virtual yloader::StringPtr preprocess(yloader::StringPtr str) const {
    return yloader::removeLines(*str, 1, _T( "\n" ));
  }
};

YDSPLUGIN_API const TCHAR* getData(const TCHAR* symbol, const TCHAR* startDate,
                                   const TCHAR* endDate, Period period,
                                   bool adjusted, bool& error, int retries) {
  GoogleMultiDataLoader gdl;

  return validated ? gdl.getData(symbol, startDate, endDate, period, adjusted,
                                 error, retries)
                   : 0;
}

YDSPLUGIN_API void freeString(const TCHAR* str) { strings.remove(str); }

YDSPLUGIN_API const TCHAR* getShortName() {
  return strings.add(_T( "Google" ));
}

YDSPLUGIN_API const TCHAR* getSupportedPeriods() {
  return strings.add(_T( "0 1" ));
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

const uint32_t VERSION_KEY[4] = {0x3, 0x1, 0x0, 0x0};

YDSPLUGIN_API bool validation(const TCHAR* str) {
  yloader::Crypt crypt(VERSION_KEY, true, true);

  boost::shared_ptr<std::string> s(crypt.de(yloader::ws2s(str)));

  validated = s && yloader::s2ws(*s) == getVersion();

  return validated;
}

YDSPLUGIN_API int getRetries() { return 0; }

YDSPLUGIN_API bool parseLine(const TCHAR* line, bool adjusted,
                             DatasourceBar* bar) {
  if (line == 0 || _tcslen(line) == 0) {
    return false;
  }
  Tokenizer tokens(line, _T(","));

  if (tokens.size() < 6) {
    return false;
  }

  _tcscpy(bar->date, tokens[0].c_str());
  bar->open = _tstof(tokens[1].c_str());
  bar->high = _tstof(tokens[2].c_str());
  bar->low = _tstof(tokens[3].c_str());
  bar->close = _tstof(tokens[4].c_str());
  bar->volume = (__int64)(_tstof(tokens[5].c_str()));
  return true;
}

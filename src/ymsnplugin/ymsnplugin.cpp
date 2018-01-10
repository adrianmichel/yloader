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
#include "ymsnplugin.h"

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

YDSPLUGIN_API const TCHAR* getName() { return strings.add(_T( "MSN" )); }
YDSPLUGIN_API const TCHAR* getDescription() {
  return strings.add(_T( "Get free quotes from MSN Finance" ));
}

YDSPLUGIN_API const TCHAR* getId() {
  return strings.add(_T( "D6AFAE35-F124-4f15-A0B5-ABFB4BA70621" ));
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
YDSPLUGIN_API const TCHAR* getHelpURL() { return strings.add(_T( "" )); }

YDSPLUGIN_API const TCHAR* getAPIVersion() {
  return strings.add(WSTRPRODUCTVER);
}

YDSPLUGIN_API void showSettingsDialog() {}

#define FIRST_MSN_DATE_STR _T( "1/1/1960" )
#define FIRST_MSN_DATE yloader::Date(FIRST_MSN_DATE_STR)

YDSPLUGIN_API const TCHAR* getFirstDate() {
  return strings.add(FIRST_MSN_DATE_STR);
}
// http://data.moneycentral.msn.com/scripts/chrtsrv.dll?Symbol=IBM&FileDownload=&C1=2&C5=1&C5D=1&C6=2004&C7=1&C7D=1&C8=2008&C9=0

class MSNData : public yloader::ReqBuilder {
 public:
  MSNData() {}
  virtual ~MSNData(void) {}

 private:
  static std::wstring buildRequestDatePart(
      const std::wstring& symbol, const yloader::DateRange& dateRange) {
    yloader::Date start =
        dateRange.first.isNotADate() ? FIRST_MSN_DATE : dateRange.first;
    yloader::Date end = dateRange.second.isNotADate()
                            ? yloader::LocalTimeSec().date()
                            : dateRange.second;

    std::wstring req = std::wstring(_T( "/scripts/chrtsrv.dll?Symbol="))
                       << urlencode(symbol) << _T( "&FileDownload=&C1=2" )
                       << _T( "&C5=" ) << start.month() << _T( "&C5D=" )
                       << start.day() << _T( "&C6=" ) << start.year()
                       << _T( "&C7=" ) << end.month() << _T( "&C7D=" )
                       << end.day() << _T( "&C8=" ) << end.year();

    return req;
  }

 public:
  virtual std::wstring buildRequest(const std::wstring& symbol,
                                    const yloader::DateRange& dateRange,
                                    Period period) const {
    std::wstring req = buildRequestDatePart(symbol, dateRange);

    switch (period) {
      case daily:
        req += _T( "&C9=0" );
        break;
      case weekly:
        req += _T( "&C9=1" );
        break;
      case monthly:
        req += _T( "&C9=2" );
        break;
      default:
        break;
    }

    //    req += _T( "&q=q&y=0&z=file&x=.csv" );

    return req;
  }

  std::wstring rootURL() const { return _T( "data.moneycentral.msn.com" ); }
};

class MSNHTTPErrorHandler : public yloader::HTTPErrorHandler {
 public:
  virtual yloader::HTTPErrorType handler(unsigned int error) const {
    switch (error) {
      case 503:
        return yloader::retry;
      default:
        return yloader::unknown_http_error;
    }
  }
};

class MSNResultHandler : public yloader::ResultHandler {
 public:
  virtual std::wstring handler(const yloader::StringPtr result) const {
    if (!result->empty() &&
        result->substr(0, std::wstring(_T( "Symbol" )).length()) ==
            _T( "Symbol" )) {
      return _T( "Symbol not found" );
    } else
      return _T( "" );
  }
};

class MSNMultiDataLoader : public yloader::MultiDataLoader {
 public:
  MSNMultiDataLoader()
      : MultiDataLoader(yloader::ReqBuilderPtr(new MSNData()),
                        yloader::HTTPErrorHandlerPtr(new MSNHTTPErrorHandler()),
                        strings, 1000,
                        yloader::ResultHandlerPtr(new MSNResultHandler())) {}

  virtual yloader::StringPtr preprocess(yloader::StringPtr str) const {
    return yloader::removeLines(*str, 6, _T( "\r\n" ));
  }
};

YDSPLUGIN_API const TCHAR* getData(const TCHAR* symbol, const TCHAR* startDate,
                                   const TCHAR* endDate, Period period,
                                   bool adjusted, bool& error, int retries) {
  if (!validated) return 0;

  MSNMultiDataLoader msndl;

  return msndl.getData(symbol, startDate, endDate, period, adjusted, error,
                       retries);
}

YDSPLUGIN_API void freeString(const TCHAR* str) { strings.remove(str); }

YDSPLUGIN_API const TCHAR* getShortName() { return strings.add(_T( "MSN" )); }

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

#include <crypt.h>

const uint32_t VERSION_KEY[4] = {0x3, 0x1, 0x0, 0x0};

YDSPLUGIN_API bool validation(const TCHAR* str) {
  yloader::Crypt crypt(VERSION_KEY, true, true);

  boost::shared_ptr<std::string> s(crypt.de(yloader::ws2s(str)));

  validated = s && yloader::s2ws(*s) == getVersion();

  return validated;
}

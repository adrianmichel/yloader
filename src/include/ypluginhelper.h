/*
Copyright(C) 2017  YLoader.com

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <list>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>
#include "datetime.h"
#include "httprequest.h"
#include "strings.h"

#ifdef YDSPLUGINHELPER_EXPORTS
#define YDSPLUGINHELPER_API __declspec(dllexport)
#else
#define YDSPLUGINHELPER_API __declspec(dllimport)
#endif

namespace yloader {

typedef public std::pair<yloader::Date, yloader::Date> DateRangeBase;

class YDSPLUGINHELPER_API DateRange : public DateRangeBase {
 public:
  DateRange() {}
  DateRange(const std::wstring& d1, const std::wstring& d2);
  DateRange(const yloader::Date& d1, const yloader::Date& d2);
  std::wstring toString() const;
  yloader::DateDuration duration();
};

typedef std::vector<DateRange> DateRangesBase;

class DateRanges : public DateRangesBase {
 public:
  DateRanges(DateRange range, unsigned int maxUnits = 1000,
             unsigned int unit = 1);
  std::wstring toString() const;
};

YDSPLUGINHELPER_API StringPtr removeLines(const std::wstring& str,
                                          unsigned int count,
                                          const std::wstring& eol);

class Requests {
 private:
  typedef std::list<HTTPRequestBasePtr> RequestList;
  RequestList m_requests;

  yloader::Mutex _mx;

 public:
  HTTPRequestBasePtr getRequest(const std::wstring& serverName, bool http);
  void cancel();
};

class ReqBuilder {
 public:
  virtual ~ReqBuilder() {}
  virtual std::wstring buildRequest(const std::wstring& symbol,
                                    const yloader::DateRange& dateRange,
                                    Period period) const = 0;
  virtual std::wstring rootURL() const = 0;
  virtual bool hasCookie() const { return false; }
  virtual std::wstring cookie(const std::wstring& symbol) const {
    return _T("");
  }
};

typedef yloader::ManagedPtr<ReqBuilder> ReqBuilderPtr;

enum HTTPErrorType {
  no_data_for_symbol,
  no_data_for_range,
  no_data_for_symbol_or_range,
  retry,
  unknown_http_error
};

class HTTPErrorHandler {
 public:
  virtual ~HTTPErrorHandler() {}

  virtual HTTPErrorType handler(unsigned int error) const = 0;
};

typedef yloader::ManagedPtr<HTTPErrorHandler> HTTPErrorHandlerPtr;

class ResultHandler {
 public:
  virtual ~ResultHandler() {}

  virtual std::wstring handler(const StringPtr result) const {
    return _T( "" );
  }
};

typedef yloader::ManagedPtr<ResultHandler> ResultHandlerPtr;

class YDSPLUGINHELPER_API Request {
 private:
  const ReqBuilder& _reqBuilder;
  const HTTPErrorHandler& _httpErrorHandler;
  const ResultHandlerPtr _resultHandler;
  const std::wstring _symbol;
  const DateRange _dateRange;
  const Period _period;
  std::wstring _error;
  static Requests m_requests;
  bool _canceled;
  StringPtr _response;
  bool _http;

  static std::wstring m_proxyServerAddress;
  static std::wstring m_proxyServerUserName;
  static std::wstring m_proxyServerPassword;
  static unsigned int m_httpRequestTimeout;

  bool processCurlError(const InternetException& e);

 public:
  void static setProxyServerAddress(const TCHAR* proxyServerAddress) {
    m_proxyServerAddress = proxyServerAddress;
  }
  void static setProxyServerUserName(const TCHAR* proxyServerUserName) {
    m_proxyServerUserName = proxyServerUserName;
  }
  void static setProxyServerPassword(const TCHAR* proxyServerPassword) {
    m_proxyServerPassword = proxyServerPassword;
  }
  void static setHTTPRequestTimeout(unsigned int httpRequestTimeout) {
    m_httpRequestTimeout = httpRequestTimeout;
  }

  StringPtr response() const { return _response; }

  Request(const ReqBuilder& reqBuilder,
          const HTTPErrorHandler& httpErrorHandler, const std::wstring& symbol,
          const DateRange& dateRange, Period period, bool http,
          ResultHandlerPtr resultHandler = ResultHandlerPtr());

  bool handleInternetException(const InternetException& e);

  bool run();
  const std::wstring& error() const { return _error; }

  ~Request() {}

  static void cancel();

  bool canceled() const;
};

class StrPtrMap : private std::map<const TCHAR*, StringPtr> {
 private:
  yloader::Mutex _mx;

 public:
  void add(StringPtr str) {
    yloader::Lock lock(_mx);

    __super::insert(__super::value_type(str.get()->c_str(), str));
  }

  const TCHAR* add(const TCHAR* str) {
    StringPtr s(new std::wstring(str));
    add(s);
    return s->c_str();
  }

  void remove(const TCHAR* str) {
    yloader::Lock lock(_mx);
    assert(__super::find(str) != __super::end());
    __super::erase(str);
  }
};

class YDSPLUGINHELPER_API MultiDataLoader {
 private:
  StrPtrMap& _strings;
  const ReqBuilderPtr _reqBuilder;
  const HTTPErrorHandlerPtr _httpErrorHandler;
  const ResultHandlerPtr _resultHandler;
  const unsigned int _width;

 private:
  StringPtr stitchStr(yloader::StringPtrVector& v);

 public:
  // width: the max number of days in a range, when creating the date ranges.
  MultiDataLoader(ReqBuilderPtr reqBuilder,
                  HTTPErrorHandlerPtr httpErrorHandler, StrPtrMap& strings,
                  unsigned int width,
                  ResultHandlerPtr resultHandler = ResultHandlerPtr());
  ~MultiDataLoader();

  virtual StringPtr preprocess(StringPtr str) const = 0;

  const TCHAR* getData(const TCHAR* symbol, const TCHAR* startDate,
                       const TCHAR* endDate, Period period, bool adjusted,
                       bool& error, int retries);
};

}  // namespace yloader

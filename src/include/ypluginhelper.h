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
#include <atomic>

#include "datetime.h"
#include "httprequest.h"
#include "strings.h"
#include <objcounter.h>

#define DSPLUGINHELPER_API

namespace yloader {

using DateRangeBase = std::pair<yloader::Date, yloader::Date>;

class DSPLUGINHELPER_API DateRange : public DateRangeBase {
 public:
  DateRange() {}
  DateRange(const std::wstring& d1, const std::wstring& d2);
  DateRange(const yloader::Date& d1, const yloader::Date& d2);
  std::wstring toString() const;
  yloader::DateDuration duration();
};

using DateRangesBase = std::vector<DateRange>;

class DateRanges : public DateRangesBase {
 public:
  DateRanges(DateRange range, unsigned int maxUnits = 1000, unsigned int unit = 1);
  std::wstring toString() const;
};

DSPLUGINHELPER_API StringPtr removeLines(const std::wstring& str, unsigned int count, const std::wstring& eol);

class Requests {
 private:
  using RequestMap = std::map<uint64_t, HTTPRequestBasePtr>;
  RequestMap m_requests;

  std::mutex m_mx;

 public:
  HTTPRequestBasePtr getRequest(uint64_t id, const std::wstring& serverName, bool http);
  void cancel();
  void remove(uint64_t id) {
    std::scoped_lock(m_mx);
    m_requests.erase(id);
  }
};

class ReqBuilder {
 public:
  virtual ~ReqBuilder() {}
  virtual std::wstring buildRequest(const std::wstring& symbol, const yloader::DateRange& dateRange, Period period) const = 0;
  virtual std::wstring rootURL() const = 0;
  virtual bool hasCookie() const { return false; }
  virtual std::wstring cookie(const std::wstring& symbol) const {
    return L"";
  }
};

using ReqBuilderPtr = std::shared_ptr<ReqBuilder>;

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

using HTTPErrorHandlerPtr = std::shared_ptr<HTTPErrorHandler>;

class ResultHandler {
 public:
  virtual ~ResultHandler() {}

  virtual std::wstring handler(const StringPtr result) const {
    return L"";
  }
};

using ResultHandlerPtr = std::shared_ptr<ResultHandler>;

class DSPLUGINHELPER_API Request {
  OBJ_COUNTER(Request)
private:
  static std::atomic_uint64_t m_crtId;
  const uint64_t m_id;
  const ReqBuilder& m_reqBuilder;
  const HTTPErrorHandler& m_httpErrorHandler;
  const ResultHandlerPtr m_resultHandler;
  const std::wstring m_symbol;
  const DateRange m_dateRange;
  const Period m_period;
  std::wstring m_error;
  static Requests m_requests;
  bool m_canceled;
  StringPtr m_response;
  bool m_http;

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

  StringPtr response() const { return m_response; }

  Request(const ReqBuilder& reqBuilder, const HTTPErrorHandler& httpErrorHandler, const std::wstring& symbol,
          const DateRange& dateRange, Period period, bool http, ResultHandlerPtr resultHandler = ResultHandlerPtr());

  bool handleInternetException(const InternetException& e);

  bool run();
  const std::wstring& error() const { return m_error; }

  ~Request() {
    m_requests.remove(m_id);
  }

  static void cancel();

  bool canceled() const;
};

class StrPtrMap : private std::map<const TCHAR*, StringPtr> {
 private:
  std::mutex m_mx;

 public:
  void add(StringPtr str) {
    std::scoped_lock lock(m_mx);

    __super::insert(__super::value_type(str.get()->c_str(), str));
  }

  const TCHAR* add(const TCHAR* str) {
    StringPtr s(new std::wstring(str));
    add(s);
    return s->c_str();
  }

  void remove(const TCHAR* str) {
    std::scoped_lock lock(m_mx);
    assert(__super::find(str) != __super::end());
    __super::erase(str);
  }
};

class DSPLUGINHELPER_API MultiDataLoader {
 private:
  StrPtrMap& m_strings;
  const ReqBuilderPtr m_reqBuilder;
  const HTTPErrorHandlerPtr m_httpErrorHandler;
  const ResultHandlerPtr m_resultHandler;
  const unsigned int m_width;

 private:
  StringPtr stitchStr(yloader::StringPtrVector& v);

 public:
  // width: the max number of days in a range, when creating the date ranges.
  MultiDataLoader(ReqBuilderPtr reqBuilder, HTTPErrorHandlerPtr httpErrorHandler, StrPtrMap& strings,
                  unsigned int width, ResultHandlerPtr resultHandler = ResultHandlerPtr());
  ~MultiDataLoader();

  virtual StringPtr preprocess(StringPtr str) const = 0;

  const TCHAR* getData(const TCHAR* symbol, const TCHAR* startDate, const TCHAR* endDate, Period period, bool adjusted,
                       bool& error, int retries);
};

}  // namespace yloader

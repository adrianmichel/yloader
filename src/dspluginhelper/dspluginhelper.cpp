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

#include "stdafx.h"

using namespace yloader;

DSPLUGINHELPER_API yloader::Requests yloader::Request::m_requests;
std::atomic_uint64_t yloader::Request::m_crtId = 0;

DSPLUGINHELPER_API std::wstring yloader::Request::m_proxyServerAddress;
DSPLUGINHELPER_API std::wstring yloader::Request::m_proxyServerUserName;
DSPLUGINHELPER_API std::wstring yloader::Request::m_proxyServerPassword;
DSPLUGINHELPER_API unsigned int yloader::Request::m_httpRequestTimeout;

DateRange::DateRange(const std::wstring& d1, const std::wstring& d2)
    : DateRangeBase(d1.empty() ? yloader::Date() : yloader::Date(d1),
                    d2.empty() ? yloader::Date() : yloader::Date(d2)) {}

DateRange::DateRange(const yloader::Date& d1, const yloader::Date& d2)
    : DateRangeBase(d1, d2) {}

std::wstring DateRange::toString() const {
  return __super::first.toString() + L"-" + __super::second.toString();
}

yloader::DateDuration DateRange::duration() {
  return __super::second - __super::first;
}

DateRanges::DateRanges(DateRange range, unsigned int maxUnits, unsigned int unit) {
  yloader::DateDuration dd = range.duration();

  unsigned int totalDays = dd.days();

  unsigned int current = 0;

  do {
    yloader::Date d1 = range.first + DateDuration(current);
    current += maxUnits * unit;
    yloader::Date d2 = range.first + DateDuration(current);
    d2 = min(d2, range.second);
    __super::push_back(DateRange(d1, d2));
    ++current;
  } while (current < totalDays);
}

std::wstring DateRanges::toString() const {
  std::wstring str;
  for (const auto& dr : *this) {
    str += dr.toString() + L"\n";
  }

  return str;
}

yloader::StringPtr yloader::removeLines(const std::wstring& str, unsigned int count, const std::wstring& eol) {
  if (!str.empty()) {
    std::wstring::size_type crt = 0;
    for (; count > 0 && crt != std::wstring::npos; --count) {
      crt = str.find(eol, crt) + eol.length();
    }

    if (crt != std::wstring::npos) {
      yloader::StringPtr s(new std::wstring());

      *s = str.substr(crt, str.length() - crt);
      return s;
    }
    else {
      return nullptr;
    }
  }
  else {
    return nullptr;
  }
}

HTTPRequestBasePtr Requests::getRequest(uint64_t id, const std::wstring& serverName, bool http) {
  std::scoped_lock lock(m_mx);

  for (auto r : m_requests) {
    auto request = r.second;
    if (*request != serverName) {
      // if the server name has changed, then clear the list of requsts and get
      // a new one
      m_requests.clear();
      break;
    }

    if (0 == request.use_count()) {
      request->unCancel();
      return request;
    }
  }

  auto result = m_requests.emplace(id, std::make_shared< CurlHTTPRequest>(serverName, http));
  return result.first->second;
}

void Requests::cancel() {
  std::scoped_lock lock(m_mx);
  {
    for (auto request : m_requests) {
      request.second->cancel();
    }
  }
}

bool Request::processCurlError(const InternetException& e) {
  switch (e.httpResponseCode()) {
    case 52:
      return true;
    default:
      m_error = e.message();
      return false;
  }
}

Request::Request(const ReqBuilder& reqBuilder, const HTTPErrorHandler& httpErrorHandler, const std::wstring& symbol, const DateRange& dateRange,
                 Period period, bool http, ResultHandlerPtr resultHandler)
    : m_reqBuilder(reqBuilder),
      m_symbol(symbol),
      m_dateRange(dateRange),
      m_period(period),
      m_canceled(false),
      m_httpErrorHandler(httpErrorHandler),
      m_http(http),
      m_resultHandler(resultHandler),
      m_id(m_crtId++){}

bool Request::handleInternetException(const InternetException& e) {
  switch (e.errorType()) {
    case InternetException::ErrorType::FAILED_TO_CONNECT:
      return true;
    case InternetException::ErrorType::HTTP_RESPONSE_ERROR: {
      unsigned int httpError = e.httpResponseCode();

      switch (m_httpErrorHandler.handler(httpError)) {
        case no_data_for_symbol:
          m_error = L"No data for symbol";
          return false;
        case no_data_for_range:
          m_error = L"No data for the date range";
          return false;
        case no_data_for_symbol_or_range:
          m_error = L"No data for symbol or date range";
          return false;
        case unknown_http_error:
          m_error = e.message().c_str() + std::to_wstring( e.httpResponseCode());
          return false;
        case retry:
          return true;
        default:
          assert(false);
          return false;
      }
    } break;
    case InternetException::REQUEST_CANCELLED:
      m_error = e.message();
      m_canceled = true;
      return false;
    case InternetException::CURL_ERROR:
      return processCurlError(e);
    default:
      m_error = e.message();
      return false;
  }
}

bool Request::run() {
  HTTPRequestBasePtr req = m_requests.getRequest(m_id, m_reqBuilder.rootURL(), m_http);
  int n = 60;
  do {
    try {
      m_error.clear();
      std::shared_ptr<std::string> r(req->get(m_reqBuilder.buildRequest(m_symbol, m_dateRange, m_period), std::wstring{},
        std::wstring{}, m_proxyServerAddress, m_proxyServerUserName, m_proxyServerPassword,
          m_httpRequestTimeout, m_reqBuilder.cookie(m_symbol)));

      yloader::StringPtr resp(std::make_shared< std::wstring >(yloader::s2ws(*r)));

      m_response = resp ? std::make_shared< std::wstring >(*resp) : nullptr;

      std::wstring str;

      if (m_resultHandler) {
        str = m_resultHandler->handler(m_response);
      }

      if (!str.empty()) {
        m_error = str;
        return false;
      }
      else {
        return true;
      }
    }
    catch (const InternetException& e) {
      if (!handleInternetException(e)) {
        return false;
      }
    }
    Sleep(1000);
  } while (--n > 0);

  m_error = L"Could not get data for symbol";
  return false;
}

void yloader::Request::cancel() { m_requests.cancel(); }

bool yloader::Request::canceled() const { return m_canceled; }

yloader::StringPtr MultiDataLoader::stitchStr(yloader::StringPtrVector& v) {
  StringPtr s(std::make_shared< std::wstring>());

  for (auto str : v) {
    *s += *str;
  }

  return s;
}

// width: the max number of days in a range, when creating the date ranges.
MultiDataLoader::MultiDataLoader(ReqBuilderPtr reqBuilder, HTTPErrorHandlerPtr httpErrorHandler, StrPtrMap& strings, unsigned int width,
                                 ResultHandlerPtr resultHandler)
    : m_reqBuilder(reqBuilder),
      m_strings(strings),
      m_width(width),
      m_httpErrorHandler(httpErrorHandler),
      m_resultHandler(resultHandler) {}
MultiDataLoader::~MultiDataLoader() {}

const TCHAR* yloader::MultiDataLoader::getData(const TCHAR* symbol, const TCHAR* startDate, const TCHAR* endDate, Period period, bool adjusted,
                                               bool& error, int retries) {
  DateRanges drv(DateRange(startDate, endDate), m_width);

  std::wstring rangestr;

  yloader::StringPtrVector data;

  error = true;
  yloader::StringPtr errorString(new std::wstring());

  bool hasData = false;

  for (DateRanges::reverse_iterator i = drv.rbegin(); i != drv.rend(); ++i) {
    rangestr = i->toString();
    Request req(*m_reqBuilder, *m_httpErrorHandler, symbol, *i, period, (bool)m_resultHandler);

    if (!req.run()) {
      *errorString = req.error();
      error = true;
      break;
    }

    StringPtr response = req.response();

    assert(response);

    if (response) response = preprocess(response);

    if (response && !response->empty()) {
      error = false;
      data.push_back(response);
      hasData = true;
    }
    else {
      if (hasData) {
        break;
      }
      *errorString = req.error();
      if (req.canceled()) {
        error = true;
        break;
      }
    }
  }

  if (error) {
    m_strings.add(errorString);
    return errorString.get()->c_str();
  }
  else {
    StringPtr dataStr = stitchStr(data);
    m_strings.add(dataStr);
    return dataStr.get()->c_str();
  }
}

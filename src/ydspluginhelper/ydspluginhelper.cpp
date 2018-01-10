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

using namespace yloader;
using yloader::operator<<;

YDSPLUGINHELPER_API yloader::Requests yloader::Request::m_requests;
YDSPLUGINHELPER_API std::wstring yloader::Request::m_proxyServerAddress;
YDSPLUGINHELPER_API std::wstring yloader::Request::m_proxyServerUserName;
YDSPLUGINHELPER_API std::wstring yloader::Request::m_proxyServerPassword;
YDSPLUGINHELPER_API unsigned int yloader::Request::m_httpRequestTimeout;

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

DateRange::DateRange(const std::wstring& d1, const std::wstring& d2)
    : DateRangeBase(d1.empty() ? yloader::Date() : yloader::Date(d1),
                    d2.empty() ? yloader::Date() : yloader::Date(d2)) {}

DateRange::DateRange(const yloader::Date& d1, const yloader::Date& d2)
    : DateRangeBase(d1, d2) {}

std::wstring DateRange::toString() const {
  return __super::first.toString() + _T( "-" ) + __super::second.toString();
}

yloader::DateDuration DateRange::duration() {
  return __super::second - __super::first;
}

DateRanges::DateRanges(DateRange range, unsigned int maxUnits,
                       unsigned int unit) {
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
  for (DateRangesBase::size_type n = 0; n < __super::size(); ++n) {
    const DateRange& dr(__super::at(n));
    str += dr.toString() + _T( "\n" );
  }

  return str;
}

yloader::StringPtr yloader::removeLines(const std::wstring& str,
                                        unsigned int count,
                                        const std::wstring& eol) {
  if (!str.empty()) {
    std::wstring::size_type crt = 0;
    for (; count > 0 && crt != std::wstring::npos; --count)
      crt = str.find(eol, crt) + eol.length();

    if (crt != std::wstring::npos) {
      yloader::StringPtr s(new std::wstring());

      *s = str.substr(crt, str.length() - crt);
      return s;
    } else
      return yloader::StringPtr();
  } else
    return yloader::StringPtr();
}

HTTPRequestBasePtr Requests::getRequest(const std::wstring& serverName,
                                        bool http) {
  yloader::Lock lock(_mx);

  for (RequestList::iterator i = m_requests.begin(); i != m_requests.end();
       ++i) {
    if (**i != serverName) {
      // if the server name has changed, then clear the list of requsts and get
      // a new one
      m_requests.clear();
      break;
    }

    if (!i->isReferred()) {
      (*i)->unCancel();
      return *i;
    }
  }
  HTTPRequestBasePtr p(new CurlHTTPRequest(serverName, http));

  m_requests.push_back(p);
  return p;
}

void Requests::cancel() {
  yloader::Lock lock(_mx);
  {
    for (RequestList::iterator i = m_requests.begin(); i != m_requests.end();
         ++i)
      (*i)->cancel();
  }
}

bool Request::processCurlError(const InternetException& e) {
  switch (e.httpResponseCode()) {
    case 52:
      return true;
    default:
      _error = e.message();
      return false;
  }
}

Request::Request(const ReqBuilder& reqBuilder,
                 const HTTPErrorHandler& httpErrorHandler,
                 const std::wstring& symbol, const DateRange& dateRange,
                 Period period, bool http, ResultHandlerPtr resultHandler)
    : _reqBuilder(reqBuilder),
      _symbol(symbol),
      _dateRange(dateRange),
      _period(period),
      _canceled(false),
      _httpErrorHandler(httpErrorHandler),
      _http(http),
      _resultHandler(resultHandler) {}

bool Request::handleInternetException(const InternetException& e) {
  switch (e.errorType()) {
    case InternetException::ErrorType::FAILED_TO_CONNECT:
      return true;
    case InternetException::ErrorType::HTTP_RESPONSE_ERROR: {
      unsigned int httpError = e.httpResponseCode();

      switch (_httpErrorHandler.handler(httpError)) {
        case no_data_for_symbol:
          _error = _T( "No data for symbol" );
          return false;
        case no_data_for_range:
          _error = _T( "No data for the date range" );
          return false;
        case no_data_for_symbol_or_range:
          _error = _T( "No data for symbol or date range" );
          return false;
        case unknown_http_error: {
          std::wstring t;

          t << 5;
        }
          _error = std::wstring(e.message().c_str()) << e.httpResponseCode();
          return false;
        case retry:
          return true;
        default:
          assert(false);
          return false;
      }
    } break;
    case InternetException::REQUEST_CANCELLED:
      _error = e.message();
      _canceled = true;
      return false;
    case InternetException::CURL_ERROR:
      return processCurlError(e);
    default:
      _error = e.message();
      return false;
  }
}

bool Request::run() {
  HTTPRequestBasePtr req = m_requests.getRequest(_reqBuilder.rootURL(), _http);
  int n = 60;
  do {
    try {
      _error.clear();
      boost::shared_ptr<std::string> r(req->get(
          _reqBuilder.buildRequest(_symbol, _dateRange, _period),
          std::wstring(),
          std::wstring(
              /*_T( "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:31.0) Gecko/20100101 Firefox/31.0" ) */),
          m_proxyServerAddress, m_proxyServerUserName, m_proxyServerPassword,
          m_httpRequestTimeout, _reqBuilder.cookie(_symbol)));

      yloader::StringPtr resp(new std::wstring(yloader::s2ws(*r)));

      _response = resp ? yloader::StringPtr(new std::wstring(*resp))
                       : yloader::StringPtr();

      std::wstring str;

      if (_resultHandler) str = _resultHandler->handler(_response);

      if (!str.empty()) {
        _error = str;
        return false;
      } else
        return true;
    } catch (const InternetException& e) {
      if (!handleInternetException(e)) return false;
    }
    Sleep(1000);
  } while (--n > 0);

  _error = _T( "Could not get data for symbol" );
  return false;
}

void yloader::Request::cancel() { m_requests.cancel(); }

bool yloader::Request::canceled() const { return _canceled; }

yloader::StringPtr MultiDataLoader::stitchStr(yloader::StringPtrVector& v) {
  StringPtr s(new std::wstring());

  for (StringPtrVector::iterator i = v.begin(); i != v.end(); ++i) *s += **i;

  return s;
}

// width: the max number of days in a range, when creating the date ranges.
MultiDataLoader::MultiDataLoader(ReqBuilderPtr reqBuilder,
                                 HTTPErrorHandlerPtr httpErrorHandler,
                                 StrPtrMap& strings, unsigned int width,
                                 ResultHandlerPtr resultHandler)
    : _reqBuilder(reqBuilder),
      _strings(strings),
      _width(width),
      _httpErrorHandler(httpErrorHandler),
      _resultHandler(resultHandler) {}
MultiDataLoader::~MultiDataLoader() {}

const TCHAR* yloader::MultiDataLoader::getData(const TCHAR* symbol,
                                               const TCHAR* startDate,
                                               const TCHAR* endDate,
                                               Period period, bool adjusted,
                                               bool& error, int retries) {
  DateRanges drv(DateRange(startDate, endDate), _width);

  {
    std::wstring str;

    str << _T( "InputRange: " ) << startDate << _T( " - " ) << endDate
        << _T( "\n" );

    OutputDebugString(str.c_str());

    OutputDebugString(_T( "Ranges:\n" ));
    OutputDebugString(drv.toString().c_str());
  }

  std::wstring rangestr;

  yloader::StringPtrVector data;

  error = true;
  yloader::StringPtr errorString(new std::wstring());

  bool hasData = false;

  for (DateRanges::reverse_iterator i = drv.rbegin(); i != drv.rend(); ++i) {
    rangestr = i->toString();
    Request req(*_reqBuilder, *_httpErrorHandler, symbol, *i, period,
                _resultHandler);

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
    } else {
      if (hasData) break;
      *errorString = req.error();
      if (req.canceled()) {
        error = true;
        break;
      }
    }
  }

  if (error) {
    _strings.add(errorString);
    return errorString.get()->c_str();
  } else {
    StringPtr dataStr = stitchStr(data);
    _strings.add(dataStr);
    return dataStr.get()->c_str();
  }
}

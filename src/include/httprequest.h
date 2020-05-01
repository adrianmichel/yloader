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

#include <wininet.h>
#include <mutex>
#include <objcounter.h>
#include "namevalue.h"

#ifdef MISCWIN_EXPORTS
#define MISCWIN_API __declspec(dllexport)
#else
#define MISCWIN_API __declspec(dllimport)
#endif

using namespace std::string_literals;

constexpr auto SERVER_OBJECT_NAME_KEY = L"Server object name";

class ServerContext {
 private:
  std::wstring m_serverName;
  std::wstring m_objectName;
  std::wstring m_extraHeaders;


 public:
  ServerContext(const std::wstring& serverName, const std::wstring& objectName, const std::wstring& extraHeaders = L""s )
      : m_serverName(serverName),
        m_objectName(objectName),
        m_extraHeaders(extraHeaders) {
    persist();
  }

  ServerContext(const ServerContext& sc)
      : m_serverName(sc.serverName()),
        m_objectName(sc.objectName()),
        m_extraHeaders(sc.extraHeaders()) {}

  const std::wstring& serverName() const { return m_serverName; }
  const std::wstring& objectName() const { return m_objectName; }
  const std::wstring& extraHeaders() const { return m_extraHeaders; }

 private:
  void persist() {
    // no saving, has to be explicitly set in the registry
  }

  void load() {}
};

class InternetException {
 public:
  /**
   * Various error types
   */
  enum ErrorType {
    FAILED_TO_CONNECT,
    OPERATION_TIMEDOUT,
    HTTP_RESPONSE_ERROR,
    REQUEST_CANCELLED,
    CURL_ERROR
  };

 private:
  const DWORD m_lastError;
  const ErrorType m_errorType;
  const unsigned int m_httpResponseCode;
  const std::wstring m_additionalInfo;

 public:
  std::wstring message() const {
    switch (m_errorType) {
      case FAILED_TO_CONNECT:
        return L"failed to connect";
      case OPERATION_TIMEDOUT:
        return L"operation timed out";
      case HTTP_RESPONSE_ERROR:
        return L"HTTP response error";
      case REQUEST_CANCELLED:
        return L"Request canceled";
      case CURL_ERROR:
        return L"Curl error: "s + m_additionalInfo + L" " + std::to_wstring( httpResponseCode() );
      default:
        return L"Unknown Internet error";
    }
  }
  /**
   * Constructor for InternetException - takes the error type as parameter
   *
   * @param errorType Error type
   */
  InternetException(ErrorType errorType, unsigned int httpResponseCode = 0,
                    const std::wstring& additionalInfo = L"")
      : m_lastError(GetLastError()),
        m_errorType(errorType),
        m_httpResponseCode(httpResponseCode),
        m_additionalInfo(additionalInfo) {}

  /**
   * Returns the last error, i.e. the value set by the call to GetLastError
   *
   * @return Value returned by GetLastError
   */
  DWORD lastError() const { return m_lastError; }
  /**
   * Returns the error type
   *
   * @return The error type
   */
  ErrorType errorType() const { return m_errorType; }
  unsigned int httpResponseCode() const { return m_httpResponseCode; }
};

class HTTPRequestBase {
 private:
  const std::wstring m_serverName;
  bool m_http;  // true for http, false for https

 public:
  HTTPRequestBase(const std::wstring& serverName, bool http = true)
      : m_serverName(serverName), m_http(http) {}

  virtual ~HTTPRequestBase() {}

  virtual std::shared_ptr<std::string> get(const std::wstring& object, const std::wstring& extraHeaders = L""s,
      const std::wstring& userAgent = L""s, const std::wstring& proxyServerAddress = L""s,
      const std::wstring& proxyServerUserName = L""s, const std::wstring& proxyServerPassword = L""s,
      unsigned int timeout = 0,
      const std::wstring& cookie = std::wstring()) = 0;

  std::shared_ptr<std::string> get(const std::wstring& object, const yloader::NameValueMap& data, const std::wstring& extraHeaders = L""s,
      const std::wstring& userAgent = L""s, const std::wstring& proxyServerAddress = L""s,
      const std::wstring& proxyServerUserName = L""s, const std::wstring& proxyServerPassword = L""s, unsigned int timeout = 0) {
    std::wstring query = object;

    if (!query.empty() && query.find(L"?") == std::wstring::npos) {
      query += L"?";
    }

    query += *data.toString();

    return get(query, extraHeaders, userAgent, proxyServerAddress, proxyServerUserName, proxyServerPassword, timeout);
  }

  virtual std::shared_ptr<std::string> post(const std::wstring& object, const yloader::NameValueMap& data, const std::wstring& extraHeaders = L""s,
      const std::wstring& proxyServerAddress = L""s, const std::wstring& proxyServerUserName = L""s, const std::wstring& proxyServerPassword = L""s,
      unsigned int timeout = 0) = 0;

  bool operator==(const std::wstring& serverName) const {
    return serverName == m_serverName;
  }
  bool operator!=(const std::wstring& serverName) const {
    return serverName != m_serverName;
  }
  virtual void cancel() = 0;
  virtual void unCancel() = 0;

 protected:
  const std::wstring& getServerName() const { return m_serverName; }
  std::wstring protocol() const {
    return m_http ? L"http" : L"https";
  }
  std::wstring url(const std::wstring& object);
};

using HTTPRequestBasePtr = std::shared_ptr<HTTPRequestBase>;
using StrStrMap = std::map<std::string, std::string>;

class Headers : public StrStrMap {
 public:
  void add(const std::string& header) {
    std::string::size_type pos = header.find(":");

    if (pos != std::string::npos) {
      __super::insert(StrStrMap::value_type(
          boost::to_lower_copy(boost::trim_copy(header.substr(0, pos))),
          boost::trim_copy(header.substr(pos + 1))));
    }
  }

  const std::string* find(const std::string& name) const {
    StrStrMap::const_iterator i = __super::find(boost::to_lower_copy(name));
    return i != __super::end() ? &i->second : 0;
  }
};

class MISCWIN_API CurlHTTPRequest : public HTTPRequestBase {
  OBJ_COUNTER(CurlHTTPRequest)
 private:
  void* m_curl;
  std::mutex m_mx;
  bool m_cancel;
  bool m_canceled;
  bool m_verifyPeer;
  bool m_verifyHost;
  Headers m_headers;

  static int xprogress(void* req, double t, double d, double ultotal,
                       double ulnow);
  void handleCurlResult(unsigned int res, const char* name);

  int progress();

  void setVerify();

 public:
  CurlHTTPRequest(const std::wstring& serverName, bool http = true, bool verifyPeer = false, bool verifyHost = false);
  virtual ~CurlHTTPRequest();

  virtual std::shared_ptr<std::string> get(const std::wstring& data, const std::wstring& extraHeaders = L""s,
      const std::wstring& userAgent = L""s, const std::wstring& proxyServerAddress = L""s,
      const std::wstring& proxyServerUserName = L""s, const std::wstring& proxyServerPassword = L""s,
      unsigned int timeout = 0, const std::wstring& cookie = L""s);

  virtual std::shared_ptr<std::string> post(const std::wstring& object, const yloader::NameValueMap& data,
      const std::wstring& extraHeaders = L""s, const std::wstring& proxyServerAddress = L""s,
      const std::wstring& proxyServerUserName = L""s, const std::wstring& proxyServerPassword = L""s,
      unsigned int timeout = 0);

  std::shared_ptr<std::string> get( const std::wstring& object, const yloader::NameValueMap& data,
      const std::wstring& extraHeaders = L""s, const std::wstring& userAgent = L""s,
      const std::wstring& proxyServerAddress = L""s, const std::wstring& proxyServerUserName = L""s,
      const std::wstring& proxyServerPassword = L""s, unsigned int timeout = 0) {
    return __super::get(object, data, extraHeaders, userAgent, proxyServerAddress, proxyServerUserName, proxyServerPassword, timeout);
  }

  virtual void cancel();
  void unCancel();

  void addHeader(const std::string& header) { m_headers.add(header); }

  const Headers& getHeaders() const { return m_headers; }
};

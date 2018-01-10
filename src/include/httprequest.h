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
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include "namevalue.h"

#ifdef MISCWIN_EXPORTS
#define MISCWIN_API __declspec(dllexport)
#else
#define MISCWIN_API __declspec(dllimport)
#endif

class ServerContext {
 private:
  std::wstring _serverName;
  std::wstring _objectName;
  std::wstring _extraHeaders;

#define SERVER_OBJECT_NAME_KEY _T( "Server object name" )

 public:
  ServerContext(const std::wstring& serverName, const std::wstring& objectName,
                const std::wstring& extraHeaders = _T( "" ))
      : _serverName(serverName),
        _objectName(objectName),
        _extraHeaders(extraHeaders) {
    persist();
  }

  ServerContext(const ServerContext& sc)
      : _serverName(sc.serverName()),
        _objectName(sc.objectName()),
        _extraHeaders(sc.extraHeaders()) {}

  const std::wstring& serverName() const { return _serverName; }
  const std::wstring& objectName() const { return _objectName; }
  const std::wstring& extraHeaders() const { return _extraHeaders; }

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
  const DWORD _lastError;
  const ErrorType _errorType;
  const unsigned int _httpResponseCode;
  const std::wstring _additionalInfo;

 public:
  std::wstring message() const {
    switch (_errorType) {
      case FAILED_TO_CONNECT:
        return _T( "failed to connect" );
        break;
      case OPERATION_TIMEDOUT:
        return _T( "operation timed out" );
        break;
      case HTTP_RESPONSE_ERROR:
        return _T( "HTTP response error" );
        break;
      case REQUEST_CANCELLED:
        return _T( "Request canceled" );
        break;
      case CURL_ERROR: {
        std::wostringstream os;

        os << _T( "Curl error: " ) << _additionalInfo << " "
           << httpResponseCode();
        return os.str();
      }
      default:
        return _T( "Unknown internet error" );
        break;
    }
  }
  /**
   * Constructor for InternetException - takes the error type as parameter
   *
   * @param errorType Error type
   */
  InternetException(ErrorType errorType, unsigned int httpResponseCode = 0,
                    const std::wstring& additionalInfo = std::wstring())
      : _lastError(GetLastError()),
        _errorType(errorType),
        _httpResponseCode(httpResponseCode),
        _additionalInfo(additionalInfo) {}

  /**
   * Returns the last error, i.e. the value set by the call to GetLastError
   *
   * @return Value returned by GetLastError
   */
  DWORD lastError() const { return _lastError; }
  /**
   * Returns the error type
   *
   * @return The error type
   */
  ErrorType errorType() const { return _errorType; }
  unsigned int httpResponseCode() const { return _httpResponseCode; }
};

class HTTPRequestBase {
 private:
  const std::wstring m_serverName;
  bool m_http;  // true for http, false for https

 public:
  HTTPRequestBase(const std::wstring& serverName, bool http = true)
      : m_serverName(serverName), m_http(http) {}

  virtual ~HTTPRequestBase() {}

  virtual boost::shared_ptr<std::string> get(
      const std::wstring& object,
      const std::wstring& extraHeaders = std::wstring(),
      const std::wstring& userAgent = std::wstring(),
      const std::wstring& proxyServerAddress = std::wstring(),
      const std::wstring& proxyServerUserName = std::wstring(),
      const std::wstring& proxyServerPassword = std::wstring(),
      unsigned int timeout = 0,
      const std::wstring& cookie = std::wstring()) = 0;

  boost::shared_ptr<std::string> get(
      const std::wstring& object, const yloader::NameValueMap& data,
      const std::wstring& extraHeaders = std::wstring(),
      const std::wstring& userAgent = std::wstring(),
      const std::wstring& proxyServerAddress = std::wstring(),
      const std::wstring& proxyServerUserName = std::wstring(),
      const std::wstring& proxyServerPassword = std::wstring(),
      unsigned int timeout = 0) {
    std::wstring query = object;

    if (!query.empty() && query.find(_T( "?" )) == std::wstring::npos)
      query += _T( "?" );

    query += data.toString();

    return get(query, extraHeaders, userAgent, proxyServerAddress,
               proxyServerUserName, proxyServerPassword, timeout);
  }

  virtual boost::shared_ptr<std::string> post(
      const std::wstring& object, const yloader::NameValueMap& data,
      const std::wstring& extraHeaders = std::wstring(),
      const std::wstring& proxyServerAddress = std::wstring(),
      const std::wstring& proxyServerUserName = std::wstring(),
      const std::wstring& proxyServerPassword = std::wstring(),
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
    return m_http ? _T( "http" ) : _T( "https" );
  }
  std::wstring url(const std::wstring& object);
};

typedef yloader::ManagedPtr<HTTPRequestBase> HTTPRequestBasePtr;

typedef std::map<std::string, std::string> StrStrMap;
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
 private:
  void* _curl;
  yloader::Mutex _mx;
  bool _cancel;
  bool _canceled;
  bool m_verifyPeer;
  bool m_verifyHost;
  Headers headers;

  static int xprogress(void* req, double t, double d, double ultotal,
                       double ulnow);
  void handleCurlResult(unsigned int res, const char* name);

  int progress();

  void setVerify();

 public:
  CurlHTTPRequest(const std::wstring& serverName, bool http = true,
                  bool verifyPeer = false, bool verifyHost = false);
  virtual ~CurlHTTPRequest();

  virtual boost::shared_ptr<std::string> get(
      const std::wstring& data,
      const std::wstring& extraHeaders = std::wstring(),
      const std::wstring& userAgent = std::wstring(),
      const std::wstring& proxyServerAddress = std::wstring(),
      const std::wstring& proxyServerUserName = std::wstring(),
      const std::wstring& proxyServerPassword = std::wstring(),
      unsigned int timeout = 0, const std::wstring& cookie = std::wstring());

  virtual boost::shared_ptr<std::string> post(
      const std::wstring& object, const yloader::NameValueMap& data,
      const std::wstring& extraHeaders = std::wstring(),
      const std::wstring& proxyServerAddress = std::wstring(),
      const std::wstring& proxyServerUserName = std::wstring(),
      const std::wstring& proxyServerPassword = std::wstring(),
      unsigned int timeout = 0);

  boost::shared_ptr<std::string> get(
      const std::wstring& object, const yloader::NameValueMap& data,
      const std::wstring& extraHeaders = std::wstring(),
      const std::wstring& userAgent = std::wstring(),
      const std::wstring& proxyServerAddress = std::wstring(),
      const std::wstring& proxyServerUserName = std::wstring(),
      const std::wstring& proxyServerPassword = std::wstring(),
      unsigned int timeout = 0) {
    return __super::get(object, data, extraHeaders, userAgent,
                        proxyServerAddress, proxyServerUserName,
                        proxyServerPassword, timeout);
  }

  virtual void cancel();
  void unCancel();

  void addHeader(const std::string& header) { headers.add(header); }

  const Headers& getHeaders() const { return headers; }
};

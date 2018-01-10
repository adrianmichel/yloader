// yyahoodsplugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "yyahoodsplugin.h"
#include "token.h"

yloader::StrPtrMap _strings;

using yloader::DateTime;

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

YDSPLUGIN_API int nyyahoodsplugin = 0;

YDSPLUGIN_API const TCHAR* getYDataSourcePluginInfo() { return 0; }

YDSPLUGIN_API unsigned int getMaxConnections() { return 20; }
YDSPLUGIN_API bool getCanAdjust() { return true; }

YDSPLUGIN_API bool getCanUnAdjust() { return true; }

YDSPLUGIN_API const TCHAR* getName() { return _strings.add(_T( "Yahoo" )); }
YDSPLUGIN_API const TCHAR* getDescription() {
  return _strings.add(_T( "Get free quotes from Yahoo Finance" ));
}

YDSPLUGIN_API const TCHAR* getId() {
  return _strings.add(_T( "97895A87-1DF3-49a6-AF38-AC7F79843E99" ));
}

YDSPLUGIN_API const TCHAR* getError() { return _strings.add(_T( "" )); }

YDSPLUGIN_API const TCHAR* getDeveloperName() {
  return _strings.add(_T( "yloader.com" ));
}
YDSPLUGIN_API const TCHAR* getDeveloperURL() {
  return _strings.add(_T( "http://www.yloader.com" ));
}
YDSPLUGIN_API const TCHAR* getDeveloperEmail() {
  return _strings.add(_T( "info@yloader.com" ));
}
YDSPLUGIN_API const TCHAR* getVersion() { return _strings.add(WSTRPRODUCTVER); }

YDSPLUGIN_API const TCHAR* getUpdateInfoURL() { return _strings.add(_T( "" )); }
YDSPLUGIN_API const TCHAR* getHelpURL() { return _strings.add(_T( "" )); }

YDSPLUGIN_API const TCHAR* getAPIVersion() {
  return _strings.add(WSTRPRODUCTVER);
}

YDSPLUGIN_API void showSettingsDialog() {}

#define FIRST_YAHOO_DATE_STR _T( "1/1/1960" )
#define FIRST_YAHOO_DATE yloader::Date(FIRST_YAHOO_DATE_STR)

YDSPLUGIN_API const TCHAR* getFirstDate() {
  return _strings.add(FIRST_YAHOO_DATE_STR);
}

Token token;

class YahooData : public yloader::ReqBuilder {
 public:
  YahooData() {}
  virtual ~YahooData(void) {}

 private:
  static std::wstring getPeriod(Period period) {
    switch (period) {
      case daily:
        return _T( "1d");
      case weekly:
        return _T( "1wk" );
      case monthly:
        return _T( "1mo" );
      case quarterly:
      case yearly:
      default:
        throw std::exception("Invalid period");
    }
  }

 public:
  virtual std::wstring buildRequest(const std::wstring& symbol,
                                    const yloader::DateRange& dateRange,
                                    Period period) const {
    if (token.getCrumb(symbol).empty() || token.getCookie(symbol).empty())
      token.update(symbol);

    static const std::wstring QUERY(
        _T("/v7/finance/download/")
        _T("%1%?period1=%2%&period2=%3%&interval=%4%&events=history&crumb=%")
        _T("5%"));

    DateTime startDate = DateTime(
        dateRange.first.isNotADate() ? FIRST_YAHOO_DATE : dateRange.first);
    std::wstring sd = startDate.to_iso_string();
    DateTime endDate =
        DateTime(dateRange.second.isNotADate() ? yloader::LocalTimeSec().date()
                                               : dateRange.second);
    std::wstring ed = endDate.to_iso_string();

    __int64 start = startDate.to_epoch_time();
    // this is 12am, we have to add 23h 59min 59 sec
    __int64 end = endDate.to_epoch_time() + 23 * 3600 + 59 * 60 + 59;

    std::wstring req =
        yloader::format(QUERY, urlencode(symbol), start, end, getPeriod(period),
                        yloader::s2ws(token.getCrumb(symbol)));

    LOG(log_debug, _T( "query: " ) << req)

    return req;
  }

  std::wstring rootURL() const { return _T( "query1.finance.yahoo.com" ); }

  virtual bool hasCookie() const { return true; }
  virtual std::wstring cookie(const std::wstring& symbol) const {
    return yloader::s2ws(token.getCookie(symbol));
  }
};

std::string proxyServerAddress;
std::string proxyServerUserName;
std::string proxyServerPassword;
unsigned int httpRequestTimeout = 0;

class YahooHTTPErrorHandler : public yloader::HTTPErrorHandler {
 public:
  virtual yloader::HTTPErrorType handler(unsigned int error) const {
    switch (error) {
      case 404:
        // yahoo responds the same way if the symbols doesn't exist, or it
        // exists but there is no data for that range
        return yloader::no_data_for_symbol_or_range;
      default:
        return yloader::unknown_http_error;
    }
  }
};

/*

StrPtr getHtml(std::wstring& symbol) {

        std::wstring u =

        const Headers& headers = req.getHeaders();

        const std::string* ck = headers.find("Set-Cookie");

        std::string cookie;

        if (ck != 0) {
                cookie = ck->substr(0, ck->find(";"));
        }

        std::string crumb;
        boost::smatch result;
        if (boost::regex_search(*html, result, crumbRegex)) {
                crumb = result[1];
        }

        std::string cookieHeader = "Cookie: " + cookie;

        yloader::NameValueMap hx;

        hx.add(_T( "Cookie" ), yloader::s2ws( cookie) );

        std::wstring query1 =
_T("/v7/finance/download/IBM?period1=1493334242&period2=1495926242&interval=1d&events=history&crumb=")
+ yloader::s2ws(crumb); std::wstring query2 =
_T("/v7/finance/download/GOOG?period1=1493334242&period2=1495926242&interval=1d&events=history&crumb=")
+ yloader::s2ws(crumb);

        CurlHTTPRequest req1(_T("query1.finance.yahoo.com"), false);

        StrPtr data = req1.get(query, std::wstring(), std::wstring(),
std::wstring(), std::wstring(), std::wstring(), 0, yloader::s2ws( cookie ));
        StrPtr data1 = req1.get(query1, std::wstring(), std::wstring(),
std::wstring(), std::wstring(), std::wstring(), 0, yloader::s2ws(cookie));
        StrPtr data2 = req1.get(query2, std::wstring(), std::wstring(),
std::wstring(), std::wstring(), std::wstring(), 0, yloader::s2ws(cookie));

        return html;
}
*/

YDSPLUGIN_API const TCHAR* getData(const TCHAR* symbol, const TCHAR* startDate,
                                   const TCHAR* endDate, Period period,
                                   bool adjusted, bool& error, int retries) {
  if (!validated) return 0;
  YahooData yd;
  YahooHTTPErrorHandler yheh;
  yloader::Request req(yd, yheh, symbol, yloader::DateRange(startDate, endDate),
                       period, false);

  yloader::StringPtr str;

  if (!req.run()) {
    LOG(log_error, _T( "error getting data: ") << req.error())
    if (retries > 0) {
      Sleep(500);
      LOG(log_error, _T("updating token and retrying"))
      token.update(symbol);
      return getData(symbol, startDate, endDate, period, adjusted, error,
                     --retries);
    } else {
      LOG(log_error, _T("too many retries, giving up"))
      str = yloader::StringPtr(new std::wstring(req.error()));
      error = true;
    }
  } else {
    str = yloader::removeLines(*req.response(), 1, _T( "\n" ));
    error = false;
  }

  _strings.add(str);
  return str.get()->c_str();
}

YDSPLUGIN_API void freeString(const TCHAR* str) { _strings.remove(str); }

YDSPLUGIN_API const TCHAR* getShortName() {
  return _strings.add(_T( "Yahoo" ));
}

YDSPLUGIN_API const TCHAR* getSupportedPeriods() {
  return _strings.add(_T( "0 1 2" ));
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

YDSPLUGIN_API bool getAscendingSort() { return true; }

#include <crypt.h>

const uint32_t VERSION_KEY[4] = {0x3, 0x1, 0x0, 0x0};

YDSPLUGIN_API bool validation(const TCHAR* str) {
  yloader::Crypt crypt(VERSION_KEY, true, true);

  boost::shared_ptr<std::string> s(crypt.de(yloader::ws2s(str)));

  validated = s && yloader::s2ws(*s) == getVersion();

  return validated;
}

YDSPLUGIN_API int getRetries() { return 4; }

// Date, Open, High, Low, Close, Adj Close, Volume
// 0     1     2     3    4      5          6

YDSPLUGIN_API bool parseLine(const TCHAR* line, bool adjusted,
                             DatasourceBar* bar) {
  if (line == 0 || _tcslen(line) == 0) {
    return false;
  }
  Tokenizer tokens(line, _T(","));

  if (tokens.size() < 7) {
    return false;
  }

  // OHLC are split adjusted
  // Adj Close is split and dividend adjusted

  double unadjClose = _tstof(tokens[4].c_str());
  _tcscpy(bar->date, tokens[0].c_str());
  bar->open = _tstof(tokens[1].c_str());
  bar->high = _tstof(tokens[2].c_str());
  bar->low = _tstof(tokens[3].c_str());
  bar->volume = (__int64)(_tstof(tokens[6].c_str()));
  bar->close = unadjClose;

  if (adjusted) {
    double adjClose = _tstof(tokens[5].c_str());
    double ratio = bar->close > 0 ? adjClose / unadjClose : 1;
    bar->open *= ratio;
    bar->high *= ratio;
    bar->low *= ratio;
    bar->close = adjClose;
    bar->volume /= ratio;
  }

  return true;
}

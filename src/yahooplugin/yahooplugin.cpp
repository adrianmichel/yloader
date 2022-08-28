// yyahoodsplugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "yahooplugin.h"
#include "token.h"

yloader::StrPtrMap m_strings;

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

YDSPLUGIN_API const TCHAR* getName() { return m_strings.add(L"Yahoo"); }
YDSPLUGIN_API const TCHAR* getDescription() {
  return m_strings.add(L"Get free quotes from Yahoo Finance" );
}

YDSPLUGIN_API const TCHAR* getId() {
  return m_strings.add(L"97895A87-1DF3-49a6-AF38-AC7F79843E99" );
}

YDSPLUGIN_API const TCHAR* getError() { return m_strings.add(L""); }

YDSPLUGIN_API const TCHAR* getDeveloperName() {
  return m_strings.add(L"yloader.com" );
}
YDSPLUGIN_API const TCHAR* getDeveloperURL() {
  return m_strings.add(L"http://www.yloader.com" );
}
YDSPLUGIN_API const TCHAR* getDeveloperEmail() {
  return m_strings.add(L"info@yloader.com" );
}
YDSPLUGIN_API const TCHAR* getVersion() { return m_strings.add(WSTRPRODUCTVER); }

YDSPLUGIN_API const TCHAR* getUpdateInfoURL() { return m_strings.add(L""); }
YDSPLUGIN_API const TCHAR* getHelpURL() { return m_strings.add(L""); }

YDSPLUGIN_API const TCHAR* getAPIVersion() {
  return m_strings.add(WSTRPRODUCTVER);
}

YDSPLUGIN_API void showSettingsDialog() {}

constexpr auto FIRST_YAHOO_DATE_STR = L"1/1/1960";
#define FIRST_YAHOO_DATE yloader::Date(FIRST_YAHOO_DATE_STR)

YDSPLUGIN_API const TCHAR* getFirstDate() {
  return m_strings.add(FIRST_YAHOO_DATE_STR);
}

Token token;

constexpr auto QUERY = L"/v7/finance/download/%1%?period1=%2%&period2=%3%&interval=%4%&events=history&includeAdjustedClose=true";

class YahooData : public yloader::ReqBuilder {
 public:
  YahooData() {}
  virtual ~YahooData(void) {}

 private:
  static std::wstring getPeriod(Period period) {
    switch (period) {
      case daily:
        return L"1d";
      case weekly:
        return L"1wk";
      case monthly:
        return L"1mo";
      case quarterly:
      case yearly:
      default:
        throw std::exception("Invalid period");
    }
  }

 public:
  virtual std::wstring buildRequest(const std::wstring& symbol, const yloader::DateRange& dateRange, Period period) const {
 /*   if (token.getCrumb(symbol).empty() || token.getCookie(symbol).empty()) {
      token.update(symbol);
    }
 */

    DateTime startDate = DateTime(dateRange.first.isNotADate() ? FIRST_YAHOO_DATE : dateRange.first);
    std::wstring sd = startDate.to_iso_string();
    DateTime endDate = DateTime(dateRange.second.isNotADate() ? yloader::LocalTimeSec().date() : dateRange.second);
    std::wstring ed = endDate.to_iso_string();

    __int64 start = startDate.to_epoch_time();
    // this is 12am, we have to add 23h 59min 59 sec
    __int64 end = endDate.to_epoch_time() + 23 * 3600 + 59 * 60 + 59;

    std::wstring req = yloader::format(QUERY, urlencode(symbol), start, end, getPeriod(period));

    LOG(log_debug, L"query: ", req);

    return req;
  }

  std::wstring rootURL() const { return L"query1.finance.yahoo.com"; }

  bool hasCookie() const override { return true; }
  std::wstring cookie(const std::wstring& symbol) const override {
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


YDSPLUGIN_API const TCHAR* getData(const TCHAR* symbol, const TCHAR* startDate,
                                   const TCHAR* endDate, Period period,
                                   bool adjusted, bool& error, int retries) {
  if (!validated) {
    return 0;
  }
  LOG(log_info, L"Yahoo plugin getData, symbol: ", symbol, L", start date: ", startDate, L", endDate", endDate,
    L", period", period, L", adjusted: ", adjusted);
  YahooData yd;
  YahooHTTPErrorHandler yheh;
  LOG(log_info, L"Constructing request");
  yloader::Request req(yd, yheh, symbol, yloader::DateRange(startDate, endDate),
                       period, false);

  yloader::StringPtr str;

  LOG(log_info, L"Running request");
  if (!req.run()) {
    LOG(log_error, L"error getting data: ", req.error());
    if (retries > 0) {
      Sleep(500);
      LOG(log_error, L"updating token and retrying");
      token.update(symbol);
      return getData(symbol, startDate, endDate, period, adjusted, error, --retries);
    }
    else {
      LOG(log_error, L"too many retries, giving up");
      str = std::make_shared<std::wstring>(req.error());
      error = true;
    }
  }
  else {
    LOG(log_info, L"Request succeeded");

    str = yloader::removeLines(*req.response(), 1, L"\n" );
    error = false;
  }

  m_strings.add(str);
  return str.get()->c_str();
}

YDSPLUGIN_API void freeString(const TCHAR* str) { m_strings.remove(str); }

YDSPLUGIN_API const TCHAR* getShortName() {
  return m_strings.add(L"Yahoo" );
}

YDSPLUGIN_API const TCHAR* getSupportedPeriods() {
  return m_strings.add(L"0 1 2" );
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

  std::shared_ptr<std::string> s(crypt.de(yloader::ws2s(str)));

  validated = s && yloader::s2ws(*s) == getVersion();

  return validated;
}

YDSPLUGIN_API int getRetries() { return 4; }

// Date, Open, High, Low, Close, Adj Close, Volume
// 0     1     2     3    4      5          6

YDSPLUGIN_API bool parseLine(const TCHAR* line, bool adjusted, DatasourceBar* bar) {
  if (line == 0 || _tcslen(line) == 0) {
    return false;
  }
  Tokenizer tokens(line, L",");

  if (tokens.size() < 7) {
    return false;
  }

  // OHLC are split adjusted
  // Adj Close is split and dividend adjusted

  double unadjClose = _tstof(tokens[4].c_str());
  wcscpy_s(bar->date, tokens[0].c_str());
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

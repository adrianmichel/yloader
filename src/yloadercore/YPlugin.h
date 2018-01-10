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

#pragma once

#include <crypt.h>
#include <plugin.h>
#include <ypluginhelper.h>
#include <hinstance.h>
#include <log.h>

typedef const TCHAR* (*GET_STRING_PROC)();
typedef int (*GET_INT_PROC)();
typedef bool (*GET_BOOL_PROC)();
typedef void (*VOID_PROC)();
typedef void (*SET_STRING_PROC)(const TCHAR*);
typedef void (*SET_UINT_PROC)(unsigned int);
typedef bool (*VALIDATION_PROC)(const TCHAR*);
typedef bool (*PARSE_LINE_PROC)(const TCHAR*, bool adjusted,
                                DatasourceBar* bar);

class YPluginInfo : public yloader::Info {
 private:
  const std::wstring _shortName;
  const std::wstring _developerURL;
  const std::wstring _developerName;
  const yloader::Version _version;
  const std::wstring _email;
  const std::wstring _updateURL;
  const std::wstring _helpURL;
  const std::wstring _developerEmail;
  const yloader::Version _apiVersion;
  const std::wstring _firstDate;

 public:
  YPluginInfo(const yloader::Info& info, const std::wstring& shortName,
              const std::wstring& developerURL,
              const std::wstring& developerEmail,
              const std::wstring& developerName, const std::wstring version,
              const std::wstring& apiVersion, const std::wstring& email,
              const std::wstring& updateURL, const std::wstring& helpURL,
              const std::wstring& firstDate)
      : yloader::Info(info),
        _developerURL(developerURL),
        _developerName(developerName),
        _version(version),
        _email(email),
        _updateURL(updateURL),
        _helpURL(helpURL),
        _shortName(shortName),
        _apiVersion(apiVersion),
        _developerEmail(developerEmail),
        _firstDate(firstDate) {}

  const std::wstring& developerURL() const { return _developerURL; }
  const std::wstring& developerName() const { return _developerName; }
  std::wstring version() const { return _version.toString(); }
  const std::wstring& shortName() const { return _shortName; }
  std::wstring apiVersion() const { return _apiVersion.toString(); }
  const std::wstring& developerEmail() const { return _developerEmail; }
  const std::wstring& firstDate() const { return _firstDate; }
};

class YPluginBase : public HInstance {
 private:
  SET_STRING_PROC _freeString;

 public:
  YPluginBase(const std::wstring& path);
  void freeString(const TCHAR* str) const { (*_freeString)(str); }
};

class YPluginString {
 private:
  const TCHAR* _string;
  const YPluginBase* _plugin;

 public:
  YPluginString(const TCHAR* str, const YPluginBase* plugin)
      : _string(str), _plugin(plugin) {}

  ~YPluginString() { _plugin->freeString(_string); }

  operator const TCHAR*() { return _string; }
};

class YPlugin : public YPluginBase, public YPluginInfo {
 public:
  YPlugin(const std::wstring& path) try : YPluginBase(path),
                                          YPluginInfo(getInfo()) {
  } catch (const HInstanceException& e) {
    std::wstring x;

    x << 2;
    LOG(log_debug, path << _T( " HInstanceException exception last error: " )
                        << e.getLastError())
    throw yloader::PluginException(_T(""), _T(""));
  } catch (const yloader::VersionException& ve) {
    LOG(log_error,
        path << _T( "  doesn't have version info (or malformed info): " )
             << ve.message())
    throw yloader::PluginException(_T(""), _T(""));
  } catch (const HInstanceMethodException& e) {
    LOG(log_debug, __super::path() << _T( ", exported function \"" ) << e.name()
                                   << _T( "\" not found" ))
  }

  ~YPlugin() {}

 protected:
  template <typename U>
  void f(const std::wstring& procName, U& u) {
    try {
      GET_STRING_PROC proc =
          reinterpret_cast<GET_STRING_PROC>(__super::getProcAddress(procName));
      YPluginString p(proc(), this);
      LOG(log_debug,
          path() << _T( ", exported function \"" ) << u << _T( "\" found" ));
      u = p;
    } catch (const HInstanceMethodException&) {
      LOG(log_debug, path() << _T( ", exported function \"" ) << u
                            << _T( "\" not found" ));
    }
  }

 private:
  YPluginInfo getInfo();
};

typedef yloader::ManagedPtr<YPlugin> PluginPtr;

typedef const TCHAR* (*GET_DATA_PROC)(const TCHAR* symbol,
                                      const TCHAR* startDate,
                                      const TCHAR* endDate, Period period,
                                      bool adjusted, bool& error, int retries);

const uint32_t VERSION_KEY[4] = {0x3, 0x1, 0x0, 0x0};

class YDataSourcePlugin : public YPlugin {
 private:
  GET_DATA_PROC _getData;
  GET_STRING_PROC _getFirstDate;
  GET_BOOL_PROC _canUnadjust;
  GET_BOOL_PROC _canAdjust;
  GET_STRING_PROC _getSupportedPeriods;
  GET_INT_PROC _getMaxConnections;
  GET_BOOL_PROC _ascendingSort;
  VOID_PROC _cancelData;
  SET_STRING_PROC _setProxyServerAddress;
  SET_STRING_PROC _setProxyServerUserName;
  SET_STRING_PROC _setProxyServerPassword;
  SET_UINT_PROC _setHTTPRequestTimeout;
  GET_INT_PROC _retries;
  PARSE_LINE_PROC _parseLine;

  std::set<Period> _supportedPeriods;

 public:
  YDataSourcePlugin(const std::wstring& path);
  void setSupportedPeriods();

 public:
  const TCHAR* getData(const std::wstring& symbol,
                       const yloader::DateRange& range, Period period,
                       bool adjusted, bool& error) const;

  bool canUnadjust() const { return (*_canUnadjust)(); }
  bool canAdjust() const { return (*_canAdjust)(); }
  int getMaxConnections() const { return (*_getMaxConnections)(); }
  void cancelData() const { return (*_cancelData)(); }
  void setProxyServerAddress(const TCHAR* address) {
    (*_setProxyServerAddress)(address);
  }
  void setProxyServerUserName(const TCHAR* userName) {
    (*_setProxyServerUserName)(userName);
  }
  void setProxyServerPassword(const TCHAR* password) {
    (*_setProxyServerPassword)(password);
  }
  void setHTTPRequestTimeout(unsigned int timeout) {
    (*_setHTTPRequestTimeout)(timeout);
  }
  bool ascendingSort() const { return (*_ascendingSort)(); }
  bool retries() const { return (*_retries)(); }
  bool parseLine(const TCHAR* line, bool adjusted, DatasourceBar* bar) const {
    return (*_parseLine)(line, adjusted, bar);
  }

  bool isPeriodSupported(Period period) const {
    return _supportedPeriods.find(period) != _supportedPeriods.end();
  }
};

class YDataOutputPlugin : public YPlugin {
  typedef int (*GET_YDO_PLUGIN)();

 public:
  YDataOutputPlugin(const std::wstring& path);
};

typedef boost::shared_ptr<YDataSourcePlugin> YDataSourcePluginPtr;
typedef boost::shared_ptr<YDataOutputPlugin> YDataOutputPluginPtr;

typedef std::map<yloader::UniqueId, YDataSourcePluginPtr> YPluginsBase;
typedef std::vector<YDataSourcePluginPtr> YDataSourcePluginVector;
typedef yloader::ManagedPtr<YDataSourcePluginVector> YDataSourcePluginVectorPtr;

class YDataSourcePlugins : public YPluginsBase {
 public:
  YDataSourcePluginVectorPtr getMatchingDataSources(
      const std::wstring& str) const;
};

typedef std::map<yloader::UniqueId, YDataOutputPluginPtr> YDataOutputPlugins;

class YPlugins : public yloader::PluginExplorer {
  YDataSourcePlugins _dsPlugins;
  YDataOutputPlugins _doPlugins;

 public:
  YPlugins() {}
  void load(const std::wstring& path);

  virtual void process(
      const std::wstring& path,
      yloader::PluginLoadingStatusHandler* loadingStatusHandler,
      std::vector<yloader::InfoPtr>& duplicates);

  const YDataSourcePlugins& getDataSourcePlugins() const { return _dsPlugins; }
  const YDataOutputPlugins& getDataOutputPlugins() const { return _doPlugins; }

  const YDataSourcePluginPtr getDataSourcePlugin(
      const yloader::UniqueId* id) const {
    if (id != 0) {
      YDataSourcePlugins::const_iterator i = _dsPlugins.find(*id);

      return i != _dsPlugins.end() ? i->second : YDataSourcePluginPtr();
    } else
      return YDataSourcePluginPtr();
  }

  const YDataSourcePluginPtr getDataSourcePlugin(
      const yloader::UniqueIdPtr id) const {
    return getDataSourcePlugin(id.get());
  }

  const YDataSourcePluginVectorPtr getMatchingDataSourcePlugins(
      const std::wstring& str) const {
    return _dsPlugins.getMatchingDataSources(str);
  }
};

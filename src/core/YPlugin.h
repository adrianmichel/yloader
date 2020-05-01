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

#pragma once

#include <crypt.h>
#include <plugin.h>
#include <ypluginhelper.h>
#include <hinstance.h>
#include <log.h>

using GetStringProc = const TCHAR* (*)();
using GetIntProc = int (*)();
using GetBoolProc = bool (*)();
using VoidProc = void (*)();
using SetStringProc = void (*)(const TCHAR*);
using SetUIntProc = void (*)(unsigned int);
using ValidationProc = bool (*)(const TCHAR*);
using ParseLineProc = bool (*)(const TCHAR*, bool adjusted, DatasourceBar* bar);

class YPluginInfo : public yloader::Info {
 private:
  const std::wstring m_shortName;
  const std::wstring m_developerURL;
  const std::wstring m_developerName;
  const yloader::Version m_version;
  const std::wstring m_email;
  const std::wstring m_updateURL;
  const std::wstring m_helpURL;
  const std::wstring m_developerEmail;
  const yloader::Version m_apiVersion;
  const std::wstring m_firstDate;

 public:
  YPluginInfo(const yloader::Info& info, const std::wstring& shortName,
              const std::wstring& developerURL,
              const std::wstring& developerEmail,
              const std::wstring& developerName, const std::wstring version,
              const std::wstring& apiVersion, const std::wstring& email,
              const std::wstring& updateURL, const std::wstring& helpURL,
              const std::wstring& firstDate)
      : yloader::Info(info),
        m_developerURL(developerURL),
        m_developerName(developerName),
        m_version(version),
        m_email(email),
        m_updateURL(updateURL),
        m_helpURL(helpURL),
        m_shortName(shortName),
        m_apiVersion(apiVersion),
        m_developerEmail(developerEmail),
        m_firstDate(firstDate) {}

  const std::wstring& developerURL() const { return m_developerURL; }
  const std::wstring& developerName() const { return m_developerName; }
  std::wstring version() const { return m_version.toString(); }
  const std::wstring& shortName() const { return m_shortName; }
  std::wstring apiVersion() const { return m_apiVersion.toString(); }
  const std::wstring& developerEmail() const { return m_developerEmail; }
  const std::wstring& firstDate() const { return m_firstDate; }
};

class YPluginBase : public HInstance {
 private:
  SetStringProc m_freeString;

 public:
  YPluginBase(const std::wstring& path);
  void freeString(const TCHAR* str) const { (*m_freeString)(str); }
};

class YPluginString {
 private:
  const TCHAR* m_string;
  const YPluginBase* m_plugin;

 public:
  YPluginString(const TCHAR* str, const YPluginBase* plugin)
      : m_string(str), m_plugin(plugin) {}

  ~YPluginString() { m_plugin->freeString(m_string); }

  operator const TCHAR*() { return m_string; }
};

class YPlugin : public YPluginBase, public YPluginInfo {
 public:
  YPlugin(const std::wstring& path) 
    try 
    : YPluginBase(path), YPluginInfo(getInfo()) {
    }
    catch (const HInstanceException& e) {
      LOG(log_debug, path, L" HInstanceException exception last error: ", e.getLastError());
      throw yloader::PluginException();
    }
    catch (const yloader::VersionException& ve) {
      LOG(log_error, path, L"  doesn't have version info (or malformed info): ", ve.message());
      throw yloader::PluginException();
    }
    catch (const HInstanceMethodException& e) {
      LOG(log_debug, __super::path(), L", exported function \"", e.name(), "\" not found");
    }

  ~YPlugin() {}

 protected:
  template <typename U>
  void f(const std::wstring& procName, U& u) {
    try {
      GetStringProc proc = reinterpret_cast<GetStringProc>(__super::getProcAddress(procName));
      YPluginString p(proc(), this);
      LOG(log_debug, path(), L", exported function \"", u, L"\" found");
      u = p;
    }
    catch (const HInstanceMethodException&) {
      LOG(log_debug, path(), L", exported function \"", u, L"\" not found");
    }
  }

 private:
  YPluginInfo getInfo();
};

using PluginPtr = std::shared_ptr<YPlugin>;
using GET_DATA_PROC = const TCHAR* (*)(const TCHAR* symbol, const TCHAR* startDate, const TCHAR* endDate, Period period,
                                      bool adjusted, bool& error, int retries);

constexpr uint32_t VERSION_KEY[4] = {0x3, 0x1, 0x0, 0x0};

class YDataSourcePlugin : public YPlugin {
  OBJ_COUNTER(YDataSourcePlugin)
 private:
  GET_DATA_PROC m_getData;
  GetStringProc m_getFirstDate;
  GetBoolProc m_canUnadjust;
  GetBoolProc m_canAdjust;
  GetStringProc m_getSupportedPeriods;
  GetIntProc m_getMaxConnections;
  GetBoolProc m_ascendingSort;
  VoidProc m_cancelData;
  SetStringProc m_setProxyServerAddress;
  SetStringProc m_setProxyServerUserName;
  SetStringProc m_setProxyServerPassword;
  SetUIntProc m_setHTTPRequestTimeout;
  GetIntProc m_retries;
  ParseLineProc m_parseLine;

  std::set<Period> m_supportedPeriods;

 public:
  YDataSourcePlugin(const std::wstring& path);
  void setSupportedPeriods();

 public:
  const TCHAR* getData(const std::wstring& symbol, const yloader::DateRange& range, Period period,
                       bool adjusted, bool& error) const;

  bool canUnadjust() const { return (*m_canUnadjust)(); }
  bool canAdjust() const { return (*m_canAdjust)(); }
  int getMaxConnections() const { return (*m_getMaxConnections)(); }
  void cancelData() const { return (*m_cancelData)(); }
  void setProxyServerAddress(const TCHAR* address) {
    (*m_setProxyServerAddress)(address);
  }
  void setProxyServerUserName(const TCHAR* userName) {
    (*m_setProxyServerUserName)(userName);
  }
  void setProxyServerPassword(const TCHAR* password) {
    (*m_setProxyServerPassword)(password);
  }
  void setHTTPRequestTimeout(unsigned int timeout) {
    (*m_setHTTPRequestTimeout)(timeout);
  }
  bool ascendingSort() const { return (*m_ascendingSort)(); }
  bool retries() const { return (*m_retries)(); }
  bool parseLine(const TCHAR* line, bool adjusted, DatasourceBar* bar) const {
    return (*m_parseLine)(line, adjusted, bar);
  }

  bool isPeriodSupported(Period period) const {
    return m_supportedPeriods.find(period) != m_supportedPeriods.end();
  }
};

class YDataOutputPlugin : public YPlugin {
  using GET_YDO_PLUGIN = int (*)();

 public:
  YDataOutputPlugin(const std::wstring& path);
};

using YDataSourcePluginPtr = std::shared_ptr<YDataSourcePlugin>;
using YDataOutputPluginPtr = std::shared_ptr<YDataOutputPlugin>;

using YPluginsBase = std::map<yloader::UniqueId, YDataSourcePluginPtr>;
using YDataSourcePluginVector = std::vector<YDataSourcePluginPtr>;
using YDataSourcePluginVectorPtr = std::shared_ptr<YDataSourcePluginVector>;

class YDataSourcePlugins : public YPluginsBase {
 public:
  YDataSourcePluginVectorPtr getMatchingDataSources(const std::wstring& str) const;
};

using YDataOutputPlugins = std::map<yloader::UniqueId, YDataOutputPluginPtr>;

class YPlugins : public yloader::PluginExplorer {
  YDataSourcePlugins m_dsPlugins;
  YDataOutputPlugins m_doPlugins;

 public:
  YPlugins() {}
  void load(const std::wstring& path);

  virtual void process(const std::wstring& path, yloader::PluginLoadingStatusHandler* loadingStatusHandler,
      std::vector<yloader::InfoPtr>& duplicates);

  const YDataSourcePlugins& getDataSourcePlugins() const { return m_dsPlugins; }
  const YDataOutputPlugins& getDataOutputPlugins() const { return m_doPlugins; }

  const YDataSourcePluginPtr getDataSourcePlugin(
      const yloader::UniqueId* id) const {
    if (id != 0) {
      YDataSourcePlugins::const_iterator i = m_dsPlugins.find(*id);

      return i != m_dsPlugins.end() ? i->second : YDataSourcePluginPtr();
    }
    else {
      return YDataSourcePluginPtr{};
    }
  }

  const YDataSourcePluginPtr getDataSourcePlugin(const yloader::UniqueIdPtr id) const {
    return getDataSourcePlugin(id.get());
  }

  const YDataSourcePluginVectorPtr getMatchingDataSourcePlugins(const std::wstring& str) const {
    return m_dsPlugins.getMatchingDataSources(str);
  }
};

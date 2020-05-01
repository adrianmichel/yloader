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

#include "StdAfx.h"
#include ".\yplugin.h"

constexpr auto GET_DATA_NAME = L"getData";
constexpr auto VALIDATION_NAME = L"validation";
constexpr auto GET_FIRST_DATE_NAME = L"getFirstDate";
constexpr auto GET_YDATASOURCE_PLUGIN_INFO_NAME = L"getYDataSourcePluginInfo";
constexpr auto GET_SUPPORTED_PERIODS_NAME = L"getSupportedPeriods";
constexpr auto GET_CAN_UNADJUST_NAME = L"getCanUnAdjust";
constexpr auto GET_CAN_ADJUST_NAME = L"getCanAdjust";
constexpr auto GET_MAX_CONNECTIONS_NAME = L"getMaxConnections";
constexpr auto CANCEL_DATA_NAME = L"cancelData";
constexpr auto SET_PROXY_SERVER_ADDRESS_NAME = L"setProxyServerAddress";
constexpr auto SET_PROXY_SERVER_USER_NAME = L"setProxyServerUserName";
constexpr auto SET_PROXY_SERVER_PASSWORD = L"setProxyServerPassword";
constexpr auto SET_HTTP_REQUEST_TIMEOUT = L"setHTTPRequestTimeout";
constexpr auto GET_ASCENDING_SORT_NAME = L"getAscendingSort";
constexpr auto FREE_STRING_NAME = L"freeString";
constexpr auto GET_ID_NAME = L"getId";
constexpr auto GET_SHORT_NAME_NAME = L"getShortName";
constexpr auto GET_NAME_NAME = L"getName";
constexpr auto GET_VERSION_NAME = L"getVersion";
constexpr auto GET_DESCRIPTION_NAME = L"getDescription";
constexpr auto GET_DEVELOPER_NAME = L"getDeveloperName";
constexpr auto GET_DEVELOPER_URL_NAME = L"getDeveloperURL";
constexpr auto GET_UPDATE_INFO_URL_NAME = L"getUpdateInfoURL";
constexpr auto GET_HELP_URL_NAME = L"getHelpURL";
constexpr auto GET_DEVELOPER_EMAIL_NAME = L"getDeveloperEmail";
constexpr auto GET_API_VERSION_NAME = L"getAPIVersion";
constexpr auto PLUGIN_EXTENSION = L"ypi";
constexpr auto GET_RETRIES = L"getRetries";
constexpr auto PARSE_LINE = L"parseLine";

YDataSourcePluginVectorPtr YDataSourcePlugins::getMatchingDataSources(const std::wstring& str) const {
  YDataSourcePluginVectorPtr v(new YDataSourcePluginVector());
  for (const auto& p : *this) {
    if (boost::icontains(p.second->shortName(), str)) {
      v->push_back(p.second);
    }
    else if (boost::icontains( p.second->id(), str)) {
      v->push_back(p.second);
    }
  }

  return v;
}

void YPlugins::process(const std::wstring& path, yloader::PluginLoadingStatusHandler* loadingStatusHandler, std::vector<yloader::InfoPtr>& duplicates) {
  LOG(log_debug, L"processing ", path);
  try {
    YDataSourcePluginPtr plugin(std::make_shared< YDataSourcePlugin >(path));

    m_dsPlugins.insert(YDataSourcePlugins::value_type(plugin->id(), plugin));
  }
  catch (const yloader::PluginException&) {
    LOG(log_debug, path, L" not a data source plugin");
  }

  try {
    YDataOutputPluginPtr plugin(std::make_shared< YDataOutputPlugin >(path));
    m_doPlugins.insert(YDataOutputPlugins::value_type(plugin->id(), plugin));
  } catch (const yloader::PluginException&) {
    LOG(log_debug, path, L" not a data source plugin");
  }
}

YDataOutputPlugin::YDataOutputPlugin(const std::wstring& path) 
try
    : YPlugin(path) {
  GET_YDO_PLUGIN doPlugin = reinterpret_cast<GET_YDO_PLUGIN>(__super::getProcAddress(GET_YDATASOURCE_PLUGIN_INFO_NAME));
}
catch (const HInstanceMethodException& e) {
  LOG(log_debug, __super::path(), L", exported function \"", e.name(), L"\" not found");
  // not a valid plugin
  throw yloader::PluginException();
}

void YDataSourcePlugin::setSupportedPeriods() {
  Tokenizer tokens((*m_getSupportedPeriods)(),  L" -,;\t");

  for (auto token : tokens) {
    m_supportedPeriods.insert((Period)_ttoi(token.c_str()));
  }
}

YPluginBase::YPluginBase(const std::wstring& path) : HInstance(path) {
  try {
    m_freeString = reinterpret_cast<SetStringProc>(__super::getProcAddress(FREE_STRING_NAME));
  }
  catch (const HInstanceMethodException&) {
    throw yloader::PluginException();
  }
}

YPluginInfo YPlugin::getInfo() {
  GetStringProc getId = reinterpret_cast<GetStringProc>(__super::getProcAddress(GET_ID_NAME));
  std::wstring id = (*getId)();
  LOG(log_debug, L"Path: ", __super::path(), L", plugin id: ", id);

  std::wstring shortName;
  std::wstring name;
  std::wstring description;
  std::wstring developerName;
  std::wstring developerURL;
  std::wstring updateInfoURL;
  std::wstring helpURL;
  std::wstring developerEmail;
  std::wstring version;
  std::wstring apiVersion;
  std::wstring firstDate;

  f(GET_SHORT_NAME_NAME, shortName);
  f(GET_NAME_NAME, name);
  f(GET_VERSION_NAME, version);
  f(GET_DESCRIPTION_NAME, description);
  f(GET_DEVELOPER_NAME, developerName);
  f(GET_DEVELOPER_URL_NAME, developerURL);
  f(GET_UPDATE_INFO_URL_NAME, updateInfoURL);
  f(GET_HELP_URL_NAME, helpURL);
  f(GET_DEVELOPER_EMAIL_NAME, developerEmail);
  f(GET_API_VERSION_NAME, apiVersion);
  f(GET_FIRST_DATE_NAME, firstDate);

  // todo: handle the case of a bad id - handle exception
  return YPluginInfo(yloader::Info(id.c_str(), name, description), shortName, developerURL, developerEmail, developerName, version,
                     apiVersion, developerEmail, updateInfoURL, helpURL, firstDate);
}

const TCHAR* YDataSourcePlugin::getData(const std::wstring& symbol, const yloader::DateRange& range, Period period, bool adjusted,
                                        bool& error) const {
  int retries = (*m_retries)();
  return (*m_getData)(symbol.c_str(), dateToUSString(range.first, firstDate()).c_str(),
      dateToUSString(range.second, yloader::LocalTimeSec().date()).c_str(), period, adjusted, error, retries);
}

void YPlugins::load(const std::wstring& path) {
  yloader::NullPluginLoadingStatusHandler handler;

  const std::wstring ext(PLUGIN_EXTENSION);

  // here we try to get the plugin extension from Armadillo (in release mode
  // protected). If empty, we get a bogus extension so it won't find any plugins
  explore(path, (ext.empty() ? L" ypi" : ext), true, &handler);
}

YDataSourcePlugin::YDataSourcePlugin(const std::wstring& path)
try
    : YPlugin(path), m_getData(0) {
  ValidationProc validation = reinterpret_cast<ValidationProc>(__super::getProcAddress(VALIDATION_NAME));

  yloader::Crypt crypt(VERSION_KEY, true, true);

  std::shared_ptr<std::string> enVersion = crypt.en(STRPRODUCTVER);

  if (!validation(yloader::s2ws(*enVersion).c_str())) {
    throw yloader::PluginException();
  }
  // these are mandatory methods, so we won't catch the exception here
  GetIntProc dsPlugin = reinterpret_cast<GetIntProc>(__super::getProcAddress(GET_YDATASOURCE_PLUGIN_INFO_NAME));

  m_getData =reinterpret_cast<GET_DATA_PROC>(__super::getProcAddress(GET_DATA_NAME));
  m_getFirstDate = reinterpret_cast<GetStringProc>(__super::getProcAddress(GET_FIRST_DATE_NAME));
  m_getSupportedPeriods = reinterpret_cast<GetStringProc>(__super::getProcAddress(GET_SUPPORTED_PERIODS_NAME));
  setSupportedPeriods();
  m_canUnadjust = reinterpret_cast<GetBoolProc>(__super::getProcAddress(GET_CAN_UNADJUST_NAME));
  m_canAdjust = reinterpret_cast<GetBoolProc>(__super::getProcAddress(GET_CAN_ADJUST_NAME));
  m_getMaxConnections = reinterpret_cast<GetIntProc>(__super::getProcAddress(GET_MAX_CONNECTIONS_NAME));
  m_cancelData = reinterpret_cast<VoidProc>(__super::getProcAddress(CANCEL_DATA_NAME));
  m_setProxyServerAddress = reinterpret_cast<SetStringProc>(__super::getProcAddress(SET_PROXY_SERVER_ADDRESS_NAME));
  m_setProxyServerUserName = reinterpret_cast<SetStringProc>(__super::getProcAddress(SET_PROXY_SERVER_USER_NAME));
  m_setProxyServerPassword = reinterpret_cast<SetStringProc>(__super::getProcAddress(SET_PROXY_SERVER_PASSWORD));
  m_setHTTPRequestTimeout = reinterpret_cast<SetUIntProc>(__super::getProcAddress(SET_HTTP_REQUEST_TIMEOUT));
  m_ascendingSort = reinterpret_cast<GetBoolProc>(__super::getProcAddress(GET_ASCENDING_SORT_NAME));
  m_retries =reinterpret_cast<GetIntProc>(__super::getProcAddress(GET_RETRIES));
  m_parseLine = reinterpret_cast<ParseLineProc>(__super::getProcAddress(PARSE_LINE));

}
catch (const HInstanceMethodException& e) {
  LOG(log_debug, __super::path(), L", exported function \"", e.name(), L"\" not found");

  // not a valid plugin
  throw yloader::PluginException();
}

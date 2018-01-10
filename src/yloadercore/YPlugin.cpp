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

#include "StdAfx.h"
#include ".\yplugin.h"

#define GET_DATA_NAME _T( "getData" )
#define VALIDATION_NAME _T( "validation" )
#define GET_FIRST_DATE_NAME _T( "getFirstDate" )
#define GET_YDATASOURCE_PLUGIN_INFO_NAME _T( "getYDataSourcePluginInfo" )
#define GET_SUPPORTED_PERIODS_NAME _T( "getSupportedPeriods" )
#define GET_CAN_UNADJUST_NAME _T( "getCanUnAdjust" )
#define GET_CAN_ADJUST_NAME _T( "getCanAdjust" )
#define GET_MAX_CONNECTIONS_NAME _T( "getMaxConnections" )
#define CANCEL_DATA_NAME _T( "cancelData" )
#define SET_PROXY_SERVER_ADDRESS_NAME _T( "setProxyServerAddress" )
#define SET_PROXY_SERVER_USER_NAME _T( "setProxyServerUserName" )
#define SET_PROXY_SERVER_PASSWORD _T( "setProxyServerPassword" )
#define SET_HTTP_REQUEST_TIMEOUT _T( "setHTTPRequestTimeout" )
#define GET_ASCENDING_SORT_NAME _T( "getAscendingSort" )
#define FREE_STRING_NAME _T( "freeString" )
#define GET_ID_NAME _T( "getId" )
#define GET_SHORT_NAME_NAME _T( "getShortName" )
#define GET_NAME_NAME _T( "getName" )
#define GET_VERSION_NAME _T( "getVersion" )
#define GET_DESCRIPTION_NAME _T( "getDescription" )
#define GET_DEVELOPER_NAME _T( "getDeveloperName" )
#define GET_DEVELOPER_URL_NAME _T( "getDeveloperURL" )
#define GET_UPDATE_INFO_URL_NAME _T( "getUpdateInfoURL" )
#define GET_HELP_URL_NAME _T( "getHelpURL" )
#define GET_DEVELOPER_EMAIL_NAME _T( "getDeveloperEmail" )
#define GET_API_VERSION_NAME _T( "getAPIVersion" )
#define PLUGIN_EXTENSION _T( "ypi" )
#define GET_RETRIES _T( "getRetries" )
#define PARSE_LINE _T( "parseLine" )

YDataSourcePluginVectorPtr YDataSourcePlugins::getMatchingDataSources(
    const std::wstring& str) const {
  YDataSourcePluginVectorPtr v(new YDataSourcePluginVector());
  for (YPluginsBase::const_iterator i = __super::begin(); i != __super::end();
       i++) {
    if (yloader::to_lower_case(i->second->shortName())
            .find(yloader::to_lower_case(str)) == 0)
      v->push_back(i->second);
    else if (yloader::to_lower_case(i->second->id())
                 .find(yloader::to_lower_case(str)) == 0)
      v->push_back(i->second);
  }

  return v;
}

void YPlugins::process(
    const std::wstring& path,
    yloader::PluginLoadingStatusHandler* loadingStatusHandler,
    std::vector<yloader::InfoPtr>& duplicates) {
  LOG(log_debug, _T( "processing " ) << path)
  try {
    YDataSourcePluginPtr plugin(new YDataSourcePlugin(path));

    { _dsPlugins.insert(YDataSourcePlugins::value_type(plugin->id(), plugin)); }
  } catch (const yloader::PluginException&) {
    LOG(log_debug, path << _T( " not a data source plugin" ))
  }

  try {
    YDataOutputPluginPtr plugin(new YDataOutputPlugin(path));
    _doPlugins.insert(YDataOutputPlugins::value_type(plugin->id(), plugin));
  } catch (const yloader::PluginException&) {
    LOG(log_debug, path << _T( " not a data source plugin" ))
  }
}

YDataOutputPlugin::YDataOutputPlugin(const std::wstring& path) try
    : YPlugin(path) {
  GET_YDO_PLUGIN doPlugin = reinterpret_cast<GET_YDO_PLUGIN>(
      __super::getProcAddress(GET_YDATASOURCE_PLUGIN_INFO_NAME));
} catch (const HInstanceMethodException& e) {
  LOG(log_debug, __super::path() << _T( ", exported function \"" ) << e.name()
                                 << _T( "\" not found" ));
  // not a valid plugin
  throw yloader::PluginException(_T(""), _T(""));
}

void YDataSourcePlugin::setSupportedPeriods() {
  Tokenizer tokens((*_getSupportedPeriods)(), _T( " -,;\t" ));

  for (Tokenizer::iterator i = tokens.begin(); i != tokens.end(); i++) {
    _supportedPeriods.insert((Period)_ttoi(i->c_str()));
  }
}

YPluginBase::YPluginBase(const std::wstring& path) : HInstance(path) {
  try {
    _freeString = reinterpret_cast<SET_STRING_PROC>(
        __super::getProcAddress(FREE_STRING_NAME));
  } catch (const HInstanceMethodException&) {
    throw yloader::PluginException(_T( "" ), _T( "" ));
  }
}

YPluginInfo YPlugin::getInfo() {
  GET_STRING_PROC getId =
      reinterpret_cast<GET_STRING_PROC>(__super::getProcAddress(GET_ID_NAME));
  std::wstring id = (*getId)();
  LOG(log_debug,
      _T( "Path: " ) << __super::path() << _T( ", plugin id: " ) << id);

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
  return YPluginInfo(yloader::Info(id.c_str(), name, description), shortName,
                     developerURL, developerEmail, developerName, version,
                     apiVersion, developerEmail, updateInfoURL, helpURL,
                     firstDate);
}

const TCHAR* YDataSourcePlugin::getData(const std::wstring& symbol,
                                        const yloader::DateRange& range,
                                        Period period, bool adjusted,
                                        bool& error) const {
  int retries = (*_retries)();
  return (*_getData)(
      symbol.c_str(), dateToUSString(range.first, firstDate()).c_str(),
      dateToUSString(range.second, yloader::LocalTimeSec().date()).c_str(),
      period, adjusted, error, retries);
}

void YPlugins::load(const std::wstring& path) {
  yloader::NullPluginLoadingStatusHandler handler;

  const std::wstring ext(PLUGIN_EXTENSION);

  // here we try to get the plugin extension from Armadillo (in release mode
  // protected). If empty, we get a bogus extension so it won't find any plugins
  explore(path, (ext.empty() ? _T( " ypi" ) : ext), true, &handler);
}

YDataSourcePlugin::YDataSourcePlugin(const std::wstring& path) try
    : YPlugin(path),
      _getData(0) {
  VALIDATION_PROC validation = reinterpret_cast<VALIDATION_PROC>(
      __super::getProcAddress(VALIDATION_NAME));

  yloader::Crypt crypt(VERSION_KEY, true, true);

  boost::shared_ptr<std::string> enVersion = crypt.en(STRPRODUCTVER);

  if (!validation(yloader::s2ws(*enVersion).c_str()))
    throw yloader::PluginException(_T(""), _T(""));
  // these are mandatory methods, so we won't catch the exception here
  GET_INT_PROC dsPlugin = reinterpret_cast<GET_INT_PROC>(
      __super::getProcAddress(GET_YDATASOURCE_PLUGIN_INFO_NAME));

  _getData =
      reinterpret_cast<GET_DATA_PROC>(__super::getProcAddress(GET_DATA_NAME));
  _getFirstDate = reinterpret_cast<GET_STRING_PROC>(
      __super::getProcAddress(GET_FIRST_DATE_NAME));
  _getSupportedPeriods = reinterpret_cast<GET_STRING_PROC>(
      __super::getProcAddress(GET_SUPPORTED_PERIODS_NAME));
  setSupportedPeriods();
  _canUnadjust = reinterpret_cast<GET_BOOL_PROC>(
      __super::getProcAddress(GET_CAN_UNADJUST_NAME));
  _canAdjust = reinterpret_cast<GET_BOOL_PROC>(
      __super::getProcAddress(GET_CAN_ADJUST_NAME));
  _getMaxConnections = reinterpret_cast<GET_INT_PROC>(
      __super::getProcAddress(GET_MAX_CONNECTIONS_NAME));
  _cancelData =
      reinterpret_cast<VOID_PROC>(__super::getProcAddress(CANCEL_DATA_NAME));
  _setProxyServerAddress = reinterpret_cast<SET_STRING_PROC>(
      __super::getProcAddress(SET_PROXY_SERVER_ADDRESS_NAME));
  _setProxyServerUserName = reinterpret_cast<SET_STRING_PROC>(
      __super::getProcAddress(SET_PROXY_SERVER_USER_NAME));
  _setProxyServerPassword = reinterpret_cast<SET_STRING_PROC>(
      __super::getProcAddress(SET_PROXY_SERVER_PASSWORD));
  _setHTTPRequestTimeout = reinterpret_cast<SET_UINT_PROC>(
      __super::getProcAddress(SET_HTTP_REQUEST_TIMEOUT));
  _ascendingSort = reinterpret_cast<GET_BOOL_PROC>(
      __super::getProcAddress(GET_ASCENDING_SORT_NAME));
  _retries =
      reinterpret_cast<GET_INT_PROC>(__super::getProcAddress(GET_RETRIES));
  _parseLine =
      reinterpret_cast<PARSE_LINE_PROC>(__super::getProcAddress(PARSE_LINE));

} catch (const HInstanceMethodException& e) {
  LOG(log_debug, __super::path() << _T( ", exported function \"" ) << e.name()
                                 << _T( "\" not found" ));

  // not a valid plugin
  throw yloader::PluginException(_T( ""), _T( "" ));
}

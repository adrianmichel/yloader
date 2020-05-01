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


enum StartMode { START_NORMAL, START_AUTO };
enum ExitMode { EXIT_NORMAL, EXIT_AUTO };

#ifdef YDSPLUGIN_EXPORTS
#define YDSPLUGIN_API __declspec(dllexport)
#else
#define YDSPLUGIN_API __declspec(dllimport)
#endif

enum Period { daily, weekly, monthly, quarterly, yearly, max_period };

extern "C" {
// plug-in info
YDSPLUGIN_API const TCHAR* getShortName();
YDSPLUGIN_API const TCHAR* getName();
YDSPLUGIN_API const TCHAR* getDescription();
YDSPLUGIN_API const TCHAR* getId();
YDSPLUGIN_API const TCHAR* getDeveloperName();
YDSPLUGIN_API const TCHAR* getDeveloperURL();
YDSPLUGIN_API const TCHAR* getUpdateInfoURL();
YDSPLUGIN_API const TCHAR* getHelpURL();
YDSPLUGIN_API const TCHAR* getDeveloperEmail();
YDSPLUGIN_API const TCHAR* getVersion();
YDSPLUGIN_API const TCHAR* getAPIVersion();
YDSPLUGIN_API const TCHAR* getPluginAPIVersion();

YDSPLUGIN_API const TCHAR* getYDataSourcePluginInfo();
YDSPLUGIN_API const TCHAR* getYDataSourcePluginOutput();
YDSPLUGIN_API void showSettingsDialog();
YDSPLUGIN_API const TCHAR* getParametersToSave();
YDSPLUGIN_API void setSavedParameters(const TCHAR*);
YDSPLUGIN_API const TCHAR* getIcon(unsigned int* size);

// plug-in capabilities
YDSPLUGIN_API unsigned int getMaxConnections();
YDSPLUGIN_API bool getCanAdjust();
YDSPLUGIN_API bool getCanUnAdjust();
YDSPLUGIN_API bool getCanGetSymbolLongName();
YDSPLUGIN_API const TCHAR* getFirstDate();
// indicates the direction the downloaded data is sorted.
YDSPLUGIN_API bool getAscendingSort();

YDSPLUGIN_API bool validation(const TCHAR* str);

// a string containing the periods separated by space:
// 	- 0: daily
// 	- 1: weekly
//  - 2: monthly
// 	- 3: quartely
//  - 4: yearly
//
// Ex: "0 1 2" - plug-in supports daily, weekly, monthly
YDSPLUGIN_API const TCHAR* getSupportedPeriods();

YDSPLUGIN_API const TCHAR* getData(const TCHAR* symbol, const TCHAR* startDate,
                                   const TCHAR* endDate, Period period,
                                   bool adjusted, bool& error, int retries);
YDSPLUGIN_API void cancelData();
YDSPLUGIN_API void setProxyServerAddress(const TCHAR* address);
YDSPLUGIN_API void setProxyServerUserName(const TCHAR* userName);
YDSPLUGIN_API void setProxyServerPassword(const TCHAR* password);
YDSPLUGIN_API void setHTTPRequestTimeout(unsigned int);

// this is usually the company name
YDSPLUGIN_API const TCHAR* getSymbolLongName(const TCHAR* symbol);

// helper
YDSPLUGIN_API void freeString(const TCHAR* str);
YDSPLUGIN_API int getRetries();

struct DatasourceBar {
  TCHAR date[20];
  double open;
  double high;
  double low;
  double close;
  __int64 volume;
};

YDSPLUGIN_API bool parseLine(const TCHAR* line, bool adjusted,
                             DatasourceBar* bar);
}

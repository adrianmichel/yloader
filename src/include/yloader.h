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

/*#ifdef YDOWNLOADERLIB_EXPORTS
#define YDOWNLOADERLIB_API __declspec(dllexport)
#else
#define YDOWNLOADERLIB_API __declspec(dllimport)
#endif*/
#define YDOWNLOADERLIB_API
extern "C" {
/**
 * Creates and runs the Yahoo quotes downloader dialog box
 *
 *
 * @param dialog   input - must be true if the downloader is to be used as a
dialog box within an application
 *                 or false if it is to be used as the main window the
application.
 * @param autoExit input - if true, will force the downloader to close upon
completion of downloading of all symbols, if auto start downloading is also set.
Default is false
 * @param autoStart
             input - if true, will force the downloader to autostart downloading
without waiting for user input. If there are file errors (symbols file or output
directory not found), the downloading will not start though.
 * @param symbolsFileName
 *                 input - the name of a symbols file name to be used instead of
the default value.
 * @param dataPath the path where to save the downloaded data, to override the
default path set on the downloader main window
* @param errorMessage
             output - the address of a string buffer to receive error messages
upon return. The calling application is responsible for allocating and freeing
this buffer.
* @param errorMessageLength
             input - the length of the error message buffer allocated by the
caller
 *
 * @return not used
 */
YDOWNLOADERLIB_API bool __stdcall yahooDownloadDlg(
    bool dialog, bool autoExit, bool autoStart, const char* symbolsFileName,
    const char* dataPath, char* errorMessage, unsigned int errorMessageLength);

/**
 * Creates a downloader.
 *
 * If the call succeds, it will return an integer value
 * greater than 0 which is the downloader id and which will be used in all
 * subsequent API calls.
 *
 * If the return value is 0, the call has failed and a downloader could
 * not be created. Use getLastError to get more information on the error.
 *
 * @param mode   Currently not used. Must be set to 0.
 *
 * @return Downloader id - 0 if the call failed, > 0 if the call succeded.
 */
YDOWNLOADERLIB_API unsigned int __stdcall create(unsigned int mode);

enum StartMode { START_NORMAL, START_AUTO };

enum ExitMode { EXIT_NORMAL, EXIT_AUTO };

/**
 * Shows a previously created downloader.
 *
 * For a GUI downloader, that will bring up the downloder
 * window.
 *
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @param mode
 *
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall start(unsigned int downloaderId,
                                        StartMode startMode, ExitMode exitMode);

/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall isRunning(unsigned int downloaderId,
                                            bool* isRunning);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall getLastError(char* error,
                                               unsigned int errorMessageLength);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall clearLastError();
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall startSession(unsigned int downloaderId,
                                               bool sync);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall isSessionActive(unsigned int downloaderId,
                                                  bool* isSessionActive);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall hasHadSession(unsigned int downloaderId,
                                                bool* hasHadSession);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setSymbolsFileName(
    unsigned int downloaderId, const char* symbolsFileName);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setDataPath(unsigned int downloaderId,
                                              const char* dataPath);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setDateRange(unsigned int downloaderId,
                                               const char* startDate,
                                               const char* endDate);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setStartDate(unsigned int downloaderId,
                                               const char* startDate);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setEndDate(unsigned int downloaderId,
                                             const char* endDate);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @param dataSource
 *                Can be one of
 *                  - the data source short name or the first
 *                    characters of the short name that uniquely
 *                    indentify the plug-in, for example:
 *                    "Y" (if there is no other plug-in whose
 *                    short name starts with "Y"), or "Ya", or
 *                    "Yahoo"
 *                  - the data source id or the first characters
 *                    of the id that uniquely identify the
 *                    plug-in
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setDataSourcePlugin(unsigned int downloaderId,
                                                      const char* dataSource);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall loadSettings(unsigned int downloaderId,
                                               const char* settingsName);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setAllAvailable(unsigned int downloaderId,
                                                  bool allAvailable);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setUpdate(unsigned int downloaderId,
                                            bool update);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setAdjust(unsigned int downloaderId,
                                            bool adjust);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setValidatePrices(unsigned int downloaderId,
                                                    bool validate);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setPeriod(unsigned int downloaderId,
                                            unsigned int period);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall setDateFormat(unsigned int downloaderId,
                                                unsigned int dateFormat);
YDOWNLOADERLIB_API bool __stdcall setDateSeparator(unsigned int downloaderId,
                                                   const char* dateSeparator);
YDOWNLOADERLIB_API bool __stdcall setAddSymbolToColumn(
    unsigned int downloaderId, bool add, unsigned int colum);

YDOWNLOADERLIB_API bool __stdcall setSortBarsAscending(
    unsigned int downloaderId, bool sortBarsAscending);
YDOWNLOADERLIB_API bool __stdcall setPadDateFields(unsigned int downloaderId,
                                                   bool padDateFields);
YDOWNLOADERLIB_API bool __stdcall setDataFileHeader(unsigned int downloaderId,
                                                    const char* dataFileHeader);
YDOWNLOADERLIB_API bool __stdcall setInvalidDataHandling(
    unsigned int downloaderId, unsigned int invalidDataHandling);
YDOWNLOADERLIB_API bool __stdcall setVolume0Handling(unsigned int downloaderId,
                                                     bool volume0Handling);
YDOWNLOADERLIB_API bool __stdcall setPrependToDataFileName(
    unsigned int downloaderId, const char* prepend);
YDOWNLOADERLIB_API bool __stdcall setAppendToDataFileName(
    unsigned int downloaderId, const char* append);
YDOWNLOADERLIB_API bool __stdcall setExtension(unsigned int downloaderId,
                                               const char* extension);
YDOWNLOADERLIB_API bool __stdcall setConnections(unsigned int downloaderId,
                                                 unsigned int connections);
YDOWNLOADERLIB_API bool __stdcall setOutputFile(unsigned int downloaderId,
                                                const char* outputFile);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall startSession(unsigned int downloaderId,
                                               bool sync);
/**
 * @param downloaderId
 *               The downloader id, an integer > 0 obtained through a previous
 *               call to create.
 * @return true if succesful, false if failed. Call getLastError for more info
 * in case of a failed call.
 */
YDOWNLOADERLIB_API bool __stdcall close(unsigned int downloaderId);

YDOWNLOADERLIB_API bool __stdcall cancel(unsigned int downloaderId);
YDOWNLOADERLIB_API bool __stdcall isSessionCanceling(unsigned int downloaderId,
                                                     bool* isSessionCanceling);
YDOWNLOADERLIB_API bool __stdcall clearSettings(unsigned int downloaderId);
YDOWNLOADERLIB_API bool __stdcall saveSettings(unsigned int downloaderId);
YDOWNLOADERLIB_API bool __stdcall saveSettingsAs(unsigned int downloaderId,
                                                 const TCHAR* settingsName);

YDOWNLOADERLIB_API bool __stdcall getSessionStats(unsigned int downloaderId);
YDOWNLOADERLIB_API bool __stdcall setNoReloadOldData(unsigned int downloaderId,
                                                     bool noReloadOldData);
YDOWNLOADERLIB_API bool __stdcall setCreateSubdirs(unsigned int downloaderId,
                                                   bool createSubdirs);
YDOWNLOADERLIB_API bool __stdcall setFieldSeparator(
    unsigned int downloaderId, const TCHAR* fieldSeparator);
YDOWNLOADERLIB_API bool __stdcall setVolumeMultiplier(unsigned int downloaderId,
                                                      double volumeMultiplier);
YDOWNLOADERLIB_API bool __stdcall setLogFile(unsigned int downloaderId,
                                             const TCHAR* logFile);
YDOWNLOADERLIB_API bool __stdcall setAppendToLog(unsigned int downloaderId,
                                                 bool appendToLog);
YDOWNLOADERLIB_API bool __stdcall setLogOnlyErrors(unsigned int downloaderId,
                                                   bool logOnlyErrors);
YDOWNLOADERLIB_API bool __stdcall setRegexMatch(unsigned int downloaderId,
                                                const TCHAR* regexMatch);
YDOWNLOADERLIB_API bool __stdcall setRegexFormat(unsigned int downloaderId,
                                                 const TCHAR* regexFormat);
YDOWNLOADERLIB_API bool __stdcall setRegexFlags(unsigned int downloaderId,
                                                unsigned long regexFlags);
YDOWNLOADERLIB_API bool __stdcall setEnableRegexFormatting(
    unsigned int downloaderId, bool enable);
YDOWNLOADERLIB_API bool __stdcall setErrorSymbolsListFileName(
    unsigned int downloaderId, TCHAR* errorSymbolsListFileName);
YDOWNLOADERLIB_API bool __stdcall setAppendToErrorSymbolsList(
    unsigned int downloaderId, bool append);
YDOWNLOADERLIB_API bool __stdcall setIgnoreErrorSymbolsList(
    unsigned int downloaderId, bool ignore);
YDOWNLOADERLIB_API bool __stdcall setIgnoreSymbolsList(
    unsigned int downloaderId, const TCHAR* symbolsList);

YDOWNLOADERLIB_API bool __stdcall cl();
}

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

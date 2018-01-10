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

class DownloaderException {
 private:
  const std::wstring _message;

 public:
  DownloaderException(const std::wstring& message) : _message(message) {}

  const std::wstring& message() const { return _message; }
};

class Downloader {
 public:
  virtual ~Downloader() {}

  virtual bool show(StartMode mode, ExitMode exitMode) = 0;
  virtual bool isRunning() const = 0;
  virtual bool loadSettings(const std::wstring& settingsName) = 0;
  virtual bool isSessionActive() const = 0;
  virtual bool setSymbolsFileName(const std::wstring& symbolsFileName) = 0;
  virtual bool setDataPath(const std::wstring& dataPath) = 0;
  virtual bool setDateRange(const std::wstring& startDate,
                            const std::wstring& endDate) = 0;
  virtual bool setStartDate(const std::wstring& startDate) = 0;
  virtual bool setEndDate(const std::wstring& endDate) = 0;
  virtual bool setDataSourcePlugin(const std::wstring& dataSource) = 0;
  virtual bool setAllAvailable(bool allAvailable) = 0;
  virtual bool setUpdate(bool update) = 0;
  virtual bool setAdjust(bool adjust) = 0;
  virtual bool setValidatePrices(bool validate) = 0;
  virtual bool setPeriod(unsigned int period) = 0;
  virtual bool setConnections(unsigned int connections) = 0;
  virtual bool setDateFormat(unsigned int dateFormat) = 0;
  virtual bool setAddSymbolToColumn(bool add, unsigned int column) = 0;
  virtual bool setDateSeparator(const std::wstring& dateSeparator) = 0;
  virtual bool startSession(bool sync) = 0;
  virtual bool setSortBarsAscending(bool sortAscending) = 0;
  virtual bool setPadDateFields(bool padDateFields) = 0;
  virtual bool setDataFileHeader(const std::wstring& dataFileHeader) = 0;
  virtual bool setInvalidDataHandling(unsigned int invalidDataHandling) = 0;
  virtual bool setVolume0Handling(bool volume0Handling) = 0;
  virtual bool setPrependToDataFileName(
      const std::wstring& prependToDataFileName) = 0;
  virtual bool setAppendToDataFileName(
      const std::wstring& appendToDataFileName) = 0;
  virtual bool setExtension(const std::wstring& extension) = 0;
  virtual bool setOutputFile(const std::wstring& outputFile) = 0;
  virtual bool setNoReloadOldData(bool noReloadOldData) = 0;
  virtual bool setCreateSubdirs(bool createSubdirs) = 0;
  virtual bool setFieldSeparator(const std::wstring& fieldSeparator) = 0;
  virtual bool setLogFile(const std::wstring& logFile) = 0;
  virtual bool setVolumeMultiplier(double volumeMultiplier) = 0;
  virtual bool setAppendToLog(bool appendToLog) = 0;
  virtual bool setLogOnlyErrors(bool logOnlyErrors) = 0;
  virtual bool setRegexMatch(const std::wstring& regexMatch) = 0;
  virtual bool setRegexFormat(const std::wstring& regexFormat) = 0;
  virtual bool setRegexFlags(unsigned long regexFlags) = 0;
  virtual bool setEnableRegexFormatting(bool enable) = 0;
  virtual bool setErrorSymbolsListFileName(
      const std::wstring& errorSymbolsListFileName) = 0;
  virtual bool setAppendToErrorSymbolsList(bool append) = 0;
  virtual bool setIgnoreErrorSymbolsList(bool ignore) = 0;
  virtual bool setIgnoreSymbolsList(const std::wstring& symbolsList) = 0;

  virtual bool closeDownloader(bool exc = true) = 0;
  virtual bool hasHadSession() = 0;
};

typedef yloader::ManagedPtr<Downloader> DownloaderPtr;

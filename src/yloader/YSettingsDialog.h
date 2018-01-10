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
#include "settingsdlg.h"

#include "dataformattingsettingspage.h"
#include "datavalidationsettingspage.h"
#include "generalsettingspage.h"
#include "loginsettingspage.h"
#include "outputsettingspage.h"
#include "pluginsettingspage.h"
#include "symbolssettingspage.h"

class YSettingsDialog : public CSettingsDialog {
 public:
  YSettingsDialog(
      const YPlugins& plugins,
      const yloader::UniqueId* selectedDataSourcePlugin,
      bool addSymbolsToColumn, unsigned int columnNumber,
      const std::wstring& prependToFileName,
      const std::wstring& appendToFileName, bool checkForUpdatesAtStartup,
      const std::wstring& fileNameExtension, const std::wstring& logFile,
      UINT threads, bool appendToLog, bool padDate, int dateFormat,
      const std::wstring& fieldSeparator, bool sortBarsInAscendingOrder,
      bool autoStartDownloading, unsigned int symbolTimeout, bool logOnlyErrors,
      bool createSubdirs, int invalidDataHandling, bool volume0invalid,
      bool hideNews, bool autoSelectOutputPath,
      const std::wstring& autoSelectOutputPathName, bool diagnosticLogging,
      const CharactersMapping& cm, double volumeMultiplier,
      const std::wstring& fileHeader, const std::wstring& dateSeparator,
      const std::wstring& proxyServerAddress,
      const std::wstring& proxyServerUserName,
      const std::wstring& proxyServerPassword, unsigned int httpTimeout,
      const std::wstring& openDataFileWithithApp,
      bool dontReloadOldDataInUpdateMode, bool minimizeToTray,
      bool showInitialMB, bool enableRegexFormatting, bool unlockMatchRegex,
      const std::wstring& matchRegex, const std::wstring& formatString,
      const std::wstring& errorSymbolsFile, bool appendToErrorSymbolsFile,
      bool ignoreErrorSymbolsList, const std::wstring& ignoreSymbolsList,
      CWnd* parent);

  ~YSettingsDialog(void);

  virtual BOOL OnInitDialog();

  bool addSymbolsToColumn() const { return m_dfs->addSymbolsToColumn(); }
  unsigned int columnNumber() const { return m_dfs->columnNumber(); }
  const CString& prependToFileName() const {
    return m_dffs->prependToFileName();
  }
  const CString& appendToFileName() const { return m_dffs->appendToFileName(); }
  const CString& fileNameExtension() const {
    return m_dffs->fileNameExtension();
  }
  bool checkForUpdatesAtStartup() const {
    return m_gs->checkForUpdatesAtStartup();
  }
  UINT threads() const {
    return (!_disableExtraFeatures) ? m_gs->threads() : 1;
  }
  const CString logFile() const {
    return (!_disableExtraFeatures) ? m_ls->logFile() : _T( "" );
  }
  bool appendToLog() const { return m_ls->appendToLog(); }
  bool padDate() const { return m_dfs->padDate(); }
  int dateFormat() const { return m_dfs->dateFormat(); }
  std::wstring fieldSeparator() const { return m_dfs->fieldSeparator(); }
  bool sortBarsInAscendingOrder() const {
    return m_dffs->sortBarsInAscendingOrder();
  }
  bool autoStartDownloading() const {
    return (!_disableExtraFeatures) ? m_gs->autoStartDownloading() : false;
  }
  //	unsigned int symbolTimeout() const { return ; }
  bool logOnlyErrors() const { return m_ls->logOnlyErrors(); }
  bool createSubdirs() const { return m_dffs->createSubdirs(); }
  bool volume0invalid() const { return m_dvs->volume0invalid(); }
  int invalidDataHandling() const { return m_dvs->invalidDataHandling(); }
  //	bool hideNews() const { return m_hideNews != 0; }
  bool minimizeToTray() const { return m_gs->minimizeToTray(); }
  bool autoSelectOutputPath() const { return m_dffs->autoSelectOutputPath(); }
  const CString& autoSelectOutputPathName() const {
    return m_dffs->autoSelectOutputPathName();
  }
  bool diagnosticLogging() const { return m_ls->diagnosticLogging(); }
  CharactersMapping characterMapping() const {
    return m_dffs->characterMapping();
  }
  double volumeMultiplier() const { return m_dfs->volumeMultiplier(); }
  std::wstring fileHeader() const { return m_dffs->fileHeader(); }
  std::wstring dateSeparator() const { return m_dfs->dateSeparator(); }
  std::wstring proxyServerAddress() const { return m_gs->proxyServerAddress(); }
  std::wstring proxyServerUserName() const {
    return m_gs->proxyServerUserName();
  }
  std::wstring proxyServerPassword() const {
    return m_gs->proxyServerPassword();
  }
  unsigned int httpRequestTimeout() const { return m_gs->httpRequestTimeout(); }
  std::wstring openDataFileWithApp() const {
    return m_gs->openDataFileWithApp();
  }
  bool dontReloadOldDataInUpdateMode() const {
    return m_gs->dontReloadOldDataInUpdateMode();
  }
  bool enableRegexFormatting() const { return m_dfs->enableRegexFormatting(); }
  bool matchRegexUnlocked() const { return m_dfs->unlockMatchRegex(); }
  std::wstring matchRegex() const { return m_dfs->matchRegex(); }
  std::wstring formatString() const { return m_dfs->formatString(); }
  std::wstring errorSymbolsFile() const { return m_slls->errorSymbolsFile(); }
  bool appendToErrorSymbolsFile() const {
    return m_slls->appendToErrorSymbolsFile();
  }
  bool ignoreErrorSymbolsList() const {
    return m_slls->ignoreErrorSymbolsList();
  }
  std::wstring ignoreSymbolsList() const { return m_slls->ignoreSymbolsList(); }

  const yloader::UniqueId* getSelectedDataSourcePlugin() const {
    return m_ps->getSelectedDataSourcePlugin();
  }

 private:
  bool _enableExtraFeatures;
  bool _disableExtraFeatures;
  CPluginSettings* m_ps;
  CGeneralSettings* m_gs;
  CDataFormattingSettings* m_dfs;
  CDataValidationSettings* m_dvs;
  CLoggingSettings* m_ls;
  CDataFileSettings* m_dffs;
  CSymbolsListsSettings* m_slls;
  void OnOK();

  bool _showInitialMB;
  bool _hasSelectedPlugin;
};

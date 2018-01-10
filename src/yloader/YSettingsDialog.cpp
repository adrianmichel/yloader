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

#include "stdafx.h"
#include "ysettingsdialog.h"

YSettingsDialog::YSettingsDialog(
    const YPlugins& plugins, const yloader::UniqueId* selectedDataSourcePlugin,
    bool addSymbolsToColumn, unsigned int columnNumber,
    const std::wstring& prependToFileName, const std::wstring& appendToFileName,
    bool checkForUpdatesAtStartup, const std::wstring& fileNameExtension,
    const std::wstring& logFile, UINT threads, bool appendToLog, bool padDate,
    int dateFormat, const std::wstring& fieldSeparator,
    bool sortBarsInAscendingOrder,
    /*bool enableExtraFeatures, */ bool autoStartDownloading,
    unsigned int symbolTimeout, bool logOnlyErrors, bool createSubdirs,
    int invalidDataHandling, bool volume0invalid, bool hideNews,
    bool autoSelectOutputPath, const std::wstring& autoSelectOutputPathName,
    bool diagnosticLogging, const CharactersMapping& cm,
    double volumeMultiplier, const std::wstring& fileHeader,
    const std::wstring& dateSeparator, const std::wstring& proxyServerAddress,
    const std::wstring& proxyServerUserName,
    const std::wstring& proxyServerPassword, unsigned int httpTimeout,
    const std::wstring& openDataFileWithithApp,
    bool dontReloadOldDataInUpdateMode, bool minimizeToTray

    ,
    bool showInitialMB, bool enableRegexFormatting, bool unlockMatchRegex,
    const std::wstring& matchRegex, const std::wstring& formatString,
    const std::wstring& errorSymbolsFile, bool appendToErrorSymbolsFile,
    bool ignoreErrorSymbolsList, const std::wstring& ignoreSymbolsList,
    CWnd* parent)
    : /*_enableExtraFeatures( enableExtraFeatures ), */ _disableExtraFeatures(
          false),
      CSettingsDialog(parent),
      _showInitialMB(showInitialMB),
      _hasSelectedPlugin(
          selectedDataSourcePlugin &&
          plugins.getDataSourcePlugin(selectedDataSourcePlugin)) {
  m_ps = dynamic_cast<CPluginSettings*>(AddPage(RUNTIME_CLASS(CPluginSettings),
                                                _T("Data Sources"),
                                                CPluginSettings::IDD));
  m_gs = dynamic_cast<CGeneralSettings*>(
      AddPage(RUNTIME_CLASS(CGeneralSettings), _T( "General Preferences" ),
              CGeneralSettings::IDD));
  m_dfs = dynamic_cast<CDataFormattingSettings*>(
      AddPage(RUNTIME_CLASS(CDataFormattingSettings), _T( "Formatting" ),
              CDataFormattingSettings::IDD));
  m_dvs = dynamic_cast<CDataValidationSettings*>(
      AddPage(RUNTIME_CLASS(CDataValidationSettings), _T("Validation"),
              CDataValidationSettings::IDD));
  m_dffs = dynamic_cast<CDataFileSettings*>(
      AddPage(RUNTIME_CLASS(CDataFileSettings), _T( "Output" ),
              CDataFileSettings::IDD, _T( "Session Settings" )));
  m_slls = dynamic_cast<CSymbolsListsSettings*>(
      AddPage(RUNTIME_CLASS(CSymbolsListsSettings), _T( "Symbols Lists" ),
              CSymbolsListsSettings::IDD, _T( "Session Settings" )));
  m_ls = dynamic_cast<CLoggingSettings*>(
      AddPage(RUNTIME_CLASS(CLoggingSettings), _T( "Logging" ),
              CLoggingSettings::IDD, _T( "Session Settings" )));

  m_ps->init(plugins, selectedDataSourcePlugin);
  m_ls->init(logFile, appendToLog, logOnlyErrors, diagnosticLogging);
  m_dfs->init(addSymbolsToColumn, columnNumber, padDate, dateFormat,
              fieldSeparator, volumeMultiplier, dateSeparator,
              enableRegexFormatting, unlockMatchRegex, matchRegex,
              formatString);
  m_dvs->init(invalidDataHandling, volume0invalid);
  m_gs->init(autoStartDownloading, checkForUpdatesAtStartup, minimizeToTray,
             threads, proxyServerAddress, proxyServerUserName,
             proxyServerPassword, httpTimeout, openDataFileWithithApp,
             dontReloadOldDataInUpdateMode);
  m_dffs->init(prependToFileName, appendToFileName, fileNameExtension,
               createSubdirs, autoSelectOutputPath, autoSelectOutputPathName,
               cm, fileHeader, sortBarsInAscendingOrder);
  m_slls->init(errorSymbolsFile, appendToErrorSymbolsFile,
               ignoreErrorSymbolsList, ignoreSymbolsList);

  SetTitle(_T( "YLoader Settings" ));
  SetLogoText(_T( "YLoader"));
}

YSettingsDialog::~YSettingsDialog(void) {}

BOOL YSettingsDialog::OnInitDialog() {
  CSettingsDialog::OnInitDialog();

  if (!_hasSelectedPlugin && _showInitialMB) {
    AfxMessageBox(
        _T( "Please select a data source plug-in in the next dialog.\n\nYou can also configure various other settings and parameters of the YGQD" ));
  }

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void YSettingsDialog::OnOK() {
  /*	m_ls->UpdateData();
          m_dfs->UpdateData();
          m_dvs->UpdateData();
          m_gs->UpdateData();
          m_dffs->UpdateData();
  */
  CSettingsDialog::OnOK();

  /*	if( ( threads() > 1 || logFile().GetLength() > 0 ||
     autoStartDownloading() ))
          {
            _disableExtraFeatures = true;
            AfxMessageBox( _T( "The \"Multiple simultaneous connections\", \"Log
     file\" and \"Auto start downloading\" \nfeatures are disabled in the
     unregistered version of the application\nThese fields have been reset to
     their default values\n" ) );
          }
          */
}

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

class CGeneralSettings : public CPropertyPage, public Okable {
  DECLARE_DYNCREATE(CGeneralSettings)

 public:
  CGeneralSettings(CWnd* pParent = NULL);  // standard constructor
  virtual ~CGeneralSettings();

  void init(bool autoStartDownloading, bool checkForUpdatesAtStartup,
            bool minimizeToTray, UINT threads,
            const std::wstring& proxyServerAddress,
            const std::wstring& proxyServerUserName,
            const std::wstring& proxyServerPassword, unsigned int httpTimeout,
            const std::wstring& openDataFileWithithApp,
            bool dontReloadOldDataInUpdateMode) {
    m_autoStartDownloading = autoStartDownloading;
    m_checkForUpdatesAtStartupValue = checkForUpdatesAtStartup;
    m_threadsValue = threads;
    m_minimizeToTrayValue = minimizeToTray;
    m_proxyServerAddress = proxyServerAddress.c_str();
    m_proxyServerUserName = proxyServerUserName.c_str();
    m_proxyServerPassword = proxyServerPassword.c_str();
    m_httpRequestTimeout = httpTimeout;
    m_openDataFileWithAppStr = openDataFileWithithApp.c_str();
    m_dontReloadOldDataInUpdateModeBool = dontReloadOldDataInUpdateMode;
  }

  void postInit() {
    m_threadsSpin.SetRange(1, 20);
    m_timeoutSpin.SetRange(0, 9999);
  }

  bool autoStartDownloading() const { return m_autoStartDownloading != 0; }
  bool checkForUpdatesAtStartup() const {
    return m_checkForUpdatesAtStartupValue != 0;
  }
  UINT threads() const { return m_threadsValue; }
  bool minimizeToTray() const { return m_minimizeToTrayValue != 0; }
  std::wstring proxyServerAddress() const {
    return (LPCTSTR)m_proxyServerAddress;
  }
  std::wstring proxyServerUserName() const {
    return (LPCTSTR)m_proxyServerUserName;
  }
  std::wstring proxyServerPassword() const {
    return (LPCTSTR)m_proxyServerPassword;
  }
  unsigned int httpRequestTimeout() const { return m_httpRequestTimeout; }
  std::wstring openDataFileWithApp() const {
    return (LPCTSTR)m_openDataFileWithAppStr;
  }

  bool dontReloadOldDataInUpdateMode() const {
    return m_dontReloadOldDataInUpdateModeBool != 0;
  }

  // Dialog Data
  enum { IDD = IDD_PROPPAGE_GENERAL_SETTINGS };

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

  DECLARE_MESSAGE_MAP()
 private:
  //  CButton m_searchForUpdatesAtStartup;

 public:
  bool onOk() {
    //    GeneralSettings gs( m_searchForUpdatesAtStartupBool != 0 );
    return true;
  }

 private:
  CButton m_yahooDataSourceCtrl;
  CButton m_googleDataSourceCtrl;
  CButton m_autoStartDownloadingCheck;
  CButton m_minimizeToTrayCheck;
  CButton m_checkForUpdatesAtStartupButton;
  CStatic m_simultaneousConnectionsLabel;
  CEdit m_threadsEdit;
  CMFCSpinButtonCtrl m_threadsSpin;
  CStatic m_proxyServerGroup;
  CEdit m_proxyServerAddressEdit;
  CEdit m_proxyServerUserNameEdit;
  CEdit m_proxyServerPasswordEdit;
  CEdit m_httpRequestTimeoutEdit;
  CMFCSpinButtonCtrl m_timeoutSpin;
  CEdit m_openDataFileWithAppEdit;
  CButton m_browseForAppButton;
  CButton m_dontReloadOldDataInUpdateMode;

  BOOL m_autoStartDownloading;
  BOOL m_checkForUpdatesAtStartupValue;
  BOOL m_minimizeToTrayValue;
  UINT m_threadsValue;
  CString m_proxyServerAddress;
  CString m_proxyServerUserName;
  CString m_proxyServerPassword;
  UINT m_httpRequestTimeout;

  CString m_openDataFileWithAppStr;

  BOOL m_dontReloadOldDataInUpdateModeBool;

 public:
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedButtonBrowseForApp();
};

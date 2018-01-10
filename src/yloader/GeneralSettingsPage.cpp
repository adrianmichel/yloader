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
#include "GeneralSettingsPage.h"

// GeneralSettings dialog

IMPLEMENT_DYNCREATE(CGeneralSettings, CPropertyPage)
CGeneralSettings::CGeneralSettings(CWnd* pParent /*=NULL*/)
    : CPropertyPage(CGeneralSettings::IDD) {}

CGeneralSettings::~CGeneralSettings() {}

void CGeneralSettings::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_CHECK_AUTO_START_DOWNLOADING,
              m_autoStartDownloadingCheck);
  DDX_Control(pDX, IDC_CHECK_MINIMIZE_TO_TRAY, m_minimizeToTrayCheck);
  DDX_Control(pDX, IDC_CHECK_CHECK_FOR_UPDATES_AT_STARTUP,
              m_checkForUpdatesAtStartupButton);
  DDX_Control(pDX, IDC_SIMULTANEOUS_CONNECTIONS_LABEL,
              m_simultaneousConnectionsLabel);
  DDX_Control(pDX, IDC_EDIT_THREADS, m_threadsEdit);
  DDX_Control(pDX, IDC_PROXY_SERVER_GROUP, m_proxyServerGroup);
  DDX_Control(pDX, IDC_PROXY_SERVER_ADDRESS_EDIT, m_proxyServerAddressEdit);
  DDX_Control(pDX, IDC_PROXY_SERVER_USER_NAME_EDIT, m_proxyServerUserNameEdit);
  DDX_Control(pDX, IDC_PROXY_SERVER_PASSWORD_EDIT, m_proxyServerPasswordEdit);
  DDX_Control(pDX, IDC_HTTP_REQUEST_TIMEOUT_EDIT, m_httpRequestTimeoutEdit);

  DDX_Control(pDX, IDC_EDIT_OPEN_DATA_FILE_WITH, m_openDataFileWithAppEdit);
  DDX_Control(pDX, IDC_BUTTON_BROWSE_FOR_APP, m_browseForAppButton);
  DDX_Control(pDX, IDC_CHECK_DONT_RELOAD_IN_UPDATE_MODE,
              m_dontReloadOldDataInUpdateMode);

  DDX_Check(pDX, IDC_CHECK_AUTO_START_DOWNLOADING, m_autoStartDownloading);
  DDX_Check(pDX, IDC_CHECK_MINIMIZE_TO_TRAY, m_minimizeToTrayValue);
  DDX_Check(pDX, IDC_CHECK_CHECK_FOR_UPDATES_AT_STARTUP,
            m_checkForUpdatesAtStartupValue);
  DDX_Text(pDX, IDC_EDIT_THREADS, m_threadsValue);
  DDV_MinMaxUInt(pDX, m_threadsValue, 1, 50);
  DDX_Control(pDX, IDC_SPIN_THREADS, m_threadsSpin);
  DDX_Text(pDX, IDC_PROXY_SERVER_ADDRESS_EDIT, m_proxyServerAddress);
  DDX_Text(pDX, IDC_PROXY_SERVER_USER_NAME_EDIT, m_proxyServerUserName);
  DDX_Text(pDX, IDC_PROXY_SERVER_PASSWORD_EDIT, m_proxyServerPassword);
  DDX_Text(pDX, IDC_HTTP_REQUEST_TIMEOUT_EDIT, m_httpRequestTimeout);
  DDX_Text(pDX, IDC_EDIT_OPEN_DATA_FILE_WITH, m_openDataFileWithAppStr);

  DDV_MinMaxUInt(pDX, m_httpRequestTimeout, 0, 9999);
  DDX_Control(pDX, IDC_SPIN_TIMEOUT, m_timeoutSpin);

  DDX_Check(pDX, IDC_CHECK_DONT_RELOAD_IN_UPDATE_MODE,
            m_dontReloadOldDataInUpdateModeBool);
}

BEGIN_MESSAGE_MAP(CGeneralSettings, CPropertyPage)
ON_BN_CLICKED(IDC_BUTTON_BROWSE_FOR_APP, OnBnClickedButtonBrowseForApp)
END_MESSAGE_MAP()

// GeneralSettings message handlers

BOOL CGeneralSettings::OnInitDialog() {
  __super::OnInitDialog();

  postInit();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeneralSettings::OnBnClickedButtonBrowseForApp() {
  CFileDialog dlg(true, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  _T( "EXE files (*.exe)|*.exe|All Files (*.*)|*.*||" ));

  if (dlg.DoModal() == IDOK) {
    m_openDataFileWithAppStr = dlg.GetPathName();
    UpdateData(FALSE);
  }
}

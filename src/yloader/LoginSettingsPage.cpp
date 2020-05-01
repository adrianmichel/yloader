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
#include "LoginSettingsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CLoggingSettings, CPropertyPage)

CLoggingSettings::CLoggingSettings(CWnd* pParent /*=NULL*/)
    : CPropertyPage(CLoggingSettings::IDD) {}

CLoggingSettings::~CLoggingSettings() {}

void CLoggingSettings::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_BUTTON_LOG_FILE, m_logFileButton);
  DDX_Control(pDX, IDC_EDIT_LOG_FILE, m_logFileEdit);
  DDX_Control(pDX, IDC_CHECK_APPEND_TO_LOG, m_appendLogCheck);
  DDX_Control(pDX, IDC_CHECK_LOG_ONLY_ERRORS, m_logOnlyErrorsButton);
  DDX_Control(pDX, IDC_DIAGNOSTIC_LOGGING, m_diagnosticLoggingCheck);

  DDX_Check(pDX, IDC_CHECK_LOG_ONLY_ERRORS, m_logOnlyErrorsValue);
  DDX_Text(pDX, IDC_EDIT_LOG_FILE, m_logFileName);
  DDX_Check(pDX, IDC_CHECK_APPEND_TO_LOG, m_appendToLogValue);
  DDX_Check(pDX, IDC_DIAGNOSTIC_LOGGING, m_diagnosticLoggingValue);

  DDX_Control(pDX, IDC_DIAGNOSTIC_LOGGING_GROUP, m_diagnosticLoggingGroup);
  DDX_Control(pDX, IDC_SESSION_LOGGING_GROUP, m_sessionLoggingGroup);
}

BEGIN_MESSAGE_MAP(CLoggingSettings, CPropertyPage)
ON_BN_CLICKED(IDC_BUTTON_LOG_FILE, OnBnClickedButtonLogFile)
END_MESSAGE_MAP()

// GeneralSettings message handlers

BOOL CLoggingSettings::OnInitDialog() {
  __super::OnInitDialog();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

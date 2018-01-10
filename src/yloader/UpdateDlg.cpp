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
#include "yloader.h"
#include "UpdateDlg.h"
#include "afxdialogex.h"

// CUpdateDlg dialog

IMPLEMENT_DYNAMIC(CUpdateDlg, CDialogEx)

CUpdateDlg::CUpdateDlg(const VersionInfo& newVersion, bool checkAtStartup,
                       bool notifyOnlyIfNewerVersion, CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_UPDATE, pParent),
      m_checkAtStartup(checkAtStartup ? TRUE : FALSE),
      m_notifyOnlyIfNewerVersion(notifyOnlyIfNewerVersion ? TRUE : FALSE),
      m_newVersion(newVersion),
      m_currentVersionInfo(_T("")),
      m_newVersionInfo(_T("")),
      m_updateUrl(_T("")) {}

CUpdateDlg::~CUpdateDlg() {}

void CUpdateDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_RICHEDIT2_UPDATE_DESCRIPTION, m_description);
  DDX_Check(pDX, IDC_CHECK_STARTUP, m_checkAtStartup);
  DDX_Check(pDX, IDC_CHECK_SHOW_FOR_NEW_VERSION, m_notifyOnlyIfNewerVersion);
  DDX_Text(pDX, IDC_STATIC_CURRENT_VERSION_INFO, m_currentVersionInfo);
  DDX_Text(pDX, IDC_STATIC_NEW_VERSION_INFO, m_newVersionInfo);
  DDX_Control(pDX, IDC_CHECK_SHOW_FOR_NEW_VERSION,
              m_showOnlyIfNewerVersionCtrl);
  DDX_Control(pDX, IDC_STATIC_CURRENT_VERSION, m_labelCurrentVersion);
  DDX_Control(pDX, IDC_STATIC_NEW_VERSION, m_labelNewVersion);
  DDX_Control(pDX, IDC_STATIC_DESCRIPTION, m_labelDescription);
  DDX_Text(pDX, IDC_MFCLINK_UPDATE_LINK, m_updateUrl);
}

bool CUpdateDlg::getCheckAtStartup() const {
  return this->m_checkAtStartup != FALSE;
}
bool CUpdateDlg::getNotifyOnlyIfNewerVersion() const {
  return this->m_notifyOnlyIfNewerVersion != FALSE;
}

BEGIN_MESSAGE_MAP(CUpdateDlg, CDialogEx)
ON_BN_CLICKED(IDC_CHECK_STARTUP, &CUpdateDlg::OnBnClickedCheckStartup)
END_MESSAGE_MAP()

BOOL CUpdateDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();

  m_description.SetWindowText(m_newVersion.hasDescription()
                                  ? m_newVersion.getDescription()->c_str()
                                  : _T( "No description available" ));

  m_currentVersionInfo =
      (VersionInfo::CURRENT_VERSION_INFO.getVersion().toString() +
       _T( ", built on " ) +
       VersionInfo::CURRENT_VERSION_INFO.getReleaseDate()->to_simple_string())
          .c_str();

  m_newVersionInfo = (m_newVersion.getVersion().toString() + _T(", built on ") +
                      m_newVersion.getReleaseDate()->to_simple_string())
                         .c_str();
  m_showOnlyIfNewerVersionCtrl.EnableWindow(m_checkAtStartup);

  m_labelCurrentVersion.setBold();
  m_labelNewVersion.setBold();
  m_labelDescription.setBold();
  m_updateUrl = m_newVersion.getDownloadUrl()->c_str();

  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CUpdateDlg::OnBnClickedCheckStartup() {
  UpdateData();
  m_showOnlyIfNewerVersionCtrl.EnableWindow(m_checkAtStartup);
}

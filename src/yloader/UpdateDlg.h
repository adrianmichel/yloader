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
#include "afxwin.h"

class CFontStatic : public CStatic {
 private:
  CFont m_font;

 public:
  void setBold() {
    CFont* font = __super::GetFont();

    LOGFONT lf;

    font->GetLogFont(&lf);

    lf.lfWeight = 800;
    m_font.CreateFontIndirectW(&lf);
    __super::SetFont(&m_font);
  }
};
class CUpdateDlg : public CDialogEx {
  DECLARE_DYNAMIC(CUpdateDlg)

 public:
  CUpdateDlg(const VersionInfo& newVersion, bool checkAtStartup,
             bool showForNewerVersion,
             CWnd* pParent = NULL);  // standard constructor
  virtual ~CUpdateDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_DIALOG_UPDATE };
#endif

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

  DECLARE_MESSAGE_MAP()
 private:
  CRichEditCtrl m_description;
  BOOL m_checkAtStartup;
  BOOL m_notifyOnlyIfNewerVersion;
  std::wstring m_descriptionValue;
  VersionInfo m_newVersion;

 public:
  void setDescription(std::wstring& description);
  void setCheckAtStartup(bool check);
  void setNotifyOnlyIfNewerVersion(bool show);
  void setNewVersionInfo(const std::wstring& newVersionInfo) {
    m_newVersionInfo = newVersionInfo.c_str();
  }

  bool getCheckAtStartup() const;
  bool getNotifyOnlyIfNewerVersion() const;
  virtual BOOL OnInitDialog();
  CString m_currentVersionInfo;
  CString m_newVersionInfo;
  afx_msg void OnBnClickedCheckStartup();
  CButton m_showOnlyIfNewerVersionCtrl;
  CFontStatic m_labelCurrentVersion;
  CFontStatic m_labelNewVersion;
  CFontStatic m_labelDescription;
  CString m_updateUrl;
};

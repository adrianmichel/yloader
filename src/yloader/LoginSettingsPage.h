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

class CLoggingSettings : public CPropertyPage, public Okable {
  DECLARE_DYNCREATE(CLoggingSettings)

 public:
  CLoggingSettings(CWnd* pParent = NULL);  // standard constructor
  virtual ~CLoggingSettings();
  void init(const std::wstring& logFile, bool appendToLog, bool logOnlyErrors,
            bool diagnosticLogging) {
    m_logOnlyErrorsValue = logOnlyErrors;
    m_logFileName = logFile.c_str();
    m_appendToLogValue = appendToLog;
    m_diagnosticLoggingValue = diagnosticLogging;
  }

  bool appendToLog() const { return m_appendToLogValue != 0; }
  bool logOnlyErrors() const { return m_logOnlyErrorsValue != 0; }
  const CString& logFile() const { return m_logFileName; }
  bool diagnosticLogging() const { return m_diagnosticLoggingValue != 0; }

  // Dialog Data
  enum { IDD = IDD_PROPPAGE_LOGGING_SETTINGS };

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

  DECLARE_MESSAGE_MAP()
 private:
  CButton m_logFileButton;
  CEdit m_logFileEdit;
  CButton m_appendLogCheck;
  CButton m_logOnlyErrorsButton;
  CButton m_diagnosticLoggingCheck;
  CStatic m_sessionLoggingGroup;
  CStatic m_diagnosticLoggingGroup;

  BOOL m_logOnlyErrorsValue;
  CString m_logFileName;
  BOOL m_appendToLogValue;
  BOOL m_diagnosticLoggingValue;

 public:
  bool onOk() {
    //    GeneralSettings gs( m_searchForUpdatesAtStartupBool != 0 );
    return true;
  }

 private:
  //  BOOL m_searchForUpdatesAtStartupBool;
 public:
  virtual BOOL OnInitDialog();

  void OnBnClickedButtonLogFile() {
    UpdateData();
    CFileDialog dlg(true, 0, m_logFileName,
                    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                    _T( "Text files (*.txt)|*.txt|All Files (*.*)|*.*||" ));

    if (dlg.DoModal() == IDOK) {
      m_logFileName = dlg.GetPathName();
      UpdateData(FALSE);
    }
  }
};

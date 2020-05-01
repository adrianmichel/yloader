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

#pragma once

#include "settingsdlg.h"

class CSymbolsListsSettings : public CPropertyPage, public Okable {
 private:
  CStatic m_errorSymbolsGroup;
  CButton m_appendToErrorSymbolsFileCtrl;
  CButton m_ignoreErrorsSymbolsCtrl;

  CString m_errorSymbolsFile;
  BOOL m_appendToErrorSymbolsFile;
  BOOL m_ignoreErrorSymbolsList;
  CString m_ignoreSymbolsList;

  DECLARE_DYNCREATE(CSymbolsListsSettings)

 public:
  CSymbolsListsSettings(CWnd* pParent = NULL);  // standard constructor
  virtual ~CSymbolsListsSettings();

  void init(const std::wstring& errorSymbolsFile, bool appendToErrorSymbolsFile,
            bool ignoreErrorSymbolsList, const std::wstring& ignoreSymbolsList);

  // Dialog Data
  enum { IDD = IDD_PROPPAGE_SYMBOLS_LISTS_SETTINGS };

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

  DECLARE_MESSAGE_MAP()
 private:
 public:
  bool onOk() { return true; }

 private:
 public:
  BOOL OnInitDialog() override;

  const std::wstring errorSymbolsFile() const {
    return (LPCTSTR)m_errorSymbolsFile;
  }

  bool appendToErrorSymbolsFile() const {
    return m_appendToErrorSymbolsFile != FALSE;
  }

  bool ignoreErrorSymbolsList() const {
    return m_ignoreErrorSymbolsList != FALSE;
  }

  std::wstring ignoreSymbolsList() const {
    return (LPCTSTR)m_ignoreSymbolsList;
  }
};

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
#include "SymbolsSettingsPage.h"

IMPLEMENT_DYNCREATE(CSymbolsListsSettings, CPropertyPage)

CSymbolsListsSettings::CSymbolsListsSettings(CWnd* pParent) {}
CSymbolsListsSettings::~CSymbolsListsSettings() {}

void CSymbolsListsSettings::init(const std::wstring& errorSymbolsFile, bool appendToErrorSymbolsFile,
                                 bool ignoreErrorSymbolsList,const std::wstring& ignoreSymbolsList) {
  m_errorSymbolsFile = errorSymbolsFile.c_str();
  m_appendToErrorSymbolsFile = appendToErrorSymbolsFile;
  m_ignoreErrorSymbolsList = ignoreErrorSymbolsList;
  m_ignoreSymbolsList = ignoreSymbolsList.c_str();
}

void CSymbolsListsSettings::DoDataExchange(CDataExchange* pDX) {
  DDX_Text(pDX, IDC_EDIT_ERRORS_SYMBOLS_FILE, m_errorSymbolsFile);
  DDX_Check(pDX, IDC_CHECK_APPEND_TO_ERROR_SYMBOLS_LIST_FILE, m_appendToErrorSymbolsFile);

  DDX_Check(pDX, IDC_CHECK_IGNORE_ERRORS_SYMBOLS_LIST, m_ignoreErrorSymbolsList);
  DDX_Text(pDX, IDC_EDIT_IGNORE_SYMBOLS_LIST, m_ignoreSymbolsList);

  DDX_Control(pDX, IDC_STATIC_SYMBOLS_WITH_ERRORS_GROUP, m_errorSymbolsGroup);

  DDX_Control(pDX, IDC_CHECK_APPEND_TO_ERROR_SYMBOLS_LIST_FILE, m_appendToErrorSymbolsFileCtrl);
  DDX_Control(pDX, IDC_CHECK_IGNORE_ERRORS_SYMBOLS_LIST, m_ignoreErrorsSymbolsCtrl);
}

BEGIN_MESSAGE_MAP(CSymbolsListsSettings, CPropertyPage)
END_MESSAGE_MAP()

BOOL CSymbolsListsSettings::OnInitDialog() {
  __super::OnInitDialog();
  return true;
}

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

#include "stdafx.h"
#include "EmptyDataPathPromptDlg.h"

// CEmptyDataPathPromptDlg dialog

IMPLEMENT_DYNAMIC(CEmptyDataPathPromptDlg, CDialog)
CEmptyDataPathPromptDlg::CEmptyDataPathPromptDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CEmptyDataPathPromptDlg::IDD, pParent),
      m_autoSelectDataPathValue(FALSE) {}

CEmptyDataPathPromptDlg::~CEmptyDataPathPromptDlg() {}

void CEmptyDataPathPromptDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_STATIC_MESSAGE, m_message);
  DDX_Control(pDX, IDC_CHECK_AUTO_SELECT, m_checkAutoSelect);
  DDX_Control(pDX, IDOK, m_ok);
  DDX_Check(pDX, IDC_CHECK_AUTO_SELECT, m_autoSelectDataPathValue);
}

BEGIN_MESSAGE_MAP(CEmptyDataPathPromptDlg, CDialog)
ON_STN_CLICKED(IDC_STATIC_MESSAGE, OnStnClickedStaticMessage)
END_MESSAGE_MAP()

// CEmptyDataPathPromptDlg message handlers

void CEmptyDataPathPromptDlg::OnStnClickedStaticMessage() {
  // TODO: Add your control notification handler code here
}

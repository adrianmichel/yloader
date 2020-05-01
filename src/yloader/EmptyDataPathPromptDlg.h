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

#include "afxwin.h"
#include "resource.h"

// CEmptyDataPathPromptDlg dialog

class CEmptyDataPathPromptDlg : public CDialog
{
	DECLARE_DYNAMIC(CEmptyDataPathPromptDlg)

public:
	CEmptyDataPathPromptDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEmptyDataPathPromptDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_EMPTY_DATA_PATH_PROMPT  };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_message;
	afx_msg void OnStnClickedStaticMessage();
	CButton m_checkAutoSelect;
	CButton m_ok;
	BOOL m_autoSelectDataPathValue;

public:
	bool autoSelectDataPath() const { return m_autoSelectDataPathValue != 0; }
};

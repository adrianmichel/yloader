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
#include "OutputSettingsPage.h"

IMPLEMENT_DYNCREATE(CDataFileSettings, CPropertyPage)
CDataFileSettings::CDataFileSettings(CWnd* pParent /*=NULL*/)
    : CPropertyPage(CDataFileSettings::IDD) {}

CDataFileSettings::~CDataFileSettings() {}

void CDataFileSettings::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_CHECK_CREATE_SUBDIRS, m_createSubdirsButton);
  DDX_Control(pDX, IDC_CH_MAP_FROM_1, m_chMapFrom1);
  DDX_Control(pDX, IDC_CH_MAP_FROM_2, m_chMapFrom2);
  DDX_Control(pDX, IDC_CH_MAP_FROM_3, m_chMapFrom3);
  DDX_Control(pDX, IDC_CH_MAP_FROM_4, m_chMapFrom4);
  DDX_Control(pDX, IDC_CH_MAP_TO_1, m_chMapTo1);
  DDX_Control(pDX, IDC_CH_MAP_TO_2, m_chMapTo2);
  DDX_Control(pDX, IDC_CH_MAP_TO_3, m_chMapTo3);
  DDX_Control(pDX, IDC_CH_MAP_TO_4, m_chMapTo4);
  DDX_Control(pDX, IDC_DATA_FILE_HEADER_EDIT, m_fileHeaderCtrl);
  DDX_Control(pDX, IDC_CHECK_MOST_RECENT_BARS_LAST, m_mostRecentBarsLastCtrl);

  DDX_Text(pDX, IDC_CH_MAP_FROM_1, m_chMapFromValue1);
  DDV_MaxChars(pDX, m_chMapFromValue1, 1);
  DDX_Text(pDX, IDC_CH_MAP_FROM_2, m_chMapFromValue2);
  DDV_MaxChars(pDX, m_chMapFromValue2, 1);
  DDX_Text(pDX, IDC_CH_MAP_FROM_3, m_chMapFromValue3);
  DDV_MaxChars(pDX, m_chMapFromValue3, 1);
  DDX_Text(pDX, IDC_CH_MAP_FROM_4, m_chMapFromValue4);
  DDV_MaxChars(pDX, m_chMapFromValue4, 1);
  DDX_Text(pDX, IDC_CH_MAP_TO_1, m_chMapToValue1);
  DDV_MaxChars(pDX, m_chMapToValue1, 1);
  DDX_Text(pDX, IDC_CH_MAP_TO_2, m_chMapToValue2);
  DDV_MaxChars(pDX, m_chMapToValue2, 1);
  DDX_Text(pDX, IDC_CH_MAP_TO_3, m_chMapToValue3);
  DDV_MaxChars(pDX, m_chMapToValue3, 1);
  DDX_Text(pDX, IDC_CH_MAP_TO_4, m_chMapToValue4);
  DDV_MaxChars(pDX, m_chMapToValue4, 1);

  DDX_Check(pDX, IDC_CHECK_MOST_RECENT_BARS_LAST, m_mostRecentBarsLastBool);
  DDX_Text(pDX, IDC_DATA_FILE_HEADER_EDIT, m_fileHeaderStr);
  DDX_Control(pDX, IDC_STATIC_CH_MAPPING, m_chMappingGroup);
  DDX_Check(pDX, IDC_CHECK_CREATE_SUBDIRS, m_createSubdirsValue);
  DDX_Control(pDX, IDC_STATIC_FILE_NAME_TRANSFORMATIONS, m_fileNameTransformationsGroup);
  DDX_Control(pDX, IDC_EDIT_FILE_NAME_EXTENSION, m_fileNameExtensionEdit);
  DDX_Text(pDX, IDC_EDIT_FILE_NAME_EXTENSION, m_fileNameExtensionValue);
  DDX_Control(pDX, IDC_EDIT_PREPEND_TO_FILE_NAME, m_prependToFileNameEdit);
  DDX_Control(pDX, IDC_EDIT_APPEND_TO_FILE_NAME, m_appendToFileNameEdit);
  DDX_Control(pDX, IDC_STATIC_PREPEND_TO_FILE_NAME, m_prependToFileNameLabel);
  DDX_Control(pDX, IDC_STATIC_APPEND_TO_FILE_NAME, m_appendToFileNameLabel);
  DDX_Text(pDX, IDC_EDIT_PREPEND_TO_FILE_NAME, m_prependToFileNameValue);
  DDX_Text(pDX, IDC_EDIT_APPEND_TO_FILE_NAME, m_appendToFileNameValue);
  DDX_Control(pDX, IDC_CHECK_AUTO_SELECT_OUTPUT_PATH, m_autoSelectOutputPathButton);
  DDX_Check(pDX, IDC_CHECK_AUTO_SELECT_OUTPUT_PATH, m_autoSelectOutputPathValue);
  DDX_Control(pDX, IDC_EDIT_AUTO_DATA_PATH, m_autoSelectOutputPathEdit);
  DDX_Text(pDX, IDC_EDIT_AUTO_DATA_PATH, m_autoSelectOutputPathName);
}

BEGIN_MESSAGE_MAP(CDataFileSettings, CPropertyPage)
ON_EN_CHANGE(IDC_EDIT_AUTO_DATA_PATH, OnEnChangeEditAutoDataPath)
ON_BN_CLICKED(IDC_CHECK_AUTO_SELECT_OUTPUT_PATH, OnBnClickedCheckAutoSelectOutputPath)
END_MESSAGE_MAP()

// GeneralSettings message handlers

BOOL CDataFileSettings::OnInitDialog() {
  __super::OnInitDialog();

  m_autoSelectOutputPathEdit.ShowWindow(m_autoSelectOutputPathValue ? SW_SHOW : SW_HIDE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDataFileSettings::OnEnChangeEditAutoDataPath() {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the __super::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
}

void CDataFileSettings::OnBnClickedCheckAutoSelectOutputPath() {
  UpdateData();

  m_autoSelectOutputPathEdit.ShowWindow(m_autoSelectOutputPathValue ? SW_SHOW : SW_HIDE);
}

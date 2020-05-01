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
#include "DataFormattingSettingsPage.h"
#include "defaults.h"
#include <boost/regex.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDataFormattingSettings, CPropertyPage)
CDataFormattingSettings::CDataFormattingSettings(CWnd* pParent /*=NULL*/)
    : CPropertyPage(CDataFormattingSettings::IDD)
//  , m_searchForUpdatesAtStartupBool(FALSE)
{}

CDataFormattingSettings::~CDataFormattingSettings() {}

void CDataFormattingSettings::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_CHECK_ADD_SYMBOLS_TO_COLUMN, m_addSymbolsToColumnCtrl);
  DDX_Control(pDX, IDC_EDIT_COLUMN_NUMBER, m_columnNumberCtrl);
  DDX_Control(pDX, IDC_CHECK_PAD_DATE, m_padDateCheck);
  DDX_Control(pDX, IDC_STATIC_DATE_FORMAT_GROUP, m_dateFormatGroupCtrl);
  DDX_Control(pDX, IDC_RADIO_EUROPEAN_DATE_FORMAT, m_europeanDateFormatCtrl);
  DDX_Control(pDX, IDC_RADIO_US_DATE_FORMAT, m_usDateFormatCtrl);
  DDX_Control(pDX, IDC_RADIO_YEAR_FIRST_DATE_FORMAT, m_yearFirstDateFormatCtrl);

  DDX_Control(pDX, IDC_EDIT_FIELD_SEPARATOR, m_fieldSeparatorCtrl);
  DDX_Control(pDX, IDC_EDIT_VOLUME_MULTIPLIER_EDIT, m_volumeMultiplierCtrl);
  DDX_Control(pDX, IDC_EDIT_DATE_SEPARATOR, m_dateSeparatorCtrl);
  DDX_Control(pDX, IDC_BUTTON_RESET_REGEX_FORMATTING_TO_DEFAULT, m_resetRegexFormattingCtrl);

  DDX_Check(pDX, IDC_CHECK_ADD_SYMBOLS_TO_COLUMN, m_addSymbolsToColumnBool);
  DDX_Text(pDX, IDC_EDIT_COLUMN_NUMBER, m_columnNumberUInt);
  DDV_MinMaxUInt(pDX, m_columnNumberUInt, 0, 6);
  DDX_Check(pDX, IDC_CHECK_PAD_DATE, m_padDateValue);
  DDX_Radio(pDX, IDC_RADIO_US_DATE_FORMAT, m_dateFormat);
  DDX_Control(pDX, IDC_SPIN_SYMBOL_COLUMN, m_symbolColum);
  DDX_Text(pDX, IDC_EDIT_VOLUME_MULTIPLIER_EDIT, m_volumeMultiplierDouble);
  DDX_Text(pDX, IDC_EDIT_FIELD_SEPARATOR, m_fieldSeparatorStr);
  DDX_Text(pDX, IDC_EDIT_DATE_SEPARATOR, m_dateSeparatorStr);
  DDV_MaxChars(pDX, m_dateSeparatorStr, 1);

  DDX_Control(pDX, IDC_CHECK_USE_REGEX_FORMATTING, m_enableRegexFormattingCtrl);
  DDX_Control(pDX, IDC_CHECK_UNLOCK_MATCH_REGEX, m_unlockMatchRegexCtrl);
  DDX_Control(pDX, IDC_EDIT_MATCH_REGEX, m_matchRegexCtrl);
  DDX_Control(pDX, IDC_EDIT_FORMAT_STRING, m_formatStringCtrl);
  DDX_Control(pDX, IDC_STATIC_REGEX_FORMATTING_GROUP, m_regexFormattingGroupCtrl);

  DDX_Check(pDX, IDC_CHECK_USE_REGEX_FORMATTING, m_enableRegexFormatting);
  DDX_Text(pDX, IDC_EDIT_MATCH_REGEX, m_matchRegex);
  DDX_Text(pDX, IDC_EDIT_FORMAT_STRING, m_formatString);

  //	 DDV_MinMaxDouble(pDX, m_volumeMultiplierDouble, 0, 9999999999 );
}

void CDataFormattingSettings::updateColumnNumber(CCmdUI* pCmdUI) {
  pCmdUI->Enable(m_addSymbolsToColumnCtrl.GetState() != 3);
}

BEGIN_MESSAGE_MAP(CDataFormattingSettings, CPropertyPage)
ON_UPDATE_COMMAND_UI(IDC_EDIT_COLUMN_NUMBER, updateColumnNumber)
ON_BN_CLICKED(IDC_CHECK_ADD_SYMBOLS_TO_COLUMN, OnBnClickedCheckAddSymbolsToColumn)
ON_BN_CLICKED(IDC_BUTTON_RESET_REGEX_FORMATTING_TO_DEFAULT, OnBnClickedButtonResetRegexFormattingToDefault)
ON_BN_CLICKED(IDC_CHECK_UNLOCK_MATCH_REGEX, OnBnClickedCheckUnlockMatchRegex)
ON_BN_CLICKED(IDC_CHECK_USE_REGEX_FORMATTING, OnBnClickedCheckUseRegexFormatting)
END_MESSAGE_MAP()

// GeneralSettings message handlers

BOOL CDataFormattingSettings::OnInitDialog() {
  __super::OnInitDialog();
  postInit();
  enableColumnNumber();
  lockRegexFormatting();
  setEnableRegexFormatting();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDataFormattingSettings::OnBnClickedButtonResetRegexFormattingToDefault() {
  int x = AfxMessageBox(L"Are you sure you want to reset the regex formatting parameters to their default values?", MB_YESNO);

  if (x == IDYES) {
    m_matchRegex = DEFAULT_MATCH_REGEX;
    m_formatString = DEFAULT_FORMAT_STRING;
    UpdateData(FALSE);
  }
}

void CDataFormattingSettings::OnBnClickedCheckUnlockMatchRegex() {
  m_matchRegexUnlocked = !m_matchRegexUnlocked;

  lockRegexFormatting();
}

void CDataFormattingSettings::lockRegexFormatting() {
  m_unlockMatchRegexCtrl.SetWindowText(m_matchRegexUnlocked ? L"Lock" : L"Unlock");
  m_resetRegexFormattingCtrl.EnableWindow(m_matchRegexUnlocked);
  m_matchRegexCtrl.SetReadOnly(!m_matchRegexUnlocked);
  m_formatStringCtrl.SetReadOnly(!m_matchRegexUnlocked);
}

void CDataFormattingSettings::OnBnClickedCheckUseRegexFormatting() {
  UpdateData();

  setEnableRegexFormatting();
}

void CDataFormattingSettings::setEnableRegexFormatting() {
  m_addSymbolsToColumnCtrl.EnableWindow(!m_enableRegexFormatting);
  m_symbolColum.EnableWindow(!m_enableRegexFormatting);
  m_columnNumberCtrl.EnableWindow(!m_enableRegexFormatting);
  m_fieldSeparatorCtrl.EnableWindow(!m_enableRegexFormatting);
  m_padDateCheck.EnableWindow(!m_enableRegexFormatting);
  m_usDateFormatCtrl.EnableWindow(!m_enableRegexFormatting);
  m_europeanDateFormatCtrl.EnableWindow(!m_enableRegexFormatting);
  m_yearFirstDateFormatCtrl.EnableWindow(!m_enableRegexFormatting);
  m_volumeMultiplierCtrl.EnableWindow(!m_enableRegexFormatting);
  m_dateSeparatorCtrl.EnableWindow(!m_enableRegexFormatting);

  m_unlockMatchRegexCtrl.EnableWindow(m_enableRegexFormatting);
  m_matchRegexCtrl.EnableWindow(m_enableRegexFormatting);
  m_formatStringCtrl.EnableWindow(m_enableRegexFormatting);
  m_regexFormattingGroupCtrl.EnableWindow(m_enableRegexFormatting);
  m_resetRegexFormattingCtrl.EnableWindow(m_enableRegexFormatting && m_matchRegexUnlocked);
}

bool CDataFormattingSettings::onOk() {
  try {
    boost::wregex rx(matchRegex());
  }
  catch (const boost::bad_expression& e) {
    AfxMessageBox((L"Regular expression error:\n\n"s + yloader::s2ws(e.what()) + L"\n\nPlease fix the error and try again." ).c_str(), MB_OK);
    return false;
  }

  return true;
}

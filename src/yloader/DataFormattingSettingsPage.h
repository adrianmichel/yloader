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

class CDataFormattingSettings : public CPropertyPage, public Okable {
  DECLARE_DYNCREATE(CDataFormattingSettings)

 public:
  CDataFormattingSettings(CWnd* pParent = NULL);  // standard constructor
  virtual ~CDataFormattingSettings();

  void init(bool addSymbolsToColumn, UINT columnNumber, bool padDate,
            int dateFormat, const std::wstring& fieldSeparator,
            double volumeMultiplier, const std::wstring& dateSeparator,
            bool enableRegexFormatting, bool matchRegexUnlocked,
            const std::wstring& matchRegex, const std::wstring& formatString) {
    m_addSymbolsToColumnBool = addSymbolsToColumn;
    m_columnNumberUInt = columnNumber;
    m_padDateValue = padDate;
    m_dateFormat = dateFormat;
    m_fieldSeparatorStr = fieldSeparator.c_str();
    m_volumeMultiplierDouble = volumeMultiplier;
    m_dateSeparatorStr = dateSeparator.c_str();
    m_enableRegexFormatting = enableRegexFormatting;
    m_matchRegexUnlocked = matchRegexUnlocked;
    m_matchRegex = matchRegex.c_str();
    m_formatString = formatString.c_str();
  }

  void postInit() { m_symbolColum.SetRange(0, 6); }

  bool addSymbolsToColumn() const { return m_addSymbolsToColumnBool != 0; }
  unsigned int columnNumber() const { return m_columnNumberUInt; }
  bool padDate() const { return m_padDateValue != 0; }
  int dateFormat() const { return m_dateFormat; }
  std::wstring fieldSeparator() const { return (LPCTSTR)m_fieldSeparatorStr; }
  double volumeMultiplier() const { return m_volumeMultiplierDouble; }
  std::wstring dateSeparator() const { return (LPCTSTR)m_dateSeparatorStr; }

  bool enableRegexFormatting() const {
    return m_enableRegexFormatting != FALSE;
  }
  bool unlockMatchRegex() const { return m_matchRegexUnlocked; }
  std::wstring matchRegex() const { return (LPCTSTR)m_matchRegex; }
  std::wstring formatString() const { return (LPCTSTR)m_formatString; }

  // Dialog Data
  enum { IDD = IDD_PROPPAGE_DATA_FORMATTING_SETTINGS };

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

  DECLARE_MESSAGE_MAP()
 private:
  CButton m_addSymbolsToColumnCtrl;
  CEdit m_columnNumberCtrl;
  CEdit m_fieldSeparatorCtrl;
  CButton m_padDateCheck;
  CButton m_usDateFormatCtrl;
  CButton m_europeanDateFormatCtrl;
  CButton m_yearFirstDateFormatCtrl;
  CStatic m_dateFormatGroupCtrl;
  CMFCSpinButtonCtrl m_symbolColum;
  CEdit m_volumeMultiplierCtrl;
  CEdit m_dateSeparatorCtrl;

  CButton m_enableRegexFormattingCtrl;
  CButton m_unlockMatchRegexCtrl;
  CEdit m_matchRegexCtrl;
  CEdit m_formatStringCtrl;
  CStatic m_regexFormattingGroupCtrl;
  CButton m_resetRegexFormattingCtrl;

  BOOL m_addSymbolsToColumnBool;
  UINT m_columnNumberUInt;
  BOOL m_padDateValue;
  int m_dateFormat;
  CString m_fieldSeparatorStr;
  double m_volumeMultiplierDouble;
  CString m_dateSeparatorStr;

  BOOL m_enableRegexFormatting;
  bool m_matchRegexUnlocked;
  CString m_matchRegex;
  CString m_formatString;

 public:
  bool onOk();

 private:
  //  BOOL m_searchForUpdatesAtStartupBool;
 public:
  virtual BOOL OnInitDialog();

  afx_msg void updateColumnNumber(CCmdUI* pCmdUI);

  afx_msg void OnBnClickedCheckAddSymbolsToColumn() {
    UpdateData();

    enableColumnNumber();
  }

  void enableColumnNumber() {
    m_columnNumberCtrl.ShowWindow(m_addSymbolsToColumnBool ? SW_SHOW : SW_HIDE);
    m_symbolColum.ShowWindow(m_addSymbolsToColumnBool ? SW_SHOW : SW_HIDE);
  }

  afx_msg void OnBnClickedButtonResetRegexFormattingToDefault();
  afx_msg void OnBnClickedCheckUnlockMatchRegex();

  void lockRegexFormatting();
  afx_msg void OnBnClickedCheckUseRegexFormatting();

  void setEnableRegexFormatting();
};

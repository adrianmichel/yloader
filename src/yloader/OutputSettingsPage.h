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

#include "charactermapping.h"
#include "settingsdlg.h"

class CDataFileSettings : public CPropertyPage, public Okable {
  DECLARE_DYNCREATE(CDataFileSettings)

 public:
  CDataFileSettings(CWnd* pParent = NULL);  // standard constructor
  virtual ~CDataFileSettings();

  void init(const std::wstring& prependToFileName,
            const std::wstring& appendToFileName,
            const std::wstring& fileNameExtension, bool createSubdirs,
            bool autoSelectOutputPath,
            const std::wstring& autoSelectOutputPathName,
            const CharactersMapping& cm, const std::wstring& fileHeader,
            bool sortBarsInAscendingOrder) {
    m_createSubdirsValue = createSubdirs;
    m_fileNameExtensionValue = fileNameExtension.c_str();
    m_prependToFileNameValue = prependToFileName.c_str();
    m_appendToFileNameValue = appendToFileName.c_str();
    m_autoSelectOutputPathValue = autoSelectOutputPath;
    m_autoSelectOutputPathName = autoSelectOutputPathName.c_str();
    m_fileHeaderStr = fileHeader.c_str();
    m_mostRecentBarsLastBool = sortBarsInAscendingOrder;

    CharacterMappingPtr cmp;

    cmp = cm.getFirstMapping();

    if (cmp) {
      m_chMapFromValue1 = cmp->first.c_str();
      m_chMapToValue1 = cmp->second.c_str();
    }

    cmp = cm.getNextMapping();

    if (cmp) {
      m_chMapFromValue2 = cmp->first.c_str();
      m_chMapToValue2 = cmp->second.c_str();
    }

    cmp = cm.getNextMapping();
    if (cmp) {
      m_chMapFromValue3 = cmp->first.c_str();
      m_chMapToValue3 = cmp->second.c_str();
    }

    cmp = cm.getNextMapping();
    if (cmp) {
      m_chMapFromValue4 = cmp->first.c_str();
      m_chMapToValue4 = cmp->second.c_str();
    }
  }

  std::wstring fileHeader() const { return (LPCTSTR)m_fileHeaderStr; }
  const CString& prependToFileName() const { return m_prependToFileNameValue; }
  const CString& appendToFileName() const { return m_appendToFileNameValue; }
  const CString& fileNameExtension() const { return m_fileNameExtensionValue; }
  bool createSubdirs() const { return m_createSubdirsValue != 0; }
  bool autoSelectOutputPath() const { return m_autoSelectOutputPathValue != 0; }
  const CString& autoSelectOutputPathName() const {
    return m_autoSelectOutputPathName;
  }
  CharactersMapping characterMapping() const {
    CharactersMapping cm;

    cm.add(m_chMapFromValue1, m_chMapToValue1);
    cm.add(m_chMapFromValue2, m_chMapToValue2);
    cm.add(m_chMapFromValue3, m_chMapToValue3);
    cm.add(m_chMapFromValue4, m_chMapToValue4);

    return cm;
  }

  bool sortBarsInAscendingOrder() const {
    return m_mostRecentBarsLastBool != 0;
  }

  // Dialog Data
  enum { IDD = IDD_PROPPAGE_DATA_FILE_SETTINGS };

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

  DECLARE_MESSAGE_MAP()
 private:
  CStatic m_fileNameTransformationsGroup;
  CStatic m_chMappingGroup;
  CEdit m_fileNameExtensionEdit;
  CEdit m_prependToFileNameEdit;
  CEdit m_appendToFileNameEdit;

  CEdit m_chMapFrom1;
  CEdit m_chMapFrom2;
  CEdit m_chMapFrom3;
  CEdit m_chMapFrom4;
  CEdit m_chMapTo1;
  CEdit m_chMapTo2;
  CEdit m_chMapTo3;
  CEdit m_chMapTo4;
  CStatic m_prependToFileNameLabel;
  CStatic m_appendToFileNameLabel;
  CButton m_createSubdirsButton;
  CButton m_autoSelectOutputPathButton;
  CEdit m_autoSelectOutputPathEdit;
  CEdit m_fileHeaderCtrl;
  CButton m_mostRecentBarsLastCtrl;

  BOOL m_createSubdirsValue;
  BOOL m_autoSelectOutputPathValue;
  CString m_autoSelectOutputPathName;
  CString m_fileNameExtensionValue;
  CString m_prependToFileNameValue;
  CString m_appendToFileNameValue;

  std::vector<CString> m_from;
  std::vector<CString> m_to;

  CString m_chMapFromValue1;
  CString m_chMapFromValue2;
  CString m_chMapFromValue3;
  CString m_chMapFromValue4;
  CString m_chMapToValue1;
  CString m_chMapToValue2;
  CString m_chMapToValue3;
  CString m_chMapToValue4;
  CString m_fileHeaderStr;
  BOOL m_mostRecentBarsLastBool;

 public:
  bool onOk() {
    //    GeneralSettings gs( m_searchForUpdatesAtStartupBool != 0 );
    return true;
  }

 private:
 public:
  virtual BOOL OnInitDialog();
  afx_msg void OnEnChangeEditAutoDataPath();
  afx_msg void OnBnClickedCheckAutoSelectOutputPath();
};

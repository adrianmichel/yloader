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

class CDataValidationSettings : public CDialog, public Okable {
  DECLARE_DYNCREATE(CDataValidationSettings)

 public:
  CDataValidationSettings(CWnd* pParent = NULL);  // standard constructor
  virtual ~CDataValidationSettings();

  void init(int invalidDataHandling, bool volume0invalid) {
    m_invalidDataHandling = invalidDataHandling;
    m_volume0invalid = volume0invalid;
  }

  bool volume0invalid() const { return m_volume0invalid != 0; }
  int invalidDataHandling() const { return m_invalidDataHandling != 0; }

  // Dialog Data
  enum { IDD = IDD_PROPPAGE_DATA_VALIDATION_SETTINGS };

 protected:
  void DoDataExchange(CDataExchange* pDX) override;  // DDX/DDV support

  DECLARE_MESSAGE_MAP()
 private:
 public:
  bool onOk() {
    //		UpdateData();
    //    GeneralSettings gs( m_searchForUpdatesAtStartupBool != 0 );

    return true;
  }

 private:
  CButton m_invalidDataAsWarningCtrl;
  CButton m_invalidDataAsErrorCtrl;
  CStatic m_handlingOfInvalidDataGroup;
  CButton m_volume0invalidCtrl;

  int m_invalidDataHandling;
  BOOL m_volume0invalid;

 public:
  virtual BOOL OnInitDialog();
};

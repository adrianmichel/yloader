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

#include "stdafx.h"
#include "datavalidationsettingspage.h"

// GeneralSettings dialog

IMPLEMENT_DYNCREATE(CDataValidationSettings, CDialog)
CDataValidationSettings::CDataValidationSettings(CWnd* pParent /*=NULL*/)
    : CDialog(CDataValidationSettings::IDD, pParent)
//  , m_searchForUpdatesAtStartupBool(FALSE)
{}

CDataValidationSettings::~CDataValidationSettings() {}

void CDataValidationSettings::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_RADIO_INVALID_DATA_IS_WARNING,
              m_invalidDataAsWarningCtrl);
  DDX_Control(pDX, IDC_RADIO_INVALID_DATA_IS_ERROR, m_invalidDataAsErrorCtrl);
  DDX_Control(pDX, IDC_STATIC_HANDLING_OF_INVALID_DATA_GROUP,
              m_handlingOfInvalidDataGroup);
  DDX_Control(pDX, IDC_CHECK_VOLUME_0_INVALID, m_volume0invalidCtrl);

  DDX_Radio(pDX, IDC_RADIO_INVALID_DATA_IS_WARNING, m_invalidDataHandling);
  DDX_Check(pDX, IDC_CHECK_VOLUME_0_INVALID, m_volume0invalid);
}

BEGIN_MESSAGE_MAP(CDataValidationSettings, CDialog)
END_MESSAGE_MAP()

// GeneralSettings message handlers

BOOL CDataValidationSettings::OnInitDialog() {
  __super::OnInitDialog();

  /*  GeneralSettings gs;
    m_searchForUpdatesAtStartupBool = gs.searchForUpdatesAtStartup();
    UpdateData( false );
    */

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

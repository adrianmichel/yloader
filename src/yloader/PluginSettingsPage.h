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

#pragma warning(disable : 4482)

#include <yplugin.h>
#include "settingsdlg.h"

class CPluginSettings : public CPropertyPage, public Okable {
  DECLARE_DYNCREATE(CPluginSettings)

  const YPlugins* m_plugins;
  const yloader::UniqueId* m_selectedDSPlugin;

  //  CExtPropertyStore * m_pPS;

 public:
  CPluginSettings(CWnd* pParent = NULL)
      : CPropertyPage(CPluginSettings::IDD) /*: m_pPS( 0 ) */ {
  }                            // standard constructor
  virtual ~CPluginSettings() { /* if( m_pPS != NULL ) m_pPS->Delete(); */
  }

  void init(const YPlugins& plugins, const yloader::UniqueId* selectedDSPlugin) {
    m_plugins = &plugins;
    m_selectedDSPlugin = selectedDSPlugin;
  }

  void postInit() {}

  // Dialog Data
  enum { IDD = IDD_PROPPAGE_PLUGINS_SETTINGS };

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  DECLARE_MESSAGE_MAP()
 private:
  CListBox m_dataSourcePlugins;
  //	CExtPropertyGridCtrl m_pluginInfo;

 public:
  bool onOk() {
    //    GeneralSettings gs( m_searchForUpdatesAtStartupBool != 0 );
    return true;
  }

 public:
  virtual BOOL OnInitDialog();

  const yloader::UniqueId* getSelectedDataSourcePlugin() const {
    return m_selectedDSPlugin;
  }
  afx_msg void OnLbnSelchangeListDsPlugins();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

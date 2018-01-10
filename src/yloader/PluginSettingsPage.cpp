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

#include "StdAfx.h"
#include "PluginSettingsPage.h"

IMPLEMENT_DYNCREATE(CPluginSettings, CPropertyPage)
BEGIN_MESSAGE_MAP(CPluginSettings, CPropertyPage)
ON_LBN_SELCHANGE(IDC_LIST_DS_PLUGINS, OnLbnSelchangeListDsPlugins)
//	ON_WM_CREATE()
END_MESSAGE_MAP()

void CPluginSettings::OnLbnSelchangeListDsPlugins() {
  int n = m_dataSourcePlugins.GetCurSel();

  const YDataSourcePlugin* plugin = reinterpret_cast<const YDataSourcePlugin*>(
      m_dataSourcePlugins.GetItemDataPtr(n));

  assert(plugin != 0);

  _selectedDSPlugin = n >= 0 ? &plugin->id() : 0;

  //	setPluginInfo( *plugin );
}

BOOL CPluginSettings::OnInitDialog() {
  __super::OnInitDialog();

  const YDataSourcePlugins& dsp = _plugins->getDataSourcePlugins();

  /*	CExtPropertyGridComboBoxBar * pWnd = STATIC_DOWNCAST(
   CExtPropertyGridComboBoxBar, m_pluginInfo.GetChildByRTC(
   RUNTIME_CLASS(CExtPropertyGridComboBoxBar) ) ); pWnd->ShowWindow( SW_HIDE );
   // or SW_SHOW
          //m_pluginInfo.RecalcLayout(); // obligatory to call this method of
   the property grid

          CExtPropertyGridTipBar * pWnd1 = STATIC_DOWNCAST(
   CExtPropertyGridTipBar, m_pluginInfo.GetChildByRTC(
   RUNTIME_CLASS(CExtPropertyGridTipBar) ) ); pWnd1->ShowWindow( SW_HIDE ); //
   or SW_SHOW


    CExtPropertyGridToolBar * pWnd2 = STATIC_DOWNCAST( CExtPropertyGridToolBar,
   m_pluginInfo.GetChildByRTC( RUNTIME_CLASS(CExtPropertyGridToolBar) ) );
          pWnd2->ShowWindow( SW_HIDE ); // or SW_SHOW
   

          m_pluginInfo.RecalcLayout(); // obligatory to call this method of the
   property grid
          */
  bool selected = false;

  for (YDataSourcePlugins::const_iterator i = dsp.begin(); i != dsp.end();
       i++) {
    YDataSourcePluginPtr p = i->second;

    int n = m_dataSourcePlugins.AddString(p->name().c_str());
    m_dataSourcePlugins.SetItemDataPtr(n, p.get());
    if (_selectedDSPlugin != 0 && p->id() == *_selectedDSPlugin) {
      m_dataSourcePlugins.SetCurSel(n);
      const YDataSourcePlugin* plugin =
          reinterpret_cast<const YDataSourcePlugin*>(
              m_dataSourcePlugins.GetItemDataPtr(n));
      //			setPluginInfo( *plugin );
      selected = true;
    }
  }

  postInit();

  return true;
}

void CPluginSettings::DoDataExchange(CDataExchange* pDX) {
  DDX_Control(pDX, IDC_LIST_DS_PLUGINS, m_dataSourcePlugins);
  // 	DDX_Control(pDX, IDC_PLUGIN_INFO, m_pluginInfo );

}  // DDX/DDV support

int CPluginSettings::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (__super::OnCreate(lpCreateStruct) == -1) return -1;

  // TODO:  Add your specialized creation code here

  return 0;
}

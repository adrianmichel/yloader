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

#pragma warning(disable : 4482)

#include <yplugin.h>
#include "settingsdlg.h"

/*
class CPluginPropertyValueBase : public CExtPropertyValue
{
private:
//    DataSourcePlugin* m_dsPlugin;

public:
    CPluginPropertyValueBase( LPCTSTR strPropertyName = NULL )//,
DataSourcePlugin* plugin = NULL ) : CExtPropertyValue( strPropertyName )
{


                CExtGridCellString * pValue = STATIC_DOWNCAST(
CExtGridCellString, ValueActiveGetByRTC( RUNTIME_CLASS(CExtGridCellString) ) );
                pValue->TextColorSet( CExtGridCell::e_cell_state_t::__ECS_ALL,
RGB( 0, 0X6B, 0XF5 ) );

}


#ifdef _DEBUG
    void AssertValid()
                {
      CExtPropertyValue::AssertValid();

        

                }
                void Dump( CDumpContext & dc ) const
                {
                        CExtPropertyValue::Dump( dc );
                }
#endif
};

class CPluginPropertyLink : public CPluginPropertyValueBase
{
public:
        CPluginPropertyLink( const std::wstring& name, const std::wstring& url,
const std::wstring& text  ) : CPluginPropertyValueBase( name.c_str() )
        {
                CExtGridCellHyperLink * pValue = STATIC_DOWNCAST(
CExtGridCellHyperLink, ValueActiveGetByRTC( RUNTIME_CLASS(CExtGridCellHyperLink)
) ); ASSERT_VALID( pValue );

                pValue->SetURL( url.c_str() );
        pValue->TextSet( text.c_str() );
                pValue->SetAllowVisitedState( FALSE );
    pValue->SetTextHoverUnderline( TRUE );
//		__ECS_ALL = 0,
//		__ECS_NORMAL = 1,
//		__ECS_SELECTED = 2,
//		__ECS_HOVERED = 3,

                pValue->TextColorSet(
CExtGridCell::e_cell_state_t::__ECS_NORMAL, RGB( 0, 0X6B, 0XF5 ) );
                pValue->TextColorSet(
CExtGridCell::e_cell_state_t::__ECS_HOVERED, RGB( 0XF5, 0X6B, 0 ) );
                pValue->FontUnderlineSet();
        }

};

class CPluginPropertyEmailLink : public CPluginPropertyLink
{
public:
        CPluginPropertyEmailLink( const std::wstring& name, const std::wstring&
email ) : CPluginPropertyLink( name.c_str(), std::wstring( "mailto:" ) + email,
email )
        {
        }

};


class CPluginPropertyText : public CPluginPropertyValueBase
{
public:
        CPluginPropertyText( const std::wstring& name, int n )
        : CPluginPropertyValueBase( name.c_str() )
        {
                std::wostringstream s;
                s << n;

                setText( s.str() );
        }

        CPluginPropertyText( const std::wstring& name, const std::wstring& text
) : CPluginPropertyValueBase( name.c_str() )
        {
                setText( text );
        }

private:
        void setText( const std::wstring& text )
        {
                CExtGridCellString * pValue = STATIC_DOWNCAST(
CExtGridCellString, ValueActiveGetByRTC( RUNTIME_CLASS(CExtGridCellString) ) );
                ASSERT_VALID( pValue );

                pValue->TextSet( text.c_str() );
                pValue->ModifyStyle( __EGCS_READ_ONLY );
        }

};
*/

class CPluginSettings : public CPropertyPage, public Okable {
  DECLARE_DYNCREATE(CPluginSettings)

  const YPlugins* _plugins;
  const yloader::UniqueId* _selectedDSPlugin;

  //  CExtPropertyStore * m_pPS;

 public:
  CPluginSettings(CWnd* pParent = NULL)
      : CPropertyPage(CPluginSettings::IDD) /*: m_pPS( 0 ) */ {
  }                            // standard constructor
  virtual ~CPluginSettings() { /* if( m_pPS != NULL ) m_pPS->Delete(); */
  }

  void init(const YPlugins& plugins,
            const yloader::UniqueId* selectedDSPlugin) {
    _plugins = &plugins;
    _selectedDSPlugin = selectedDSPlugin;
  }

  void postInit() {}

  /*
  CExtPropertyStore* getPropertyStore( const YDataSourcePlugin& plugin )
  {
          ASSERT_VALID( this );
if( m_pPS != NULL )
          {
                  m_pPS->Delete();
          }

m_pPS = new CExtPropertyStore;

          CExtPropertyCategory * pCategoryWindow = new CExtPropertyCategory(
_T("Plugin") ); pCategoryWindow->DescriptionSet( _T("Info about the plug-in
developer") ); VERIFY( m_pPS->ItemInsert( pCategoryWindow ) );
          pCategoryWindow->ItemInsert( new CPluginPropertyText( "Short name",
plugin.shortName() ) ); pCategoryWindow->ItemInsert( new CPluginPropertyText(
"Name", plugin.name() ) ); pCategoryWindow->ItemInsert( new CPluginPropertyText(
"Description", plugin.description() ) ); pCategoryWindow->ItemInsert( new
CPluginPropertyText( "Version", plugin.version() ) );
          pCategoryWindow->ItemInsert( new CPluginPropertyText( "Plugin API
version", plugin.apiVersion() ) ); pCategoryWindow->ItemInsert( new
CPluginPropertyText( "Id", plugin.id() ) ); pCategoryWindow->ItemInsert( new
CPluginPropertyText( "Path", plugin.path() ) );




pCategoryWindow = new CExtPropertyCategory( _T("Developer") );
pCategoryWindow->DescriptionSet( _T("Info about the plug-in developer") );
VERIFY( m_pPS->ItemInsert( pCategoryWindow ) );
  std::wstring name = plugin.developerName();
pCategoryWindow->ItemInsert( new CPluginPropertyText( "Name", name ) );
pCategoryWindow->ItemInsert( new CPluginPropertyLink ( "URL",
plugin.developerURL(), plugin.developerURL() ) ); pCategoryWindow->ItemInsert(
new CPluginPropertyEmailLink ( "Email", plugin.developerEmail() ) );

          pCategoryWindow = new CExtPropertyCategory( _T("Capabilities") );
          VERIFY( m_pPS->ItemInsert( pCategoryWindow ) );
          pCategoryWindow->ItemInsert( new CPluginPropertyText( "Adjusted data",
plugin.canAdjust() ? "Yes" : "No" ) ); pCategoryWindow->ItemInsert( new
CPluginPropertyText( "Unadjusted data", plugin.canUnadjust() ? "Yes" : "No" ) );
          pCategoryWindow->ItemInsert( new CPluginPropertyText( "Max
Connections", plugin.getMaxConnections() ) ); pCategoryWindow->ItemInsert( new
CPluginPropertyText( "Earliest date", yloader::Date( plugin.firstDate()
).toString() ) );



return m_pPS;

  }

  void setPluginInfo( const YDataSourcePlugin& plugin )
  {
          m_pluginInfo.PropertyStoreSet( 0 );
          m_pluginInfo.PropertyStoreSet( getPropertyStore( plugin ) );


//		m_pluginInfoStr = ( std::wstring( "Name: " ) + plugin.name() +
_T( "\r\n" ) ).c_str();
//		m_pluginInfoStr += ( std::wstring( "Description: " ) +
plugin.description() + _T( "\r\n" ) ).c_str();
//		m_pluginInfoStr += ( std::wstring( "Version: " ) +
plugin.version() + _T( "\r\n" ) ).c_str();
//		m_pluginInfoStr += ( std::wstring( "Developer: " ) +
plugin.developerName() + _T( "\r\n" ) ).c_str();



//		CTypedPtrArray < CPtrArray, CExtPropertyGridWnd * > arrGrids;
//		m_pluginInfo.OnPgcQueryGrids( arrGrids );
//		INT nGridIdx = 0;
//		for( ; nGridIdx < arrGrids.GetSize(); nGridIdx ++ )
//		{
//			CExtPropertyGridWnd * pGrid = arrGrids[ nGridIdx ];
//			ASSERT_VALID( pGrid );
//			pGrid->m_bSortedCategories = false;
//		} // for( ; nGridIdx < arrGrids.GetSize(); nGridIdx ++ )

          UpdateData( false );
  }
*/

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
    return _selectedDSPlugin;
  }
  afx_msg void OnLbnSelchangeListDsPlugins();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

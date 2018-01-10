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

#include <strings.h>
#include <deque>

// there can't be more than 8 MRU
#define MRU_MAX 8

enum MRUType { symbolsFile, outputPath, outputFile };

class MRU : public std::deque<std::wstring> {
 private:
  unsigned int m_max;
  MRUType m_type;
  const std::wstring m_mruBase;

 public:
  MRU(MRUType type)
      : m_max(MRU_MAX), m_type(type), m_mruBase(mruTypeToString(type)) {
    for (unsigned int n = 0; n < MRU_MAX; n++) {
      std::wostringstream o;

      o << n;

      std::wstring mru = m_mruBase + o.str();
      CString s = AfxGetApp()->GetProfileString(_T( "YahooData" ), mru.c_str(),
                                                _T( "" ));
      if (!s.IsEmpty()) __super::push_back((LPCTSTR)s);
    }
  }

  std::wstring mruTypeToString(MRUType type) {
    switch (type) {
      case symbolsFile:
        return _T( "mruSymbolsFile" );
      case outputPath:
        return _T( "mruOutputPath" );
      case outputFile:
        return _T( "mruOutputFile" );
      default:
        assert(false);
        return _T( "" );
    }
  }

  ~MRU() {
    for (unsigned int n = 0; n < MRU_MAX; n++) {
      std::wostringstream o;

      o << n;

      std::wstring mru = m_mruBase + o.str();
      AfxGetApp()->WriteProfileString(
          _T( "YahooData" ), mru.c_str(),
          n < __super::size() ? (*this)[n].c_str() : _T( "" ));
    }
  }
  void add(const std::wstring& item) {
    __super::push_front(item);

    __super::iterator i = std::find(++__super::begin(), __super::end(), item);
    if (i != __super::end()) {
      __super::erase(i);
    }

    assert(m_max > 0);

    if (__super::size() > m_max) __super::pop_back();
  }
};

#define MRU_SYMBOLS_ID 3000
#define MRU_OUTPUT_PATH_ID 3100
#define MRU_OUTPUT_FILE_ID 3200
#define CLEAR_MRU_SYMBOLS_ID 3020
#define CLEAR_MRU_OUTPUT_PATH_ID 3120
#define CLEAR_MRU_OUTPUT_FILE_ID 3220

class CMenuButton : public CButton {
 private:
  MRU m_mru;

 protected:
  virtual unsigned int getMRUId() const = 0;
  virtual unsigned int getClearId() const = 0;

 public:
  CMenuButton(MRUType type) : m_mru(type) {}

  virtual void init() = 0;

  void clearMenu() {
    /*		if( m_menu.GetSafeHmenu() )
                    {
                            VERIFY( m_menu.DestroyMenu() );
                            CWinApp * pApp = ::AfxGetApp();
                            for( unsigned int m = 0; m < MRU_MAX; m++ )
                                    VERIFY( g_CmdManager->CmdRemove(
       pApp->m_pszProfileName, getMRUId() + m ) );

                            VERIFY( g_CmdManager->CmdRemove(
       pApp->m_pszProfileName, getClearId() ) );
                    }
                    */
  }

  void clear() {
    m_mru.clear();
    clearMenu();
  }

  void add(const std::wstring& item) {
    m_mru.add(item);
    setMenu();
  }

  void setMenu() {
    /*		m_dwMenuOpt |= TPMX_OWNERDRAW_FIXED;

                    clearMenu();

                    if( m_mru.size() > 0 )
                    {
                            VERIFY( m_menu.CreateMenu() );

                            CMenu m_xmenu;
                            VERIFY( m_xmenu.CreatePopupMenu() );
    //			OutputDebugString( "\nmenu: \n" );

                            unsigned int m;
                            for( m = 0; m < m_mru.size(); m++ )
                            {
                                    const std::wstring& value = m_mru[ m ];
    //				OutputDebugString( value.c_str() );
    //				OutputDebugString( "\n" );
                                    if( !value.empty() )
                                    {
                                            VERIFY( m_xmenu.AppendMenu(
    MF_STRING, m+getMRUId(), value.c_str() ) );
                                    }
                            }

                            m_xmenu.AppendMenu( MF_SEPARATOR );
                            m_xmenu.AppendMenu( MF_STRING, getClearId(), _T(
    "Clear list" ) );

                            m_menu.AppendMenu( MF_POPUP, (UINT)m_xmenu.Detach(),
    "mru" );

                            SetSeparatedDropDown();
                    }
    */
  }

  const std::wstring& operator[](unsigned int ix) const { return m_mru[ix]; }
};

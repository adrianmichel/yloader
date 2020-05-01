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

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"  // main symbols

// CyloaderApp:
// See yloader.cpp for the implementation of this class
//

class CYLoaderApp : public CWinAppEx {
 private:
  std::shared_ptr<Updater> m_updater;

 public:
  CYLoaderApp();

  // Overrides
 public:
  BOOL InitInstance() override;
  BOOL ExitInstance() override;

  // Implementation
  UINT m_nAppLook;
  BOOL m_bHiColorIcons;

  void PreLoadState() override;
  void LoadCustomState() override;
  void SaveCustomState() override;

  afx_msg void OnAppAbout();
  DECLARE_MESSAGE_MAP()

 public:
  void setUpdater(const StringSettable& alternateUpdateHostName, const StringSettable& alternateUpdateDirectory);
};

extern CYLoaderApp theApp;

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

#include <YahooHistData.h>
#include <miscfile.h>
#include <strings.h>
#include <thread.h>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include "DownloadListCtrl.h"
#include "EmptyDataPathPromptDlg.h"
#include "HListBox.h"
#include "YloaderView.h"
#include "downloader.h"
#include "dropedit.h"
#include "mru.h"

#include "CharacterMapping.h"

class SymbolsButton : public CMenuButton {
 public:
  SymbolsButton() : CMenuButton(symbolsFile) {}

  void init() {
    /*		setMenu();
                    m_icon.AssignFromHICON((HICON)
       ::LoadImage(::AfxGetInstanceHandle(),
       MAKEINTRESOURCE(IDI_ICON_OPEN_SYMBOLS), IMAGE_ICON, 16, 16, 0), false);
                    */
  }

 protected:
  virtual unsigned int getMRUId() const { return MRU_SYMBOLS_ID; }
  virtual unsigned int getClearId() const { return CLEAR_MRU_SYMBOLS_ID; }
};

class OutputPathButton : public CMenuButton {
 public:
  OutputPathButton() : CMenuButton(outputPath) {}
  void init() {
    /*		setMenu();
                    m_icon.AssignFromHICON((HICON)
       ::LoadImage(::AfxGetInstanceHandle(),
       MAKEINTRESOURCE(IDI_ICON_OUTPUT_PATH), IMAGE_ICON, 16, 16, 0), false);
                    */
  }

 protected:
  virtual unsigned int getMRUId() const { return MRU_OUTPUT_PATH_ID; }
  virtual unsigned int getClearId() const { return CLEAR_MRU_OUTPUT_PATH_ID; }
};

class OutputFileButton : public CMenuButton {
 public:
  OutputFileButton() : CMenuButton(outputFile) {}

  void init() {
    /*		setMenu();
                    m_icon.AssignFromHICON((HICON)
       ::LoadImage(::AfxGetInstanceHandle(),
       MAKEINTRESOURCE(IDI_ICON_SAVE_ONE_FILE), IMAGE_ICON, 16, 16, 0), false);
                    */
  }

 protected:
  virtual unsigned int getMRUId() const { return MRU_OUTPUT_FILE_ID; }
  virtual unsigned int getClearId() const { return CLEAR_MRU_OUTPUT_FILE_ID; }
};

class EditSymbolsListButton : public CButton {
 public:
  void init() {
    //		m_icon.AssignFromHICON((HICON) ::LoadImage(::AfxGetInstanceHandle(),
    //MAKEINTRESOURCE(IDI_ICON_EDIT_SYMBOLS_LIST), IMAGE_ICON, 16, 16, 0),
    //false);
  }
};

class ExploreOutputFolderButton : public CButton {
 public:
  void init() {
    //		m_icon.AssignFromHICON((HICON) ::LoadImage(::AfxGetInstanceHandle(),
    //MAKEINTRESOURCE(IDI_ICON_EXPLORE_OUTPUT_PATH), IMAGE_ICON, 16, 16, 0),
    //false);
  }
};

class OpenOneOutputFile : public CButton {
 public:
  void init() {
    //		m_icon.AssignFromHICON((HICON) ::LoadImage(::AfxGetInstanceHandle(),
    //MAKEINTRESOURCE(IDI_ICON_OPEN_ONE_OUTPUT_FILE), IMAGE_ICON, 16, 16, 0),
    //false);
  }
};

enum {
  YM_UPDATE_AVAILABLE = WM_USER + 1000,
  YM_NO_UPDATE_AVAILABLE,
  YM_CHECKING_FOR_UPDATE,
  YM_UPDATE_ERROR
};

class CMainFrame : public CFrameWndEx, Frame, public UpdaterHandler {
 public:
  CYloaderView* getView() { return (CYloaderView*)(this->GetActiveView()); }

  virtual void Frame::closeFrame();

 protected:  // create from serialization only
  CMainFrame();

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
  DECLARE_DYNCREATE(CMainFrame)

  // Attributes
 public:
  // Operations
 public:
  // Overrides
 public:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual BOOL LoadFrame(UINT nIDResource,
                         DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW |
                                                FWS_ADDTOTITLE,
                         CWnd* pParentWnd = NULL,
                         CCreateContext* pContext = NULL);

  // Implementation
 public:
  virtual ~CMainFrame();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

 protected:  // control bar embedded members
  CMFCMenuBar m_wndMenuBar;
  CMFCToolBar m_wndToolBar;
  CMFCStatusBar m_wndStatusBar;
  CMFCToolBarImages m_UserImages;

  HICON m_hIcon;

 private:
  boost::shared_ptr<Updater> m_updater;

  // Generated message map functions
 protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnViewCustomize();
  afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
  afx_msg void OnApplicationLook(UINT id);
  afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 public:
  afx_msg void OnSettings();
  afx_msg void OnDestroy();

 private:
  std::wstring getSymbolsFileSample() {
    return yloader::addBackSlash(getLocalYLoaderDataPath()) +
           _T( "symbols\\sampleSymbols.txt" );
  }

  std::wstring getDataPathSample() {
    return yloader::getDirectory(getLocalYLoaderDataPath(), _T( "data" ));
  }

 public:
  afx_msg void OnDownload();
  afx_msg void OnCancelDownload();
  afx_msg void OnUpdateDownload(CCmdUI* pCmdUI);
  afx_msg void OnUpdateCancelDownload(CCmdUI* pCmdUI);
  afx_msg void OnUpdateSettings(CCmdUI* pCmdUI);
  afx_msg void OnUpdate();
  afx_msg void OnTimer(UINT_PTR nIDEvent);

 private:
  std::wstring lastUpdateError;

 public:
  virtual void updateAvailable();
  virtual void checkingForUpdate();
  virtual void noUpdateAvailable();
  virtual void updateError(const std::wstring& error);

 protected:
  afx_msg LRESULT OnYmUpdateAvailable(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnYmNoUpdateAvailable(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnYmCheckingForUpdate(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnYmUpdateError(WPARAM wParam, LPARAM lParam);

 public:
  void init(boost::shared_ptr<Updater> updater);

 private:
  bool m_autoUpdating;

 public:
  afx_msg void OnHelpYgqdhome();
  afx_msg void OnHelpEmailygqdsupport();
  afx_msg void OnOpensettingsfile();

  std::wstring getYLoaderSettingsFile() {
    return addBackSlash(
               getDirectory(getLocalYLoaderDataPath(), _T( "settings" ))) +
           _T( "settings.ini" );
  }
  afx_msg void OnHelp();
  afx_msg void OnHelpReleasenotes();
  afx_msg void OnHelpYloaderongithub();
};

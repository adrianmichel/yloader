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
  }

 protected:
  unsigned int getMRUId() const override { return MRU_SYMBOLS_ID; }
  unsigned int getClearId() const override { return CLEAR_MRU_SYMBOLS_ID; }
};

class OutputPathButton : public CMenuButton {
 public:
  OutputPathButton() : CMenuButton(outputPath) {}
  void init() {
  }

 protected:
  unsigned int getMRUId() const override { return MRU_OUTPUT_PATH_ID; }
  unsigned int getClearId() const override { return CLEAR_MRU_OUTPUT_PATH_ID; }
};

class OutputFileButton : public CMenuButton {
 public:
  OutputFileButton() : CMenuButton(outputFile) {}

  void init() {
  }

 protected:
  unsigned int getMRUId() const override { return MRU_OUTPUT_FILE_ID; }
  unsigned int getClearId() const override { return CLEAR_MRU_OUTPUT_FILE_ID; }
};

class EditSymbolsListButton : public CButton {
 public:
  void init() {
  }
};

class ExploreOutputFolderButton : public CButton {
 public:
  void init() {
  }
};

class OpenOneOutputFile : public CButton {
 public:
  void init() {
  }
};

enum {
  YM_UPDATE_AVAILABLE = WM_USER + 1000,
  YM_NO_UPDATE_AVAILABLE,
  YM_CHECKING_FOR_UPDATE,
  YM_UPDATE_ERROR
};

class CMainFrame : public CFrameWndEx, Frame, public UpdaterHandler {
  OBJ_COUNTER(CMainFrame)
 public:
  CYLoaderView* getView() { return (CYLoaderView*)(this->GetActiveView()); }

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
  BOOL PreCreateWindow(CREATESTRUCT& cs) override;
  BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
                         CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL) override;

  // Implementation
 public:
  virtual ~CMainFrame();
#ifdef _DEBUG
  void AssertValid() const override;
  void Dump(CDumpContext& dc) const override;
#endif

 protected:  // control bar embedded members
  CMFCMenuBar m_wndMenuBar;
  CMFCToolBar m_wndToolBar;
  CMFCStatusBar m_wndStatusBar;
  CMFCToolBarImages m_UserImages;

  HICON m_hIcon;

 private:
  std::shared_ptr<Updater> m_updater;

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
    return yloader::addBackSlash(getLocalYLoaderDataPath()) + L"symbols\\sampleSymbols.txt";
  }

  std::wstring getDataPathSample() {
    return yloader::getDirectory(getLocalYLoaderDataPath(), L"data" );
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
  void updateAvailable() override;
  void checkingForUpdate() override;
  void noUpdateAvailable() override;
  void updateError(const std::wstring& error) override;

 protected:
  afx_msg LRESULT OnYmUpdateAvailable(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnYmNoUpdateAvailable(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnYmCheckingForUpdate(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnYmUpdateError(WPARAM wParam, LPARAM lParam);

 public:
  void init(std::shared_ptr<Updater> updater);

 private:
  bool m_autoUpdating;

 public:
  afx_msg void OnHelpYgqdhome();
  afx_msg void OnHelpEmailygqdsupport();
  afx_msg void OnOpensettingsfile();

  std::wstring getYLoaderSettingsFile() {
    return addBackSlash(getDirectory(getLocalYLoaderDataPath(), L"settings" )) + L"settings.ini";
  }
  afx_msg void OnHelp();
  afx_msg void OnHelpReleasenotes();
  afx_msg void OnHelpYloaderongithub();
};

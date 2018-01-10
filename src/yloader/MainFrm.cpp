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
#include "yloader.h"

#include "MainFrm.h"

#include <sharedptr.h>
#include <miscfile.h>
#include <defaults.h>
#include <YahooEventSink.h>
#include "YloaderView.h"
#include "UpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
ON_WM_CREATE()
ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7,
                 &CMainFrame::OnApplicationLook)
ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7,
                           &CMainFrame::OnUpdateApplicationLook)
ON_COMMAND(ID_SETTINGS, &CMainFrame::OnSettings)
ON_WM_DESTROY()
ON_COMMAND(ID_DOWNLOAD, &CMainFrame::OnDownload)
ON_COMMAND(ID_CANCEL_DOWNLOAD, &CMainFrame::OnCancelDownload)
ON_UPDATE_COMMAND_UI(ID_DOWNLOAD, &CMainFrame::OnUpdateDownload)
ON_UPDATE_COMMAND_UI(ID_CANCEL_DOWNLOAD, &CMainFrame::OnUpdateCancelDownload)
ON_UPDATE_COMMAND_UI(ID_SETTINGS, &CMainFrame::OnUpdateSettings)
ON_COMMAND(ID_UPDATE, &CMainFrame::OnUpdate)
ON_WM_TIMER()
ON_MESSAGE(YM_UPDATE_AVAILABLE, &CMainFrame::OnYmUpdateAvailable)
ON_MESSAGE(YM_NO_UPDATE_AVAILABLE, &CMainFrame::OnYmNoUpdateAvailable)
ON_MESSAGE(YM_UPDATE_ERROR, &CMainFrame::OnYmUpdateError)
ON_MESSAGE(YM_CHECKING_FOR_UPDATE, &CMainFrame::OnYmCheckingForUpdate)
ON_COMMAND(ID_HELP_YGQDHOME, &CMainFrame::OnHelpYgqdhome)
ON_COMMAND(ID_HELP_EMAILYGQDSUPPORT, &CMainFrame::OnHelpEmailygqdsupport)
ON_COMMAND(ID_OPENSETTINGSFILE, &CMainFrame::OnOpensettingsfile)
ON_COMMAND(ID_HELP, &CMainFrame::OnHelp)
ON_COMMAND(ID_HELP_RELEASENOTES, &CMainFrame::OnHelpReleasenotes)
ON_COMMAND(ID_HELP_YLOADERONGITHUB, &CMainFrame::OnHelpYloaderongithub)
END_MESSAGE_MAP()

static UINT indicators[] = {
    ID_SEPARATOR,  // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() {
  // TODO: add member initialization code here
  theApp.m_nAppLook =
      theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);
}

CMainFrame::~CMainFrame() {}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CFrameWndEx::OnCreate(lpCreateStruct) == -1) return -1;

  BOOL bNameValid;

  if (!m_wndMenuBar.Create(this)) {
    TRACE0("Failed to create menubar\n");
    return -1;  // fail to create
  }

  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  SetIcon(m_hIcon, TRUE);   // Set big icon
  SetIcon(m_hIcon, FALSE);  // Set small icon

  m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC |
                            CBRS_TOOLTIPS | CBRS_FLYBY);

  // prevent the menu bar from taking the focus on activation
  CMFCPopupMenu::SetForceMenuFocus(FALSE);

  if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,
                             WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER |
                                 CBRS_TOOLTIPS | CBRS_FLYBY |
                                 CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAIN_TOOLBAR
                                                       : IDR_MAIN_TOOLBAR)) {
    TRACE0("Failed to create toolbar\n");
    return -1;  // fail to create
  }

  CString strToolBarName;
  bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
  ASSERT(bNameValid);
  m_wndToolBar.SetWindowText(strToolBarName);
  /*
          CString strCustomize;
          bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
          ASSERT(bNameValid);
          m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE,
     strCustomize);
  */
  // Allow user-defined toolbars operations:
  InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

  if (!m_wndStatusBar.Create(this)) {
    TRACE0("Failed to create status bar\n");
    return -1;  // fail to create
  }
  m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

  // TODO: Delete these five lines if you don't want the toolbar and menubar to
  // be dockable
  m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockPane(&m_wndMenuBar);
  DockPane(&m_wndToolBar);

  // enable Visual Studio 2005 style docking window behavior
  //	CDockingManager::SetDockingMode(DT_SMART);
  // enable Visual Studio 2005 style docking window auto-hide behavior
  //	EnableAutoHidePanes(CBRS_ALIGN_ANY);
  // set the visual manager and style based on persisted value
  OnApplicationLook(theApp.m_nAppLook);

  // Enable toolbar and docking window menu replacement
  //	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

  // enable quick (Alt+drag) toolbar customization
  //	CMFCToolBar::EnableQuickCustomization();

  if (CMFCToolBar::GetUserImages() == NULL) {
    // load user-defined toolbar images
    if (m_UserImages.Load(_T(".\\UserImages.bmp"))) {
      CMFCToolBar::SetUserImages(&m_UserImages);
    }
  }

  // enable menu personalization (most-recently used commands)
  // TODO: define your own basic commands, ensuring that each pulldown menu has
  // at least one basic command.
  CList<UINT, UINT> lstBasicCommands;

  lstBasicCommands.AddTail(ID_FILE_NEW);
  lstBasicCommands.AddTail(ID_FILE_OPEN);
  lstBasicCommands.AddTail(ID_FILE_SAVE);
  lstBasicCommands.AddTail(ID_FILE_PRINT);
  lstBasicCommands.AddTail(ID_APP_EXIT);
  lstBasicCommands.AddTail(ID_EDIT_CUT);
  lstBasicCommands.AddTail(ID_EDIT_PASTE);
  lstBasicCommands.AddTail(ID_EDIT_UNDO);
  lstBasicCommands.AddTail(ID_APP_ABOUT);
  lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
  lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
  lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
  lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
  lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
  lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
  lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
  lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
  lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

  //	CMFCToolBar::SetBasicCommands(lstBasicCommands);

  return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
  if (!CFrameWndEx::PreCreateWindow(cs)) return FALSE;
  cs.style &= ~WS_MAXIMIZEBOX & ~WS_THICKFRAME & ~(LONG)FWS_ADDTOTITLE;

  return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const { CFrameWndEx::AssertValid(); }

void CMainFrame::Dump(CDumpContext& dc) const { CFrameWndEx::Dump(dc); }
#endif  //_DEBUG

// CMainFrame message handlers

void CMainFrame::OnViewCustomize() {
  //	CMFCToolBarsCustomizeDialog* pDlgCust = new
  //CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
  //	pDlgCust->EnableUserDefinedToolbars();
  //	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp, LPARAM lp) {
  LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp, lp);
  if (lres == 0) {
    return 0;
  }

  CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
  ASSERT_VALID(pUserToolbar);

  BOOL bNameValid;
  CString strCustomize;
  bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
  ASSERT(bNameValid);

  pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
  return lres;
}

void CMainFrame::OnApplicationLook(UINT id) {
  CWaitCursor wait;

  theApp.m_nAppLook = id;

  switch (theApp.m_nAppLook) {
    case ID_VIEW_APPLOOK_WIN_2000:
      CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
      break;

    case ID_VIEW_APPLOOK_OFF_XP:
      CMFCVisualManager::SetDefaultManager(
          RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
      break;

    case ID_VIEW_APPLOOK_WIN_XP:
      CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
      CMFCVisualManager::SetDefaultManager(
          RUNTIME_CLASS(CMFCVisualManagerWindows));
      break;

    case ID_VIEW_APPLOOK_OFF_2003:
      CMFCVisualManager::SetDefaultManager(
          RUNTIME_CLASS(CMFCVisualManagerOffice2003));
      CDockingManager::SetDockingMode(DT_SMART);
      break;

    case ID_VIEW_APPLOOK_VS_2005:
      CMFCVisualManager::SetDefaultManager(
          RUNTIME_CLASS(CMFCVisualManagerVS2005));
      CDockingManager::SetDockingMode(DT_SMART);
      break;

    case ID_VIEW_APPLOOK_VS_2008:
      CMFCVisualManager::SetDefaultManager(
          RUNTIME_CLASS(CMFCVisualManagerVS2008));
      CDockingManager::SetDockingMode(DT_SMART);
      break;

    case ID_VIEW_APPLOOK_WINDOWS_7:
      CMFCVisualManager::SetDefaultManager(
          RUNTIME_CLASS(CMFCVisualManagerWindows7));
      CDockingManager::SetDockingMode(DT_SMART);
      break;

    default:
      switch (theApp.m_nAppLook) {
        case ID_VIEW_APPLOOK_OFF_2007_BLUE:
          CMFCVisualManagerOffice2007::SetStyle(
              CMFCVisualManagerOffice2007::Office2007_LunaBlue);
          break;

        case ID_VIEW_APPLOOK_OFF_2007_BLACK:
          CMFCVisualManagerOffice2007::SetStyle(
              CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
          break;

        case ID_VIEW_APPLOOK_OFF_2007_SILVER:
          CMFCVisualManagerOffice2007::SetStyle(
              CMFCVisualManagerOffice2007::Office2007_Silver);
          break;

        case ID_VIEW_APPLOOK_OFF_2007_AQUA:
          CMFCVisualManagerOffice2007::SetStyle(
              CMFCVisualManagerOffice2007::Office2007_Aqua);
          break;
      }

      CMFCVisualManager::SetDefaultManager(
          RUNTIME_CLASS(CMFCVisualManagerOffice2007));
      CDockingManager::SetDockingMode(DT_SMART);
  }

  RedrawWindow(
      NULL, NULL,
      RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

  theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI) {
  pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
                           CWnd* pParentWnd, CCreateContext* pContext) {
  // base class does the real work

  if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd,
                              pContext)) {
    return FALSE;
  }

  /*
          // enable customization button for all user toolbars
          BOOL bNameValid;
          CString strCustomize;
          bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
          ASSERT(bNameValid);

          for (int i = 0; i < iMaxUserToolbars; i ++)
          {
                  CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
                  if (pUserToolbar != NULL)
                  {
                          pUserToolbar->EnableCustomizeButton(TRUE,
     ID_VIEW_CUSTOMIZE, strCustomize);
                  }
          }
  */
  return TRUE;
}

void CMainFrame::OnSettings() { getView()->OnSettings(); }

void CMainFrame::OnDestroy() {
  DestroyIcon(m_hIcon);
  CFrameWndEx::OnDestroy();
}

void CMainFrame::closeFrame() { PostMessage(WM_CLOSE); }

void CMainFrame::OnDownload() { getView()->OnDownload(); }

void CMainFrame::OnCancelDownload() { getView()->OnCancelDownload(); }

void CMainFrame::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);

  //	DDX_Control(pDX, IDC_MAIN_TOOLBAR, m_wndMenuBar);
  //	DDX_Control(pDX, IDC_MAIN_TOOLBAR, m_wndToolBar);
}

void CMainFrame::OnUpdateDownload(CCmdUI* pCmdUI) {
  pCmdUI->Enable(!getView()->isRunning());
}

void CMainFrame::OnUpdateCancelDownload(CCmdUI* pCmdUI) {
  pCmdUI->Enable(getView()->isRunning());
}

void CMainFrame::OnUpdateSettings(CCmdUI* pCmdUI) {
  pCmdUI->Enable(!getView()->isRunning());
}

void CMainFrame::OnUpdate() {
  m_autoUpdating = false;
  m_updater->start();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent) {
  // TODO: Add your message handler code here and/or call default

  __super::OnTimer(nIDEvent);
}

afx_msg LRESULT CMainFrame::OnYmUpdateAvailable(WPARAM wParam, LPARAM lParam) {
  std::wstring newVersion = m_updater->getVersion().toString();
  std::wstring updateOnlyVersion =
      getView()->getNotifyOnlyIfNewerVersion().toString();

  m_wndStatusBar.SetPaneText(0, (std::wstring(_T( "New version available: " )) +
                                 m_updater->getVersion().toString())
                                    .c_str());
  if (!m_autoUpdating ||
      m_updater->getVersion() > getView()->getNotifyOnlyIfNewerVersion()) {
    CUpdateDlg updateDlg(*m_updater->getUpdateInfo(),
                         getView()->checkForUpdatesAtStartup(),
                         getView()->getNotifyOnlyIfNewerVersion());
    updateDlg.DoModal();

    getView()->setNotifyOnlyIfNewerVersion(
        updateDlg.getNotifyOnlyIfNewerVersion() ? m_updater->getVersion()
                                                : Version());
    getView()->setCheckForUpdatesAtStartup(updateDlg.getCheckAtStartup());
  }

  return 0;
}

void CMainFrame::updateAvailable() { PostMessage(YM_UPDATE_AVAILABLE); }

void CMainFrame::checkingForUpdate() { PostMessage(YM_CHECKING_FOR_UPDATE); }

void CMainFrame::noUpdateAvailable() { PostMessage(YM_NO_UPDATE_AVAILABLE); }

void CMainFrame::updateError(const std::wstring& error) {
  lastUpdateError = error;
  PostMessage(YM_UPDATE_ERROR);
}

#define LATEST_VERSION _T("You are running the latest YLoader version")
#define UPDATE_ERROR std::wstring(_T("Update error: "))
afx_msg LRESULT CMainFrame::OnYmNoUpdateAvailable(WPARAM wParam,
                                                  LPARAM lParam) {
  m_wndStatusBar.SetPaneText(0, LATEST_VERSION);
  if (!m_autoUpdating) {
    AfxMessageBox(LATEST_VERSION);
  }
  return 0;
}

afx_msg LRESULT CMainFrame::OnYmCheckingForUpdate(WPARAM wParam,
                                                  LPARAM lParam) {
  m_wndStatusBar.SetPaneText(0, _T( "Checking for updates" ));
  return 0;
}

void CMainFrame::init(boost::shared_ptr<Updater> updater) {
  getView()->setFrame(this);
  getView()->processAutoStart();

  m_updater = updater;

  if (getView()->checkForUpdatesAtStartup()) {
    m_autoUpdating = true;
    m_updater->start();
  }
}

afx_msg LRESULT CMainFrame::OnYmUpdateError(WPARAM wParam, LPARAM lParam) {
  m_wndStatusBar.SetPaneText(0, (UPDATE_ERROR + lastUpdateError).c_str());
  if (!m_autoUpdating) {
    AfxMessageBox((UPDATE_ERROR + lastUpdateError).c_str());
  }
  return 0;
}

#define YGQD_DOCUMENTATION YLOADER_URL _T( "/doc/index.html" )
#define RELEASE_NOTES_DIRECTORY _T( "release-notes")
#define RELEASE_NOTES_QUERY \
  YLOADER_URL _T( "/") RELEASE_NOTES_DIRECTORY _T( "?") VERSION_QUERY_ARGS

#define YLOADER_HOME YLOADER_URL _T( "?" ) VERSION_QUERY_ARGS
#define YLOADER_ON_GITHUB _T( "https://www.github.com/adrianmichel/yloader")


void CMainFrame::OnHelpYgqdhome() {
  int ret = (int)ShellExecute(0, 0, YLOADER_HOME, 0, 0, SW_SHOWNORMAL);
}

void CMainFrame::OnHelpEmailygqdsupport() {
  int ret = (int)ShellExecute(0, 0, _T("mailto:info@yloader.com"), 0, 0,
                              SW_SHOWNORMAL);
}

void CMainFrame::OnOpensettingsfile() {
  int ret = (int)ShellExecute(0, 0, getYLoaderSettingsFile().c_str(), 0, 0,
                              SW_SHOWNORMAL);
}

void CMainFrame::OnHelp() {
  int ret = (int)ShellExecute(0, 0, YGQD_DOCUMENTATION, 0, 0, SW_SHOWNORMAL);
}

void CMainFrame::OnHelpReleasenotes() {
  int ret = (int)ShellExecute(0, 0, RELEASE_NOTES_QUERY, 0, 0, SW_SHOWNORMAL);
}


void CMainFrame::OnHelpYloaderongithub()
{
	int ret = (int)ShellExecute(0, 0, YLOADER_ON_GITHUB, 0, 0, SW_SHOWNORMAL);
}

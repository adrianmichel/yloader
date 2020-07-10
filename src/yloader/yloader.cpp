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

#include "stdafx.h"

#include "MainFrm.h"
#include "afxdialogex.h"
#include "afxwinappex.h"
#include "yloader.h"

#include <CmdLine.h>
#include <versionno.h>
#include "YloaderView.h"
#include "yloaderDoc.h"

using namespace std::string_literals;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void session();
// CyloaderApp

BEGIN_MESSAGE_MAP(CYLoaderApp, CWinAppEx)
ON_COMMAND(ID_APP_ABOUT, &CYLoaderApp::OnAppAbout)
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

// CyloaderApp construction

CYLoaderApp::CYLoaderApp() {
  m_bHiColorIcons = TRUE;

  // support Restart Manager
  m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

  // TODO: replace application ID string below with unique ID string;
  // recommended format for string is
  // CompanyName.ProductName.SubProduct.VersionInformation
  SetAppID(L"yloader.AppID.NoVersion");
}

// The one and only CyloaderApp object
CYLoaderApp theApp;

CMainFrame& getMainFrame() { return *dynamic_cast< CMainFrame* >(AfxGetMainWnd()); }

std::wstring getLogFileDirectory() {
  std::wstring logPath(getDirectory(getLocalYLoaderDataPath(), L"log"));
  if (!DirectoryExists(logPath.c_str())) {
    ::CreateDirectory(logPath.c_str(), 0);
  }

  return logPath;
}

std::wstring getLogFileName() {
  const std::wstring LOG_FILE(L"yloader");

  const std::wstring logFileDirectory(getLogFileDirectory());

  return logFileDirectory.empty()
             ? L""
             : makeFileName(getLogFileDirectory(), LOG_FILE, L"log");
}
BOOL CYLoaderApp::ExitInstance() {
  // uninit miscwin
  uninit();
  __super::ExitInstance();
  return TRUE;
}

BOOL CYLoaderApp::InitInstance() {
  // init miscwin
  init();
  // first set up logging
  Log::setLogToFile(getLogFileName());

  // InitCommonControlsEx() is required on Windows XP if an application
  // manifest specifies use of ComCtl32.dll version 6 or later to enable
  // visual styles.  Otherwise, any window creation will fail.
  INITCOMMONCONTROLSEX InitCtrls;
  InitCtrls.dwSize = sizeof(InitCtrls);
  // Set this to include all the common control classes you want to use
  // in your application.
  InitCtrls.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&InitCtrls);
  AfxInitRichEdit2();

  CWinAppEx::InitInstance();

  EnableTaskbarInteraction(FALSE);

  // AfxInitRichEdit2() is required to use RichEdit control
  // AfxInitRichEdit2();

  // Standard initialization
  // If you are not using these features and wish to reduce the size
  // of your final executable, you should remove from the following
  // the specific initialization routines you do not need
  // Change the registry key under which our settings are stored
  // TODO: You should modify this string to be something appropriate
  // such as the name of your company or organization
  SetRegistryKey(L"YLoader");
  LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
  InitContextMenuManager();
  InitKeyboardManager();
  InitTooltipManager();

  CMFCToolTipInfo ttParams;
  ttParams.m_bVislManagerTheme = TRUE;
  theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

  // Register the application's document templates.  Document templates
  //  serve as the connection between documents, frame windows and views
  CSingleDocTemplate* pDocTemplate;
  pDocTemplate = new CSingleDocTemplate(IDR_MAIN_TOOLBAR, RUNTIME_CLASS(CYLoaderDoc),
      RUNTIME_CLASS(CMainFrame),  // main SDI frame window
      RUNTIME_CLASS(CYLoaderView));
  if (!pDocTemplate) {
    return FALSE;
  }
  AddDocTemplate(pDocTemplate);

  OnFileNew();
  // The one and only window has been initialized, so show and update it
  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();

  try {
   session();
   // here one could check if the "help" flag is set,
   // and if so, show a dialog with command line help or
   // a link to the online documentation
   getMainFrame().init(m_updater);
   AfxGetMainWnd()->SetWindowText(getCaption(STRPRODUCT_NAME).c_str());

   return TRUE;
  }
  catch (const CmdLineException& e) {
    // if command line error, ignore cmd line params
    AfxMessageBox(yloader::s2ws(e.what()).c_str(), MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }
  catch (const std::exception& e) {
    AfxMessageBox(yloader::s2ws(e.what()).c_str(), MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }
  catch (...) {
    AfxMessageBox(L"Unknown error", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }
}

void CYLoaderApp::setUpdater(const StringSettable& alternateUpdateHostName, const StringSettable& alternateUpdateDirectory) {
  m_updater = std::make_shared<Updater>(getMainFrame(), L"en_us", alternateUpdateHostName, alternateUpdateDirectory);
}

class CAboutDlg : public CDialogEx {
 public:
  CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_ABOUTBOX };
#endif

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

  // Implementation
 protected:
  DECLARE_MESSAGE_MAP()
 public:
  CString m_name;
  virtual BOOL OnInitDialog();
  CString m_buildDate;
};

CAboutDlg::CAboutDlg()
    : CDialogEx(IDD_ABOUTBOX){}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_STATIC_NAME, m_name);
  DDX_Text(pDX, IDC_STATIC_BUILD_DATE, m_buildDate);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CYLoaderApp::OnAppAbout() {
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();
}

// CyloaderApp customization load/save methods

void CYLoaderApp::PreLoadState() {
  BOOL bNameValid;
  CString strName;
  bNameValid = strName.LoadString(IDS_EDIT_MENU);
  ASSERT(bNameValid);
  GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CYLoaderApp::LoadCustomState() {}

void CYLoaderApp::SaveCustomState() {}

CYLoaderView* getView() {
  return (CYLoaderView*)getMainFrame().GetActiveView();
}

bool setUpdater(const StringSettable& alternateUpdateHostName, const StringSettable& alternateUpdateDirectory) {
  theApp.setUpdater(alternateUpdateHostName, alternateUpdateDirectory);
  return true;
}

bool close() {
  AfxGetMainWnd()->PostMessageW(WM_CLOSE);
  return true;
}

void setAddSymbolToColumn(bool add, unsigned int colum) {
  getView()->setAddSymbolToColumn(add, colum);
}

bool loadSettings(const TCHAR* settingsName) {
  getView()->LoadParams(std::wstring(), std::wstring());
  return true;
}

void showInfo(const TCHAR* text, const TCHAR* title) {
  ::MessageBox(0, text, title, MB_OK | MB_ICONINFORMATION);
}

BOOL CAboutDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();

  m_name = getCaption(STRPRODUCT_NAME).c_str();
  m_buildDate = (L"Built: "s + VersionInfo::getCurrentBuildInfo()->to_simple_string() + L" UTC").c_str();

  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

template< typename T > using CmdLineGetter = std::function< const Settable< T >&(CmdLine*) >;
template< typename T > using ViewSetter = std::function< void( CYLoaderView*, const T& ) >;

class SetArg {
private:
  CmdLinePtr m_cmdLine;
  CYLoaderView* m_view;

public:
  SetArg(CmdLinePtr cmdLine, CYLoaderView* view)
    : m_cmdLine(cmdLine), m_view(view) {
  }

  template< typename T > void f(CmdLineGetter< T > getter, ViewSetter< T > setter) {
    if (std::invoke(getter, m_cmdLine.get()).isSet()) {
      std::invoke(setter, m_view, getter(m_cmdLine.get()));
    }
  }
};

void session() {
  CmdLinePtr cmdLine(new CmdLine());

  loadSettings(L"");
  SetArg setArg(cmdLine, getView());
  setArg.f<bool>(&CmdLine::help, &CYLoaderView::setHelp);
  setArg.f<std::wstring >(&CmdLine::symbolsFileName, &CYLoaderView::setSymbolsFileName);
  setArg.f< std::wstring >(&CmdLine::dataPath, &CYLoaderView::setSaveDir);
  setArg.f< std::wstring >(&CmdLine::startDate, &CYLoaderView::setStartDate);
  setArg.f<std::wstring>(&CmdLine::endDate, &CYLoaderView::setEndDate);
  setArg.f<std::wstring>(&CmdLine::dateSeparator, &CYLoaderView::setDateSeparator);
  setArg.f<bool>(&CmdLine::adjust, &CYLoaderView::setAdjust);
  setArg.f<bool>(&CmdLine::validatePrices, &CYLoaderView::setValidatePrices);
  setArg.f<bool>(&CmdLine::allAvailable, &CYLoaderView::setAllAvailable);
  setArg.f<unsigned int>(&CmdLine::period, &CYLoaderView::setPeriod);
  setArg.f<unsigned int>(&CmdLine::connections, &CYLoaderView::setConnections);
  setArg.f<bool>(&CmdLine::update, &CYLoaderView::setUpdate);
  setArg.f<unsigned int>(&CmdLine::dateFormat, &CYLoaderView::setDateFormat);
  setArg.f<bool>(&CmdLine::sortBarsAscending, &CYLoaderView::setSortBarsAscending);
  setArg.f<bool>(&CmdLine::padDateFields, &CYLoaderView::setPadDateFields);
  setArg.f<std::wstring>(&CmdLine::dataFileHeader, &CYLoaderView::setDataFileHeader);
  setArg.f<unsigned int>(&CmdLine::handlingInvalidData, &CYLoaderView::setInvalidDataHandling);
  setArg.f<bool>(&CmdLine::handlingVolume0, &CYLoaderView::setVolume0Handling);
  setArg.f<std::wstring>(&CmdLine::prependToDataFileName, &CYLoaderView::setPrependToDataFileName);
  setArg.f<std::wstring>(&CmdLine::appendToDataFileName, &CYLoaderView::setAppendToDataFileName);
  setArg.f<std::wstring>(&CmdLine::extension, &CYLoaderView::setExtension);
  setArg.f<std::wstring>(&CmdLine::dataSourcePlugin, &CYLoaderView::setDataSourcePlugin);
  setArg.f<std::wstring>(&CmdLine::outputFile, &CYLoaderView::setOutputFile);
  setArg.f<bool>(&CmdLine::noReloadOldData, &CYLoaderView::setNoReloadOldData);
  setArg.f<bool>(&CmdLine::createSubdirs, &CYLoaderView::setCreateSubdirs);
  setArg.f<std::wstring>(&CmdLine::fieldSeparator, &CYLoaderView::setFieldSeparator);
  setArg.f<double>(&CmdLine::volumeMultiplier, &CYLoaderView::setVolumeMultiplier);
  setArg.f<std::wstring>(&CmdLine::logFile, &CYLoaderView::setLogFile);
  setArg.f<bool>(&CmdLine::appendToLog, &CYLoaderView::setAppendToLog);
  setArg.f<bool>(&CmdLine::logOnlyErrors, &CYLoaderView::setLogOnlyErrors);
  setArg.f<std::wstring>(&CmdLine::regexMatch, &CYLoaderView::setRegexMatch);
  setArg.f<std::wstring>(&CmdLine::regexFormat, &CYLoaderView::setRegexFormat);
  setArg.f<unsigned long>(&CmdLine::regexFlags, &CYLoaderView::setRegexFlags);
  setArg.f<bool>(&CmdLine::enableRegexFormatting, &CYLoaderView::setEnableRegexFormatting);
  setArg.f<std::wstring>(&CmdLine::errorSymbolsListFileName, &CYLoaderView::setErrorSymbolsListFileName);
  setArg.f<bool>(&CmdLine::appendToErrorSymbolsList, &CYLoaderView::setAppendToErrorSymbolsList);
  setArg.f<bool>(&CmdLine::ignoreErrorSymbolsList, &CYLoaderView::setIgnoreErrorSymbolsList);
  setArg.f<bool>(&CmdLine::autoStart, &CYLoaderView::setAutoStartDownloading);
  setArg.f<bool>(&CmdLine::autoExit, &CYLoaderView::setAutoExit);
  setArg.f<std::wstring>(&CmdLine::ignoreSymbolsList, &CYLoaderView::setIgnoreSymbolsList);
  setArg.f<std::vector< std::string >>(&CmdLine::mapCharacters, &CYLoaderView::setCharacterMapping);
  setArg.f<int>(&CmdLine::fixedDecimalsCount, &CYLoaderView::setFixedDecimalsCount);
  setArg.f<std::wstring>(&CmdLine::decimalSeparator, &CYLoaderView::setDecimalSeparator);

  setUpdater(cmdLine->alternateUpdateHostName(), cmdLine->alternateUpdateDirectory());
  if (cmdLine->addSymbol().isSet()) {
    setAddSymbolToColumn(cmdLine->addSymbol().getValue(), cmdLine->symbolColumn());
  }
}

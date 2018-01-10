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
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "yloader.h"
#include "MainFrm.h"

#include "yloaderDoc.h"
#include "YloaderView.h"
#include <CmdLine.h>
#include <versionno.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CyloaderApp

BEGIN_MESSAGE_MAP(CyloaderApp, CWinAppEx)
ON_COMMAND(ID_APP_ABOUT, &CyloaderApp::OnAppAbout)
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

// CyloaderApp construction

CyloaderApp::CyloaderApp() {
  m_bHiColorIcons = TRUE;

  // support Restart Manager
  m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
  // If the application is built using Common Language Runtime support (/clr):
  //     1) This additional setting is needed for Restart Manager support to
  //     work properly. 2) In your project, you must add a reference to
  //     System.Windows.Forms in order to build.
  System::Windows::Forms::Application::SetUnhandledExceptionMode(
      System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

  // TODO: replace application ID string below with unique ID string;
  // recommended format for string is
  // CompanyName.ProductName.SubProduct.VersionInformation
  SetAppID(_T("yloader.AppID.NoVersion"));

  // TODO: add construction code here,
  // Place all significant initialization in InitInstance
}

// The one and only CyloaderApp object

CyloaderApp theApp;

// CyloaderApp initialization

CMainFrame& getMainFrame() { return *(CMainFrame*)AfxGetMainWnd(); }

BOOL CyloaderApp::InitInstance() {
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
  SetRegistryKey(_T("YLoader"));
  LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

  InitContextMenuManager();

  InitKeyboardManager();

  InitTooltipManager();
  CMFCToolTipInfo ttParams;
  ttParams.m_bVislManagerTheme = TRUE;
  theApp.GetTooltipManager()->SetTooltipParams(
      AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

  // Register the application's document templates.  Document templates
  //  serve as the connection between documents, frame windows and views
  CSingleDocTemplate* pDocTemplate;
  pDocTemplate = new CSingleDocTemplate(
      IDR_MAIN_TOOLBAR, RUNTIME_CLASS(CyloaderDoc),
      RUNTIME_CLASS(CMainFrame),  // main SDI frame window
      RUNTIME_CLASS(CYloaderView));
  if (!pDocTemplate) return FALSE;
  AddDocTemplate(pDocTemplate);

  // Parse command line for standard shell commands, DDE, file open
  /*	CCommandLineInfo cmdInfo;
          ParseCommandLine(cmdInfo);



          // Dispatch commands specified on the command line.  Will return FALSE
     if
          // app was launched with /RegServer, /Register, /Unregserver or
     /Unregister. if (!ProcessShellCommand(cmdInfo)) return FALSE;
          */
  OnFileNew();
  // The one and only window has been initialized, so show and update it
  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();

  if (cl()) {
    getMainFrame().init(m_updater);
    (AfxGetMainWnd())->SetWindowText(getCaption(STRPRODUCT_NAME).c_str());

    return TRUE;
  } else {
    return FALSE;
  }
}

void CyloaderApp::setUpdater(const StringSettable& alternateUpdateHostName,
                             const StringSettable& alternateUpdateDirectory) {
  m_updater = boost::make_shared<Updater>(getMainFrame(), _T("en_us"),
                                          alternateUpdateHostName,
                                          alternateUpdateDirectory);
}

// CyloaderApp message handlers

// CAboutDlg dialog used for App About

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
    : CDialogEx(IDD_ABOUTBOX), m_name(_T("")), m_buildDate(_T("")) {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_STATIC_NAME, m_name);
  DDX_Text(pDX, IDC_STATIC_BUILD_DATE, m_buildDate);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CyloaderApp::OnAppAbout() {
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();
}

// CyloaderApp customization load/save methods

void CyloaderApp::PreLoadState() {
  BOOL bNameValid;
  CString strName;
  bNameValid = strName.LoadString(IDS_EDIT_MENU);
  ASSERT(bNameValid);
  GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CyloaderApp::LoadCustomState() {}

void CyloaderApp::SaveCustomState() {}

CYloaderView* getView() {
  return (CYloaderView*)getMainFrame().GetActiveView();
}

bool _setUpdater(const StringSettable& alternateUpdateHostName,
                 const StringSettable& alternateUpdateDirectory) {
  theApp.setUpdater(alternateUpdateHostName, alternateUpdateDirectory);
  return true;
}

bool _close() {
  AfxGetMainWnd()->PostMessageW(WM_CLOSE);
  return true;
}

void _setSymbolsFileName(const TCHAR* symbolsFileName) {
  getView()->setSymbolsFileName(std::wstring(symbolsFileName));
}

void _setDataPath(const TCHAR* dataPath) {
  getView()->setSaveDir(std::wstring(dataPath));
}

void _setStartDate(const TCHAR* startDate) {
  getView()->setStartDate(std::wstring(startDate));
}

void _setEndDate(const TCHAR* endDate) {
  getView()->setEndDate(std::wstring(endDate));
}

void _setDateSeparator(const TCHAR* dateSeparator) {
  getView()->setDateSeparator(std::wstring(dateSeparator));
}

void _setAddSymbolToColumn(bool add, unsigned int colum) {
  getView()->setAddSymbolToColumn(add, colum);
}

void _setSortBarsAscending(bool sortBarsAscending) {
  getView()->setSortBarsAscending(sortBarsAscending);
}

void _setPadDateFields(bool padDateFields) {
  getView()->setPadDateFields(padDateFields);
}

void _setDataFileHeader(const TCHAR* dataFileHeader) {
  getView()->setDataFileHeader(std::wstring(dataFileHeader));
}

void _setInvalidDataHandling(unsigned int invalidDataHandling) {
  getView()->setInvalidDataHandling(invalidDataHandling);
}

void _setVolume0Handling(bool volume0Handling) {
  getView()->setVolume0Handling(volume0Handling);
}

void _setPrependToDataFileName(const TCHAR* prepend) {
  getView()->setPrependToDataFileName(std::wstring(prepend));
}

void _setAppendToDataFileName(const TCHAR* append) {
  getView()->setAppendToDataFileName(std::wstring(append));
}

void _setExtension(const TCHAR* extension) {
  getView()->setExtension(std::wstring(extension));
}

void _setConnections(unsigned int connections) {
  getView()->setConnections(connections);
}

void _setOutputFile(const TCHAR* outputFile) {
  getView()->setOutputFile(outputFile);
}

void _setDateFormat(unsigned int dateFormat) {
  getView()->setDateFormat(dateFormat);
}

void _setAdjust(bool adjust) { getView()->setAdjust(adjust); }

void _setValidatePrices(bool validate) {
  getView()->setValidatePrices(validate);
}

void _setPeriod(unsigned int period) { getView()->setPeriod(period); }

void _setAllAvailable(bool allAvailable) {
  getView()->setAllAvailable(allAvailable);
}

void _setUpdate(bool update) { getView()->setUpdate(update); }

void _setDataSourcePlugin(const TCHAR* dataSource) {
  getView()->setDataSourcePlugin(std::wstring(dataSource));
}

void _setNoReloadOldData(bool noReloadOldData) {
  getView()->setNoReloadOldData(noReloadOldData);
}

void _setCreateSubdirs(bool createSubdirs) {
  getView()->setCreateSubdirs(createSubdirs);
}

void _setFieldSeparator(const TCHAR* fieldSeparator) {
  getView()->setFieldSeparator(std::wstring(fieldSeparator));
}

void _setVolumeMultiplier(double volumeMultiplier) {
  getView()->setVolumeMultiplier(volumeMultiplier);
}

void _setLogFile(const TCHAR* logFile) {
  getView()->setLogFile(std::wstring(logFile));
}

void _setAppendToLog(bool appendToLog) {
  getView()->setAppendToLog(appendToLog);
}

void _setLogOnlyErrors(bool logOnlyErrors) {
  getView()->setLogOnlyErrors(logOnlyErrors);
}

void _setProgress(unsigned int progress) {}

void _setRegexMatch(const TCHAR* regexMatch) {
  getView()->setRegexMatch(std::wstring(regexMatch));
}

void _setRegexFormat(const TCHAR* regexFormat) {
  getView()->setRegexFormat(std::wstring(regexFormat));
}

void _setRegexFlags(unsigned long flags) { getView()->setRegexFlags(flags); }

void _setEnableRegexFormatting(bool enable) {
  getView()->setEnableRegexFormatting(enable);
}

void _setErrorSymbolsListFileName(const TCHAR* errorSymbolsListFileName) {
  getView()->setErrorSymbolsListFileName(
      std::wstring(errorSymbolsListFileName));
}

void _setAppendToErrorSymbolsList(bool append) {
  getView()->setAppendToErrorSymbolsList(append);
}

void _setIgnoreErrorSymbolsList(bool ignore) {
  getView()->setIgnoreErrorSymbolsList(ignore);
}

void _setAutoStart(bool autoStart) {
  getView()->setAutoStartDownloading(autoStart);
}

void _setAutoExit(bool autoExit) { getView()->setAutoExit(autoExit); }

void _setIgnoreSymbolsList(const TCHAR* symbolsList) {
  getView()->setIgnoreSymbolsList(std::wstring(symbolsList));
}

bool _loadSettings(const TCHAR* settingsName) {
  getView()->LoadParams(std::wstring(), std::wstring());
  return true;
}

void _showInfo(const TCHAR* text, const TCHAR* title) {
  ::MessageBox(0, text, title, MB_OK | MB_ICONINFORMATION);
}

void _setMapCharacters(const std::vector<std::string>& mapCharacters) {
  getView()->setCharacterMapping(mapCharacters);
}

bool __stdcall cl() {
  try {
    return ysession();
  } catch (const CmdLineException& e) {
    // if command line error, ignore cmd line params
    AfxMessageBox(yloader::s2ws(e.what()).c_str(), MB_OK | MB_ICONEXCLAMATION);
    return false;
  } catch (const std::exception& e) {
    AfxMessageBox(yloader::s2ws(e.what()).c_str(), MB_OK | MB_ICONEXCLAMATION);
    return false;
  } catch (...) {
    AfxMessageBox(_T( "Unknown error" ), MB_OK | MB_ICONEXCLAMATION);
    return false;
  }
}

BOOL CAboutDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();

  m_name = getCaption(STRPRODUCT_NAME).c_str();
  m_buildDate =
      (std::wstring(_T( "Built: " )) +
       VersionInfo::getCurrentBuildInfo()->to_simple_string() + _T( " UTC" ))
          .c_str();

  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

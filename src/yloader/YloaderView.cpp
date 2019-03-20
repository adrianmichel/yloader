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
// SHARED_HANDLERS can be defined in an ATL project implementing preview,
// thumbnail and search filter handlers and allows sharing of document code with
// that project.
#ifndef SHARED_HANDLERS
#include "yloader.h"
#endif

#include "YloaderView.h"
#include "yloaderDoc.h"

#include <defaults.h>
#include "EmptyDataPathPromptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CYloaderView

IMPLEMENT_DYNCREATE(CYloaderView, CFormView)

BEGIN_MESSAGE_MAP(CYloaderView, CFormView)
ON_WM_CONTEXTMENU()
ON_WM_RBUTTONUP()
ON_WM_DESTROY()
ON_WM_CLOSE()
ON_WM_CREATE()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_CHECK_PERIOD_ALL, &CYloaderView::OnClickedCheckPeriodAll)
ON_COMMAND(ID_OPEN_SYMBOLS, &CYloaderView::OnOpenSymbols)
ON_UPDATE_COMMAND_UI(ID_OPEN_SYMBOLS, &CYloaderView::OnUpdateOpenSymbols)
ON_BN_CLICKED(IDC_SYMBOLS_LIST, &CYloaderView::OnClickedSymbolsList)
ON_BN_CLICKED(IDC_BUTTON_SAVE_DIR, &CYloaderView::OnBnClickedButtonSaveDir)
ON_BN_CLICKED(IDC_BUTTON_SAVE_FILE_ALL,
              &CYloaderView::OnBnClickedButtonSaveFileAll)
ON_BN_CLICKED(IDC_EDIT_SYMBOLS_LIST, &CYloaderView::OnBnClickedEditSymbolsList)
ON_BN_CLICKED(IDC_EXPLORE_OUTPUT_FOLDER,
              &CYloaderView::OnBnClickedExploreOutputFolder)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_EVENTS, &CYloaderView::OnNMDblclkListEvents)
ON_MESSAGE(WM_START_SESSION, DoDownload)
ON_WM_SIZE()
ON_EN_CHANGE(IDC_EDIT_SYMBOLS_LIST_LOCATION,
             &CYloaderView::OnEnChangeEditSymbolsListLocation)
ON_EN_CHANGE(IDC_EDIT_SAVE_DIR, &CYloaderView::OnEnChangeEditSaveDir)
ON_EN_CHANGE(IDC_EDIT_OUTPUT_FILE_NAME,
             &CYloaderView::OnEnChangeEditOutputFileName)
ON_BN_CLICKED(IDC_CHECK_UPDATE, &CYloaderView::OnBnClickedCheckUpdate)
ON_BN_CLICKED(IDC_CHECK_DIV_SPLIT_ADJUSTED,
              &CYloaderView::OnBnClickedCheckDivSplitAdjusted)
ON_BN_CLICKED(IDC_CHECK_VALIDATE_PRICES,
              &CYloaderView::OnBnClickedCheckValidatePrices)
ON_CBN_SELCHANGE(IDC_COMBO_PERIOD, &CYloaderView::OnCbnSelchangeComboPeriod)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_PERIOD_START,
          &CYloaderView::OnDtnDatetimechangeDatetimepickerPeriodStart)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_PERIOD_END,
          &CYloaderView::OnDtnDatetimechangeDatetimepickerPeriodEnd)
END_MESSAGE_MAP()

afx_msg LRESULT CYloaderView::DoDownload(WPARAM, LPARAM) {
  OnDownload();
  return 0;
}

// CYloaderView construction/destruction

CYloaderView::CYloaderView()
    : CFormView(IDD_DIALOG_YAHOO_HIST_DATA),
      m_closing(false),
      _dirty(false),
      frame(NULL),
      m_editSymbolsList(IDI_ICON_EDIT_SYMBOLS_LIST),
      m_exploreOutputFolder(IDI_ICON_EXPLORE_DOWNLOAD_PATH),
      m_openOneOutputFile(IDI_ICON_OPEN_OUTPUT_FILE) {
  setDefaults();
}

void CYloaderView::SaveParams(bool force, bool updateData) {
  // this is not a regular setting, it's just to mark the subscription exp date
  // for the next version installer
  /*	std::wstring expDate(subscriptionExpirationDate().is_special() ?
  std::wstring() : subscriptionExpirationDate().to_iso_string());
  WriteProfileString(_T("UpdateXTime"), (boost::format("%1%") % expDate).str());
  */
  if (force || _dirty || dateRangeChanged()) {
    if (updateData) UpdateData();

    WriteProfileString(_T("SymbolsListFile"), getSymbolsListFileName());
    WriteProfileString(_T("SaveDir"), getSaveDir());
    WriteProfileString(_T("StartDate"), getStartDateAsString());
    WriteProfileInt(_T("All"), getAll());
    WriteProfileInt(_T("Period"), getPeriod());
    WriteProfileInt(_T("SplitDivAdjusted"), getAdjustment());
    WriteProfileInt(_T("AutoStartDownloading"), m_autoStartDownloading);
    WriteProfileInt(_T("Update"), getUpdate());
    WriteProfileInt(_T("ValidateData"), getValidatePrices());
    WriteProfileInt(_T("MostRecentBarsLast"), m_mostRecentBarsLastBool);
    WriteProfileInt(_T("DateFormat"), m_dateFormat);
    WriteProfileInt(_T("AddSymbolToColumn"), _addSymbolToColumn);
    WriteProfileInt(_T("SymbolColumnNumber"), _columnNumber);
    WriteProfileString(_T("PrependToFileName"), _prependToFileName);
    WriteProfileString(_T("AppendToFileName"), _appendToFileName);
    WriteProfileInt(_T("CheckForUpdatesAtStartup2"), _checkForUpdatesAtStartup);
    WriteProfileString(_T("FileNameExtension"), _fileNameExtension);
    WriteProfileInt(_T("Threads"), _threads);
    WriteProfileString(_T("LogFile"), _logFile);
    WriteProfileInt(_T("AppendToLog"), _appendToLog);
    WriteProfileInt(_T("PadDate"), _padDate);
    WriteProfileInt(_T("SymbolTimeout"), _symbolTimeout);
    WriteProfileInt(_T("LogOnlyErrors"), _logOnlyErrors);
    WriteProfileInt(_T("CreateSubdirs"), _createSubdirs);
    WriteProfileInt(_T("Volume0Invalid"), _volume0invalid);
#ifdef ENABLE_TRAY
    AfxGetApp()->WriteProfileInt(_T("MinimizeToTray"), _minimizeToTray);
#endif
    WriteProfileInt(_T("InvalidDataHandling"), _invalidDataHandling);
    WriteProfileInt(_T("HideNews"), _hideNews);
    WriteProfileInt(_T("AutoSelectOutputPath"), _autoSelectOutputPath);
    WriteProfileString(_T("AutoSelectOutputPathName"),
                       _autoSelectOutputPathName);
    WriteProfileInt(_T("DiagnosticLogging"), _diagnosticLogging);
    WriteProfileString(_T("ChMapFrom"), _characterMapping.getFromString());
    WriteProfileString(_T("ChMapTo"), _characterMapping.getToString());
    WriteProfileString(_T("FieldSeparator"), yloader::escape(m_fieldSeparator));
    WriteProfileString(_T("FileHeader"), yloader::escape(_fileHeader));
    WriteProfileString(_T("DateSeparator"), yloader::escape(_dateSeparator));
    WriteProfileString(_T("VolumeMultiplier"),
                       (std::wstring() << _volumeMultiplier));
    if (_selectedDataSourcePlugin)
      WriteProfileString(_T("SelectedDataSourcePlugin"),
                         *_selectedDataSourcePlugin);

    WriteProfileString(_T("ProxyServerAddress"), _proxyServerAddress);
    WriteProfileString(_T("ProxyServerUserName"), _proxyServerUserName);
    WriteProfileString(_T("ProxyServerPassword"), _proxyServerPassword);
    WriteProfileInt(_T("HTTPRequestTimeout"), _httpRequestTimeout);
    WriteProfileString(_T("OutputFile"), getSaveFileAll());
    WriteProfileString(_T("OpenDataFileWithithApp"),
                       yloader::escape(_openDataFileWithApp));
    WriteProfileInt(_T("DontReloadOldDataInUpdateMode"),
                    _dontReloadOldDataInUpdateMode);

    WriteProfileString(_T("MatchRegex"), yloader::escape(_matchRegex.str()));
    WriteProfileString(_T("FormatString"), yloader::escape(_formatString));
    WriteProfileInt(_T("EnableRegexFormatting"), _enableRegexFormatting);
    WriteProfileString(_T("ErrorSymbolsList"), _errorSymbolsList);
    WriteProfileInt(_T("AppendToErrorSymbolsList"), _appendToErrorSymbolsList);
    WriteProfileString(_T("IgnoreSymbolsList"), _ignoreSymbolsList);
    WriteProfileInt(_T("IgnoreErrorSymbolsList"), _ignoreErrorSymbolsList);
    WriteProfileString(_T("NotifyOnlyIfNewerVersion"),
                       _notifyOnlyIfNewerVersion);
    WriteProfileString(_T("DecimalSeparator"), _decimalSeparator);
    WriteProfileInt(_T("FixedDecimalsCount"), _fixedDecimalsCount);
  }
}

void CYloaderView::LoadParams(const std::wstring& defSymbolsFileName,
                              const std::wstring& dataPath) {
  try {
    LOG(log_info, _T("Loading settings"));
    CString date(GetProfileString(_T("StartDate")).c_str());
    setStartDate(date);

    // if _symbolsFileName is already set (by the command line), then it
    // overrides the saved data
    setSymbolsFileName(GetProfileString(_T("SymbolsListFile")));
    // if _saveDir is already set (by the command line), then it overrides the
    // saved data
    setSaveDir(GetProfileString(_T("SaveDir")));
    setAllAvailable(GetProfileBool(_T("All"), DEFAULT_ALL_AVAILABLE));
    setPeriod((Period)GetProfileInt(_T("Period"), DEFAULT_PERIOD));
    setAdjustment(GetProfileBool(_T("SplitDivAdjusted"), DEFAULT_ADJUST));
    // if m_autoStartDownloading is already set (by the command line), then it
    // overrides the saved data
    m_autoStartDownloading =
        GetProfileBool(_T("AutoStartDownloading"), DEFAULT_AUTO_START) ||
        m_autoStartDownloading;
    setValidatePrices(
        GetProfileBool(_T("ValidateData"), DEFAULT_VALIDATE_PRICES));
    setUpdate(GetProfileBool(_T("Update"), DEFAULT_UPDATE));
    m_mostRecentBarsLastBool =
        GetProfileBool(_T("MostRecentBarsLast"), DEFAULT_SORT_BARS_ASCENDING);
    m_dateFormat = GetProfileInt(_T("DateFormat"), DEFAULT_DATE_FORMAT);
    _columnNumber =
        GetProfileInt(_T("SymbolColumnNumber"), DEFAULT_COLUMN_NUMBER);
    _addSymbolToColumn =
        GetProfileBool(_T("AddSymbolToColumn"), DEFAULT_ADD_SYMBOL);
    _prependToFileName = GetProfileString(_T("PrependToFileName"));
    _appendToFileName = GetProfileString(_T("AppendToFileName"));
    _checkForUpdatesAtStartup = GetProfileBool(
        _T("CheckForUpdatesAtStartup2"), DEFAULT_CHECK_FOR_UPDATES_AT_STARTUP);
    _fileNameExtension =
        GetProfileString(_T("FileNameExtension"), DEFAULT_EXTENSION);
    _threads = GetProfileInt(_T("Threads"), DEFAULT_THREADS);
    _logFile = GetProfileString(_T("LogFile"));
    _appendToLog = GetProfileBool(_T("AppendToLog"), DEFAULT_APPEND_TO_LOG);
    _padDate = GetProfileBool(_T("PadDate"), DEFAULT_PAD_DATE_FIELDS);
    _symbolTimeout = GetProfileInt(_T("SymbolTimeout"), DEFAULT_SYMBOL_TIMEOUT);
    _logOnlyErrors =
        GetProfileBool(_T("LogOnlyErrors"), DEFAULT_LOG_ONLY_ERRORS);
    _createSubdirs =
        GetProfileBool(_T("CreateSubdirs"), DEFAULT_CREATE_SUBDIRS);
    _volume0invalid =
        GetProfileBool(_T("Volume0Invalid"), DEFAULT_VOLUME0_INVALID);
#ifdef ENABLE_TRAY
    _minimizeToTray = GetProfileBool(_T("MinimizeToTray"));
#endif
    // 0 for warning, 1 for error
    _invalidDataHandling =
        GetProfileInt(_T("InvalidDataHandling"), DEFAULT_HANDLING_INVALID_DATA);
    _hideNews = GetProfileBool(_T("HideNews"));
    _autoSelectOutputPath = GetProfileBool(_T("AutoSelectOutputPath"));
    _autoSelectOutputPathName =
        GetProfileString(_T("AutoSelectOutputPathName"), _T("data"));
    _diagnosticLogging = GetProfileBool(_T("DiagnosticLogging"));
    std::wstring cmfrom = GetProfileString(_T("ChMapFrom"), _T(":"));
    std::wstring cmto = GetProfileString(_T("ChMapTo"), _T("_"));
    m_fieldSeparator = yloader::unescape(
        GetProfileString(_T("FieldSeparator"), DEFAULT_FIELD_SEPARATOR));
    _fileHeader = yloader::unescape(
        GetProfileString(_T("FileHeader"), DEFAULT_FILE_HEADER));
    _dateSeparator = yloader::unescape(
        GetProfileString(_T("Dateseparator"), DEFAULT_DATE_SEPARATOR));
    _volumeMultiplier =
        _tstof(GetProfileString(_T("VolumeMultiplier"),
                                std::wstring() << DEFAULT_VOLUME_MULTIPLIER)
                   .c_str());
    std::wstring sds = GetProfileString(_T("SelectedDataSourcePlugin"));
    _selectedDataSourcePlugin =
        sds.empty() ? yloader::UniqueIdPtr()
                    : yloader::UniqueIdPtr(new yloader::UniqueId(sds.c_str()));
    _proxyServerAddress = GetProfileString(_T("ProxyServerAddress"));
    _proxyServerUserName = GetProfileString(_T("ProxyServerUserName"));
    _proxyServerPassword = GetProfileString(_T("ProxyServerPassword"));
    _httpRequestTimeout = GetProfileInt(_T("HTTPRequestTimeout"), 60);
    setSaveFileAll(GetProfileString(_T("OutputFile")));
    _openDataFileWithApp =
        yloader::unescape(GetProfileString(_T("OpenDataFileWithithApp")));
    _dontReloadOldDataInUpdateMode =
        GetProfileBool(_T("DontReloadOldDataInUpdateMode"),
                       DEFAULT_DONT_RELOAD_DATA_IN_UPDATE_MODE);
    _enableRegexFormatting = GetProfileBool(_T("EnableRegexFormatting"),
                                            DEFAULT_ENABLE_REGEX_FORMATTING);
    _matchRegex = yloader::unescape(
        GetProfileString(_T("MatchRegex"), DEFAULT_MATCH_REGEX));
    _formatString = yloader::unescape(
        GetProfileString(_T("FormatString"), DEFAULT_FORMAT_STRING));
    _errorSymbolsList =
        GetProfileString(_T("ErrorSymbolsList"), DEFAULT_ERROR_SYMBOLS_LIST);
    _appendToErrorSymbolsList = GetProfileInt(
        _T("AppendToErrorSymbolsList"), DEFAULT_APPEND_TO_ERROR_SYMBOLS_LIST);
    _ignoreErrorSymbolsList = GetProfileInt(_T("IgnoreErrorSymbolsList"),
                                            DEFAULT_IGNORE_ERROR_SYMBOLS_LIST);
    _ignoreSymbolsList =
        GetProfileString(_T("IgnoreSymbolsList"), DEFAULT_IGNORE_SYMBOLS_LIST);
    //		_updateXTime = GetProfileString(_T("UpdateXTime"), _T( "" ));
    _notifyOnlyIfNewerVersion =
        GetProfileString(_T("NotifyOnlyIfNewerVersion"));

    _characterMapping.set(cmfrom, cmto);

    _decimalSeparator =
        GetProfileString(_T("DecimalSeparator"), DEFAULT_DECIMAL_SEPARATOR);
    _fixedDecimalsCount =
        GetProfileInt(_T("FixedDecimalsCount"), DEFAULT_FIXED_DECIMALS_COUNT);
    /*
if (!enableExtraFeatures())
{
// these features are enabled in the registered version only
m_autoStartDownloading = false;
_threads = 1;
_logFile = _T("");
}
*/

    if (!yloader::fileExists(getYLoaderSettingsFile())) SaveParams(true, false);
  }

  catch (...) {
    // handle date/time string errors
  }
}

void CYloaderView::setDefaults() {
  m_symbolsFileName = _T("");
  m_saveDir = _T("");
  m_all = DEFAULT_ALL_AVAILABLE;
  m_period = DEFAULT_PERIOD;
  m_Adjustment = DEFAULT_ADJUST;
  m_validatePrices = FALSE;
  m_update = DEFAULT_UPDATE;
  m_startDate.SetStatus(COleDateTime::DateTimeStatus::null);
  m_endDate = COleDateTime::GetCurrentTime();
  m_saveFileAll = _T("");
  m_autoStartDownloading = DEFAULT_AUTO_START;
  m_mostRecentBarsLastBool = true;
  m_dateFormat = 0;
  _columnNumber = 0;
  _addSymbolToColumn = false;
  _prependToFileName = _T("");
  _appendToFileName = _T("");
  _checkForUpdatesAtStartup = true;
  _fileNameExtension = DEFAULT_EXTENSION;
  _threads = 5;
  _logFile = _T("");
  _appendToLog = false;
  _padDate = false;
  _symbolTimeout = 0;
  _logOnlyErrors = true;
  _createSubdirs = false;
  _volume0invalid = false;
  _invalidDataHandling = 0;
  _hideNews = false;
  _autoSelectOutputPath = false;
  _autoSelectOutputPathName = _T("data");
  _diagnosticLogging = false;
  _autoExit = false;
  m_fieldSeparator = DEFAULT_FIELD_SEPARATOR;
  _characterMapping.set(_T(":"), _T("_"));
  _dialog = true;
  _defSymbolsFileName.clear();
  _defDataPath.clear();
  _logFileError = false;
  _proxyServerAddress = _T("");
  _proxyServerUserName = _T("");
  _proxyServerPassword = _T("");
  _httpRequestTimeout = 60;
  _openDataFileWithApp = _T("");
  _startMode = START_NORMAL;
  _enableRegexFormatting = DEFAULT_ENABLE_REGEX_FORMATTING;
  _matchRegexUnlocked = DEFAULT_MATCH_REGEX_UNLOCKED;
  _matchRegex = DEFAULT_MATCH_REGEX;
  _formatString = DEFAULT_FORMAT_STRING;
  _errorSymbolsList = DEFAULT_ERROR_SYMBOLS_LIST;
}
CYloaderView::~CYloaderView() {}

void CYloaderView::DoDataExchange(CDataExchange* pDX) {
  CFormView::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_SYMBOLS, m_symbols);
  DDX_Control(pDX, IDC_SYMBOLS_LIST, m_symbolsFileButton);
  DDX_Control(pDX, IDC_EDIT_SYMBOLS_LIST, m_editSymbolsList);
  DDX_Control(pDX, IDC_EDIT_SYMBOLS_LIST_LOCATION, m_symbolsListFileEdit);
  DDX_Control(pDX, IDC_BUTTON_SAVE_DIR, m_dataPathButton);
  DDX_Control(pDX, IDC_EXPLORE_OUTPUT_FOLDER, m_exploreOutputFolder);
  DDX_Control(pDX, IDC_EDIT_SAVE_DIR, m_dataPathEdit);
  DDX_Control(pDX, IDC_BUTTON_SAVE_FILE_ALL, m_dataFileButton);
  DDX_Control(pDX, IDC_EDIT_OUTPUT_FILE, m_openOneOutputFile);
  DDX_Control(pDX, IDC_EDIT_OUTPUT_FILE_NAME, m_saveFileAllEdit);
  DDX_Control(pDX, IDC_STATIC_SESSION_STATUS_VALUE, m_sessionStatusCtrl);
  DDX_Text(pDX, IDC_EDIT_SYMBOLS_LIST_LOCATION, m_symbolsFileName);
  DDX_Text(pDX, IDC_EDIT_SAVE_DIR, m_saveDir);
  DDX_Text(pDX, IDC_EDIT_OUTPUT_FILE_NAME, m_saveFileAll);
  DDX_Check(pDX, IDC_CHECK_PERIOD_ALL, m_all);
  DDX_Control(pDX, IDC_CHECK_UPDATE, m_updateCheck);
  DDX_CBIndex(pDX, IDC_COMBO_PERIOD, (int&)m_period);
  DDX_Control(pDX, IDC_DATETIMEPICKER_PERIOD_START, m_startDateCtrl);
  DDX_Control(pDX, IDC_DATA_SOURCE_VALUE, m_dataSourceCtrl);
  DDX_Text(pDX, IDC_STATIC_TOTAL_SYMBOLS, m_totalSymbolsValue);
  DDX_Text(pDX, IDC_STATIC_TOTAL_PROCESSED, m_totalProcessedValue);
  DDX_Text(pDX, IDC_STATIC_TOTAL_PROCESSED_PCT, m_totalProcessedPctValue);
  DDX_Text(pDX, IDC_STATIC_PROCESSED_OK, m_processedOKValue);
  DDX_Text(pDX, IDC_STATIC_PROCESSED_OK_PCT, m_processedOkPctValue);
  DDX_Text(pDX, IDC_STATIC_PROCESSED_WITH_ERRORS, m_processedWithErrorsValue);
  DDX_Text(pDX, IDC_STATIC_PROCESSED_WITH_ERRORS_PCT, m_errorsPctValue);
  DDX_Text(pDX, IDC_STATIC_IGNORED, m_ignoredValue);
  DDX_Text(pDX, IDC_STATIC_IGNORED_PCT, m_ignoredPctValue);
  DDX_Text(pDX, IDC_STATIC_DURATION_VALUE, m_durationValue);
  DDX_Control(pDX, IDC_PROGRESS, m_progress);
  DDX_Control(pDX, IDC_LIST_EVENTS, m_events);
  DDX_Check(pDX, IDC_CHECK_DIV_SPLIT_ADJUSTED, m_Adjustment);
  DDX_Check(pDX, IDC_CHECK_VALIDATE_PRICES, m_validatePrices);
  DDX_Control(pDX, IDC_DATETIMEPICKER_PERIOD_END, m_endDateCtrl);
  DDX_Control(pDX, IDC_CHECK_DIV_SPLIT_ADJUSTED, m_adjustmentButton);
  DDX_Control(pDX, IDC_CHECK_VALIDATE_PRICES, m_validatePricesButton);
  DDX_Control(pDX, IDC_CHECK_PERIOD_ALL, m_allAvailableButton);
  DDX_Control(pDX, IDC_COMBO_PERIOD, m_periodCombo);
  DDX_Check(pDX, IDC_CHECK_UPDATE, m_update);
  DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_PERIOD_START, m_startDate);
  DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_PERIOD_END, m_endDate);
  DDX_Text(pDX, IDC_STATIC_CONNECTIONS, m_totalConnectionsValue);
}

BOOL CYloaderView::PreCreateWindow(CREATESTRUCT& cs) {
  return CFormView::PreCreateWindow(cs);
}

void CYloaderView::OnInitialUpdate() {
  CFormView::OnInitialUpdate();
  GetParentFrame()->RecalcLayout();
  ResizeParentToFit(FALSE);

  // Create the colored <SoftechSoftware> sign
  //	m_sessionStatusCtrl.SubclassDlgItem(IDC_STATIC_SESSION_STATUS_VALUE,
  // this);

  LoadParams(_defSymbolsFileName, _defDataPath);

  _plugins.load(yloader::getModulePath());

  m_dataPathEdit.SetUseDir();
  m_symbolsListFileEdit.setRefreshable(this);

  setStatistics(_statistics);
  m_events.init();

  setDataSourceText();

  m_editSymbolsList.init();
  m_exploreOutputFolder.init();
  m_openOneOutputFile.init();

  SetTimer(1, 200, NULL);
  EnableScrollBarCtrl(SB_BOTH, FALSE);
}

void CYloaderView::OnRButtonUp(UINT /* nFlags */, CPoint point) {
  ClientToScreen(&point);
  OnContextMenu(this, point);
}

void CYloaderView::OnContextMenu(CWnd* /* pWnd */, CPoint point) {
#ifndef SHARED_HANDLERS
  theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x,
                                                point.y, this, TRUE);
#endif
}

// CYloaderView diagnostics

#ifdef _DEBUG
void CYloaderView::AssertValid() const { CFormView::AssertValid(); }

void CYloaderView::Dump(CDumpContext& dc) const { CFormView::Dump(dc); }

CyloaderDoc* CYloaderView::GetDocument() const  // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CyloaderDoc)));
  return (CyloaderDoc*)m_pDocument;
}
#endif  //_DEBUG

// CYloaderView message handlers
BEGIN_MESSAGE_MAP(DateCtrl, CDateTimeCtrl)
ON_NOTIFY_REFLECT(DTN_DATETIMECHANGE, &DateCtrl::OnDtnDatetimechange)
END_MESSAGE_MAP()

void DateCtrl::OnDtnDatetimechange(NMHDR* pNMHDR, LRESULT* pResult) {
  LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
  m_bChanged = true;
  *pResult = 0;
}

void CYloaderView::OnDestroy() {
  SaveParams();

  CFormView::OnDestroy();
}

void CYloaderView::OnClose() {
  OutputDebugString(_T( "OnClose start\n"));
  if (_downloading && _downloading->isRunning()) {
    if (AfxMessageBox(_T("A downloading session is in progress. Are you sure ")
                      _T("you want to exit?"),
                      MB_YESNO) == IDYES) {
      if (_downloading && _downloading->isRunning()) _downloading->cancel();

      m_closing = true;
    }
  } else {
    OutputDebugString(_T( "OnClose - before EndDialog\n"));
    CFormView::OnClose();
  }
}

int CYloaderView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CFormView::OnCreate(lpCreateStruct) == -1) return -1;

  return 0;
}

void CYloaderView::OnDownload() {
  // this will set the flag to true upon exit of this function
  AutoBool ab(_hasHadSession, true);
  if (_downloading && _downloading->isRunning()) {
    _downloading->cancel();
  } else {
    try {
      UpdateData();

      YDataSourcePluginPtr plugin = getDataSourcePlugin();

      if (!plugin) {
        AfxMessageBox(
            _T("No data source plugin has been selected. Please select a data ")
            _T("source plugin and try again"),
            MB_OK | MB_ICONSTOP);
        return;
      }

      assert((bool)plugin);

      const std::wstring& pluginShortName = plugin->shortName();

      if (getSymbolsFileName().IsEmpty()) {
        AfxMessageBox(
            _T("No symbols list file has been selected. Please select a ")
            _T("symbols list file and try again"),
            MB_OK | MB_ICONSTOP);
        return;
      }

      if (!getAdjustment() && !plugin->canUnadjust()) {
        AfxMessageBox(
            (std::wstring(_T("Unadjusted data is not supported by the ")) +
             pluginShortName +
             _T(" data source plug-in\n\nChange the setting and try again"))
                .c_str(),
            MB_OK | MB_ICONSTOP);
        return;
      }

      if (getAdjustment() && !plugin->canAdjust()) {
        AfxMessageBox(
            (std::wstring(_T("Adjusted data is not supported by the ")) +
             pluginShortName +
             _T(" data source plug-in\n\nChange the setting and try again"))
                .c_str(),
            MB_OK | MB_ICONSTOP);
        return;
      }

      if (oleDateTimeToDate(getStartDate()) <
          yloader::Date(plugin->firstDate())) {
        AfxMessageBox(
            (std::wstring(_T("Data older than than ")) +
             yloader::Date(plugin->firstDate()).toString() +
             _T(" is not supported by the ") + pluginShortName +
             _T(" data source plug-in\n\nChange the setting and try again"))
                .c_str(),
            MB_OK | MB_ICONSTOP);
        return;
      }

      if (!plugin->isPeriodSupported(getPeriod())) {
        AfxMessageBox(
            (std::wstring(_T("The current period is not supported by the ")) +
             pluginShortName +
             _T(" data source plug-in\n\nChange the setting and try again"))
                .c_str(),
            MB_OK | MB_ICONSTOP);
        return;
      }

      DeleteAllEvents();

      YahooEventDelegatorPtr yed(new YahooEventDelegator());
      addSinks(yed);

      if (getSaveDir().IsEmpty() && !_autoSelectOutputPath) {
        CEmptyDataPathPromptDlg dlg;
        dlg.DoModal();
        if (dlg.autoSelectDataPath())
          _autoSelectOutputPath = true;
        else
          return;
      }

      Date endDate = oleDateTimeToDate(getEndDate());
      /*
      if( endDate.isNotADate() )
      {
      AfxMessageBox( _T( "Please enter a valid end date and try again" ), MB_OK
      | MB_ICONSTOP ); return;
      }
      */
      Date startDate = oleDateTimeToDate(getStartDate());
      /*			if( startDate.isNotADate() )
      {
      AfxMessageBox( _T( "Please enter a valid start date and try again" ),
      MB_OK | MB_ICONSTOP ); return;
      }

      assert( !endDate.isNotADate() );
      assert( !startDate.isNotADate() );
      */
      if (!endDate.isNotADate() && !startDate.isNotADate() &&
          getEndDate() < getStartDate())
        AfxMessageBox(
            _T("The start date should occur after the end date.\nPlease ")
            _T("correct and try again"),
            MB_OK | MB_ICONSTOP);
      else {
        StrListPtr symbolsList(
            yloader::getSymbols((LPCTSTR)getSymbolsFileName(), true));

        DataParamsPtr dataParams(getDataParams());

        std::wstring outputPath = dataParams->outputPath();

        CFileFind ff;
        if (!(ff.FindFile(outputPath.c_str()) && ff.FindNextFile() &&
              ff.IsDirectory()))
          CreateDirectory(outputPath.c_str(), 0);

        ManagedPtr<SymbolsListIterator> si(
            new SymbolsListIterator(symbolsList.get()));

        startStatistics(si->size());

        //				setTotalConnectionsValue(_threads);
        m_sessionStatusCtrl.SetWindowText(_T("Downloading"));

        try {
          if (!_logFile.empty()) {
            _log =
                ManagedPtr<YahooSessionLog>(new YahooSessionLog(*dataParams));

            yed->addSink(_log.get());
          } else
            _log.reset();
        } catch (const LogFileException& e) {
          if (!m_autoStartDownloading) {
            std::wostringstream o;

            o << _T("Could not open log file \"") << e.fileName()
              << _T("\". Start downloading?");
            if (AfxMessageBox(o.str().c_str(), MB_ICONEXCLAMATION | MB_YESNO) ==
                IDNO)
              return;
          } else
            _logFileError = true;
        }

        setProgressRange(si->size());

        assert(_downloading.get() == 0);

        enableControls(false);
        //		  m_downloadButton.SetWindowText( _T( "Cancel" ) );

        _errorSymbolsListFile =
            hasErrorSymbolsListFile()
                ? boost::make_shared<WriteFileSymbolsList>(_errorSymbolsList)
                : WriteFileSymbolsListPtr();

        yloader::UniqueSymbolsSetPtr ignoreSymbolsSet(
            dataParams->getIgnoreSymbols(yed));

        _downloading =
            new CDownloading(*dataParams, _errorSymbolsListFile.get(),
                             symbolsList, ignoreSymbolsSet, si, yed);

        Timer timer;
        LOG(log_info, _T("Starting a downloading session"));
        LOG(log_info, _T("Session parameters:\n") << dataParams->toString());

        plugin->setProxyServerAddress(_proxyServerAddress.c_str());
        plugin->setProxyServerUserName(_proxyServerUserName.c_str());
        plugin->setProxyServerPassword(_proxyServerPassword.c_str());
        plugin->setHTTPRequestTimeout(_httpRequestTimeout);

        m_sessionStatusCtrl.StartTextBlink(true, CColorStaticST::ST_FLS_FAST);

        _downloading->start();

        if (false) {
          std::wostringstream o;
          /*
          o << _T( "total on timer duration in Downloading: " ) <<
          downloading.onTimerDuration() << std::endl; o << _T( "total duration:
          " ) << Seconds( timer.elapsed() ).toString() << std::endl; o << _T(
          "total history duration: " ) <<
          YahooHistData::totalHistoryDuration().toString() << std::endl; o <<
          _T( "total writing duration: " ) <<
          YahooHistData::totalWritingDuration().toString() << std::endl; o <<
          _T( "total downloading duration: " ) <<
          YahooHistData::totalDownloadingDuration().toString() << std::endl;

          AfxMessageBox( o.str().c_str() );
          */
        }

        /*
        setStatistics( downloading.statistics() );
        m_sessionStatusValue = downloading.canceled() ? _T( "Canceled" ) : _T(
        "Completed" ); UpdateData( false );
        */
      }
    }
    /*		catch (const SubscriptionExpiredException& e)
    {
    AfxMessageBox(e.message().c_str());
    return;
    }
    catch (const NotRegisteredException& e)
    {
    AfxMessageBox(e.message().c_str());
    return;
    }
    */
    catch (const FileSymbolsParserException&) {
      AfxMessageBox(_T("Symbols list file error"), MB_OK | MB_ICONSTOP);
    } catch (...) {
      LOG(log_error, _T("Unknown exception"));
      AfxMessageBox(_T("Unknown exception, please contact yloader.com support"),
                    MB_OK | MB_ICONSTOP);
    }
  }
}

void CYloaderView::OnSettings() {
  YSettingsDialog dlg(
      _plugins, _selectedDataSourcePlugin.get(), _addSymbolToColumn,
      _columnNumber, _prependToFileName, _appendToFileName,
      _checkForUpdatesAtStartup, _fileNameExtension, _logFile, _threads,
      _appendToLog, _padDate, m_dateFormat, m_fieldSeparator,
      m_mostRecentBarsLastBool != 0, m_autoStartDownloading != 0,
      _symbolTimeout, _logOnlyErrors, _createSubdirs, _invalidDataHandling,
      _volume0invalid, _hideNews, _autoSelectOutputPath,
      _autoSelectOutputPathName, _diagnosticLogging, _characterMapping,
      _volumeMultiplier, _fileHeader, _dateSeparator, _proxyServerAddress,
      _proxyServerUserName, _proxyServerPassword, _httpRequestTimeout,
      _openDataFileWithApp, _dontReloadOldDataInUpdateMode, _minimizeToTray,
      showInitialMB, _enableRegexFormatting, _matchRegexUnlocked,
      _matchRegex.str(), _formatString, _errorSymbolsList,
      _appendToErrorSymbolsList, _ignoreErrorSymbolsList, _ignoreSymbolsList,
      this);

  if (dlg.DoModal() == IDOK) {
    _dirty = true;
    _addSymbolToColumn = dlg.addSymbolsToColumn();
    _columnNumber = dlg.columnNumber();
    _prependToFileName = dlg.prependToFileName();
    _appendToFileName = dlg.appendToFileName();
    _checkForUpdatesAtStartup = dlg.checkForUpdatesAtStartup();
    _fileNameExtension = dlg.fileNameExtension();
    _threads = dlg.threads();
    _logFile = dlg.logFile();
    _appendToLog = dlg.appendToLog();
    _padDate = dlg.padDate();
    m_dateFormat = dlg.dateFormat();
    m_mostRecentBarsLastBool = dlg.sortBarsInAscendingOrder();
    m_autoStartDownloading = dlg.autoStartDownloading();
    //  _symbolTimeout = dlg.symbolTimeout();
    _logOnlyErrors = dlg.logOnlyErrors();
    _createSubdirs = dlg.createSubdirs();
    _invalidDataHandling = dlg.invalidDataHandling();
    _volume0invalid = dlg.volume0invalid();
    _autoSelectOutputPath = dlg.autoSelectOutputPath();
    _autoSelectOutputPathName = dlg.autoSelectOutputPathName();
    _diagnosticLogging = dlg.diagnosticLogging();
    _characterMapping = dlg.characterMapping();
    m_fieldSeparator = dlg.fieldSeparator();
    _volumeMultiplier = dlg.volumeMultiplier();
    _fileHeader = dlg.fileHeader();
    _dateSeparator = dlg.dateSeparator();
    _proxyServerAddress = dlg.proxyServerAddress();
    _proxyServerUserName = dlg.proxyServerUserName();
    _proxyServerPassword = dlg.proxyServerPassword();
    _httpRequestTimeout = dlg.httpRequestTimeout();
#ifdef ENABLE_TRAY
    _minimizeToTray = dlg.minimizeToTray();
#endif
    _selectedDataSourcePlugin =
        dlg.getSelectedDataSourcePlugin() != 0
            ? yloader::UniqueIdPtr(
                  new yloader::UniqueId(*dlg.getSelectedDataSourcePlugin()))
            : yloader::UniqueIdPtr();
    _openDataFileWithApp = dlg.openDataFileWithApp();
    _dontReloadOldDataInUpdateMode = dlg.dontReloadOldDataInUpdateMode();
    _enableRegexFormatting = dlg.enableRegexFormatting();
    _matchRegexUnlocked = dlg.matchRegexUnlocked();
    _matchRegex = dlg.matchRegex();
    _formatString = dlg.formatString();
    _errorSymbolsList = dlg.errorSymbolsFile();
    _appendToErrorSymbolsList = dlg.appendToErrorSymbolsFile();
    _ignoreSymbolsList = dlg.ignoreSymbolsList();
    _ignoreErrorSymbolsList = dlg.ignoreErrorSymbolsList();

    enableUpdate();

    Log::setLevel(_diagnosticLogging ? log_debug : log_none);

    setDataSourceText();
  }
}

const unsigned long WM_AUTO_START = WM_USER + 3454;

void CYloaderView::OnTimer(UINT_PTR nIDEvent) {
  if (_showSettings) {
    _showSettings = false;
    PostMessage(WM_SHOW_SETTINGS);
  }
  // TODO: Add your message handler code here and/or call default
  if (m_autoStartDownloading && _notStarted) {
    PostMessage(WM_AUTO_START);
    _notStarted = false;
  }

  if (_downloading) {
    m_progress.SetPos(_statistics.totalProcessedCount());
    if (_downloading->isRunning()) {
      OutputDebugString(_T("running\n"));
      if (!_downloading->canceling()) {
        m_sessionStatusCtrl.SetBlinkTextColors(0X7f00, 0xffffff);

        //				if (b)
        {
            //					m_sessionStatusCtrl.SetTextColor(true,
            // 0X7f00);
            //					m_sessionStatusCtrl.SetBkColor(_bk);
        }  //				else
        {
          //					m_sessionStatusCtrl.SetTextColor(true,
          // 0xffffff);
          //					m_sessionStatusCtrl.SetBkColor(0x7f00);
        }
        //    m_sessionStatusCtrl.RedrawWindow();
      } else {
        m_sessionStatusCtrl.SetBlinkTextColors(0Xff, 0xffffff);
        OutputDebugString(_T("cancelling\n"));
        m_sessionStatusCtrl.SetWindowText(_T("Canceling"));
        //				if (b)
        {
            //					m_sessionStatusCtrl.SetTextColor(true,
            // 0Xff);
            //					m_sessionStatusCtrl.SetBkColor(_bk);
        }  //				else
        {
          //					m_sessionStatusCtrl.SetTextColor(true,
          // 0xffffff);
          //					m_sessionStatusCtrl.SetBkColor(0xff);
        }
      }
      //			b = !b;
    } else {
      bool canceled = _downloading->canceling();
      m_sessionStatusCtrl.StartTextBlink(FALSE);
      if (canceled) {
        OutputDebugString(_T("cancelled\n"));
        m_sessionStatusCtrl.SetWindowText(_T( "Canceled"));
        //				m_sessionStatusCtrl.SetBkColor(0xff);
        //				m_sessionStatusCtrl.SetTextColor(true,
        // 0xffffff);
      } else {
        OutputDebugString(_T("completed\n"));
        m_sessionStatusCtrl.SetWindowText(_T("Completed"));
        //				m_sessionStatusCtrl.SetBkColor(0x7f00);
        //				m_sessionStatusCtrl.SetTextColor(true,
        // 0xffffff);
      }

      // todo: write errors if session cancelled? Do write for now
      if (hasErrorSymbolsListFile()) {
        if (!_errorSymbolsListFile->write(_appendToErrorSymbolsList)) {
          AfxMessageBox(_T("Couldn't write the error symbols list file"));
        }
      }

      _downloading.reset();

      if (!_logFile.empty() && _log.get() != 0)
        _log->setStatistics(_statistics, canceled);

      if (_autoExit || m_closing) {
        //				OnClose();
        getFrame()->closeFrame();
      } else
        enableControls(true);
    }
    setStatistics(_statistics);
  } else {
    // not downloading
  }

  initSymbolsList();

  __super::OnTimer(nIDEvent);
}

void CYloaderView::OnClickedCheckPeriodAll() {
  UpdateData();
  enableTimeRange();
  _dirty = true;
}

void CYloaderView::OnOpenSymbols() { setSymbolsListFile(); }

void CYloaderView::OnUpdateOpenSymbols(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
}

void CYloaderView::OnClickedSymbolsList() { setSymbolsListFile(); }

void CYloaderView::OnBnClickedButtonSaveDir() {
  CSBDestination sb(m_hWnd, _T( "Select the data folder"));
  sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_NEWDIALOGSTYLE);
  //  sb.SetRoot( SHGetSpecialFolderLocation
  sb.SetInitialSelection(m_saveDir);
  if (sb.SelectFolder()) {
    m_saveDir = sb.GetSelectedFolder();
    //		m_dataPathButton.add((LPCTSTR)m_saveDir);
    UpdateData(FALSE);
    _dirty = true;
  }
}

void CYloaderView::OnBnClickedButtonSaveFileAll() {
  CFileDialog dlg(false, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  _T("CSV files (*.csv)|*.cxv|Text files (*.txt)|*.txt|All ")
                  _T("Files (*.*)|*.*||"));

  if (dlg.DoModal() == IDOK) {
    m_saveFileAll = dlg.GetPathName();
    //		m_dataFileButton.add((LPCTSTR)_saveFileAll);
    UpdateData(FALSE);
    _dirty = true;
  }
}

void CYloaderView::OnBnClickedEditSymbolsList() {
  HINSTANCE ret =
      ShellExecute(0, 0, (LPCTSTR)m_symbolsFileName, 0, 0, SW_SHOWNORMAL);
}

void CYloaderView::OnBnClickedExploreOutputFolder() {
  UpdateData();
  HINSTANCE ret =
      ShellExecute(0, _T( "explore" ), (LPCTSTR)m_saveDir, 0, 0, SW_SHOWNORMAL);
}

void CYloaderView::openDataFile(boost::shared_array<TCHAR> symbol) {
  DataParamsPtr dataParams(getDataParams());

  assert(dataParams);

  // create the full path name for the file to be opened
  FileName x(!dataParams->createSubdirs());
  std::wstring fileName = x.makePath(dataParams->outputPath(), symbol.get(),
                                     dataParams->getFileName(symbol.get()));

  std::wstring yloaderPath(getModulePath());

  if (fileExists(fileName)) {
    if (!_openDataFileWithApp.empty()) {
      try {
        RunProcessResult result(runProcess(
            _T( "" ), quote(_openDataFileWithApp) + _T( " " ) + quote(fileName),
            false, &yloaderPath));
      } catch (const RunProcessException&) {
        AfxMessageBox(
            (std::wstring(_T("Could not run ")) << quote(_openDataFileWithApp))
                .c_str());
      }
    } else {
      int h = (int)ShellExecute(0, _T("open"), fileName.c_str(), 0,
                                getModulePath().c_str(), SW_SHOWDEFAULT);

      if (h <= 32)
        AfxMessageBox((std::wstring(_T("Could not open "))
                       << quote(fileName)
                       << _T(" with the associated application"))
                          .c_str());
    }
  } else {
    AfxMessageBox(
        (std::wstring(_T("No data file for symbol ")) << quote(symbol.get()))
            .c_str());
  }
}

void CYloaderView::OnNMDblclkListEvents(NMHDR* pNMHDR, LRESULT* pResult) {
  LPNMITEMACTIVATE p(reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR));

  if (p->iItem >= 0) {
    boost::shared_array<TCHAR> symbol(new TCHAR[100]);

    m_events.GetItemText(p->iItem, 1, symbol.get(), 99);

    openDataFile(symbol);
  }

  // TODO: Add your control notification handler code here
  *pResult = 0;
}

void CYloaderView::refresh() {
  UpdateData();
  try {
    clearCurrentFileInfo();
  } catch (const FileSymbolsParserException&) {
    // error in the symbols list, clear the list box
    m_symbols.ResetContent();
  }
}

void CYloaderView::OnSize(UINT nType, int cx, int cy) {
  //	__super::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
}

void CYloaderView::OnEnChangeEditSymbolsListLocation() {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the __super::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  _dirty = true;
}

void CYloaderView::OnEnChangeEditSaveDir() {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the __super::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  _dirty = true;
}

void CYloaderView::OnEnChangeEditOutputFileName() {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the __super::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  _dirty = true;
}

void CYloaderView::OnBnClickedCheckUpdate() { _dirty = true; }

void CYloaderView::OnBnClickedCheckDivSplitAdjusted() { _dirty = true; }

void CYloaderView::OnBnClickedCheckValidatePrices() { _dirty = true; }

void CYloaderView::OnCbnSelchangeComboPeriod() { _dirty = true; }

void CYloaderView::OnDtnDatetimechangeDatetimepickerPeriodStart(
    NMHDR* pNMHDR, LRESULT* pResult) {
  LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
  _dirty = true;
  *pResult = 0;
}

void CYloaderView::OnDtnDatetimechangeDatetimepickerPeriodEnd(
    NMHDR* pNMHDR, LRESULT* pResult) {
  LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
  _dirty = true;
  *pResult = 0;
}

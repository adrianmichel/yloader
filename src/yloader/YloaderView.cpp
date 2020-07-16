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

IMPLEMENT_DYNCREATE(CYLoaderView, CFormView)

BEGIN_MESSAGE_MAP(CYLoaderView, CFormView)
ON_WM_CONTEXTMENU()
ON_WM_RBUTTONUP()
ON_WM_DESTROY()
ON_WM_CLOSE()
ON_WM_CREATE()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_CHECK_PERIOD_ALL, &CYLoaderView::OnClickedCheckPeriodAll)
ON_COMMAND(ID_OPEN_SYMBOLS, &CYLoaderView::OnOpenSymbols)
ON_UPDATE_COMMAND_UI(ID_OPEN_SYMBOLS, &CYLoaderView::OnUpdateOpenSymbols)
ON_BN_CLICKED(IDC_SYMBOLS_LIST, &CYLoaderView::OnClickedSymbolsList)
ON_BN_CLICKED(IDC_BUTTON_SAVE_DIR, &CYLoaderView::OnBnClickedButtonSaveDir)
ON_BN_CLICKED(IDC_BUTTON_SAVE_FILE_ALL, &CYLoaderView::OnBnClickedButtonSaveFileAll)
ON_BN_CLICKED(IDC_EDIT_SYMBOLS_LIST, &CYLoaderView::OnBnClickedEditSymbolsList)
ON_BN_CLICKED(IDC_EXPLORE_OUTPUT_FOLDER, &CYLoaderView::OnBnClickedExploreOutputFolder)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_EVENTS, &CYLoaderView::OnNMDblclkListEvents)
ON_MESSAGE(WM_START_SESSION, DoDownload)
ON_WM_SIZE()
ON_EN_CHANGE(IDC_EDIT_SYMBOLS_LIST_LOCATION, &CYLoaderView::OnEnChangeEditSymbolsListLocation)
ON_EN_CHANGE(IDC_EDIT_SAVE_DIR, &CYLoaderView::OnEnChangeEditSaveDir)
ON_EN_CHANGE(IDC_EDIT_OUTPUT_FILE_NAME, &CYLoaderView::OnEnChangeEditOutputFileName)
ON_BN_CLICKED(IDC_CHECK_UPDATE, &CYLoaderView::OnBnClickedCheckUpdate)
ON_BN_CLICKED(IDC_CHECK_DIV_SPLIT_ADJUSTED, &CYLoaderView::OnBnClickedCheckDivSplitAdjusted)
ON_BN_CLICKED(IDC_CHECK_VALIDATE_PRICES, &CYLoaderView::OnBnClickedCheckValidatePrices)
ON_CBN_SELCHANGE(IDC_COMBO_PERIOD, &CYLoaderView::OnCbnSelchangeComboPeriod)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_PERIOD_START, &CYLoaderView::OnDtnDatetimechangeDatetimepickerPeriodStart)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_PERIOD_END, &CYLoaderView::OnDtnDatetimechangeDatetimepickerPeriodEnd)
END_MESSAGE_MAP()

afx_msg LRESULT CYLoaderView::DoDownload(WPARAM, LPARAM) {
  OnDownload();
  return 0;
}

// CYloaderView construction/destruction

CYLoaderView::CYLoaderView()
    : CFormView(IDD_DIALOG_YAHOO_HIST_DATA),
      m_closing(false),
      m_dirty(false),
      frame(NULL),
      m_editSymbolsList(IDI_ICON_EDIT_SYMBOLS_LIST),
      m_exploreOutputFolder(IDI_ICON_EXPLORE_DOWNLOAD_PATH),
      m_openOneOutputFile(IDI_ICON_OPEN_OUTPUT_FILE) {
  setDefaults();
}

constexpr auto SYMBOLS_LIST_FILE_PROP = L"SymbolsListFile";
constexpr auto SAVE_DIR_PROP = L"SaveDir";
constexpr auto START_DATE_PROP = L"StartDate";
constexpr auto ALL_PROP = L"All";
constexpr auto PERIOD_PROP = L"Period";
constexpr auto SPLIT_DIV_ADJUSTED_PROP = L"SplitDivAdjusted";
constexpr auto AUTO_START_DOWNLOADING_PROP = L"AutoStartDownloading";
constexpr auto UPDATE_PROP = L"Update";
constexpr auto VALIDATE_DATA = L"ValidateData";
constexpr auto MOST_RECENT_BARS_LAST_PROP = L"MostRecentBarsLast";
constexpr auto DATE_FORMAT_PROP = L"DateFormat";
constexpr auto ADD_SYMBOLS_TO_COLUMN_PROP = L"AddSymbolToColumn";
constexpr auto SYMBOL_COLUMN_NUMBER_PROP = L"SymbolColumnNumber";
constexpr auto PREPREND_TO_FILE_NAME_PROP = L"PrependToFileName";
constexpr auto APPEND_TO_FILE_NAME_PROP = L"AppendToFileName";
constexpr auto CHECK_FOR_UPDATES_AT_STARTUP2_PROP = L"CheckForUpdatesAtStartup2";
constexpr auto FILE_NAME_EXTENSION_PROP = L"FileNameExtension";
constexpr auto THREADS_PROP = L"Threads";
constexpr auto LOG_FILE_PROP = L"LogFile";
constexpr auto APPEND_TO_LOG_PROP = L"AppendToLog";
constexpr auto PAD_DATE_PROP = L"PadDate";
constexpr auto SYMBOL_TIMEOUT_PROP = L"SymbolTimeout";
constexpr auto LOG_ONLY_ERRORS_PROP = L"LogOnlyErrors";
constexpr auto CREATE_SUBDIR_PROP = L"CreateSubdirs";
constexpr auto VOLUME_0_INVALID_PROP = L"Volume0Invalid";
constexpr auto INVALID_DATA_HANDLING = L"InvalidDataHandling";
constexpr auto HIDE_NEWS_PROP = L"HideNews";
constexpr auto AUTO_SELECT_OUTPUT_PATH_PROP = L"AutoSelectOutputPath";
constexpr auto AUTO_SELECT_OUTPUT_PATH_NAME_PROP = L"AutoSelectOutputPathName";
constexpr auto DIAGNOSTIC_LOGGING_PROP = L"DiagnosticLogging";
constexpr auto CH_MAP_FROM_PROP = L"ChMapFrom";
constexpr auto CH_MAP_TO_PROP = L"ChMapTo";
constexpr auto FIELD_SEPARATOR_PROP = L"FieldSeparator";
constexpr auto FILE_HEADER_PROP = L"FileHeader";
constexpr auto DATE_SEPARATOR_PROP = L"DateSeparator";
constexpr auto VOLUME_MULTIPLIER_PROP = L"VolumeMultiplier";
constexpr auto SELECTED_DATA_SOURCE_PLUGIN_PROP = L"SelectedDataSourcePlugin";
constexpr auto PROXY_SERVER_ADDRESS_PROP = L"ProxyServerAddress";
constexpr auto PROXY_SERVER_USER_NAME_PROP = L"ProxyServerUserName";
constexpr auto PROXY_SERVER_PASSWORD_PROP = L"ProxyServerPassword";
constexpr auto HTTP_REQUEST_TIMEOUT_PROP = L"HTTPRequestTimeout";
constexpr auto OUTPUT_FILE_PROP = L"OutputFile";
constexpr auto OPEN_DATA_FILE_WITH_APP_PROP = L"OpenDataFileWithithApp";
constexpr auto DONT_RELOAD_OLD_DATA_IN_UPDATE_MODE_PROP = L"DontReloadOldDataInUpdateMode";
constexpr auto MATCH_REGEX_PROP = L"MatchRegex";
constexpr auto FORMAT_STRING_PROP = L"FormatString";
constexpr auto ENABLE_REGEX_FORMATTING_PROP = L"EnableRegexFormatting";
constexpr auto ERROR_SYMBOLS_LIST_PROP = L"ErrorSymbolsList";
constexpr auto APPEND_TO_ERRORS_SYMBOLS_LIST_PROP = L"AppendToErrorSymbolsList";
constexpr auto IGNORE_SYMBOLS_LIST_PROP = L"IgnoreSymbolsList";
constexpr auto IGNORE_ERRORS_SYMBOLS_LIST_PROP = L"IgnoreErrorSymbolsList";
constexpr auto NOTIFY_ONLY_IF_NEWER_VERSION_PROP = L"NotifyOnlyIfNewerVersion";
constexpr auto DECIMAL_SEPARATOR_PROP = L"DecimalSeparator";
constexpr auto FIXED_DECIMALS_COUNT_PROP = L"FixedDecimalsCount";

void CYLoaderView::SaveParams(bool force, bool updateData) {
  // this is not a regular setting, it's just to mark the subscription exp date
  // for the next version installer
  /*	std::wstring expDate(subscriptionExpirationDate().is_special() ?
  std::wstring() : subscriptionExpirationDate().to_iso_string());
  WriteProfileString(_T("UpdateXTime"), (boost::format("%1%") % expDate).str());
  */
  if (force || m_dirty || dateRangeChanged()) {
    if (updateData) UpdateData();

    WriteProfileString(SYMBOLS_LIST_FILE_PROP, getSymbolsListFileName());
    WriteProfileString(SAVE_DIR_PROP, getSaveDir());
    WriteProfileString(START_DATE_PROP, getStartDateAsString());
    WriteProfileInt(ALL_PROP, getAll());
    WriteProfileInt(PERIOD_PROP, getPeriod());
    WriteProfileInt(SPLIT_DIV_ADJUSTED_PROP, getAdjustment());
    WriteProfileInt(AUTO_START_DOWNLOADING_PROP, m_autoStartDownloading);
    WriteProfileInt(UPDATE_PROP, getUpdate());
    WriteProfileInt(VALIDATE_DATA, getValidatePrices());
    WriteProfileInt(MOST_RECENT_BARS_LAST_PROP, m_mostRecentBarsLastBool);
    WriteProfileInt(DATE_FORMAT_PROP, m_dateFormat);
    WriteProfileInt(ADD_SYMBOLS_TO_COLUMN_PROP, m_addSymbolToColumn);
    WriteProfileInt(SYMBOL_COLUMN_NUMBER_PROP, m_columnNumber);
    WriteProfileString(PREPREND_TO_FILE_NAME_PROP, m_prependToFileName);
    WriteProfileString(APPEND_TO_FILE_NAME_PROP, m_appendToFileName);
    WriteProfileInt(CHECK_FOR_UPDATES_AT_STARTUP2_PROP, m_checkForUpdatesAtStartup);
    WriteProfileString(FILE_NAME_EXTENSION_PROP, m_fileNameExtension);
    WriteProfileInt(THREADS_PROP, m_threads);
    WriteProfileString(LOG_FILE_PROP, m_logFile);
    WriteProfileInt(APPEND_TO_LOG_PROP, m_appendToLog);
    WriteProfileInt(PAD_DATE_PROP, m_padDate);
    WriteProfileInt(SYMBOL_TIMEOUT_PROP, m_symbolTimeout);
    WriteProfileInt(LOG_ONLY_ERRORS_PROP, m_logOnlyErrors);
    WriteProfileInt(CREATE_SUBDIR_PROP, m_createSubdirs);
    WriteProfileInt(VOLUME_0_INVALID_PROP, m_volume0invalid);
    WriteProfileInt(INVALID_DATA_HANDLING, m_invalidDataHandling);
    WriteProfileInt(HIDE_NEWS_PROP, m_hideNews);
    WriteProfileInt(AUTO_SELECT_OUTPUT_PATH_PROP, m_autoSelectOutputPath);
    WriteProfileString(AUTO_SELECT_OUTPUT_PATH_NAME_PROP, m_autoSelectOutputPathName);
    WriteProfileInt(DIAGNOSTIC_LOGGING_PROP, m_diagnosticLogging);
    WriteProfileString(CH_MAP_FROM_PROP, m_characterMapping.getFromString());
    WriteProfileString(CH_MAP_TO_PROP, m_characterMapping.getToString());
    WriteProfileString(FIELD_SEPARATOR_PROP, yloader::escape(m_fieldSeparator));
    WriteProfileString(FILE_HEADER_PROP, yloader::escape(m_fileHeader));
    WriteProfileString(DATE_SEPARATOR_PROP, yloader::escape(m_dateSeparator));
    WriteProfileString(VOLUME_MULTIPLIER_PROP, std::to_wstring(m_volumeMultiplier));
    if (m_selectedDataSourcePlugin) {
      WriteProfileString(SELECTED_DATA_SOURCE_PLUGIN_PROP, *m_selectedDataSourcePlugin);
    }

    WriteProfileString(PROXY_SERVER_ADDRESS_PROP, m_proxyServerAddress);
    WriteProfileString(PROXY_SERVER_USER_NAME_PROP, m_proxyServerUserName);
    WriteProfileString(PROXY_SERVER_PASSWORD_PROP, m_proxyServerPassword);
    WriteProfileInt(HTTP_REQUEST_TIMEOUT_PROP, m_httpRequestTimeout);
    WriteProfileString(OUTPUT_FILE_PROP, getSaveFileAll());
    WriteProfileString(OPEN_DATA_FILE_WITH_APP_PROP, yloader::escape(m_openDataFileWithApp));
    WriteProfileInt(DONT_RELOAD_OLD_DATA_IN_UPDATE_MODE_PROP, m_dontReloadOldDataInUpdateMode);

    WriteProfileString(MATCH_REGEX_PROP, yloader::escape(m_matchRegex.str()));
    WriteProfileString(FORMAT_STRING_PROP, yloader::escape(m_formatString));
    WriteProfileInt(ENABLE_REGEX_FORMATTING_PROP, m_enableRegexFormatting);
    WriteProfileString(ERROR_SYMBOLS_LIST_PROP, m_errorSymbolsList);
    WriteProfileInt(APPEND_TO_ERRORS_SYMBOLS_LIST_PROP, m_appendToErrorSymbolsList);
    WriteProfileString(IGNORE_SYMBOLS_LIST_PROP, m_ignoreSymbolsList);
    WriteProfileInt(IGNORE_ERRORS_SYMBOLS_LIST_PROP, m_ignoreErrorSymbolsList);
    WriteProfileString(NOTIFY_ONLY_IF_NEWER_VERSION_PROP, m_notifyOnlyIfNewerVersion);
    WriteProfileString(DECIMAL_SEPARATOR_PROP, m_decimalSeparator);
    WriteProfileInt(FIXED_DECIMALS_COUNT_PROP, m_fixedDecimalsCount);
  }
}

void CYLoaderView::LoadParams(const std::wstring& defSymbolsFileName, const std::wstring& dataPath) {
  try {
    LOG(log_info, L"Loading settings");
    setStartDate(GetProfileString(START_DATE_PROP));

    // if m_symbolsFileName is already set (by the command line), then it
    // overrides the saved data
    setSymbolsFileName(GetProfileString(SYMBOLS_LIST_FILE_PROP));
    // if m_saveDir is already set (by the command line), then it overrides the
    // saved data
    setSaveDir(GetProfileString(SAVE_DIR_PROP));
    setAllAvailable(GetProfileBool(ALL_PROP, DEFAULT_ALL_AVAILABLE));
    setPeriod((Period)GetProfileInt(PERIOD_PROP, DEFAULT_PERIOD));
    setAdjustment(GetProfileBool(SPLIT_DIV_ADJUSTED_PROP, DEFAULT_ADJUST));
    // if m_autoStartDownloading is already set (by the command line), then it
    // overrides the saved data
    m_autoStartDownloading = GetProfileBool(AUTO_START_DOWNLOADING_PROP, DEFAULT_AUTO_START) || m_autoStartDownloading;
    setValidatePrices(GetProfileBool(VALIDATE_DATA, DEFAULT_VALIDATE_PRICES));
    setUpdate(GetProfileBool(UPDATE_PROP, DEFAULT_UPDATE));
    m_mostRecentBarsLastBool = GetProfileBool(MOST_RECENT_BARS_LAST_PROP, DEFAULT_SORT_BARS_ASCENDING);
    m_dateFormat = GetProfileInt(DATE_FORMAT_PROP, DEFAULT_DATE_FORMAT);
    m_columnNumber = GetProfileInt(SYMBOL_COLUMN_NUMBER_PROP, DEFAULT_COLUMN_NUMBER);
    m_addSymbolToColumn = GetProfileBool(ADD_SYMBOLS_TO_COLUMN_PROP, DEFAULT_ADD_SYMBOL);
    m_prependToFileName = GetProfileString(PREPREND_TO_FILE_NAME_PROP);
    m_appendToFileName = GetProfileString(APPEND_TO_FILE_NAME_PROP);
    m_checkForUpdatesAtStartup = GetProfileBool(CHECK_FOR_UPDATES_AT_STARTUP2_PROP, DEFAULT_CHECK_FOR_UPDATES_AT_STARTUP);
    m_fileNameExtension = GetProfileString(FILE_NAME_EXTENSION_PROP, DEFAULT_EXTENSION);
    m_threads = GetProfileInt(THREADS_PROP, DEFAULT_THREADS);
    m_logFile = GetProfileString(LOG_FILE_PROP);
    m_appendToLog = GetProfileBool(APPEND_TO_LOG_PROP, DEFAULT_APPEND_TO_LOG);
    m_padDate = GetProfileBool(PAD_DATE_PROP, DEFAULT_PAD_DATE_FIELDS);
    m_symbolTimeout = GetProfileInt(SYMBOL_TIMEOUT_PROP, DEFAULT_SYMBOL_TIMEOUT);
    m_logOnlyErrors = GetProfileBool(LOG_ONLY_ERRORS_PROP, DEFAULT_LOG_ONLY_ERRORS);
    m_createSubdirs = GetProfileBool(CREATE_SUBDIR_PROP, DEFAULT_CREATE_SUBDIRS);
    m_volume0invalid = GetProfileBool(VOLUME_0_INVALID_PROP, DEFAULT_VOLUME0_INVALID);
    // 0 for warning, 1 for error
    m_invalidDataHandling = GetProfileInt(INVALID_DATA_HANDLING, DEFAULT_HANDLING_INVALID_DATA);
    m_hideNews = GetProfileBool(HIDE_NEWS_PROP);
    m_autoSelectOutputPath = GetProfileBool(AUTO_SELECT_OUTPUT_PATH_PROP);
    m_autoSelectOutputPathName = GetProfileString(AUTO_SELECT_OUTPUT_PATH_NAME_PROP, L"data");
    m_diagnosticLogging = GetProfileBool(DIAGNOSTIC_LOGGING_PROP);
    std::wstring cmfrom = GetProfileString(CH_MAP_FROM_PROP, L":");
    std::wstring cmto = GetProfileString(CH_MAP_TO_PROP, L"_");
    m_fieldSeparator = yloader::unescape(GetProfileString(FIELD_SEPARATOR_PROP, DEFAULT_FIELD_SEPARATOR));
    m_fileHeader = yloader::unescape(GetProfileString(FILE_HEADER_PROP, DEFAULT_FILE_HEADER));
    m_dateSeparator = yloader::unescape(GetProfileString(DATE_SEPARATOR_PROP, DEFAULT_DATE_SEPARATOR));
    m_volumeMultiplier = _tstof(GetProfileString(VOLUME_MULTIPLIER_PROP, std::to_wstring( DEFAULT_VOLUME_MULTIPLIER)).c_str());
    std::wstring sds = GetProfileString(SELECTED_DATA_SOURCE_PLUGIN_PROP);
    m_selectedDataSourcePlugin = sds.empty() ? nullptr : std::make_shared< yloader::UniqueId>(sds.c_str());
    m_proxyServerAddress = GetProfileString(PROXY_SERVER_ADDRESS_PROP);
    m_proxyServerUserName = GetProfileString(PROXY_SERVER_USER_NAME_PROP);
    m_proxyServerPassword = GetProfileString(PROXY_SERVER_PASSWORD_PROP);
    m_httpRequestTimeout = GetProfileInt(HTTP_REQUEST_TIMEOUT_PROP, 60);
    setSaveFileAll(GetProfileString(OUTPUT_FILE_PROP));
    m_openDataFileWithApp = yloader::unescape(GetProfileString(OPEN_DATA_FILE_WITH_APP_PROP));
    m_dontReloadOldDataInUpdateMode = GetProfileBool(DONT_RELOAD_OLD_DATA_IN_UPDATE_MODE_PROP, DEFAULT_DONT_RELOAD_DATA_IN_UPDATE_MODE);
    m_enableRegexFormatting = GetProfileBool(ENABLE_REGEX_FORMATTING_PROP, DEFAULT_ENABLE_REGEX_FORMATTING);
    m_matchRegex = yloader::unescape(GetProfileString(MATCH_REGEX_PROP, DEFAULT_MATCH_REGEX));
    m_formatString = yloader::unescape(GetProfileString(FORMAT_STRING_PROP, DEFAULT_FORMAT_STRING));
    m_errorSymbolsList = GetProfileString(ERROR_SYMBOLS_LIST_PROP, DEFAULT_ERROR_SYMBOLS_LIST);
    m_appendToErrorSymbolsList = GetProfileInt(APPEND_TO_ERRORS_SYMBOLS_LIST_PROP, DEFAULT_APPEND_TO_ERROR_SYMBOLS_LIST);
    m_ignoreErrorSymbolsList = GetProfileInt(IGNORE_ERRORS_SYMBOLS_LIST_PROP, DEFAULT_IGNORE_ERROR_SYMBOLS_LIST);
    m_ignoreSymbolsList = GetProfileString(IGNORE_SYMBOLS_LIST_PROP, DEFAULT_IGNORE_SYMBOLS_LIST);
    //		_updateXTime = GetProfileString(XX, m_T( "" ));
    m_notifyOnlyIfNewerVersion = GetProfileString(NOTIFY_ONLY_IF_NEWER_VERSION_PROP);

    m_characterMapping.set(cmfrom, cmto);

    m_decimalSeparator = GetProfileString(DECIMAL_SEPARATOR_PROP, DEFAULT_DECIMAL_SEPARATOR);
    m_fixedDecimalsCount = GetProfileInt(FIXED_DECIMALS_COUNT_PROP, DEFAULT_FIXED_DECIMALS_COUNT);

    if (!yloader::fileExists(getYLoaderSettingsFile())) {
      SaveParams(true, false);
    }
  }

  catch (...) {
    // handle date/time string errors
  }
}

void CYLoaderView::setDefaults() {
  m_symbolsFileName = L"";
  m_saveDir = L"";
  m_all = DEFAULT_ALL_AVAILABLE;
  m_period = DEFAULT_PERIOD;
  m_Adjustment = DEFAULT_ADJUST;
  m_validatePrices = FALSE;
  m_update = DEFAULT_UPDATE;
  m_startDate.SetStatus(COleDateTime::DateTimeStatus::null);
  m_endDate = COleDateTime::GetCurrentTime();
  m_saveFileAll = L"";
  m_autoStartDownloading = DEFAULT_AUTO_START;
  m_mostRecentBarsLastBool = true;
  m_dateFormat = 0;
  m_columnNumber = 0;
  m_addSymbolToColumn = false;
  m_prependToFileName = L"";
  m_appendToFileName = L"";
  m_checkForUpdatesAtStartup = true;
  m_fileNameExtension = DEFAULT_EXTENSION;
  m_threads = 5;
  m_logFile = L"";
  m_appendToLog = false;
  m_padDate = false;
  m_symbolTimeout = 0;
  m_logOnlyErrors = true;
  m_createSubdirs = false;
  m_volume0invalid = false;
  m_invalidDataHandling = 0;
  m_hideNews = false;
  m_autoSelectOutputPath = false;
  m_autoSelectOutputPathName = L"data";
  m_diagnosticLogging = false;
  m_autoExit = false;
  m_fieldSeparator = DEFAULT_FIELD_SEPARATOR;
  m_characterMapping.set(L":", L"_");
  m_dialog = true;
  m_defSymbolsFileName.clear();
  m_defDataPath.clear();
  m_logFileError = false;
  m_proxyServerAddress = L"";
  m_proxyServerUserName = L"";
  m_proxyServerPassword = L"";
  m_httpRequestTimeout = 60;
  m_openDataFileWithApp = L"";
  m_startMode = START_NORMAL;
  m_enableRegexFormatting = DEFAULT_ENABLE_REGEX_FORMATTING;
  m_matchRegexUnlocked = DEFAULT_MATCH_REGEX_UNLOCKED;
  m_matchRegex = DEFAULT_MATCH_REGEX;
  m_formatString = DEFAULT_FORMAT_STRING;
  m_errorSymbolsList = DEFAULT_ERROR_SYMBOLS_LIST;
}
CYLoaderView::~CYLoaderView() {}

void CYLoaderView::DoDataExchange(CDataExchange* pDX) {
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

BOOL CYLoaderView::PreCreateWindow(CREATESTRUCT& cs) {
  return CFormView::PreCreateWindow(cs);
}

void CYLoaderView::OnInitialUpdate() {
  CFormView::OnInitialUpdate();
  GetParentFrame()->RecalcLayout();
  ResizeParentToFit(FALSE);

  // Create the colored <SoftechSoftware> sign
  //	m_sessionStatusCtrl.SubclassDlgItem(IDC_STATIC_SESSION_STATUS_VALUE,
  // this);

  LoadParams(m_defSymbolsFileName, m_defDataPath);

  m_plugins.load(yloader::getModulePath());

  m_dataPathEdit.SetUseDir();
  m_symbolsListFileEdit.setRefreshable(this);

  setStatistics(m_statistics);
  m_events.init();

  setDataSourceText();

  m_editSymbolsList.init();
  m_exploreOutputFolder.init();
  m_openOneOutputFile.init();

  SetTimer(1, 200, NULL);
  EnableScrollBarCtrl(SB_BOTH, FALSE);
}

void CYLoaderView::OnRButtonUp(UINT /* nFlags */, CPoint point) {
  ClientToScreen(&point);
  OnContextMenu(this, point);
}

void CYLoaderView::OnContextMenu(CWnd* /* pWnd */, CPoint point) {
#ifndef SHARED_HANDLERS
  theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CYloaderView diagnostics

#ifdef _DEBUG
void CYLoaderView::AssertValid() const { CFormView::AssertValid(); }

void CYLoaderView::Dump(CDumpContext& dc) const { CFormView::Dump(dc); }

CYLoaderDoc* CYLoaderView::GetDocument() const  // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CYLoaderDoc)));
  return (CYLoaderDoc*)m_pDocument;
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

void CYLoaderView::OnDestroy() {
  SaveParams();

  CFormView::OnDestroy();
}

void CYLoaderView::OnClose() {
  if (m_downloading && m_downloading->isRunning()) {
    if (AfxMessageBox(L"A downloading session is in progress. Are you sure you want to exit?", MB_YESNO) == IDYES) {
      if (m_downloading && m_downloading->isRunning()) {
        m_downloading->cancel();
      }

      m_closing = true;
    }
  }
  else {
    CFormView::OnClose();
  }
}

int CYLoaderView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CFormView::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  return 0;
}

void CYLoaderView::OnDownload() {
  // this will set the flag to true upon exit of this function
  AutoBool ab(m_hasHadSession, true);
  if (m_downloading && m_downloading->isRunning()) {
    m_downloading->cancel();
  }
  else {
    try {
      UpdateData();

      YDataSourcePluginPtr plugin = getDataSourcePlugin();

      if (!plugin) {
        AfxMessageBox(L"No data source plugin has been selected. Please select a data source plugin and try again", MB_OK | MB_ICONSTOP);
        return;
      }

      assert((bool)plugin);

      const std::wstring& pluginShortName = plugin->shortName();

      if (getSymbolsFileName().IsEmpty()) {
        AfxMessageBox(L"No symbols list file has been selected. Please select a symbols list file and try again", MB_OK | MB_ICONSTOP);
        return;
      }

      if (!getAdjustment() && !plugin->canUnadjust()) {
        AfxMessageBox((L"Unadjusted data is not supported by the "s + pluginShortName +
          L" data source plug-in\n\nChange the setting and try again").c_str(), MB_OK | MB_ICONSTOP);
        return;
      }

      if (getAdjustment() && !plugin->canAdjust()) {
        AfxMessageBox((L"Adjusted data is not supported by the "s + pluginShortName +
          L" data source plug-in\n\nChange the setting and try again").c_str(), MB_OK | MB_ICONSTOP);
        return;
      }

      if (oleDateTimeToDate(getStartDate()) < yloader::Date(plugin->firstDate())) {
        AfxMessageBox((L"Data older than "s + yloader::Date(plugin->firstDate()).toString() + L" is not supported by the " +
          pluginShortName + L" data source plug-in\n\nChange the setting and try again").c_str(), MB_OK | MB_ICONSTOP);
        return;
      }

      if (!plugin->isPeriodSupported(getPeriod())) {
        AfxMessageBox((L"The current period is not supported by the " + pluginShortName +
          L" data source plug-in\n\nChange the setting and try again").c_str(), MB_OK | MB_ICONSTOP);
        return;
      }

      DeleteAllEvents();

      YahooEventDelegatorPtr yed(new YahooEventDelegator());
      addSinks(yed);

      if (getSaveDir().IsEmpty() && !m_autoSelectOutputPath) {
        CEmptyDataPathPromptDlg dlg;
        dlg.DoModal();
        if (dlg.autoSelectDataPath()) {
          m_autoSelectOutputPath = true;
        }
        else {
          return;
        }
      }

      Date endDate = oleDateTimeToDate(getEndDate());
      Date startDate = oleDateTimeToDate(getStartDate());
      if (!endDate.isNotADate() && !startDate.isNotADate() && getEndDate() < getStartDate()) {
        AfxMessageBox(L"The start date should occur after the end date.\nPlease correct and try again", MB_OK | MB_ICONSTOP);
      }
      else {
        StrListPtr symbolsList(yloader::getSymbols((LPCTSTR)getSymbolsFileName(), true));

        DataParamsPtr dataParams(getDataParams());

        std::wstring outputPath = dataParams->outputPath();

        CFileFind ff;
        if (!(ff.FindFile(outputPath.c_str()) && ff.FindNextFile() && ff.IsDirectory())) {
          CreateDirectory(outputPath.c_str(), 0);
        }

        std::shared_ptr<SymbolsListIterator> si( make_shared< SymbolsListIterator >(symbolsList.get()));

        startStatistics(si->size());

        //				setTotalConnectionsValue(_threads);
        m_sessionStatusCtrl.SetWindowText(L"Downloading");

        try {
          if (!m_logFile.empty()) {
            m_log = std::make_shared<YahooSessionLog>(*dataParams);

            yed->addSink(m_log.get());
          }
          else {
            m_log.reset();
          }
        }
        catch (const LogFileException& e) {
          if (!m_autoStartDownloading) {
            std::wstring message = L"Could not open log file \"" + e.fileName() + L"\". Start downloading?";
            if (AfxMessageBox(message.c_str(), MB_ICONEXCLAMATION | MB_YESNO) == IDNO) {
              return;
            }
          }
          else {
            m_logFileError = true;
          }
        }

        setProgressRange(si->size());

        assert(m_downloading.get() == 0);

        enableControls(false);
        //		  m_downloadButton.SetWindowText( _T( "Cancel" ) );

        m_errorSymbolsListFile = hasErrorSymbolsListFile() ? std::make_shared<WriteFileSymbolsList>(m_errorSymbolsList) : nullptr;

        yloader::UniqueSymbolsSetPtr ignoreSymbolsSet(dataParams->getIgnoreSymbols(yed));

        m_downloading = std::make_shared< CDownloading >(*dataParams, m_errorSymbolsListFile.get(), symbolsList, ignoreSymbolsSet, si, yed);

        Timer timer;
        LOG(log_info, L"Starting a downloading session");
        LOG(log_info, L"Session parameters:\n", dataParams->toString());

        plugin->setProxyServerAddress(m_proxyServerAddress.c_str());
        plugin->setProxyServerUserName(m_proxyServerUserName.c_str());
        plugin->setProxyServerPassword(m_proxyServerPassword.c_str());
        plugin->setHTTPRequestTimeout(m_httpRequestTimeout);

        m_sessionStatusCtrl.StartTextBlink(true, CColorStaticST::ST_FLS_FAST);

        m_downloading->start();
      }
    }
    catch (const FileSymbolsParserException&) {
      AfxMessageBox(L"Symbols list file error", MB_OK | MB_ICONSTOP);
    }
    catch (...) {
      LOG(log_error, L"Unknown exception");
      AfxMessageBox(L"Unknown exception, please contact yloader.com support", MB_OK | MB_ICONSTOP);
    }
  }
}

void CYLoaderView::OnSettings() {
  YSettingsDialog dlg(
      m_plugins, m_selectedDataSourcePlugin.get(), m_addSymbolToColumn,
      m_columnNumber, m_prependToFileName, m_appendToFileName,
      m_checkForUpdatesAtStartup, m_fileNameExtension, m_logFile, m_threads,
      m_appendToLog, m_padDate, m_dateFormat, m_fieldSeparator,
      m_mostRecentBarsLastBool != 0, m_autoStartDownloading != 0,
      m_symbolTimeout, m_logOnlyErrors, m_createSubdirs, m_invalidDataHandling,
      m_volume0invalid, m_hideNews, m_autoSelectOutputPath,
      m_autoSelectOutputPathName, m_diagnosticLogging, m_characterMapping,
      m_volumeMultiplier, m_fileHeader, m_dateSeparator, m_proxyServerAddress,
      m_proxyServerUserName, m_proxyServerPassword, m_httpRequestTimeout,
      m_openDataFileWithApp, m_dontReloadOldDataInUpdateMode, m_minimizeToTray,
      showInitialMB, m_enableRegexFormatting, m_matchRegexUnlocked,
      m_matchRegex.str(), m_formatString, m_errorSymbolsList,
      m_appendToErrorSymbolsList, m_ignoreErrorSymbolsList, m_ignoreSymbolsList,
      this);

  if (dlg.DoModal() == IDOK) {
    m_dirty = true;
    m_addSymbolToColumn = dlg.addSymbolsToColumn();
    m_columnNumber = dlg.columnNumber();
    m_prependToFileName = dlg.prependToFileName();
    m_appendToFileName = dlg.appendToFileName();
    m_checkForUpdatesAtStartup = dlg.checkForUpdatesAtStartup();
    m_fileNameExtension = dlg.fileNameExtension();
    m_threads = dlg.threads();
    m_logFile = dlg.logFile();
    m_appendToLog = dlg.appendToLog();
    m_padDate = dlg.padDate();
    m_dateFormat = dlg.dateFormat();
    m_mostRecentBarsLastBool = dlg.sortBarsInAscendingOrder();
    m_autoStartDownloading = dlg.autoStartDownloading();
    //  m_symbolTimeout = dlg.symbolTimeout();
    m_logOnlyErrors = dlg.logOnlyErrors();
    m_createSubdirs = dlg.createSubdirs();
    m_invalidDataHandling = dlg.invalidDataHandling();
    m_volume0invalid = dlg.volume0invalid();
    m_autoSelectOutputPath = dlg.autoSelectOutputPath();
    m_autoSelectOutputPathName = dlg.autoSelectOutputPathName();
    m_diagnosticLogging = dlg.diagnosticLogging();
    m_characterMapping = dlg.characterMapping();
    m_fieldSeparator = dlg.fieldSeparator();
    m_volumeMultiplier = dlg.volumeMultiplier();
    m_fileHeader = dlg.fileHeader();
    m_dateSeparator = dlg.dateSeparator();
    m_proxyServerAddress = dlg.proxyServerAddress();
    m_proxyServerUserName = dlg.proxyServerUserName();
    m_proxyServerPassword = dlg.proxyServerPassword();
    m_httpRequestTimeout = dlg.httpRequestTimeout();
#ifdef ENABLE_TRAY
    m_minimizeToTray = dlg.minimizeToTray();
#endif
    m_selectedDataSourcePlugin = dlg.getSelectedDataSourcePlugin() != 0 ?
      std::make_shared< yloader::UniqueId>(*dlg.getSelectedDataSourcePlugin()) : nullptr;
    m_openDataFileWithApp = dlg.openDataFileWithApp();
    m_dontReloadOldDataInUpdateMode = dlg.dontReloadOldDataInUpdateMode();
    m_enableRegexFormatting = dlg.enableRegexFormatting();
    m_matchRegexUnlocked = dlg.matchRegexUnlocked();
    m_matchRegex = dlg.matchRegex();
    m_formatString = dlg.formatString();
    m_errorSymbolsList = dlg.errorSymbolsFile();
    m_appendToErrorSymbolsList = dlg.appendToErrorSymbolsFile();
    m_ignoreSymbolsList = dlg.ignoreSymbolsList();
    m_ignoreErrorSymbolsList = dlg.ignoreErrorSymbolsList();

    enableUpdate();

    Log::setLevel(m_diagnosticLogging ? log_debug : log_none);

    setDataSourceText();
  }
}

const unsigned long WM_AUTO_START = WM_USER + 3454;

void CYLoaderView::OnTimer(UINT_PTR nIDEvent) {
  if (m_showSettings) {
    m_showSettings = false;
    PostMessage(WM_SHOW_SETTINGS);
  }
  // TODO: Add your message handler code here and/or call default
  if (m_autoStartDownloading && m_notStarted) {
    PostMessage(WM_AUTO_START);
    m_notStarted = false;
  }

  if (m_downloading) {
    m_progress.SetPos(m_statistics.totalProcessedCount());
    if (m_downloading->isRunning()) {
      if (!m_downloading->canceling()) {
        m_sessionStatusCtrl.SetBlinkTextColors(0X7f00, 0xffffff);
      }
      else {
        m_sessionStatusCtrl.SetBlinkTextColors(0Xff, 0xffffff);
        m_sessionStatusCtrl.SetWindowText(L"Canceling");
      }
    }
    else {
      bool canceled = m_downloading->canceling();
      m_sessionStatusCtrl.StartTextBlink(FALSE);
      if (canceled) {
        m_sessionStatusCtrl.SetWindowText(L"Canceled");
      }
      else {
        m_sessionStatusCtrl.SetWindowText(L"Completed");
      }

      // todo: write errors if session cancelled? Do write for now
      if (hasErrorSymbolsListFile()) {
        if (!m_errorSymbolsListFile->write(m_appendToErrorSymbolsList)) {
          AfxMessageBox(L"Couldn't write the error symbols list file");
        }
      }

      m_downloading.reset();

      if (!m_logFile.empty() && m_log.get() != 0) {
        m_log->setStatistics(m_statistics, canceled);
      }

      if (m_autoExit || m_closing) {
        //				OnClose();
        getFrame()->closeFrame();
      }
      else {
        enableControls(true);
      }
    }
    setStatistics(m_statistics);
  }
  else {
    // not downloading
  }

  initSymbolsList();

  __super::OnTimer(nIDEvent);
}

void CYLoaderView::OnClickedCheckPeriodAll() {
  UpdateData();
  enableTimeRange();
  m_dirty = true;
}

void CYLoaderView::OnOpenSymbols() { setSymbolsListFile(); }

void CYLoaderView::OnUpdateOpenSymbols(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
}

void CYLoaderView::OnClickedSymbolsList() { setSymbolsListFile(); }

void CYLoaderView::OnBnClickedButtonSaveDir() {
  CSBDestination sb(m_hWnd, L"Select the data folder");
  sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_NEWDIALOGSTYLE);
  //  sb.SetRoot( SHGetSpecialFolderLocation
  sb.SetInitialSelection(m_saveDir);
  if (sb.SelectFolder()) {
    m_saveDir = sb.GetSelectedFolder();
    UpdateData(FALSE);
    m_dirty = true;
  }
}

void CYLoaderView::OnBnClickedButtonSaveFileAll() {
  CFileDialog dlg(false, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
    L"CSV files (*.csv)|*.cxv|Text files (*.txt)|*.txt|All Files (*.*)|*.*||");

  if (dlg.DoModal() == IDOK) {
    m_saveFileAll = dlg.GetPathName();
    UpdateData(FALSE);
    m_dirty = true;
  }
}

void CYLoaderView::OnBnClickedEditSymbolsList() {
  HINSTANCE ret = ShellExecute(0, 0, (LPCTSTR)m_symbolsFileName, 0, 0, SW_SHOWNORMAL);
}

void CYLoaderView::OnBnClickedExploreOutputFolder() {
  UpdateData();
  HINSTANCE ret = ShellExecute(0, L"explore", (LPCTSTR)m_saveDir, 0, 0, SW_SHOWNORMAL);
}

void CYLoaderView::openDataFile(boost::shared_array<TCHAR> symbol) {
  DataParamsPtr dataParams(getDataParams());

  assert(dataParams);

  // create the full path name for the file to be opened
  FileName x(!dataParams->createSubdirs());
  std::wstring fileName = x.makePath(dataParams->outputPath(), symbol.get(), dataParams->getFileName(symbol.get()));

  std::wstring yloaderPath(getModulePath());

  if (fileExists(fileName)) {
    if (!m_openDataFileWithApp.empty()) {
      try {
        RunProcessResult result(runProcess(L"", quote(m_openDataFileWithApp) + L" " + quote(fileName), false, &yloaderPath));
      }
      catch (const RunProcessException&) {
        AfxMessageBox((L"Could not run "s + quote(m_openDataFileWithApp)).c_str());
      }
    }
    else {
      HINSTANCE h = ShellExecute(0, L"open", fileName.c_str(), 0, getModulePath().c_str(), SW_SHOWDEFAULT);

      if (h <= (HINSTANCE)32) {
        AfxMessageBox((L"Could not open " + quote(fileName) + L" with the associated application").c_str());
      }
    }
  }
  else {
    AfxMessageBox((L"No data file for symbol " + quote(symbol.get())).c_str());
  }
}

void CYLoaderView::OnNMDblclkListEvents(NMHDR* pNMHDR, LRESULT* pResult) {
  LPNMITEMACTIVATE p(reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR));

  if (p->iItem >= 0) {
    boost::shared_array<TCHAR> symbol(new TCHAR[100]);

    m_events.GetItemText(p->iItem, 1, symbol.get(), 99);

    openDataFile(symbol);
  }

  // TODO: Add your control notification handler code here
  *pResult = 0;
}

void CYLoaderView::refresh() {
  UpdateData();
  try {
    clearCurrentFileInfo();
  }
  catch (const FileSymbolsParserException&) {
    // error in the symbols list, clear the list box
    m_symbols.ResetContent();
  }
}

void CYLoaderView::OnSize(UINT nType, int cx, int cy) {
  //	__super::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
}

void CYLoaderView::OnEnChangeEditSymbolsListLocation() {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the __super::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  m_dirty = true;
}

void CYLoaderView::OnEnChangeEditSaveDir() {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the __super::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  m_dirty = true;
}

void CYLoaderView::OnEnChangeEditOutputFileName() {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the __super::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  m_dirty = true;
}

void CYLoaderView::OnBnClickedCheckUpdate() { m_dirty = true; }

void CYLoaderView::OnBnClickedCheckDivSplitAdjusted() { m_dirty = true; }

void CYLoaderView::OnBnClickedCheckValidatePrices() { m_dirty = true; }

void CYLoaderView::OnCbnSelchangeComboPeriod() { m_dirty = true; }

void CYLoaderView::OnDtnDatetimechangeDatetimepickerPeriodStart(NMHDR* pNMHDR, LRESULT* pResult) {
  LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
  m_dirty = true;
  *pResult = 0;
}

void CYLoaderView::OnDtnDatetimechangeDatetimepickerPeriodEnd(NMHDR* pNMHDR, LRESULT* pResult) {
  LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
  m_dirty = true;
  *pResult = 0;
}

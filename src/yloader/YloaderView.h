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

#include <Downloading.h>
#include <YahooEventSink.h>
#include "ATLComTime.h"
#include "ColorStaticST.h"
#include "DownloadListCtrl.h"
#include "DropEdit.h"
#include "HListBox.h"
#include "YSettingsDialog.h"
#include "downloader.h"
#include "yloaderDoc.h"
#include "ylog.h"

enum { WM_UPDATE_DATA = WM_USER + 1000, WM_START_SESSION, WM_SHOW_SETTINGS };

class DateCtrl : public CDateTimeCtrl {
  mutable bool m_bChanged;

 public:
  DateCtrl() : m_bChanged(false), CDateTimeCtrl() {}

  void init() {
  }

  void setChanged(bool changed) { m_bChanged = changed; }
  bool changed() const { return m_bChanged; }

  DECLARE_MESSAGE_MAP()
  afx_msg void OnDtnDatetimechange(NMHDR* pNMHDR, LRESULT* pResult);
};

class CIconButton : public CButton {
 private:
  HICON m_hIcon;

 public:
  CIconButton(int iconId) {
    m_hIcon = (HICON)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(iconId), IMAGE_ICON, 16, 16, LR_DEFAULTSIZE);
  }
  ~CIconButton() { DestroyIcon(m_hIcon); }

  void init() { SetIcon(m_hIcon); }
};

class Frame {
 public:
  virtual ~Frame() {}
  virtual void closeFrame() = 0;
};

class CYLoaderView : public CFormView, public Refreshable {
  OBJ_COUNTER(CYLoaderView)
  class AutoBool {
   private:
    bool& m_value;
    const bool _newValue;

   public:
    AutoBool(bool& value, bool newValue) : m_value(value), _newValue(newValue) {}

    ~AutoBool() { m_value = _newValue; }
  };

  bool m_downloadSyncEvent;
  bool m_hasHadSession;

 public:
  void startSession() {
    UpdateData(FALSE);
    PostMessage(WM_START_SESSION);
  }

  void processAutoStart() {
    if (getAutoStartDownloading()) {
      startSession();
    }
  }

 protected:  // create from serialization only
  CYLoaderView();
  DECLARE_DYNCREATE(CYLoaderView)

 public:
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_DIALOG_YAHOO_HIST_DATA };
#endif

  // Attributes
 public:
  CYLoaderDoc* GetDocument() const;

  // Operations
 public:
  // Overrides
 public:
  BOOL PreCreateWindow(CREATESTRUCT& cs) override;

 protected:
  void DoDataExchange(CDataExchange* pDX) override;  // DDX/DDV support
  void OnInitialUpdate() override;  // called first time after construct

  // Implementation
 public:
  ~CYLoaderView() override;
#ifdef _DEBUG
  void AssertValid() const override;
  void Dump(CDumpContext& dc) const override;
#endif

 protected:
  // Generated message map functions
 protected:
  afx_msg void OnFilePrintPreview();
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  DECLARE_MESSAGE_MAP()
 public:
  CHListBox m_symbols;
  CButton m_symbolsFileButton;
  CIconButton m_editSymbolsList;
  CDropEdit m_symbolsListFileEdit;
  CButton m_dataPathButton;
  CButton m_adjustmentButton;
  CIconButton m_exploreOutputFolder;
  CDropEdit m_dataPathEdit;
  CButton m_dataFileButton;
  CIconButton m_openOneOutputFile;
  CDropEdit m_saveFileAllEdit;
  CString m_symbolsFileName;
  CString m_saveDir;
  CString m_saveFileAll;
  BOOL m_all;
  CButton m_updateCheck;
  Period m_period;
  DateCtrl m_startDateCtrl;
  DateCtrl m_endDateCtrl;
  CColorStaticST m_dataSourceCtrl;
  CString m_totalSymbolsValue;
  CString m_totalProcessedValue;
  CString m_totalProcessedPctValue;
  CString m_processedOKValue;
  CString m_processedOkPctValue;
  CString m_processedWithErrorsValue;
  CString m_errorsPctValue;
  CString m_ignoredValue;
  CString m_ignoredPctValue;
  CString m_durationValue;
  CProgressCtrl m_progress;
  CYahooEventList m_events;
  BOOL m_Adjustment;
  BOOL m_validatePrices;

  Statistics m_statistics;

  bool m_dirty;

  BOOL m_autoExit;

  bool m_closing;

  std::shared_ptr<YahooSessionLog> m_log;

  unsigned long m_regexFlags;

  std::wstring m_defSymbolsFileName;
  std::wstring m_defDataPath;

  bool m_logFileError;
  StartMode m_startMode;
  bool m_dialog;

  CColorStaticST m_sessionStatusCtrl;

  bool isRunning() { return m_downloading && m_downloading->isRunning(); }

  YPlugins m_plugins;
  yloader::UniqueIdPtr m_selectedDataSourcePlugin;
  bool m_help;
  bool m_addSymbolToColumn;
  unsigned int m_columnNumber;
  std::wstring m_prependToFileName;
  std::wstring m_appendToFileName;
  bool m_checkForUpdatesAtStartup;
  std::wstring m_fileNameExtension;
  std::wstring m_logFile;
  UINT m_threads;
  bool m_appendToLog;
  bool m_padDate;
  int m_dateFormat;
  std::wstring m_fieldSeparator;
  BOOL m_mostRecentBarsLastBool;
  BOOL m_autoStartDownloading;
  unsigned int m_symbolTimeout;
  bool m_logOnlyErrors;
  bool m_createSubdirs;
  int m_invalidDataHandling;
  bool m_volume0invalid;
  bool m_hideNews;
  bool m_autoSelectOutputPath;
  bool m_diagnosticLogging;
  std::wstring m_autoSelectOutputPathName;
  CharactersMapping m_characterMapping;
  double m_volumeMultiplier;
  std::wstring m_fileHeader;
  std::wstring m_dateSeparator;
  std::wstring m_proxyServerAddress;
  std::wstring m_proxyServerUserName;
  std::wstring m_proxyServerPassword;
  unsigned int m_httpRequestTimeout;
  std::wstring m_openDataFileWithApp;
  bool m_dontReloadOldDataInUpdateMode;
  bool m_minimizeToTray;
  bool showInitialMB = true;  // TODO implement correctly
  bool m_enableRegexFormatting;
  bool m_matchRegexUnlocked;
  boost::wregex m_matchRegex;
  std::wstring m_formatString;
  std::wstring m_errorSymbolsList;
  bool m_appendToErrorSymbolsList;
  bool m_ignoreErrorSymbolsList;
  std::wstring m_ignoreSymbolsList;
  std::wstring m_notifyOnlyIfNewerVersion;
  unsigned int m_fixedDecimalsCount;
  std::wstring m_decimalSeparator;

  WriteFileSymbolsListPtr m_errorSymbolsListFile;

  bool m_showSettings;
  bool m_notStarted;

  Frame* frame;

  void setFrame(Frame* frame) { this->frame = frame; }

  YDataSourcePluginPtr getDataSourcePlugin() {
    return m_selectedDataSourcePlugin ? m_plugins.getDataSourcePlugin(m_selectedDataSourcePlugin) : YDataSourcePluginPtr();
  }

  bool setSymbolsListFile() {
    CFileDialog dlg(true, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Text files (*.txt)|*.txt|CSV files (*.csv)|*.cxv|All Files (*.*)|*.*||");

    if (dlg.DoModal() == IDOK) {
      m_symbolsFileName = dlg.GetPathName();
      // m_symbolsListFileButton.add((LPCTSTR)_symbolsFileName);
      UpdateData(FALSE);
      initSymbolsList();
      m_dirty = true;
      return true;
    }
    else {
      return false;
    }
  }

  void setDataSourceText() {
    YDataSourcePluginPtr plugin = getDataSourcePlugin();

    if (plugin) {
      m_dataSourceCtrl.SetWindowText(plugin->shortName().c_str());

      m_dataSourceCtrl.SetTextColor(0x0000ff);
    }
    else {
      m_dataSourceCtrl.SetWindowText(L"[None]");
      m_dataSourceCtrl.SetTextColor(0xff0000);
    }
  }

  void setStatistics(const Statistics& statistics) {
    m_totalSymbolsValue = statistics.totalCountAsString().c_str();
    m_totalProcessedValue = statistics.totalProcessedCountAsString().c_str();
    m_processedOKValue = statistics.processedOkCountAsString().c_str();
    m_processedWithErrorsValue = statistics.processedWithErrorsCountAsString().c_str();
    m_durationValue = statistics.durationAsString().c_str();
    m_ignoredValue = statistics.ignoredAsString().c_str();

    m_totalConnectionsValue = std::to_wstring(m_threads).c_str();

    m_totalProcessedPctValue = statistics.totalProcessedCountPctAsString().c_str();
    m_processedOkPctValue = statistics.processedOkCountPctAsString().c_str();
    m_errorsPctValue = statistics.processedWithErrorsCountPctAsString().c_str();
    m_ignoredPctValue = statistics.ignoredPctAsString().c_str();

    UpdateData(false);
  }

  virtual void refresh();

 public:
  void DeleteAllEvents() { m_events.DeleteAllItems(); }

  void OnCancelDownload() {
    if (m_downloading && m_downloading->isRunning()) {
      m_downloading->cancel();
    }
  }

  Frame* getFrame() { return frame; }

  void addSinks(YahooEventDelegatorPtr sink) {
    sink->addSink(&m_events);
    sink->addSink(&m_statistics);
  }

  void startStatistics(size_t count) { m_statistics.start(count); }

  bool getAdjustment() { return m_Adjustment != FALSE; }

  void setAutoStartDownloading(bool autoStartDownloading) {
    m_autoStartDownloading = autoStartDownloading;
  }

  void setHelp(bool help) {
    m_help = help;
  }
  bool getHelp() const {
    return m_help;
  }

  void setAutoExit(bool autoExit) { m_autoExit = autoExit; }

  bool getAutoExit() const { return m_autoExit; }

  bool getAutoStartDownloading() const { return m_autoStartDownloading; }

  void setAddSymbolToColumn(bool add, unsigned int columnNumber) {
    m_addSymbolToColumn = add;
    m_columnNumber = columnNumber;
  }

  void setSortBarsAscending(bool sortBarsAscending) {
    m_mostRecentBarsLastBool = sortBarsAscending;
  }

  void setPadDateFields(bool pad) { m_padDate = pad; }

  void setInvalidDataHandling(unsigned int handling) {
    m_invalidDataHandling = handling;
  }

  void setVolume0Handling(bool invalid) { m_volume0invalid = invalid; }

  void setPrependToDataFileName(const std::wstring& prepend) {
    m_prependToFileName = prepend;
  }

  void setAppendToDataFileName(const std::wstring& append) {
    m_appendToFileName = append;
  }

  void setExtension(const std::wstring& extension) {
    m_fileNameExtension = extension;
  }

  bool checkForUpdatesAtStartup() const {
    return m_checkForUpdatesAtStartup && !m_autoExit;
  }

  void setCheckForUpdatesAtStartup(bool checkForUpdatesAtStartup) {
    m_checkForUpdatesAtStartup = checkForUpdatesAtStartup;
  }

  void setConnections(unsigned int connections) { m_threads = connections; }

  void setOutputFile(const std::wstring& outputFile) {
    m_saveFileAll = outputFile.c_str();
  }

  void setDateFormat(unsigned int format) { m_dateFormat = format; }

  void setAdjust(bool adjust) { m_Adjustment = adjust; }

  void setPeriod(unsigned int period) { m_period = (Period)period; }

  void setAllAvailable(bool allAvailable) { m_all = allAvailable; }

  void setDataSourcePlugin(const std::wstring& plugin) {
    YDataSourcePluginVectorPtr matchingDSPlugins = m_plugins.getMatchingDataSourcePlugins(plugin);
    if (matchingDSPlugins->size() > 1) {
      throw DownloaderException(L"More than one data source plugin match: "s + plugin);
    }
    else if (matchingDSPlugins->size() == 0) {
      throw DownloaderException(L"No data source plugin matches: "s + plugin);
    }
    else {
      m_selectedDataSourcePlugin = std::make_shared< yloader::UniqueId >((*matchingDSPlugins)[0]->id());
    }
  }

  void setNoReloadOldData(bool noReloadOldData) {
    m_dontReloadOldDataInUpdateMode = noReloadOldData;
  }

  void setCreateSubdirs(bool createSubdirs) { m_createSubdirs = createSubdirs; }

  void setFieldSeparator(const std::wstring& fieldSeparator) {
    m_fieldSeparator = fieldSeparator;
  }

  void setVolumeMultiplier(const double volumeMultiplier) {
    m_volumeMultiplier = volumeMultiplier;
  }

  void setStartDate(const std::wstring& date) {
    static const COleDateTime START(1970, 1, 2, 0, 0, 0);

    if (!date.empty()) {
      try {
        yloader::Date s(date);
        m_startDate = COleDateTime(s.year(), s.month(), s.day(), 0, 0, 0);
      }
      catch (const DateException&) {
        m_startDate = START;
      }
    }
    else {
      m_startDate = START;
    }
  }

  void setEndDate(const std::wstring& date) {
    if (!date.empty()) {
      yloader::Date s(date);
      m_endDate = COleDateTime(s.year(), s.month(), s.day(), 0, 0, 0);
    }
    else {
      m_endDate.SetStatus(COleDateTime::DateTimeStatus::null);
    }
  }

  void setDateSeparator(const std::wstring& separator) {
    m_dateSeparator = separator;
  }

  void setDataFileHeader(const std::wstring& header) { m_fileHeader = header; }

  void setErrorSymbolsListFileName(const std::wstring& fileName) {
    m_errorSymbolsList = fileName;
  }

  void setSymbolsFileName(const std::wstring& fileName) {
    m_symbolsFileName = fileName.c_str();
  }

  void setAppendToLog(bool appendToLog) { m_appendToLog = appendToLog; }

  void setLogOnlyErrors(bool logOnlyErrors) { m_logOnlyErrors = logOnlyErrors; }

  void setLogFile(const std::wstring& logFile) { m_logFile = logFile; }

  CString getSymbolsListFileName() { return m_symbolsFileName; }

  void setRegexMatch(const std::wstring& matchRegex) {
    m_matchRegex = matchRegex;
  }

  void setRegexFormat(const std::wstring& formatString) {
    m_formatString = formatString;
  }

  void setRegexFlags(unsigned long regexFlags) { m_regexFlags = regexFlags; }

  void setEnableRegexFormatting(bool enable) {
    m_enableRegexFormatting = enable;
  }

  void setSaveDir(const std::wstring& dir) { m_saveDir = dir.c_str(); }

  void setAppendToErrorSymbolsList(bool append) {
    m_appendToErrorSymbolsList = append;
  }

  void setIgnoreErrorSymbolsList(bool ignore) {
    m_ignoreErrorSymbolsList = ignore;
  }

  void setIgnoreSymbolsList(const std::wstring& ignore) {
    m_ignoreSymbolsList = ignore;
  }

  void setCharacterMapping(const std::vector<std::string>& map) {
    m_characterMapping = map;
  }

  void setFixedDecimalsCount(unsigned int count) {
    m_fixedDecimalsCount = count;
  }
  void setDecimalSeparator(const std::wstring& decimalSeparator) {
    m_decimalSeparator = decimalSeparator;
  }

  void setAdjustment(bool a) { m_Adjustment = a ? TRUE : FALSE; }

  void setValidatePrices(bool v) { m_validatePrices = v; }

  void setUpdate(bool u) { m_update = u; }

  void setSaveFileAll(const std::wstring& sfa) { m_saveFileAll = sfa.c_str(); }

  CString getSaveFileAll() { return m_saveFileAll; }

  bool hasErrorSymbolsListFile() { return !m_errorSymbolsList.empty(); }

  DataParamsPtr getDataParams() {
    return DataParamsPtr(new DataParams(
        m_errorSymbolsList, m_appendToErrorSymbolsList, m_ignoreErrorSymbolsList,
        m_ignoreSymbolsList,
        m_plugins.getDataSourcePlugin(m_selectedDataSourcePlugin),
        std::wstring(getSaveDir()), getPeriod(), getAdjustment(), getAll(),
        getUpdate(), getStartDate(), getEndDate(), getValidatePrices() != 0,
        m_mostRecentBarsLastBool != 0, (DateFormat)m_dateFormat,
        m_addSymbolToColumn, m_columnNumber, m_prependToFileName,
        m_appendToFileName, m_padDate, m_symbolTimeout, m_createSubdirs,
        m_invalidDataHandling, m_volume0invalid, m_threads, m_autoSelectOutputPath,
        m_autoSelectOutputPathName, m_characterMapping, m_fieldSeparator,
        m_volumeMultiplier, m_fileHeader, m_dateSeparator,
        std::wstring(getSaveFileAll()), std::wstring(getSymbolsFileName()),
        m_fileNameExtension, m_dontReloadOldDataInUpdateMode, m_logFile,
        m_appendToLog, m_logOnlyErrors, m_fixedDecimalsCount, m_decimalSeparator,
        m_enableRegexFormatting, m_matchRegex, m_formatString, m_regexFlags));
  }

  void LoadParams(const std::wstring& defSymbolsFileName,
                  const std::wstring& dataPath);
  void SaveParams(bool force = false, bool updateData = true);
  void setDefaults();
  void enableControls(bool enable) {
    m_symbolsFileButton.EnableWindow(enable);
    m_dataPathButton.EnableWindow(enable);
    m_symbolsListFileEdit.EnableWindow(enable);
    m_dataPathEdit.EnableWindow(enable);
    m_saveFileAllEdit.EnableWindow(enable);
    m_dataFileButton.EnableWindow(enable);
    //      m_refreshSymbolsButton.EnableWindow( enable );
    m_adjustmentButton.EnableWindow(enable);
    m_validatePricesButton.EnableWindow(enable);
    //      m_otherOptionsCtrl.EnableWindow( enable );
    m_allAvailableButton.EnableWindow(enable);
    m_updateCheck.EnableWindow(enable);
    m_periodCombo.EnableWindow(enable);
    m_editSymbolsList.EnableWindow(enable);
    m_exploreOutputFolder.EnableWindow(enable);
    m_openOneOutputFile.EnableWindow(enable);
    enableTimeRange(enable);
    enableUpdate(enable);
  }

  bool isSessionActive() const {
    return m_downloading && m_downloading->isRunning();
  }

  std::shared_ptr<CDownloading> m_downloading;

  void OnSettings();
  void CYLoaderView::OnDownload();

  std::wstring getYLoaderSettingsFile() {
    return yloader::addBackSlash(yloader::getDirectory(getLocalYLoaderDataPath(), L"settings")) + L"settings.ini";
  }

  std::wstring GetProfileString(const std::wstring& name, const std::wstring& defValue = L"") {
    assert(!name.empty());
    std::wstring settingsFile(getYLoaderSettingsFile());

    if (yloader::fileExists(settingsFile)) {
      TCHAR value[10000] = {0};
      GetPrivateProfileString(L"Settings", name.c_str(), defValue.c_str(), value, 10000, settingsFile.c_str());
      return value;
    }
    else {
      return AfxGetApp()->GetProfileString(L"YahooData", name.c_str(), defValue.c_str()).GetString();
    }
  }

  int GetProfileInt(const std::wstring& name, int defValue = 0) {
    assert(!name.empty());
    std::wstring settingsFile(getYLoaderSettingsFile());

    return yloader::fileExists(settingsFile)
               ? GetPrivateProfileInt(L"Settings", name.c_str(), defValue, settingsFile.c_str())
               : AfxGetApp()->GetProfileInt(L"YahooData", name.c_str(), defValue);
  }

  bool GetProfileBool(const std::wstring& name, bool defValue = false) {
    return GetProfileInt(name, defValue ? 1 : 0) != 0;
  }

  void WriteProfileInt(const std::wstring& name, int value) {
    assert(!name.empty());
    std::wstring settingsFile(getYLoaderSettingsFile());

    WritePrivateProfileString(L"Settings", name.c_str(), (boost::wformat(L"%1%") % value).str().c_str(), settingsFile.c_str());
  }

  void WriteProfileString(const std::wstring& name, const CString& value) {
    WriteProfileString(name, std::wstring((LPCTSTR)value));
  }

  void WriteProfileString(const std::wstring& name, const std::wstring& value) {
    assert(!name.empty());
    std::wstring settingsFile(getYLoaderSettingsFile());

    WritePrivateProfileString(L"Settings", name.c_str(), value.c_str(), settingsFile.c_str());
  }

  yloader::Version getNotifyOnlyIfNewerVersion() const {
    return yloader::Version(m_notifyOnlyIfNewerVersion);
  }

  void setNotifyOnlyIfNewerVersion(const Version& version) {
    m_notifyOnlyIfNewerVersion = version.toString();
  }

  std::wstring getStartDateAsString() {
    yloader::Date date(m_startDate.GetYear(), m_startDate.GetMonth(), m_startDate.GetDay());
    return date.toString(DateFormat::us);
  }

  bool getAll() { return m_all != FALSE; }

  void enableTimeRange(bool b = true) {
    m_startDateCtrl.EnableWindow(!m_all && b);
    m_endDateCtrl.EnableWindow(!m_all && b);
  }

  bool dateRangeChanged() {
    return m_endDateCtrl.changed() || m_startDateCtrl.changed();
  }

  CString getSaveDir() { return m_saveDir; }

  Period getPeriod() { return m_period; }

  bool getUpdate() { return m_update != FALSE; }

  bool getValidatePrices() { return m_validatePrices != FALSE; }

  CString getSymbolsFileName() { return m_symbolsFileName; }

  void enableUpdate(bool b = true) {
    if (b) {
      if (!m_mostRecentBarsLastBool) {
        m_updateCheck.SetCheck(false);
        m_update = false;
        m_updateCheck.EnableWindow(false);
      }
      else {
        m_updateCheck.EnableWindow();
      }
    }
    else {
      m_updateCheck.EnableWindow(false);
    }
  }

  COleDateTime getStartDate() { return m_startDate; }

  COleDateTime getEndDate() { return m_endDate; }

  void setProgressRange(size_t size) { m_progress.SetRange32(0, boost::numeric_cast< int >( size ) ); }

  CButton m_validatePricesButton;
  CButton m_allAvailableButton;
  CComboBox m_periodCombo;
  BOOL m_update;
  COleDateTime m_startDate;
  COleDateTime m_endDate;
  CString m_totalConnectionsValue;
  afx_msg void OnDestroy();
  afx_msg void OnClose();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg LRESULT DoDownload(WPARAM, LPARAM);

  void CYLoaderView::openDataFile(boost::shared_array<TCHAR> symbol);

  void initSymbolsList() {
    try {
      if (m_symbolsFileName != m_mostRecentSymbolsFile.c_str() ||
          getFileLastWriteTime(m_symbolsFileName.GetString()) != m_mostRecentSymbolsFileDateTime) {
        StrListPtr symbolsList(yloader::getSymbols((LPCTSTR)m_symbolsFileName, true));

        assert(symbolsList.use_count() == 1);

        m_symbols.ResetContent();
        for (const auto& symbol : *symbolsList) {
          m_symbols.InsertString(-1, symbol.c_str());
        }

        m_mostRecentSymbolsFile = m_symbolsFileName;
        m_mostRecentSymbolsFileDateTime = getFileLastWriteTime(std::wstring((LPCTSTR)m_symbolsFileName));
      }
    }
    catch (const FileException&) {
      m_symbols.ResetContent();
      clearCurrentFileInfo();
    }
    catch (...) {
      m_symbols.ResetContent();
      clearCurrentFileInfo();
    }
  }

  std::wstring m_mostRecentSymbolsFile;
  DateTime m_mostRecentSymbolsFileDateTime;

  void clearCurrentFileInfo() {
    m_mostRecentSymbolsFile.clear();
    m_mostRecentSymbolsFileDateTime = DateTime();
  }

  afx_msg void OnClickedCheckPeriodAll();
  afx_msg void OnOpenSymbols();
  afx_msg void OnUpdateOpenSymbols(CCmdUI* pCmdUI);
  afx_msg void OnClickedSymbolsList();
  afx_msg void OnBnClickedButtonSaveDir();
  afx_msg void OnBnClickedButtonSaveFileAll();
  afx_msg void OnBnClickedEditSymbolsList();
  afx_msg void OnBnClickedExploreOutputFolder();
  afx_msg void OnNMDblclkListEvents(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnEnChangeEditSymbolsListLocation();
  afx_msg void OnEnChangeEditSaveDir();
  afx_msg void OnEnChangeEditOutputFileName();
  afx_msg void OnBnClickedCheckUpdate();
  afx_msg void OnBnClickedCheckDivSplitAdjusted();
  afx_msg void OnBnClickedCheckValidatePrices();
  afx_msg void OnCbnSelchangeComboPeriod();
  afx_msg void OnDtnDatetimechangeDatetimepickerPeriodStart(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnDtnDatetimechangeDatetimepickerPeriodEnd(NMHDR* pNMHDR, LRESULT* pResult);
};

#ifndef _DEBUG  // debug version in YloaderView.cpp
inline CYLoaderDoc* CYLoaderView::GetDocument() const {
  return reinterpret_cast<CYLoaderDoc*>(m_pDocument);
}
#endif

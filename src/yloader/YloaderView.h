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
    //		SetMode(CExtDateTimeWnd::eMode_t::date);
  }

  /*	virtual bool OnValueChanged(const COleDateTime & dtDateTimeOld, const
     COleDateTime & dtDateTimeNew) const
          {
                  m_bChanged = true;
                  return true;
          }
          */

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
    m_hIcon =
        (HICON)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(iconId),
                           IMAGE_ICON, 16, 16, LR_DEFAULTSIZE);
  }
  ~CIconButton() { DestroyIcon(m_hIcon); }

  void init() { SetIcon(m_hIcon); }
};

class Frame {
 public:
  virtual ~Frame() {}
  virtual void closeFrame() = 0;
};

class CYloaderView : public CFormView, public Refreshable {
  class AutoBool {
   private:
    bool& _value;
    const bool _newValue;

   public:
    AutoBool(bool& value, bool newValue) : _value(value), _newValue(newValue) {}

    ~AutoBool() { _value = _newValue; }
  };

  bool downloadSyncEvent;
  bool _hasHadSession;

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
  CYloaderView();
  DECLARE_DYNCREATE(CYloaderView)

 public:
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_DIALOG_YAHOO_HIST_DATA };
#endif

  // Attributes
 public:
  CyloaderDoc* GetDocument() const;

  // Operations
 public:
  // Overrides
 public:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
  virtual void OnInitialUpdate();  // called first time after construct

  // Implementation
 public:
  virtual ~CYloaderView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
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

  Statistics _statistics;

  bool _dirty;

  BOOL _autoExit;

  bool m_closing;

  ManagedPtr<YahooSessionLog> _log;

  unsigned long _regexFlags;

  std::wstring _defSymbolsFileName;
  std::wstring _defDataPath;

  bool _logFileError;
  StartMode _startMode;
  bool _dialog;

  CColorStaticST m_sessionStatusCtrl;

  bool isRunning() { return _downloading && _downloading->isRunning(); }

  YPlugins _plugins;
  yloader::UniqueIdPtr _selectedDataSourcePlugin;
  bool _addSymbolToColumn;
  unsigned int _columnNumber;
  std::wstring _prependToFileName;
  std::wstring _appendToFileName;
  bool _checkForUpdatesAtStartup;
  std::wstring _fileNameExtension;
  std::wstring _logFile;
  UINT _threads;
  bool _appendToLog;
  bool _padDate;
  int m_dateFormat;
  std::wstring m_fieldSeparator;
  BOOL m_mostRecentBarsLastBool;
  BOOL m_autoStartDownloading;
  unsigned int _symbolTimeout;
  bool _logOnlyErrors;
  bool _createSubdirs;
  int _invalidDataHandling;
  bool _volume0invalid;
  bool _hideNews;
  bool _autoSelectOutputPath;
  bool _diagnosticLogging;
  std::wstring _autoSelectOutputPathName;
  CharactersMapping _characterMapping;
  double _volumeMultiplier;
  std::wstring _fileHeader;
  std::wstring _dateSeparator;
  std::wstring _proxyServerAddress;
  std::wstring _proxyServerUserName;
  std::wstring _proxyServerPassword;
  unsigned int _httpRequestTimeout;
  std::wstring _openDataFileWithApp;
  bool _dontReloadOldDataInUpdateMode;
  bool _minimizeToTray;
  bool showInitialMB = true;  // TODO implement correctly
  bool _enableRegexFormatting;
  bool _matchRegexUnlocked;
  boost::wregex _matchRegex;
  std::wstring _formatString;
  std::wstring _errorSymbolsList;
  bool _appendToErrorSymbolsList;
  bool _ignoreErrorSymbolsList;
  std::wstring _ignoreSymbolsList;
  std::wstring _notifyOnlyIfNewerVersion;

  WriteFileSymbolsListPtr _errorSymbolsListFile;

  bool _showSettings;
  bool _notStarted;

  Frame* frame;

  void setFrame(Frame* frame) { this->frame = frame; }

  YDataSourcePluginPtr getDataSourcePlugin() {
    return _selectedDataSourcePlugin
               ? _plugins.getDataSourcePlugin(_selectedDataSourcePlugin)
               : YDataSourcePluginPtr();
  }

  bool setSymbolsListFile() {
    CFileDialog dlg(true, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                    _T("Text files (*.txt)|*.txt|CSV files (*.csv)|*.cxv|All ")
                    _T("Files (*.*)|*.*||"));

    if (dlg.DoModal() == IDOK) {
      m_symbolsFileName = dlg.GetPathName();
      // m_symbolsListFileButton.add((LPCTSTR)_symbolsFileName);
      UpdateData(FALSE);
      initSymbolsList();
      _dirty = true;
      return true;
    } else
      return false;
  }

  void setDataSourceText() {
    YDataSourcePluginPtr plugin = getDataSourcePlugin();

    if (plugin) {
      m_dataSourceCtrl.SetWindowText(plugin->shortName().c_str());

      m_dataSourceCtrl.SetTextColor(0x0000ff);
      //			m_dataSourceCtrl.SetFontBold(true);
    } else {
      m_dataSourceCtrl.SetWindowText(_T( "[None]" ));
      m_dataSourceCtrl.SetTextColor(0xff0000);
      //			m_dataSourceCtrl.SetFontBold(true);
    }
  }

  void setStatistics(const Statistics& statistics) {
    m_totalSymbolsValue = statistics.totalCountAsString().c_str();
    m_totalProcessedValue = statistics.totalProcessedCountAsString().c_str();
    m_processedOKValue = statistics.processedOkCountAsString().c_str();
    m_processedWithErrorsValue =
        statistics.processedWithErrorsCountAsString().c_str();
    m_durationValue = statistics.durationAsString().c_str();
    m_ignoredValue = statistics.ignoredAsString().c_str();

    std::wstring connections;
    connections << _threads;
    m_totalConnectionsValue = connections.c_str();

    m_totalProcessedPctValue =
        statistics.totalProcessedCountPctAsString().c_str();
    m_processedOkPctValue = statistics.processedOkCountPctAsString().c_str();
    m_errorsPctValue = statistics.processedWithErrorsCountPctAsString().c_str();
    m_ignoredPctValue = statistics.ignoredPctAsString().c_str();

    UpdateData(false);
  }

  virtual void refresh();

 public:
  void DeleteAllEvents() { m_events.DeleteAllItems(); }

  void OnCancelDownload() {
    if (_downloading && _downloading->isRunning()) {
      _downloading->cancel();
    }
  }

  /*
          void setTotalConnectionsValue(unsigned int connections)
          {
                  m_totalConnectionsValue = connections;
          }
          */

  Frame* getFrame() { return frame; }

  void addSinks(YahooEventDelegatorPtr sink) {
    sink->addSink(&m_events);
    sink->addSink(&_statistics);
  }

  void startStatistics(size_t count) { _statistics.start(count); }

  bool getAdjustment() { return m_Adjustment != FALSE; }

  void setAutoStartDownloading(bool autoStartDownloading) {
    m_autoStartDownloading = autoStartDownloading;
  }

  void setAutoExit(bool autoExit) { _autoExit = autoExit; }

  bool getAutoExit() const { return _autoExit; }

  bool getAutoStartDownloading() const { return m_autoStartDownloading; }

  void setAddSymbolToColumn(bool add, unsigned int columnNumber) {
    _addSymbolToColumn = add;
    _columnNumber = columnNumber;
  }

  void setSortBarsAscending(bool sortBarsAscending) {
    m_mostRecentBarsLastBool = sortBarsAscending;
  }

  void setStartDate(const CString& date) {
    setStartDate(std::wstring(date.GetString()));
  }

  void setPadDateFields(bool pad) { _padDate = pad; }

  void setInvalidDataHandling(unsigned int handling) {
    _invalidDataHandling = handling;
  }

  void setVolume0Handling(bool invalid) { _volume0invalid = invalid; }

  void setPrependToDataFileName(const std::wstring& prepend) {
    _prependToFileName = prepend;
  }

  void setAppendToDataFileName(const std::wstring& append) {
    _appendToFileName = append;
  }

  void setExtension(const std::wstring& extension) {
    _fileNameExtension = extension;
  }

  bool checkForUpdatesAtStartup() const {
    return _checkForUpdatesAtStartup && !_autoExit;
  }

  void setCheckForUpdatesAtStartup(bool checkForUpdatesAtStartup) {
    _checkForUpdatesAtStartup = checkForUpdatesAtStartup;
  }

  void setConnections(unsigned int connections) { _threads = connections; }

  void setOutputFile(const std::wstring& outputFile) {
    m_saveFileAll = outputFile.c_str();
  }

  void setDateFormat(unsigned int format) { m_dateFormat = format; }

  void setAdjust(bool adjust) { m_Adjustment = adjust; }

  void setPeriod(unsigned int period) { m_period = (Period)period; }

  void setAllAvailable(bool allAvailable) { m_all = allAvailable; }

  void setDataSourcePlugin(std::wstring& plugin) {
    YDataSourcePluginVectorPtr matchingDSPlugins =
        _plugins.getMatchingDataSourcePlugins(plugin);
    if (matchingDSPlugins->size() > 1)
      throw DownloaderException(
          std::wstring(_T("More than one data source plugin match: "))
          << plugin);
    else if (matchingDSPlugins->size() == 0)
      throw DownloaderException(
          std::wstring(_T("No data source plugin matches: ")) << plugin);

    else
      _selectedDataSourcePlugin =
          new yloader::UniqueId((*matchingDSPlugins)[0]->id());
  }

  void setNoReloadOldData(bool noReloadOldData) {
    _dontReloadOldDataInUpdateMode = noReloadOldData;
  }

  void setCreateSubdirs(bool createSubdirs) { _createSubdirs = createSubdirs; }

  void setFieldSeparator(const std::wstring& fieldSeparator) {
    m_fieldSeparator = fieldSeparator;
  }

  void setVolumeMultiplier(const double volumeMultiplier) {
    _volumeMultiplier = volumeMultiplier;
  }

  void setStartDate(const std::wstring& date) {
    static const COleDateTime START(1970, 1, 2, 0, 0, 0);

    if (!date.empty()) {
      try {
        yloader::Date s(date);
        m_startDate = COleDateTime(s.year(), s.month(), s.day(), 0, 0, 0);
      } catch (const DateException& e) {
        m_startDate = START;
      }
    } else {
      m_startDate = START;
    }
  }

  void setEndDate(const CString& date) {
    setEndDate(std::wstring(date.GetString()));
  }

  void setEndDate(const std::wstring& date) {
    if (!date.empty()) {
      yloader::Date s(date);
      m_endDate = COleDateTime(s.year(), s.month(), s.day(), 0, 0, 0);
    } else {
      m_endDate.SetStatus(COleDateTime::DateTimeStatus::null);
    }
  }

  void setDateSeparator(const std::wstring& separator) {
    _dateSeparator = separator;
  }

  void setDataFileHeader(const std::wstring& header) { _fileHeader = header; }

  void setErrorSymbolsListFileName(const std::wstring& fileName) {
    _errorSymbolsList = fileName;
  }

  void setSymbolsFileName(const std::wstring& fileName) {
    m_symbolsFileName = fileName.c_str();
  }

  void setAppendToLog(bool appendToLog) { _appendToLog = appendToLog; }

  void setLogOnlyErrors(bool logOnlyErrors) { _logOnlyErrors = logOnlyErrors; }

  void setLogFile(const std::wstring& logFile) { _logFile = logFile; }

  CString getSymbolsListFileName() { return m_symbolsFileName; }

  void setRegexMatch(const std::wstring& matchRegex) {
    _matchRegex = matchRegex;
  }

  void setRegexFormat(const std::wstring& formatString) {
    _formatString = formatString;
  }

  void setRegexFlags(unsigned long regexFlags) { _regexFlags = regexFlags; }

  void setEnableRegexFormatting(bool enable) {
    _enableRegexFormatting = enable;
  }

  void setSaveDir(const std::wstring& dir) { m_saveDir = dir.c_str(); }

  void setPeriod(Period p) { m_period = p; }

  void setAppendToErrorSymbolsList(bool append) {
    _appendToErrorSymbolsList = append;
  }

  void setIgnoreErrorSymbolsList(bool ignore) {
    _ignoreErrorSymbolsList = ignore;
  }

  void setIgnoreSymbolsList(const std::wstring& ignore) {
    _ignoreSymbolsList = ignore;
  }

  void setCharacterMapping(const std::vector<std::string>& map) {
    _characterMapping = map;
  }

  void setAdjustment(bool a) { m_Adjustment = a ? TRUE : FALSE; }

  void setValidatePrices(bool v) { m_validatePrices = v; }

  void setUpdate(bool u) { m_update = u; }

  void setSaveFileAll(const std::wstring& sfa) { m_saveFileAll = sfa.c_str(); }

  CString getSaveFileAll() { return m_saveFileAll; }

  bool hasErrorSymbolsListFile() { return !_errorSymbolsList.empty(); }

  DataParamsPtr getDataParams() {
    return DataParamsPtr(new DataParams(
        _errorSymbolsList, _appendToErrorSymbolsList, _ignoreErrorSymbolsList,
        _ignoreSymbolsList,
        _plugins.getDataSourcePlugin(_selectedDataSourcePlugin),
        std::wstring(getSaveDir()), getPeriod(), getAdjustment(), getAll(),
        getUpdate(), getStartDate(), getEndDate(), getValidatePrices() != 0,
        m_mostRecentBarsLastBool != 0, (DateFormat)m_dateFormat,
        _addSymbolToColumn, _columnNumber, _prependToFileName,
        _appendToFileName, _padDate, _symbolTimeout, _createSubdirs,
        _invalidDataHandling, _volume0invalid, _threads, _autoSelectOutputPath,
        _autoSelectOutputPathName, _characterMapping, m_fieldSeparator,
        _volumeMultiplier, _fileHeader, _dateSeparator,
        std::wstring(getSaveFileAll()), std::wstring(getSymbolsFileName()),
        _fileNameExtension, _dontReloadOldDataInUpdateMode, _logFile,
        _appendToLog, _logOnlyErrors, _enableRegexFormatting, _matchRegex,
        _formatString, _regexFlags));
  }

  void LoadParams(const std::wstring& defSymbolsFileName,
                  const std::wstring& dataPath);
  void SaveParams(bool force = false, bool updateData = true);
  void setDefaults();
  void enableControls(bool enable) {
    OutputDebugString((std::wstring(_T("EnableControls")) +
                       (enable ? _T( "true" ) : _T( "false")) + _T( "\n" ))
                          .c_str());
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
    return _downloading && _downloading->isRunning();
  }

  yloader::ManagedPtr<CDownloading> _downloading;

  void OnSettings();
  void CYloaderView::OnDownload();

  std::wstring getYLoaderSettingsFile() {
    return yloader::addBackSlash(yloader::getDirectory(
               getLocalYLoaderDataPath(), _T("settings"))) +
           _T("settings.ini");
  }

  std::wstring GetProfileString(const std::wstring& name,
                                const std::wstring& defValue = std::wstring()) {
    assert(!name.empty());
    std::wstring settingsFile(getYLoaderSettingsFile());

    if (yloader::fileExists(settingsFile)) {
      TCHAR value[10000] = {0};
      GetPrivateProfileString(_T("Settings"), name.c_str(), defValue.c_str(),
                              value, 10000, settingsFile.c_str());
      return value;
    } else {
      LPCTSTR def = defValue.c_str();
      LPCTSTR n = name.c_str();
      LPCTSTR key = _T("YahooData");
      return AfxGetApp()->GetProfileString(key, n, def);
    }
  }

  int GetProfileInt(const std::wstring& name, int defValue = 0) {
    assert(!name.empty());
    std::wstring settingsFile(getYLoaderSettingsFile());

    return yloader::fileExists(settingsFile)
               ? GetPrivateProfileInt(_T("Settings"), name.c_str(), defValue,
                                      settingsFile.c_str())
               : AfxGetApp()->GetProfileInt(_T("YahooData"), name.c_str(),
                                            defValue);
  }

  bool GetProfileBool(const std::wstring& name, bool defValue = false) {
    return GetProfileInt(name, defValue ? 1 : 0) != 0;
  }

  void WriteProfileInt(const std::wstring& name, int value) {
    assert(!name.empty());
    std::wstring settingsFile(getYLoaderSettingsFile());

    WritePrivateProfileString(_T("Settings"), name.c_str(),
                              (boost::wformat(_T("%1%")) % value).str().c_str(),
                              settingsFile.c_str());
  }

  void WriteProfileString(const std::wstring& name, const CString& value) {
    WriteProfileString(name, std::wstring((LPCTSTR)value));
  }

  void WriteProfileString(const std::wstring& name, const std::wstring& value) {
    assert(!name.empty());
    std::wstring settingsFile(getYLoaderSettingsFile());

    WritePrivateProfileString(_T("Settings"), name.c_str(), value.c_str(),
                              settingsFile.c_str());
  }

  yloader::Version getNotifyOnlyIfNewerVersion() const {
    return yloader::Version(_notifyOnlyIfNewerVersion);
  }

  void setNotifyOnlyIfNewerVersion(const Version& version) {
    _notifyOnlyIfNewerVersion = version.toString();
  }

  std::wstring getStartDateAsString() {
    yloader::Date date(m_startDate.GetYear(), m_startDate.GetMonth(),
                       m_startDate.GetDay());
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
      } else
        m_updateCheck.EnableWindow();
    } else
      m_updateCheck.EnableWindow(false);
  }

  COleDateTime getStartDate() { return m_startDate; }

  COleDateTime getEndDate() { return m_endDate; }

  void setProgressRange(size_t size) { m_progress.SetRange32(0, size); }

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

  void CYloaderView::openDataFile(boost::shared_array<TCHAR> symbol);

  void initSymbolsList() {
    try {
      if (m_symbolsFileName != _mostRecentSymbolsFile.c_str() ||
          getFileLastWriteTime(std::wstring((LPCTSTR)m_symbolsFileName)) !=
              _mostRecentSymbolsFileDateTime) {
        StrListPtr symbolsList(
            yloader::getSymbols((LPCTSTR)m_symbolsFileName, true));

        assert(symbolsList.count() == 1);

        m_symbols.ResetContent();
        for (StrList::const_iterator i = symbolsList->begin();
             i != symbolsList->end(); i++)
          m_symbols.InsertString(-1, i->c_str());

        _mostRecentSymbolsFile = m_symbolsFileName;
        _mostRecentSymbolsFileDateTime =
            getFileLastWriteTime(std::wstring((LPCTSTR)m_symbolsFileName));
      }
    } catch (const FileException&) {
      m_symbols.ResetContent();
      clearCurrentFileInfo();
    } catch (...) {
      m_symbols.ResetContent();
      clearCurrentFileInfo();
    }
  }

  std::wstring _mostRecentSymbolsFile;
  DateTime _mostRecentSymbolsFileDateTime;

  void clearCurrentFileInfo() {
    _mostRecentSymbolsFile.clear();
    _mostRecentSymbolsFileDateTime = DateTime();
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
  afx_msg void OnDtnDatetimechangeDatetimepickerPeriodStart(NMHDR* pNMHDR,
                                                            LRESULT* pResult);
  afx_msg void OnDtnDatetimechangeDatetimepickerPeriodEnd(NMHDR* pNMHDR,
                                                          LRESULT* pResult);
};

#ifndef _DEBUG  // debug version in YloaderView.cpp
inline CyloaderDoc* CYloaderView::GetDocument() const {
  return reinterpret_cast<CyloaderDoc*>(m_pDocument);
}
#endif

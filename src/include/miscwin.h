#pragma once

#include <assert.h>
#include <tchar.h>
#include <string>
#include <vector>

#include <exceptions.h>
#include <misc.h>

/* @cond */
namespace yloader {
/* @endcond */

#ifdef MISCWIN_EXPORTS
#define MISCWIN_API __declspec(dllexport)
#else
#define MISCWIN_API __declspec(dllimport)
#endif

/* @cond */
// registry related APIs

#ifdef _UNICODE
#if !defined(UNICODE)
#define UNICODE
#endif
#endif

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

using namespace std;

#include <shlobj.h>
#include <memory>

//////////////////////////////////////////////////////////////////////
//
// CShellBrowser
//

class MISCWIN_API CBrowseForFolder {
 public:
  CBrowseForFolder(const HWND hParent = NULL, const LPITEMIDLIST pidl = NULL,
                   const int nTitleID = 0);
  CBrowseForFolder(const HWND hParent, const LPITEMIDLIST pidl,
                   const CString& strTitle);
  virtual ~CBrowseForFolder() = 0;

  //
  // Set the handle of the owner window for the dialog box.
  //
  void SetOwner(const HWND hwndOwner);

  //
  // Set the root of the heirarchy that will be browsed.  Get pidl from
  // SHGetSpecialFolderLocation. This can be set to NULL to use the Virtual
  // Folder that represents the Windows Desktop.
  //
  void SetRoot(const LPITEMIDLIST pidl);

  //
  // Access a string that is displayed above the tree view control in the dialog
  // box. This string can be used to specify instructions to the user.  strTitle
  // is a CString containing the text to be displayed.  nTitle is the index of a
  // string resource to be loaded.  The return value is false if the resource
  // could not be loaded.
  //
  CString GetTitle() const;
  void SetTitle(const CString& strTitle);
  bool SetTitle(const int nTitle);

  //
  // ulFlags = Value specifying the types of folders to be listed in the dialog
  // box as well as
  //           other options. This member can include zero or more of the
  //           following values:
  //
  //          BIF_BROWSEFORCOMPUTER    Only returns computers. If the user
  //          selects anything
  //                                   other than a computer, the OK button is
  //                                   grayed.
  //
  //          BIF_BROWSEFORPRINTER     Only returns printers. If the user
  //          selects anything
  //                                   other than a printer, the OK button is
  //                                   grayed.
  //
  //          BIF_DONTGOBELOWDOMAIN    Does not include network folders below
  //          the domain level
  //                                   in the tree view control.
  //
  //          BIF_RETURNFSANCESTORS    Only returns file system ancestors. If
  //          the user selects
  //                                   anything other than a file system
  //                                   ancestor, the OK button is grayed.
  //
  //          BIF_RETURNONLYFSDIRS     Only returns file system directories. If
  //          the user
  //                                   selects folders that are not part of the
  //                                   file system, the OK button is grayed.
  //
  //          BIF_STATUSTEXT           Includes a status area in the dialog box.
  //          The callback
  //                                   function can set the status text by
  //                                   sending messages to the dialog box.
  //
  UINT GetFlags() const;
  void SetFlags(const UINT ulFlags);

  //
  // Call GetSelectedFolder to retrieve the folder selected by the user.
  //
  CString GetSelectedFolder() const;

  //
  // Function to retreive the image associated with the selected folder. The
  // image is specified as an index to the system image list.
  //
  int GetImage() const;

  //
  // Call SelectFolder to display the dialog and get a selection from the user.
  // Use GetSelectedFolder and GetImage to get the results of the dialog.
  //
  bool SelectFolder();

 protected:
  //
  // OnInit is called before the dialog is displayed on the screen.
  //
  virtual void OnInit() const;

  //
  // OnSelChanged is called whenever the user selects a different directory.
  // pidl is the LPITEMIDLIST of the new selection.  Use SHGetPathFromIDList to
  // retrieve the path of the selection.
  //
  virtual void OnSelChanged(const LPITEMIDLIST pidl) const;

  //
  // Call EnableOK to enable the OK button on the active dialog.  If bEnable is
  // true then the button is enabled, otherwise it is disabled. NOTE -- This
  // function should only be called within overrides of OnInit and OnSelChanged.
  //
  void EnableOK(const bool bEnable) const;

  //
  // Call SetSelection to set the selection in the active dialog.  pidl is the
  // LPITEMIDLIST of the path to be selected.  strPath is a CString containing
  // the path to be selected. NOTE -- This function should only be called within
  // overrides of OnInit and OnSelChanged.
  //
  void SetSelection(const LPITEMIDLIST pidl) const;
  void SetSelection(const CString& strPath) const;

  //
  // Call SetStatusText to set the text in the Status area in the active dialog.
  // strText is the text to be displayed. NOTE -- This function should only be
  // called within overrides of OnInit and OnSelChanged.
  //
  void SetStatusText(const CString& strText) const;

 private:
  static int __stdcall BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam,
                                          LPARAM lpData);

  typedef std::auto_ptr<TCHAR> AUTO_STR;
  AUTO_STR m_pchTitle;

  BROWSEINFO m_bi;
  TCHAR m_szSelected[MAX_PATH];
  CString m_strPath;
  HWND m_hwnd;
};

inline UINT CBrowseForFolder::GetFlags() const { return m_bi.ulFlags; }

inline int CBrowseForFolder::GetImage() const { return m_bi.iImage; }

class MISCWIN_API CSBDestination : public CBrowseForFolder {
 public:
  CSBDestination(const HWND hParent, const int nTitleID)
      : CBrowseForFolder(hParent, NULL, nTitleID) {}

  CSBDestination(const HWND hParent, const CString& title)
      : CBrowseForFolder(hParent, NULL, title) {}
  ~CSBDestination() {}

  void SetInitialSelection(const CString& strPath) {
    m_strInitialSelection = strPath;
  }

  void OnInit() const {
    SetSelection(m_strInitialSelection);
    SetStatusText(m_strInitialSelection);
  }

  void OnSelChanged(const LPITEMIDLIST pidl) const {
    CString strBuffer;
    if (SHGetPathFromIDList(pidl, strBuffer.GetBuffer(MAX_PATH)))
      strBuffer.ReleaseBuffer();
    else
      strBuffer.ReleaseBuffer(0);
    SetStatusText(strBuffer);
  }

 private:
  CString m_strInitialSelection;
};

#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE 0x0040
#endif

/* @endcond */

///////////////////////////////////
// makes a file name from its components. Takes into account existing or non
// existing \ or / or . characters
MISCWIN_API const std::wstring makeFileName(const std::wstring& path,
                                            const std::wstring& name,
                                            const std::wstring& ext);

class RunProcessException {
 private:
  DWORD _errorCode;
  const std::wstring _processFileName;
  const std::wstring _cmdLine;

 public:
  RunProcessException(const std::wstring& processFileName,
                      const std::wstring& cmdLine, DWORD errorCode)
      : _errorCode(errorCode) {}

  const DWORD errorCode() const { return _errorCode; }

  const std::wstring& processFileName() const { return _processFileName; }
  const std::wstring& cmdLine() const { return _cmdLine; }

  const std::wstring message() const {
    std::wostringstream o;

    o << _T("Attempting to run \"" ) << _processFileName
      << _T( "\" with command line " ) << _cmdLine
      << _T( " has failed with error " ) << _errorCode;
    return o.str();
  }
};

class RunProcessResult {
 private:
  bool _timeout;
  DWORD _exitCode;

 public:
  RunProcessResult(bool timeout, DWORD exitCode)
      : _timeout(timeout), _exitCode(exitCode) {}

  bool timeout() const { return _timeout; }
  DWORD exitCode() const { return _exitCode; }
};

typedef const std::vector<std::pair<std::wstring, std::wstring> > StrPairVector;

class Environment {
 private:
  mutable bool _valid;
  mutable std::wstring _envStr;

  StrVector _envStrings;

 public:
  Environment() {}

  Environment(const Environment& env) {
    _envStrings = env._envStrings;
    _valid = env._valid;
    _envStr = env._envStr;
  }

  Environment(const StrPairVector& env) : _valid(false) {
    for (StrPairVector::const_iterator i = env.begin(); i != env.end(); i++)
      add((*i).first, (*i).second);
  }

  Environment(const StrVector& env) : _valid(false) { _envStrings = env; }

  void add(LPCTSTR name, LPCTSTR value) {
    add(std::wstring(name), std::wstring(value));
  }

  void add(const std::wstring& name, const std::wstring& value) {
    _valid = false;
    _envStrings.push_back(name + _TCHAR('=') + value);
  }

  operator LPVOID() const {
    return _envStr.empty() && _envStrings.empty() ? 0 : (LPVOID)get().data();
  }

  const std::wstring& get() const {
    if (!_valid) {
      for (StrVector::const_iterator i = _envStrings.begin();
           i != _envStrings.end(); i++)
        _envStr += *i + _TCHAR('\0');

      _envStr += _TCHAR('\0');
      _valid = true;
    }

    return _envStr;
  }

  const std::wstring toString() const {
    std::wstring str;
    for (StrVector::const_iterator i = _envStrings.begin();
         i != _envStrings.end(); i++)
      str += *i + _T( "\n" );

    return str;
  }
};

typedef yloader::ManagedPtr<Environment> EnvironmentPtr;

MISCWIN_API const RunProcessResult
runProcess(const std::wstring& processFileName, const std::wstring& cmdLine,
           bool waitForProcess, const std::wstring* startingDirectory = 0,
           const Environment& env = Environment(),
           unsigned long timeout = 0) throw(RunProcessException);
MISCWIN_API const RunProcessResult runProcessAsUser(
    const std::wstring& userName, const std::wstring& domain,
    const std::wstring& password, const std::wstring& processFileName,
    const std::wstring& cmdLine, bool waitForProcess,
    const std::wstring* startingDirectory = 0,
    unsigned long timeout = 0) throw(RunProcessException);

// Sent to parent window when user clicked on the checkbox of an item:
// wParam: The item index in the list ctrl
// lParam: The mouse event type(WM_LBUTTONDOWN, WM_RBUTTONDOWN, etc) which
// generated this event. Note: This message is not sent when the checkbox states
// were altered programmatically
//       by calling "SetItem", it is only sent when the user "physically"
//       clicked the checkbox using mouse or joystick etc.
#define WM_ON_CHKBOX (WM_APP + 10000)

// Sent to parent window when a column of items were sorted
// wParam: The column index
// lParam: The sort method, either 0(descending) or 1(ascending)
#define WM_ITEM_SORTED (WM_APP + 10001)

// Sent to parent window when an item text editing was committed
// wParam: The item index
// lParam: The column index
#define WM_EDIT_COMMITTED (WM_APP + 10002)

// Checkbox styles.
#define RC_CHKBOX_NONE 0    // No checkbox displayed
#define RC_CHKBOX_NORMAL 1  // Normal, multiple check allowed
#define RC_CHKBOX_SINGLE 2  // Single check only
#define RC_CHKBOX_DISABLED \
  3  // Disabled, cannot be checked/unchecked by user input,
     // but can be by your code.

// Item state flags for selection, deletion, etc.
// Multiple flags can be combined together using the bit-or operator.
// Note: If RC_ITEM_ALL is set, all other flags are ignored
#define RC_ITEM_NONE 0x0000        // Void, indicates invalid items only
#define RC_ITEM_ALL 0x0001         // All items regardless of states
#define RC_ITEM_SELECTED 0x0002    // Selected items
#define RC_ITEM_UNSELECTED 0x0004  // Unselected items
#define RC_ITEM_CHECKED 0x0008     // Checked items
#define RC_ITEM_UNCHECKED 0x0010   // Unchecked items
#define RC_ITEM_FOCUSED 0x0020     // Focused item
#define RC_ITEM_UNFOCUSED 0x0040   // Unfocused items

// Item inverting types
#define RC_INVERT_SELECTION 0  // Invert item selection
#define RC_INVERT_CHECKMARK 1  // Invert item check mark

// Removes any custom color from item text and item backgroun
#define COLOR_INVALID 0xffffffff

//////////////////////////////////////////////////////////////////////////
// The CReportCtrl Class Definition
//////////////////////////////////////////////////////////////////////////

class MISCWIN_API CReportCtrl : public CListCtrl {
 public:
  //////////////////////////////////////////////////////////////////////
  //		Constructor & Destructor
  //////////////////////////////////////////////////////////////////////
  CReportCtrl();
  virtual ~CReportCtrl();

  //////////////////////////////////////////////////////////////////////
  //		Run-time Creation
  //////////////////////////////////////////////////////////////////////
  virtual BOOL Create(CWnd* pParentWnd, UINT nID, LPCRECT lpRect = NULL,
                      DWORD dwStyle = WS_BORDER | WS_TABSTOP);

  ///////////////////////////////////////////////////////////////////////
  //		Column Header attributes
  ///////////////////////////////////////////////////////////////////////

  BOOL SetColumnHeader(
      const CString& strHeadings);  // Set columns and their formats.
  int GetColumnCount() const;       // Get the column count.
  BOOL DeleteAllColumns();
  CString GetHeaderText(int nColumn) const;
  BOOL SetHeaderText(int nColumn, LPCTSTR lpText);
  BOOL HasColumnHeader()
      const;  // FALSE if the list control has LVS_NOCOLUMNHEADER flag
  const CHeaderCtrl* GetHeaderCtrl() const;

  ///////////////////////////////////////////////////////////////////////
  //		Images & Color
  ///////////////////////////////////////////////////////////////////////

  // Column header images
  BOOL SetHeaderImage(int nColumn, int nImageIndex, BOOL bLeftSide = TRUE);
  int GetHeaderImage(int nColumn) const;
  CImageList* SetHeaderImageList(UINT nBitmapID,
                                 COLORREF crMask = RGB(255, 0, 255));
  CImageList* SetHeaderImageList(CImageList* pImageList);

  // Sub-item images
  BOOL SetItemImage(int nItem, int nSubItem, int nImageIndex);
  int GetItemImage(int nItem, int nSubItem) const;
  CImageList* SetImageList(UINT nBitmapID, COLORREF crMask = RGB(255, 0, 255));
  CImageList* SetImageList(CImageList* pImageList);
  CImageList* GetImageList() const;

  // Sub-item Text & Background Color
  void SetItemTextColor(int nItem = -1, int nSubItem = -1,
                        COLORREF color = COLOR_INVALID, BOOL bRedraw = TRUE);
  COLORREF GetItemTextColor(int nItem, int nSubItem) const;
  void SetItemBkColor(int nItem = -1, int nSubItem = -1,
                      COLORREF color = COLOR_INVALID, BOOL bRedraw = TRUE);
  COLORREF GetItemBkColor(int nItem, int nSubItem) const;

  //////////////////////////////////////////////////////////////////////
  //		Control Styles & Attributes
  //////////////////////////////////////////////////////////////////////

  void ResizeToFitParent();  // Resize the list control to occupy parent's
                             // client area
  void SetGridLines(BOOL bSet = TRUE);  // Show grid lines.
  void SetCheckboxeStyle(
      int nStyle = RC_CHKBOX_NORMAL);  // Set checkbox styles.
  int GetCheckboxStyle() const;
  BOOL IsSortable() const;             // Is sort allowed?
  BOOL SetSortable(BOOL bSet);         // Allow/disallow sorting
  BOOL IsEditable() const;             // Is Item text editable?
  void SetEditable(BOOL bSet = TRUE);  // Allow item text editting

  ///////////////////////////////////////////////////////////////////////
  //		Item attributes & operations
  ///////////////////////////////////////////////////////////////////////

  // Item states operation
  int GetFirstItem(DWORD dwStates = RC_ITEM_ALL, int nStartAfter = -1) const;
  int GetLastItem(DWORD dwStates = RC_ITEM_ALL, int nStartBefore = -1) const;
  int GetItemCount(DWORD dwStates = RC_ITEM_ALL) const;
  DWORD GetItemStates(int nItem) const;
  BOOL ExamItemStates(int nItem, DWORD dwStates) const;
  BOOL SetItemStates(int nItem, DWORD dwNewStates);
  int SetAllItemStates(DWORD dwOldStates, DWORD dwNewStates);
  void InvertItems(int nType);  // RC_INVERT_SELECTION or RC_INVERT_CHECKMARK

  // Item Insertion & Deletion
  int InsertItemEx(int nItem, LPCTSTR lpText, ...);
  BOOL DeleteItem(int nItem, BOOL bSelectNextItem = FALSE);  // Delete an item.
  int DeleteAllItems(
      DWORD dwStates = RC_ITEM_ALL);  // Delete all qualified items.

  // Item positioning
  int MoveUp(int nItem,
             int nCount = 1);  // Move an item upwards by "nCount" positions.
  int MoveDown(
      int nItem,
      int nCount = 1);          // Move an item downwards by "nCount" positions.
  int MoveToTop(int nItem);     // Move an item up to the top.
  int MoveToBottom(int nItem);  // Move an item down to the bottom.
  int MoveTo(int nItem,
             int nNewPosition);  // Move an item to a particular position
  BOOL SwapItems(
      int nItem1,
      int nItem2);  // Swap two items in the list, including all attributes.

  // Convenient versions of "CListCtrl::SetItemText"
  BOOL SetItemText(int nItem, int nSubItem, INT val);
  BOOL SetItemText(int nItem, int nSubItem, UINT val);
  BOOL SetItemText(int nItem, int nSubItem, LONG val);
  BOOL SetItemText(int nItem, int nSubItem, ULONG val);
  BOOL SetItemText(int nItem, int nSubItem, TCHAR val);
  BOOL SetItemText(int nItem, int nSubItem, DOUBLE val, int nPrecision = -1);
  BOOL SetItemText(int nItem, int nSubItem, const COleDateTime& dateTime,
                   DWORD dwFlags = 0);

  // Sorting
  BOOL IsSortAscending() const;
  int GetSortedColumn() const;
  void SortItems(int nColumn, BOOL bAscending);  // Sort a specified column.
  void SetSortSeparator(
      LPCTSTR lpSortSeparator = NULL);  // Sort-separator, NULL=disabled
  LPCTSTR GetSortSeparator() const;

  // Item text edit
  BOOL StartEdit(
      int nItem,
      int nSubItem);  // Display the editbox, previous edit are committed
  BOOL EndEdit(
      BOOL bCommit = TRUE);  // Commit/cancel text edit, hide the editbox
  CEdit* GetEditControl();

  ///////////////////////////////////////////////////////////////////////
  //		Necessary overloading but Irrelevant to Users
  ///////////////////////////////////////////////////////////////////////

  int InsertColumn(int nCol, LPCTSTR lpColumnHeading, int nFormat = LVCFMT_LEFT,
                   int nWidth = -1, int nSubItem = -1);
  int InsertColumn(int nCol, const LVCOLUMN* pColumn);
  BOOL DeleteColumn(int nCol);
  BOOL SetCheck(
      int nItem,
      BOOL bCheck = TRUE);  // overloaded to guard the "single" checkbox style
  BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
               int nImage, UINT nState, UINT nStateMask, LPARAM lParam);
  BOOL SetItem(const LVITEM* pItem);
  BOOL GetItem(LVITEM* pItem) const;
  LPARAM GetItemData(int nItem) const;
  BOOL SetItemData(int nItem, LPARAM dwData);
  DWORD SetExtendedStyle(DWORD dwNewStyle);
  BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);
  BOOL ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);
  int InsertItem(UINT nMask, int nItem, LPCTSTR lpItem, UINT nState,
                 UINT nStateMask, int nImage, LPARAM lParam);
  int InsertItem(int nItem, LPCTSTR lpItem, int nImage);
  int InsertItem(const LVITEM* pItem);
  int InsertItem(int nIndex, LPCTSTR lpText);
  BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpText);

  // AM 01.06.04
  // needs to be overriden for the case where we search on lparam
  int FindItem(LVFINDINFO* pFindInfo, int nStart = -1) const;

 protected:
  // Helper functions for internal use
  BOOL _IsValidIndex(int nIndex) const;
  BOOL _ItemCheckMonitor(int nIndex, BOOL bBefore, BOOL bAfter, UINT nMsg);
  BOOL _IsEditVisible() const;
  void _MouseClkMonitor(UINT nMsg, UINT nFlags, CPoint point,
                        BOOL bTriggerEdit);
  static void _StringSplit(const CString& str, CStringArray& arr,
                           TCHAR chDelimitior);
  void _UnsetSortedColumn();
  BOOL _SetItemStatesNoVerify(int nItem, DWORD dwNewStates);
  void _EnsureSingleCheck(int nItem);
  DWORD _GetHeaderTextFormat(int nColumn) const;
  void _UpdateColumn(int nColumn, BOOL bInsert);
  void _AllocItemMemory(int nItem);
  void _FreeItemMemory(int nItem);
  BOOL _PartialSort(int nStart, int nEnd);
  int _FindSeparator(int nStartAfter, int nColumn) const;
  void _QuickSortRecursive(int* pArr, int nLow, int nHigh);
  int _CompareItems(int nItem1, int nItem2);

  // Member data
  CEdit* m_pWndEdit;      // Edit control, for subitem edit
  LPTSTR m_pszSeparator;  // Sort separator
  BOOL m_bAllowEdit;      // Is subitem edit allowed?
  POINT m_ptEditting;  // Position of the subitem that is currently being edited
  int m_nChkStyle;     // Checkbox style
  DWORD m_dwPrevEditFmt;       // Format of previously edited subitem
  CImageList m_imgList;        // Image list for items
  CImageList m_headerImgList;  // Image list for the header control
  int m_nSortCol;              // The sorted column, -1 if none
  BOOL m_bSortAscending;       // Is sort ascending?

  //////////////////////////////////////////////////////////////////////
  // Wizard Generated Stuff
  //////////////////////////////////////////////////////////////////////

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CReportCtrl)
 public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);

 protected:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual void PreSubclassWindow();
  //}}AFX_VIRTUAL

  // Generated message map functions

 protected:
  //{{AFX_MSG(CReportCtrl)
  afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
// End of CReportCtrl Class Definition
//////////////////////////////////////////////////////////////////////////

/*
Copyright(C) 2017  YLoader.com

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#include <Windows.h>
#include <ostream>
#include <sstream>
#include <string>

template <class CharT, class TraitsT = std::char_traits<CharT> >
class basic_debugbuf : public std::basic_stringbuf<CharT, TraitsT> {
 public:
  virtual ~basic_debugbuf() { sync(); }

 protected:
  int sync() {
    output_debug_string(str().c_str());
    str(std::basic_string<CharT>());  // Clear the string buffer

    return 0;
  }

  void output_debug_string(const CharT* text) {}
};

template <>
void basic_debugbuf<char>::output_debug_string(const char* text) {
  ::OutputDebugStringA(text);
}

template <>
void basic_debugbuf<wchar_t>::output_debug_string(const wchar_t* text) {
  ::OutputDebugStringW(text);
}

template <class CharT, class TraitsT = std::char_traits<CharT> >
class basic_dostream : public std::basic_ostream<CharT, TraitsT> {
 public:
  basic_dostream()
      : std::basic_ostream<CharT, TraitsT>(
            new basic_debugbuf<CharT, TraitsT>()) {}
  ~basic_dostream() { delete rdbuf(); }
};

/*typedef basic_dostream<char>    dostream;
typedef basic_dostream<wchar_t> wdostream;
*/
typedef basic_dostream<TCHAR> t_dostream;

//////////////////////////////////////
/// Named mutex

class NamedMutex {
 private:
  HANDLE m_mutex;

  bool m_alreadyExists;

  DWORD m_lastError;

 public:
  NamedMutex(const std::wstring& name) : m_mutex(0) {
    m_mutex = CreateMutex(NULL, true, name.c_str());

    m_mutex != 0 ? m_lastError = GetLastError() : 0;
  }

  ~NamedMutex() {
    if (m_mutex != 0) CloseHandle(m_mutex);
  }

  bool alreadyExists() const { return m_lastError == ERROR_ALREADY_EXISTS; }
};

typedef yloader::ManagedPtr<NamedMutex> NamedMutexPtr;

}  // namespace yloader

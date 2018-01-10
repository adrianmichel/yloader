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

#include <miscwin.h>

//////////////////////////////////////////////////////////////////////////
// The CYReportCtrl Class Definition
//////////////////////////////////////////////////////////////////////////

class CYReportCtrl : public CListCtrl {
 public:
  //////////////////////////////////////////////////////////////////////
  //		Constructor & Destructor
  //////////////////////////////////////////////////////////////////////
  CYReportCtrl();
  virtual ~CYReportCtrl();

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
  void SwapItems(
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
  //{{AFX_VIRTUAL(CYReportCtrl)
 public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);

 protected:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual void PreSubclassWindow();
  //}}AFX_VIRTUAL

  // Generated message map functions

 protected:
  //{{AFX_MSG(CYReportCtrl)
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
// End of CYReportCtrl Class Definition
//////////////////////////////////////////////////////////////////////////

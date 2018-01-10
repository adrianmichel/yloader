#pragma once

//#include "controls.h"
// SettingsDlg.h : header file
//
/*********************************************************************
        CSettingsDialog 1.0

        Copyright (C) 2002

        This software is provided 'as-is', without any express or implied
        warranty.  In no event will the author be held liable for any damages
        arising from the use of this software.

        Permission is granted to anyone to use this software for any purpose,
        including commercial applications, and to alter it and redistribute it
        freely.

        IMPORTANT: This class is modified from Chris Losinger's work on
   CSAPrefsDialog. The following is the original copyright notes from his
   project. If you want to use codes in this project, please be aware that his
   statements also apply.
*/

/*********************************************************************

 Copyright (C) 2000 Smaller Animals Software, Inc.

 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

 2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

 3. This notice may not be removed or altered from any source distribution.

 http://www.smalleranimals.com
 smallest@smalleranimals.com

**********************************************************************/

#include <afxtempl.h>
//#include "PrefsStatic.h"
#include "PrefsStatic.h"
#include "afxwin.h"
#include "resource.h"

#define WM_SETTINGSDIALOG_CLOSE WM_USER + 5

class Okable {
 public:
  virtual ~Okable() {}

  virtual bool onOk() = 0;
};

typedef class _PageInfo PAGE_INFO;

class _PageInfo {
 public:
  BOOL bViewClass;          // View flag for runtime checking
  UINT nID;                 // Resource ID for the Page
  CWnd *pWnd;               // Pointer to the page
  CWnd *pWndParent;         // Pointer to the parent page if has
  CString csCaption;        // Caption on the tree
  CString csParentCaption;  // Caption of the parent on the tree
};

typedef CTypedPtrArray<CPtrArray, PAGE_INFO *> PAGE_LIST;
typedef CMap<CWnd *, CWnd *, DWORD, DWORD &> WNDTREE_MAP;

/////////////////////////////////////////////////////////////////////////////
// CSettingsDialog dialog

class CSettingsDialog : public CDialog {
  // Construction
 public:
  CSettingsDialog(CWnd *pParent = NULL);  // standard constructor
  ~CSettingsDialog();

 public:
  BOOL Create();
  void ExpandTree();
  BOOL DestroyPages();
  BOOL CreatePage(const PAGE_INFO *pInfo);
  void ShowPage(const PAGE_INFO *pInfo, UINT nShow = SW_SHOW);
  void SetLogoText(CString sText);
  void SetTitle(CString sTitle);
  HTREEITEM GetNextItemCOrS(HTREEITEM hItem);
  HTREEITEM FindItem(const CString &csCaption);
  HTREEITEM FindItem(CWnd *pWnd);
  CWnd *AddPage(CRuntimeClass *pWndClass, LPCTSTR pCaption, UINT nID,
                LPCTSTR pParentCaption);
  CWnd *AddPage(CRuntimeClass *pWndClass, LPCTSTR pCaption, UINT nID = 0,
                CWnd *pDlgParent = NULL);

  // Dialog Data
  PAGE_LIST m_pInfo;     // Containing page info
  CRect m_FrameRect;     // Rectangle size of a setting page
  CString m_csTitle;     // Title of selected page
  CString m_csLogoText;  // Logo text
  WNDTREE_MAP m_wndMap;  // MFC CMap class for internal page management
  CWnd *m_pParent;       // Parent window to receive OK, Apply, Cancel message

  // Dialog Data
  //{{AFX_DATA(CSettingsDialog)
  enum { IDD = IDD_SETTINGS_DLG };
  CStatic m_PageFrame;
  CTreeCtrl m_TreeCtrl;
  CPrefsStatic m_CaptionBarCtrl;
  //}}AFX_DATA

  // Overrides
  virtual BOOL CreateWnd(CWnd *pWnd, CCreateContext *pContext = NULL);
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CSettingsDialog)
 public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);

 protected:
  virtual void DoDataExchange(CDataExchange *pDX);  // DDX/DDV support
                                                    //}}AFX_VIRTUAL

  // Implementation
 protected:
  void ExpandBranch(HTREEITEM hti);
  void InitTreeCtrl();
  BOOL RefreshData();

  // Generated message map functions
  //{{AFX_MSG(CSettingsDialog)
  virtual BOOL OnInitDialog();
  afx_msg void OnGetDispInfoTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnTreeSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
  virtual void OnCancel();
  virtual void OnOK();
  afx_msg void OnPreferenceHelp();
  afx_msg void OnApply();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
 private:
  CButton m_applyButton;
  CButton m_cancelButton;
  CButton m_okButton;
  CButton m_helpButton;

 public:
  afx_msg void OnBnClickedPageFrame();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

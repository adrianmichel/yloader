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

class CColorStaticST : public CStatic {
  // Construction
 public:
  CColorStaticST();
  enum { ST_FLS_SLOW, ST_FLS_NORMAL, ST_FLS_FAST };

  // Attributes
 public:
  // Operations
 public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CColorStaticST)
  //}}AFX_VIRTUAL

  // Implementation
 public:
  virtual ~CColorStaticST();

  void SetTextColor(COLORREF crTextColor = 0xffffffff);
  COLORREF GetTextColor();

  void SetBkColor(COLORREF crBkColor = 0xffffffff);
  COLORREF GetBkColor();

  void SetBlinkTextColors(COLORREF crBlinkTextColor1,
                          COLORREF crBlinkTextColor2);
  void StartTextBlink(BOOL bStart = TRUE, UINT nElapse = ST_FLS_NORMAL);

  void SetBlinkBkColors(COLORREF crBlinkBkColor1, COLORREF crBlinkBkColor2);
  void StartBkBlink(BOOL bStart = TRUE, UINT nElapse = ST_FLS_NORMAL);

  void EnableNotify(CWnd* pParent = NULL, UINT nMsg = WM_USER);

  static const short GetVersionI();
  static const char* GetVersionC();

  // Generated message map functions
 protected:
  //{{AFX_MSG(CColorStaticST)
  afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnDestroy();
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
 private:
  UINT m_nTimerId;

  COLORREF m_crTextColor;
  COLORREF m_crBlinkTextColors[2];
  BOOL m_bTextBlink;
  int m_nTextBlinkStep;

  COLORREF m_crBkColor;
  COLORREF m_crBlinkBkColors[2];
  BOOL m_bBkBlink;
  int m_nBkBlinkStep;

  CBrush m_brBkgnd;
  CBrush m_brBlinkBkgnd[2];

  CWnd* m_pParent;
  UINT m_nMsg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately
// before the previous line.


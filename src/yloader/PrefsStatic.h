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

class CPrefsStatic : public CStatic {
  // Construction
 public:
  CPrefsStatic();

  // Attributes
 public:
  // Operations
 public:
  CString m_csFontName;

  void SetConstantText(LPCTSTR pText) { m_csConstantText = pText; }

  int m_fontSize, m_fontWeight;
  BOOL m_grayText;
  COLORREF m_textClr;
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPrefsStatic)
  //}}AFX_VIRTUAL

  // Implementation
 public:
  ~CPrefsStatic() override;

 protected:
  CFont m_captionFont, m_nameFont;

  CBitmap m_bm;

  CString m_csConstantText;

  void MakeCaptionBitmap();

  // Generated message map functions
 protected:
  //{{AFX_MSG(CPrefsStatic)
  afx_msg void OnPaint();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately
// before the previous line.

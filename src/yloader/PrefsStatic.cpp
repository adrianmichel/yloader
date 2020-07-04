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
#include "PrefsStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefsStatic

CPrefsStatic::CPrefsStatic() {
  m_textClr = ::GetSysColor(COLOR_3DFACE);
  m_fontWeight = FW_NORMAL;
  m_fontSize = 12;
}

CPrefsStatic::~CPrefsStatic() {
  if (m_bm.GetSafeHandle()) {
    m_bm.DeleteObject();
  }

  if (m_captionFont.GetSafeHandle()) {
    m_captionFont.DeleteObject();
  }

  if (m_nameFont.GetSafeHandle()) {
    m_nameFont.DeleteObject();
  }
}

BEGIN_MESSAGE_MAP(CPrefsStatic, CStatic)
//{{AFX_MSG_MAP(CPrefsStatic)
ON_WM_PAINT()
ON_WM_ERASEBKGND()
ON_MESSAGE(WM_SETTEXT, OnSetText)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefsStatic message handlers

void CPrefsStatic::OnPaint() {
  CPaintDC dc(this);  // device context for painting

  CFont* pOldFont = NULL;

  // Set default font
  if (m_csFontName == "") {
    CWnd* pWndParent = GetParent();
    if (pWndParent) {
      dc.SelectObject(pWndParent->GetFont());
    }
  } else {
    // create a font, if we need to
    if (m_captionFont.GetSafeHandle() == NULL) {
      m_captionFont.CreateFont(m_fontSize, 0, 0, 0, m_fontWeight, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN, m_csFontName);
    }

    if (m_captionFont.GetSafeHandle() != NULL) {
      pOldFont = dc.SelectObject(&m_captionFont);
    }
  }

  // Draw text
  CString strText;
  GetWindowText(strText);

  dc.SetTextColor(m_textClr);
  dc.SetBkMode(TRANSPARENT);

  // vertical center
  CRect cr;
  GetClientRect(cr);

  cr.left += 5;
  dc.DrawText(strText, cr, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

  if (pOldFont) dc.SelectObject(pOldFont);
}

BOOL CPrefsStatic::OnEraseBkgnd(CDC* pDC) {
  if (!m_bm.GetSafeHandle()) {
    MakeCaptionBitmap();
  }

  if (m_bm.GetSafeHandle()) {
    CRect cr;
    GetClientRect(cr);

    CDC memDC;
    memDC.CreateCompatibleDC(pDC);

    CBitmap* pOB = memDC.SelectObject(&m_bm);

    pDC->BitBlt(0, 0, cr.Width(), cr.Height(), &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOB);
  }

  return TRUE;
}

LRESULT CPrefsStatic::OnSetText(WPARAM wParam, LPARAM lParam) {
  DefWindowProc(WM_SETTEXT, wParam, lParam);
  Invalidate(TRUE);
  return (TRUE);
}

//////////////////
// Helper to paint rectangle with a color.
//
static void PaintRect(CDC& dc, int x, int y, int w, int h, COLORREF color) {
  CBrush brush(color);
  CBrush* pOldBrush = dc.SelectObject(&brush);
  dc.PatBlt(x, y, w, h, PATCOPY);
  dc.SelectObject(pOldBrush);
}

void CPrefsStatic::MakeCaptionBitmap() {
  if (m_bm.m_hObject) return;  // already have bitmap; return

  CRect cr;
  GetClientRect(cr);
  int w = cr.Width();
  int h = cr.Height();

  // Create bitmap same size as caption area and select into memory DC
  //
  CWindowDC dcWin(this);
  CDC dc;
  dc.CreateCompatibleDC(&dcWin);
  m_bm.DeleteObject();
  m_bm.CreateCompatibleBitmap(&dcWin, w, h);
  CBitmap* pOldBitmap = dc.SelectObject(&m_bm);

  COLORREF clrBG = 0xff0000;  //::GetSysColor(COLOR_3DFACE); // background color
  int r = GetRValue(clrBG);   // red..
  int g = GetGValue(clrBG);   // ..green
  int b = GetBValue(clrBG);   // ..blue color vals
  int x = 8 * cr.right / 8;   // start 5/6 of the way right
  int w1 = x - cr.left;       // width of area to shade

  int NCOLORSHADES = 128;  // this many shades in gradient

  int xDelta = max(w / NCOLORSHADES, 1);  // width of one shade band

  PaintRect(dc, x, 0, cr.right - x, h, clrBG);

  while (x > xDelta) {               // paint bands right to left
    x -= xDelta;                     // next band
    int wmx2 = (w1 - x) * (w1 - x);  // w minus x squared
    int w2 = w1 * w1;                // w squared
    PaintRect(dc, x, 0, xDelta, h, RGB(r - (r * wmx2) / w2, g - (g * wmx2) / w2, b - (b * wmx2) / w2));
  }

  PaintRect(dc, 0, 0, x, h, RGB(0, 0, 0));  // whatever's left ==> black

  // draw the 'constant' text

  // create a font, if we need to
  if (m_nameFont.GetSafeHandle() == NULL)
    m_nameFont.CreateFont(18, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET,
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN, m_csFontName);

  CFont* OldFont = dc.SelectObject(&m_nameFont);

  // back up a little
  cr.right -= 5;

  // draw text in DC
  dc.SetBkMode(TRANSPARENT);
  dc.SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
  dc.DrawText(m_csConstantText, cr + CPoint(1, 1), DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
  dc.SetTextColor(::GetSysColor(COLOR_3DSHADOW));
  dc.DrawText(m_csConstantText, cr, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

  // restore old font
  dc.SelectObject(OldFont);

  // Restore DC
  dc.SelectObject(pOldBitmap);
}

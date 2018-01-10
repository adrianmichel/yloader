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
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "yloader.h"
#endif

#include "yloaderDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CyloaderDoc

IMPLEMENT_DYNCREATE(CyloaderDoc, CDocument)

BEGIN_MESSAGE_MAP(CyloaderDoc, CDocument)
END_MESSAGE_MAP()

// CyloaderDoc construction/destruction

CyloaderDoc::CyloaderDoc() {
  // TODO: add one-time construction code here
}

CyloaderDoc::~CyloaderDoc() {}

BOOL CyloaderDoc::OnNewDocument() {
  if (!CDocument::OnNewDocument()) return FALSE;

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)

  return TRUE;
}

// CyloaderDoc serialization

void CyloaderDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
    // TODO: add storing code here
  } else {
    // TODO: add loading code here
  }
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CyloaderDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds) {
  // Modify this code to draw the document's data
  dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

  CString strText = _T("TODO: implement thumbnail drawing here");
  LOGFONT lf;

  CFont* pDefaultGUIFont =
      CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
  pDefaultGUIFont->GetLogFont(&lf);
  lf.lfHeight = 36;

  CFont fontDraw;
  fontDraw.CreateFontIndirect(&lf);

  CFont* pOldFont = dc.SelectObject(&fontDraw);
  dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
  dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CyloaderDoc::InitializeSearchContent() {
  CString strSearchContent;
  // Set search contents from document's data.
  // The content parts should be separated by ";"

  // For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
  SetSearchContent(strSearchContent);
}

void CyloaderDoc::SetSearchContent(const CString& value) {
  if (value.IsEmpty()) {
    RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
  } else {
    CMFCFilterChunkValueImpl* pChunk = NULL;
    ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
    if (pChunk != NULL) {
      pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
      SetChunkValue(pChunk);
    }
  }
}

#endif  // SHARED_HANDLERS

// CyloaderDoc diagnostics

#ifdef _DEBUG
void CyloaderDoc::AssertValid() const { CDocument::AssertValid(); }

void CyloaderDoc::Dump(CDumpContext& dc) const { CDocument::Dump(dc); }
#endif  //_DEBUG

// CyloaderDoc commands

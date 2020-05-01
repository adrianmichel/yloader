/*********************************************************************

   Copyright (C) 2002 Smaller Animals Software, Inc.

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

/**********************************************************************
	Shortcut expansion code, 1996 by Rob Warner
	part of "CShortcut"
	rhwarner@southeast.net
	http://users.southeast.net/~rhwarner

	modified for CDropEdit, Chris Losinger
**********************************************************************/

////
//
//	To use this in an app, you'll need to :
//
//	1) #include <afxole.h> in stdafx.h
//
//	2) in your CWinApp-derived class *::InitInstance, you'll need to call
//		::CoInitialize(NULL);
//
//	3) in your CWinApp-derived class *::ExitInstance, you'll need to call
//	::CoUninitialize();
//
//	4) Place a normal edit control on your dialog.
//	5) Check the "Accept Files" property.
//
//	6) In your dialog class, declare a member variable of type CDropEdit
//	(be sure to #include "CDropEdit.h")
//		ex. CDropEdit m_dropEdit;
//
//	7) In your dialog's OnInitDialog, call
//		m_dropEdit.SubclassDlgItem(IDC_YOUR_EDIT_ID, this);
//
//	8) if you want the edit control to handle directories, call
//		m_dropEdit.SetUseDir(TRUE);
//
//	9) if you want the edit control to handle files, call
//		m_dropEdit.SetUseDir(FALSE);
//
//	that's it!
//
//	This will behave exactly like a normal edit-control but with the
//	ability to accept drag-n-dropped files (or directories).
//
//


#include "stdafx.h"
#include "resource.h"
#include "DropEdit.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDropEdit

CDropEdit::CDropEdit() : _refreshable(0), m_bUseDir(false) {}

CDropEdit::~CDropEdit() {}

BEGIN_MESSAGE_MAP(CDropEdit, CEdit)
//{{AFX_MSG_MAP(CDropEdit)
ON_WM_DROPFILES()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDropEdit message handlers

/////////////////////////////////////////////////////////////////////////////
//	handles WM_DROPFILES

void CDropEdit::OnDropFiles(HDROP dropInfo) {
  // Get the number of pathnames that have been dropped
  WORD wNumFilesDropped = DragQueryFile(dropInfo, -1, NULL, 0);

  CString csFirstFile;

  // there may be many, but we'll only use the first
  if (wNumFilesDropped > 0) {
    // Get the number of bytes required by the file's full pathname
    WORD wPathnameSize = DragQueryFile(dropInfo, 0, NULL, 0);

    // Allocate memory to contain full pathname & zero byte
    wPathnameSize += 1;

    TCHAR* pFile = new TCHAR[wPathnameSize];
    if (pFile == NULL) {
      ASSERT(0);
      DragFinish(dropInfo);
      return;
    }

    // Copy the pathname into the buffer
    DragQueryFile(dropInfo, 0, pFile, wPathnameSize);

    csFirstFile = pFile;

    // clean up
    delete[] pFile;
  }

  // Free the memory block containing the dropped-file information
  DragFinish(dropInfo);

  // if this was a shortcut, we need to expand it to the target path
  CString csExpandedFile = ExpandShortcut(csFirstFile);

  // if that worked, we should have a real file name
  if (!csExpandedFile.IsEmpty()) {
    csFirstFile = csExpandedFile;
  }

  if (!csFirstFile.IsEmpty()) {
    struct _stat buf;

    // get some info about that file
    int result = _wstat(csFirstFile, &buf);
    if (result == 0) {
      // verify that we have a dir (if we want dirs)
      if ((buf.st_mode & _S_IFDIR) == _S_IFDIR) {
        if (m_bUseDir) {
          SetWindowText(csFirstFile);
        }

        // verify that we have a file (if we want files)
      } else if ((buf.st_mode & _S_IFREG) == _S_IFREG) {
        if (!m_bUseDir) {
          SetWindowText(csFirstFile);
          if (_refreshable != 0) _refreshable->refresh();
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////
//	use IShellLink to expand the shortcut
//	returns the expanded file, or "" on error
//
//	original code was part of CShortcut
//	1996 by Rob Warner
//	rhwarner@southeast.net
//	http://users.southeast.net/~rhwarner

CString CDropEdit::ExpandShortcut(CString& inFile) {
  CString outFile;

  // Make sure we have a path
  ASSERT(!inFile.IsEmpty());

  IShellLink* psl;
  HRESULT hres;
  LPTSTR lpsz = inFile.GetBuffer(MAX_PATH);

  // Create instance for shell link
  hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);

  if (SUCCEEDED(hres)) {
    // Get a pointer to the persist file interface
    IPersistFile* ppf;

    hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
    if (SUCCEEDED(hres)) {
      // Load shortcut
      hres = ppf->Load((LPCOLESTR)lpsz, STGM_READ);

      if (SUCCEEDED(hres)) {
        WIN32_FIND_DATA wfd;

        // find the path from that
        HRESULT hres = psl->GetPath(outFile.GetBuffer(MAX_PATH), MAX_PATH, &wfd, SLGP_UNCPRIORITY);

        outFile.ReleaseBuffer();
      }

      ppf->Release();
    }

    psl->Release();
  }

  inFile.ReleaseBuffer();

  // if this fails, outFile == ""
  return outFile;
}

/////////////////////////////////////////////////////////////////////////////

void CDropEdit::PreSubclassWindow() {
  DragAcceptFiles(TRUE);

  __super::PreSubclassWindow();
}

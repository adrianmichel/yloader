#pragma once
#include <shlobj.h>

//////////////////////////////////////////////////////////////////////
//
// CShellBrowser
//

class CBrowseForFolder {
public:
  CBrowseForFolder(const HWND hParent = NULL, const LPITEMIDLIST pidl = NULL, const int nTitleID = 0);
  CBrowseForFolder(const HWND hParent, const LPITEMIDLIST pidl, const CString& strTitle);
  virtual ~CBrowseForFolder() = 0;

  //
  // Set the handle of the owner window for the dialog box.
  //
  void SetOwner(const HWND hwndOwner);

  //
  // Set the root of the hierarchy that will be browsed.  Get pidl from
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
  static int __stdcall BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

  using AUTO_STR = std::shared_ptr<TCHAR>;
  AUTO_STR m_pchTitle;

  BROWSEINFO m_bi;
  TCHAR m_szSelected[MAX_PATH];
  CString m_strPath;
  HWND m_hwnd;
};

inline UINT CBrowseForFolder::GetFlags() const { return m_bi.ulFlags; }

inline int CBrowseForFolder::GetImage() const { return m_bi.iImage; }

class CSBDestination : public CBrowseForFolder {
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
    if (SHGetPathFromIDList(pidl, strBuffer.GetBuffer(MAX_PATH))) {
      strBuffer.ReleaseBuffer();
    }
    else {
      strBuffer.ReleaseBuffer(0);
    }
    SetStatusText(strBuffer);
  }

private:
  CString m_strInitialSelection;
};
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

class CHListBox : public CListBox {
  // Construction
 public:
  CHListBox();

  // Attributes
 public:
  // Operations
 public:
  int AddString(LPCTSTR s);
  int InsertString(int i, LPCTSTR s);
  void ResetContent();
  int DeleteString(int i);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CHListBox)
  //}}AFX_VIRTUAL

  // Implementation
 public:
  virtual ~CHListBox();

  // Generated message map functions
 protected:
  void updateWidth(LPCTSTR s);
  int width;
  //{{AFX_MSG(CHListBox)
  // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.


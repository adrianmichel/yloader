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

#include "reportctrl.h"
#include "resource.h"
#include "yahooeventsink.h"

using namespace yloader;

constexpr unsigned int WM_YAHOO_EVENT = WM_USER + 1;

class CYahooEventList : public ::CYReportCtrl, public YahooEventSink {
 private:
  CImageList m_imageList;

 public:
  CYahooEventList() {}

  DECLARE_MESSAGE_MAP()

 public:
  virtual void event(const YahooEvent& event) {
    YahooEvent* e = new YahooEvent(event);
    PostMessage(WM_YAHOO_EVENT, (WPARAM) const_cast<void*>((void*)e));
  }

  void init() {
    /*    DWORD dwStyle;
        dwStyle = SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0 ,0);
        dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_FLATSB |LVS_EX_GRIDLINES |
       LVS_EX_ONECLICKACTIVATE | LVS_EX_SUBITEMIMAGES   ; SendMessage(
       LVM_SETEXTENDEDLISTVIEWSTYLE, 0,dwStyle );
    */
    DWORD dwStyle;
    dwStyle = SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
    dwStyle |= LVS_EX_FLATSB | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES;
    SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle);

    m_imageList.Create(16, 16, ILC_MASK, 2, 2);
    m_imageList.Add(::AfxGetApp()->LoadIcon(IDI_ICON_INFO));
    m_imageList.Add(::AfxGetApp()->LoadIcon(IDI_ICON_ERROR));
    m_imageList.Add(::AfxGetApp()->LoadIcon(IDI_ICON_FATAL_ERROR));
    m_imageList.Add(::AfxGetApp()->LoadIcon(IDI_ICON_WARNING));

    SetImageList(&m_imageList);

    InsertColumn(0, L"", LVCFMT_LEFT, 22);
    InsertColumn(1, L"Symbol", LVCFMT_LEFT, 50);
    InsertColumn(2, L"Status", LVCFMT_LEFT, 410);
  }

  void insert(const YahooEvent& event) {
    LVITEM item;

    // type
    item.mask = LVIF_TEXT | LVIF_PARAM;
    item.iItem = GetItemCount();
    item.lParam = event.isInfoOrWarning();
    item.iSubItem = 0;
    item.state = 0;
    item.stateMask = 0;
    CString time = L"";
    item.pszText = const_cast<LPTSTR>((LPCTSTR)time);
    item.cchTextMax = time.GetLength();
    item.iIndent = 0;
    int itemIndex = item.iItem = InsertItem(&item);

    switch (event.type()) {
      case event_info:
        SetItemImage(itemIndex, 0, 0);
        break;
      case event_warning:
        SetItemImage(itemIndex, 0, 3);
        SetItemBkColor(item.iItem, -1, RGB(255, 255, 200));
        break;
      case event_error:
        SetItemImage(itemIndex, 0, 2);
        SetItemBkColor(item.iItem, -1, RGB(255, 230, 230));
        break;
      case event_fatal_error:
        SetItemImage(itemIndex, 0, 2);
        SetItemBkColor(item.iItem, -1, RGB(255, 230, 230));
        break;
      case event_ignored_symbol:
        SetItemBkColor(item.iItem, -1, RGB(173, 216, 250));
        break;
      default:
        break;
    }

    // side
    item.iSubItem = 1;
    item.mask = LVIF_TEXT;
    item.pszText = const_cast<LPTSTR>(event.symbol().c_str());
    item.cchTextMax = (int)event.symbol().length();
    SetItem(&item);

    // symbol
    item.iSubItem = 2;
    item.pszText = const_cast<LPTSTR>(event.message().c_str());
    item.cchTextMax = (int)event.message().length();
    SetItem(&item);

    EnsureVisible(GetItemCount() - 1, FALSE);
  }

  std::wstring getSelectedSymbol() { return L""; }

 public:
  afx_msg LRESULT onEvent(WPARAM wp, LPARAM lp);
};

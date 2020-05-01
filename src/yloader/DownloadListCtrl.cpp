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
#include "downloadlistctrl.h"
#include ".\downloadlistctrl.h"

BEGIN_MESSAGE_MAP(CYahooEventList, CYReportCtrl)
ON_MESSAGE(WM_YAHOO_EVENT, onEvent)
END_MESSAGE_MAP()

afx_msg LRESULT CYahooEventList::onEvent(WPARAM wp, LPARAM lp) {
  std::shared_ptr<YahooEvent> event((YahooEvent*)wp);
  insert(*event);
  return 0;
}

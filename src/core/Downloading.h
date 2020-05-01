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

#include "DataParams.h"
#include "YahooHistData.h"

class CDownloading : public yloader::Thread {
 public:
  CDownloading(const DataParams& dataParams, SymbolsList* errorSymbolsList, StrListPtr symbolsList, UniqueSymbolsSetPtr ignoreSymbols,
               SymbolsListIteratorPtr si, YahooEventDelegatorPtr yed);  // standard constructor
 public:
  void OnTimer(UINT nIDEvent);

 private:
  bool m_canceling;
  DataParams m_dataParams;
  YahooEventDelegatorPtr m_yed;
  StrListPtr m_symbols;
  SymbolsListIteratorPtr m_symbolsIterator;
  UniqueSymbolsSetPtr m_ignoreSymbols;

  FileNames m_fileNames;
  std::vector<std::shared_ptr<YahooHistData> > m_histDataVector;

 public:
  bool canceling() const { return m_canceling; }
  bool canceled() const { return m_canceling; }
  void cancel();

 private:
  bool isDownloading() const {
    for (auto t : m_histDataVector) {
      if (t->isRunning()) {
        return true;
      }
    }
    return false;
  }

 private:
  mutable Timer m_timer;

  YahooHistData::Duration m_onTimerDuration;

 public:
  const std::wstring onTimerDuration() const {
    return m_onTimerDuration.totalDuration().toString();
  }

 private:
  void run(ThreadContext* p);
};

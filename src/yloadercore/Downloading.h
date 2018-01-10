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

#include "DataParams.h"
#include "YahooHistData.h"

class CDownloading : public yloader::Thread {
 public:
  CDownloading(const DataParams& dataParams, SymbolsList* errorSymbolsList,
               StrListPtr symbolsList, UniqueSymbolsSetPtr ignoreSymbols,
               SymbolsListIteratorPtr si,
               YahooEventDelegatorPtr yed);  // standard constructor
 public:
  void OnTimer(UINT nIDEvent);

 private:
  bool _canceling;
  DataParams _dataParams;
  YahooEventDelegatorPtr _yed;
  StrListPtr _symbols;
  SymbolsListIteratorPtr _symbolsIterator;
  UniqueSymbolsSetPtr _ignoreSymbols;

  FileNames _fileNames;
  std::vector<ManagedPtr<YahooHistData> > _histDataVector;

 public:
  bool canceling() const { return _canceling; }
  bool canceled() const { return _canceling; }
  void cancel();

 private:
  bool isDownloading() const {
    for (size_t n = 0; n < _histDataVector.size(); n++) {
      if (_histDataVector[n]->isRunning()) return true;
    }
    return false;
  }

 private:
  mutable Timer _timer;

  YahooHistData::Duration _onTimerDuration;

 public:
  const std::wstring onTimerDuration() const {
    return _onTimerDuration.totalDuration().toString();
  }

 private:
  void run(ThreadContext* p);
};

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
#include "dataparams.h"
#include "yahoohistdata.h"
#include "Downloading.h"
#include <boost/format.hpp>

CDownloading::CDownloading(const DataParams& dataParams,
                           SymbolsList* errorSymbolsList,
                           StrListPtr symbolsList,
                           yloader::UniqueSymbolsSetPtr ignoreSymbols,
                           SymbolsListIteratorPtr si,
                           YahooEventDelegatorPtr yed)  // standard constructor
    : _canceling(false),
      _symbolsIterator(si),
      Thread(_T( "Downloading thread" )),
      _dataParams(dataParams),
      _symbols(symbolsList),
      _yed(yed),
      _ignoreSymbols(ignoreSymbols) {
  for (unsigned int n = 0; n < dataParams.threads(); n++) {
    /*    if( !enableExtraFeatures() && si->size() > MAX_SYMBOLS_USER )
        {
          AfxMessageBox( ( std::wstring( _T( "Only the first " ) ) << 100 << _T(
       " symbols will be downloaded. This limitation is removed in the
       registered version" ) ).c_str(), MB_OK );
        }
    */
    std::wostringstream o;
    o << _T( "Thread " ) << n;
    _histDataVector.push_back(new YahooHistData(
        o.str(), _dataParams, _ignoreSymbols, errorSymbolsList,
        *_symbolsIterator, *_yed, _fileNames));
  }
}

void CDownloading::run(ThreadContext* p) {
  for (size_t n = 0; n < _histDataVector.size(); n++)
    _histDataVector[n]->start();

  while (isRunning() && isDownloading()) {
    OnTimer(0);
    Sleep(10);
  }

  if (!canceled()) {
    // successful
    if (!_dataParams.saveFileAll().empty()) {
      // save all to output file
      std::ofstream f(_dataParams.saveFileAll().c_str());

      if (f) {
        for (StrVector::const_iterator i = _fileNames.begin();
             i != _fileNames.end(); i++) {
          std::ifstream ifs(i->c_str());
          if (ifs)
            f << ifs.rdbuf();
          else {
            // error opening a data file
            _yed->event(YahooEvent(
                _T(""), std::wstring(_T("Could not open data file " )) + (*i),
                event_error));
          }
        }

        _yed->event(YahooEvent(_T( "" ),
                               std::wstring(_T("Output file " )) +
                                   _dataParams.saveFileAll() +
                                   _T(" created successfully" ),
                               event_info));
      } else {
        // error opening the output file
        _yed->event(
            YahooEvent(_T(""),
                       std::wstring(_T("Could not open output file " )) +
                           _dataParams.saveFileAll(),
                       event_error));
      }
    }
  }
}

void CDownloading::OnTimer(UINT nIDEvent) {
  Timer timer;
  _onTimerDuration.add(timer.elapsed());
}

void CDownloading::cancel() {
  if (!_canceling) {
    for (size_t n = 0; n < _histDataVector.size(); n++)
      _histDataVector[n]->stopAsync();
    _canceling = true;
    _dataParams.dataSourcePlugin()->cancelData();
  }
}

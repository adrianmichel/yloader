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
    : m_canceling(false),
      m_symbolsIterator(si),
      Thread(L"Downloading thread"),
      m_dataParams(dataParams),
      m_symbols(symbolsList),
      m_yed(yed),
      m_ignoreSymbols(ignoreSymbols) {
  for (unsigned int n = 0; n < dataParams.threads(); n++) {
    m_histDataVector.emplace_back(std::make_shared< YahooHistData >( L"Thread "s + std::to_wstring( n ),
      m_dataParams, m_ignoreSymbols, errorSymbolsList, *m_symbolsIterator, *m_yed, m_fileNames));
  }
}

void CDownloading::run(ThreadContext* p) {
  for (auto t : m_histDataVector) {
    t->start();
  }

  while (isRunning() && isDownloading()) {
    OnTimer(0);
    Sleep(10);
  }

  if (!canceled()) {
    // successful
    if (!m_dataParams.saveFileAll().empty()) {
      // save all to output file
      std::ofstream f(m_dataParams.saveFileAll().c_str());

      if (f) {
        for (auto fileName : m_fileNames) {
          std::ifstream ifs(fileName.c_str());
          if (ifs) {
            f << ifs.rdbuf();
          }
          else {
            // error opening a data file
            m_yed->event(YahooEvent(L"", L"Could not open data file "s + fileName, event_error));
          }
        }

        m_yed->event(YahooEvent(L"", L"Output file "s + m_dataParams.saveFileAll() + L" created successfully", event_info));
      }
      else {
        // error opening the output file
        m_yed->event(YahooEvent(L"", L"Could not open output file "s + m_dataParams.saveFileAll(), event_error));
      }
    }
  }
}

void CDownloading::OnTimer(UINT nIDEvent) {
  Timer timer;
  m_onTimerDuration.add(timer.elapsed());
}

void CDownloading::cancel() {
  if (!m_canceling) {
    for (auto t: m_histDataVector) {
      t->stopAsync();
    }
    m_canceling = true;
    m_dataParams.dataSourcePlugin()->cancelData();
  }
}

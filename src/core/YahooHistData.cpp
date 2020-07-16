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
#include "YahooHistData.h"

void YahooHistData::run(ThreadContext* context) {
  LOG(log_debug, L"Starting downloading thread");
  Timer timer;
  for (const std::wstring* symbol = m_symbolsIterator.getNext(); symbol != 0 && !stopping(); symbol = m_symbolsIterator.getNext()) {
    assert(symbol != 0);
    assert(!symbol->empty());

    if (!ignoreSymbol(*symbol)) {
      try {
        const std::wstring path(makeFileName(*symbol));
        downloadSymbol(*symbol, path, m_dataParams.update());
        m_fileNames.add(path);

      }
      catch (const YahooErrorException& e) {
        m_yahooEventSink.event(YahooEvent(*symbol, e.message(), event_error));
      }
      catch (const YahooFatalErrorException& e) {
        LOG(log_error, L"Fatal error in thread, exiting thread - symbol: ", symbol);
        m_yahooEventSink.event(YahooEvent(*symbol, e.message(), event_fatal_error));
        break;
        // if fatal error break and stop execution
      }
      catch (...) {
        LOG(log_error, L"Unknown exception, stopping thread");
        m_yahooEventSink.event(YahooEvent(*symbol, L"Unknown exception, stopping thread", event_fatal_error));
        break;
      }
    }
    else {
      // ignoring symbol
      m_yahooEventSink.event(YahooEvent(*symbol, L"Ignored", event_ignored_symbol));
    }
  }
}

#define FILE_END_THRESHOLD 300
BarVectorPtr XFile::makeLastBars(const std::wstring& fileName, const DataFormatSettings& dataFormatSettings) {
  try {
    // read special file (can open con, prn etc)
    SpecialFileRead file(fileName);

    std::wifstream& f(file);
    // file should be open here (or an exception would have been thrown)
    assert(f.is_open());

    bool skip;

    // get file size
    f.seekg(0, std::ios_base::end);
    long size = f.tellg();

    // if file is larger than the threshold, limit search to the end
    if (size > FILE_END_THRESHOLD) {
      f.seekg(-FILE_END_THRESHOLD, std::ios_base::end);
      skip = true;
    }
    else {
      f.seekg(0, std::ios_base::beg);
      skip = false;
    }

    return getBars(f, skip, dataFormatSettings);
  } catch (const SpecialFileException&) {
    return BarVectorPtr();
  }
}

void YahooHistData::downloadSymbol(const std::wstring& symbol, const std::wstring& path, bool updateCurrent, bool reload) {
  // get the start date specified by the user. If all data, get an empty
  // timestamp

  //	  LOG( log_debug, _T( "Downloading data for symbol: " ) << symbol ) ;

  yloader::DateRange range(getRange());

  BarVectorPtr lastBars;

  if (updateCurrent) {
    XFile x(path, m_dataParams);
    Date firstDate = x.getFirstDate();
    Date lastDate = x.getLastDate();

#if defined(_DEBUG)
    std::wstring d1 = firstDate.to_simple_string();
    std::wstring d2 = lastDate.to_simple_string();
    std::wstring d3 = range.second.to_simple_string();
#endif

    lastBars = x.getLastBars();
    if (lastBars && !firstDate.is_not_a_date()) {
      range.first = firstDate;

      if (lastDate >= range.second) {
        m_yahooEventSink.event(YahooEvent(symbol, L"Local data up to date, nothing to load", event_info));
        return;
      }
    } else
      updateCurrent = false;
  }

  PriceDataPtr history;
  history = getData(symbol, range);
  LOG(log_info, L"getData returned, history ", history ? L" valid" : L"null");


  assert(history);

  if (history->hasInvalidBars()) {
    LOG(log_info, L"History has invalid bars, sending event");
    m_yahooEventSink.event(YahooEvent(symbol, L"Invalid bar(s): "s + history->invalidBarsToString(), event_warning));
  }


  if (updateCurrent) {
    LOG(log_info, L"In updateCurrent block");

    /*      std::wstring lastDate = lastBar.date().toString();
          std::wstring firstNewBar = (*history)[ 0 ]->date().toString();
          */
    if (!history->empty()) {
      LOG(log_info, L"History not empty, processing the update");

      const Bar newBar = *(history->first());
      LOG(log_debug, L"compared to new bar: ", newBar.toString());

      LOG(log_info, L"check if we need to reload data, lastBars: ", lastBars ? L" valid" : L"null");
      assert(lastBars);

      // only update old data if the flag is not set in General Settings
      if (!m_dataParams.dontReloadOldDataInUpdateMode() && m_dataParams.adjust() && *history != lastBars) {
        // if we're updating and adjusting, and there has been some div or split
        // during the most recent data, than do a complete download recursively,
        // to make sure we have valid data
        LOG(log_info, L"Redownloading symbol: ", symbol);
        downloadSymbol(symbol, path, false, true);
        return;
      }
      else {
        LOG(log_info, L"Erasing first ", lastBars->size(), L" bars");
        std::wstring d1 = history->first()->date().to_simple_string();
        std::wstring d2 = history->last()->date().to_simple_string();
        // erase the first n bars, which were loaded just for test
        history->eraseFirst(lastBars->size());
        //			d1 =
        //history->first()->date().to_simple_string(); 			d2 =
        //history->last()->date().to_simple_string();
      }
    }
  }

  if (history.get() != 0 && !history->empty()) {
    if (!writeData(symbol, path, history.get(), updateCurrent, reload)) {
      throw YahooErrorException(L"Could not write data to: "s + path);
    }
    else {
      size_t bars = history->count();
      std::wostringstream os;
      os << bars << (bars == 1 ? L" bar, " : L" bars, ")
         << m_dataParams.dateToString(history->date(0)) << L" - "
         << m_dataParams.dateToString(history->date(bars - 1));

      if (reload) {
        os << L", reloaded (split or dividend)";
      }
      if (updateCurrent) {
        os << L", updated";
      }

      if (history->retries() > 0) {
        os << L", " << history->retries() << (history->retries() == 1 ? L" retry" : L" retries");
      }

      m_yahooEventSink.event(YahooEvent(symbol, os.str(), event_info));
    }
  }
  else {
    m_yahooEventSink.event(YahooEvent(symbol, L"No data loaded", event_info));
  }
}

void FileNameCreateSubdir::createDir(const std::wstring& path) const {
  CFileFind ff;
  if (!(ff.FindFile(path.c_str()) && ff.FindNextFile() && ff.IsDirectory())) {
    CreateDirectory(path.c_str(), 0);
  }
}

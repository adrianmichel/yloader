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
#include "YahooHistData.h"

void YahooHistData::run(ThreadContext* context) {
  LOG(log_debug, _T( "Starting downloading thread" ));
  Timer timer;
  for (const std::wstring* symbol = _symbolsIterator.getNext();
       symbol != 0 && !stopping() &&
       (/*enableExtraFeatures() || */ _symbolsIterator.getCurrentIndex() <=
        MAX_SYMBOLS_USER);
       symbol = _symbolsIterator.getNext()) {
    assert(symbol != 0);
    assert(!symbol->empty());

    if (!ignoreSymbol(*symbol)) {
      try {
        const std::wstring path(makeFileName(*symbol));
        downloadSymbol(*symbol, path, _dataParams.update());
        _fileNames.add(path);

      } catch (const YahooErrorException& e) {
        _yahooEventSink.event(YahooEvent(*symbol, e.message(), event_error));
      } catch (const YahooFatalErrorException& e) {
        LOG(log_info,
            _T( "Fatal error in thread, exiting thread - symbol: " ) << symbol)
        _yahooEventSink.event(
            YahooEvent(*symbol, e.message(), event_fatal_error));
        break;
        // if fatal error break and stop execution
      } catch (...) {
        LOG(log_error, _T( "Unknown exception, stopping thread" ))
        _yahooEventSink.event(
            YahooEvent(*symbol, _T( "Unknown exception, stopping thread" ),
                       event_fatal_error));
        break;
      }
    } else {
      // ignoring symbol
      _yahooEventSink.event(
          YahooEvent(*symbol, _T( "Ignored" ), event_ignored_symbol));
    }
  }

  {
#ifdef _DEBUG
    std::wostringstream o;
    o << _T( "Exiting " ) << name() << std::endl;
    yloader::tsprint_debug(o);
#endif
  }
}

#define FILE_END_THRESHOLD 300
BarVectorPtr XFile::makeLastBars(const std::wstring& fileName,
                                 const DataFormatSettings& dataFormatSettings) {
  try {
    // read special file (can open con, prn etc)
    SpecialFileRead file(fileName);

    yloader::t_ifstream& f(file);
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
    } else {
      f.seekg(0, std::ios_base::beg);
      skip = false;
    }

    return getBars(f, skip, dataFormatSettings);
  } catch (const SpecialFileException&) {
    return BarVectorPtr();
  }
}

void YahooHistData::downloadSymbol(const std::wstring& symbol,
                                   const std::wstring& path, bool updateCurrent,
                                   bool reload) {
  // get the start date specified by the user. If all data, get an empty
  // timestamp

  //	  LOG( log_debug, _T( "Downloading data for symbol: " ) << symbol ) ;

  yloader::DateRange range(getRange());

  BarVectorPtr lastBars;

  if (updateCurrent) {
    XFile x(path, _dataParams);
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
        _yahooEventSink.event(
            YahooEvent(symbol, _T( "Local data up to date, nothing to load" ),
                       event_info));
        return;
      }
    } else
      updateCurrent = false;
  }

  PriceDataPtr history;
  history = getData(symbol, range);

  assert(history);

  if (history->hasInvalidBars())
    _yahooEventSink.event(YahooEvent(
        symbol,
        std::wstring(_T( "Invalid bar(s): " )) + history->invalidBarsToString(),
        event_warning));

  if (updateCurrent) {
    /*      std::wstring lastDate = lastBar.date().toString();
          std::wstring firstNewBar = (*history)[ 0 ]->date().toString();
          */
    if (!history->empty()) {
      //        LOG( log_debug, _T( "last bar in update mode: " ) <<
      //        lastBar.toString() );
      const Bar newBar = *(history->first());
      LOG(log_debug, _T( "compared to new bar: " ) << newBar.toString());

      assert(lastBars);

      // only update old data if the flag is not set in General Settings
      if (!_dataParams.dontReloadOldDataInUpdateMode() &&
          _dataParams.adjust() && *history != lastBars) {
        // if we're updating and adjusting, and there has been some div or split
        // during the most recent data, than do a complete download recursively,
        // to make sure we have valid data
        downloadSymbol(symbol, path, false, true);
        return;
      } else {
        std::wstring d1 = history->first()->date().to_simple_string();
        std::wstring d2 = history->last()->date().to_simple_string();
        // erase the first n bars, which was loaded just for test
        history->eraseFirst(lastBars->size());
        //			d1 =
        //history->first()->date().to_simple_string(); 			d2 =
        //history->last()->date().to_simple_string();
      }
    }
  }

  if (history.get() != 0 && !history->empty()) {
    std::wostringstream os;

    if (!writeData(symbol, path, history.get(), updateCurrent, reload)) {
      os << _T( "Could not write data to: " ) << path;
      throw YahooErrorException(os.str());
    } else {
      unsigned int bars = history->count();

      os << bars << (bars == 1 ? _T( " bar, " ) : _T( " bars, " ))
         << _dataParams.dateToString(history->date(0)) << _T( " - " )
         << _dataParams.dateToString(history->date(bars - 1));

      if (reload) os << _T( ", reloaded (split or dividend)" );
      if (updateCurrent) os << _T( ", updated" );

      if (history->retries() > 0)
        os << _T( ", " ) << history->retries()
           << (history->retries() == 1 ? _T( " retry" ) : _T( " retries" ));

      _yahooEventSink.event(YahooEvent(symbol, os.str(), event_info));
    }
  } else
    _yahooEventSink.event(
        YahooEvent(symbol, _T( "No data loaded" ), event_info));
}

void FileNameCreateSubdir::createDir(const std::wstring& path) const {
  CFileFind ff;
  if (!(ff.FindFile(path.c_str()) && ff.FindNextFile() && ff.IsDirectory()))
    CreateDirectory(path.c_str(), 0);
}

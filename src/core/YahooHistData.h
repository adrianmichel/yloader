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

constexpr auto MAX_SYMBOLS_USER = 100000000;

#include "yahooeventsink.h"
#include "dataparams.h"
#include <shlwapi.h>
#include "filenames.h"
#include <thread.h>
#include "yahoodata.h"
#include <miscfile.h>

using namespace yloader;

using  DateTimePtrX = std::shared_ptr<DateTime>;

// used to get the first and last date in the file used when updating
class XFile {
 private:
  BarVectorPtr m_lastBars;

 public:
  XFile(const std::wstring& fileName,
        const DataFormatSettings& dataFormatSettings) {
    //    _firstDate = getFirstDate( fileName, dataFormatSettings );
    m_lastBars = makeLastBars(fileName, dataFormatSettings);
  }

 private:
  BarVectorPtr makeLastBars(const std::wstring& fileName, const DataFormatSettings& dataFormatSettings);

  bool comment(const std::wstring& str) {
    return str.at(0) == L'#' || str.length() > 1 &&  str.at(0) == L'/' && str.at(1) == L'/';
  }

  /**
   * Parse the file and return bars from lines
   *
   * @param _file input file stream. The current position
   * may be set anywhere
   * @param skip
   * @exception BarException
   */
  BarVectorPtr getBars(std::wifstream& _file, bool skip, const DataFormatSettings& dataFormatSettings) {
    BarVectorPtr bbv(std::make_shared<BarVector>());

    std::wstring str;
    do {
      if (skip) {
        std::getline(_file, str);
        skip = false;
      }
      std::getline(_file, str);

      if (!str.empty() && !comment(str)) {
        try {
          BarPtr bb(std::make_shared<Bar>(str, dataFormatSettings));
          if (!bb->date().isNotADate()) {
            bbv->push_back(bb);
          }
        }
        catch (...) {
        }
      }
    } while (!_file.eof());

    return bbv;
  }

 public:
  Date getFirstDate() const {
    return m_lastBars && !m_lastBars->empty() ? (*m_lastBars->begin())->date() : Date();
  }
  Date getLastDate() const {
    return m_lastBars && !m_lastBars->empty() ? (*m_lastBars->rbegin())->date() : Date();
  }
  //  Date getFirstDate() const { return _firstDate; }
  const BarVectorPtr getLastBars() const { return m_lastBars; }
};

class YahooException {
 private:
  const std::wstring m_message;

 protected:
  YahooException(const std::wstring& message) : m_message(message) {}

 public:
  const std::wstring& message() const { return m_message; }
};

class YahooErrorException : public YahooException {
 public:
  YahooErrorException(const std::wstring& message) : YahooException(message) {}
};

class YahooFatalErrorException : public YahooException {
 public:
  YahooFatalErrorException(const std::wstring& message)
      : YahooException(message) {}
};

class SymbolsListIterator {
 private:
  StrList* m_symbols;
  StrList::const_iterator m_it;
  mutable std::mutex m_mx;
  size_t m_count;

 public:
  SymbolsListIterator(StrList* symbols)
      : m_symbols(symbols), m_it(symbols->begin()), m_count(0) {
    assert(m_symbols != 0);
  }

  const std::wstring* getNext() {
    assert(m_symbols != 0);
    std::scoped_lock lock(m_mx);

    if (m_it != m_symbols->end()) {
      m_count++;
      return &(*m_it++);
    }
    else {
      return 0;
    }
  }

  const std::wstring* getCurrentSymbol() const {
    assert(m_symbols != 0);
    std::scoped_lock lock(m_mx);

    return m_it != m_symbols->end() ? &(*m_it) : 0;
  }

  size_t getCurrentIndex() const {
    assert(m_symbols != 0);
    std::scoped_lock lock(m_mx);

    return m_count;
  }

  size_t size() const {
    assert(m_symbols != 0);
    return m_symbols->size();
  }
};

using SymbolsListIteratorPtr = std::shared_ptr<SymbolsListIterator>;
using SymbolsListBase = std::set<std::wstring>;

class SymbolsList : public SymbolsListBase {
 private:
  std::mutex m_mx;

 public:
  void add(const std::wstring& symbol) {
    std::scoped_lock lock(m_mx);

    if (!symbol.empty()) {
      __super::insert(yloader::to_lower_case(symbol));
    }
  }
};

using SymbolsListPtr = std::shared_ptr<SymbolsList>;

class WriteFileSymbolsList : public SymbolsList {
 private:
  const std::wstring m_fileName;

 public:
  WriteFileSymbolsList(const std::wstring& fileName) : m_fileName(fileName) {}

  bool write(bool append = false) {
    yloader::SpecialFileWrite file(m_fileName, append);

    std::wofstream& ofs(file);

    if (ofs) {
      for (auto symbol : *this) {
        ofs << symbol << L"\n";
      }
      return true;
    }
    else {
      return false;
    }
  }
};

using WriteFileSymbolsListPtr = std::shared_ptr<WriteFileSymbolsList>;

class FileNameCreateSubdir : public FileName {
 public:
  FileNameCreateSubdir(bool createSubdir)
      // the base class has the constructor flag - flatData, which is the
      // opposite of createSubdir
      : FileName(!createSubdir) {}
  virtual void createDir(const std::wstring& path) const;
};

class YahooHistData : public yloader::Thread {
 private:
  SymbolsListIterator& m_symbolsIterator;
  const DataParams& m_dataParams;
  YahooEventSink& m_yahooEventSink;
  SymbolsList* m_errorSymbolsList;
  yloader::UniqueSymbolsSetPtr m_ignoreSymbols;

  std::wstring m_currentSymbol;
  unsigned int m_count;
  FileNames& m_fileNames;

 public:
  YahooHistData(const std::wstring& threadName, const DataParams& dataParams, UniqueSymbolsSetPtr ignoreSymbols,
                SymbolsList* errorSymbolsList, SymbolsListIterator& symbolsIterator, YahooEventSink& yes, FileNames& fileNames)
      : Thread(threadName),
        m_dataParams(dataParams),
        m_symbolsIterator(symbolsIterator),
        m_yahooEventSink(yes),
        m_fileNames(fileNames),
        m_errorSymbolsList(errorSymbolsList),
        m_ignoreSymbols(ignoreSymbols) {}

  ~YahooHistData(void) {
  }

  bool ignoreSymbol(const std::wstring& symbol) {
    return m_ignoreSymbols && !m_ignoreSymbols->empty() &&
           m_ignoreSymbols->hasSymbol(symbol);
  }

  PriceDataPtr getData(const std::wstring& symbol, const yloader::DateRange& dateRange, unsigned int retries = 0) {
    try {
      //			PriceDataPtr ypd;
      //      if( _dataParams.dataSource() == 0 )
      //      {
      //			YahooData yd;
      PriceDataPtr ypd = PriceDataPtr(new PriceData(symbol, dateRange, retries, m_dataParams));
      /*	_dataParams.dataSourcePlugin().get(),
         _dataParams.adjust(), _dataParams.verify(),
         _dataParams.volume0invalid(), _dataParams.invalidDataHandling(), _dataParams.volumeMultiplier()) );
       */
      ypd->init(m_dataParams.period());
      //      }
      /*      else if( _dataParams.dataSource() == 1 )
            {
              ypd = PriceDataPtr( new PriceData( GoogleData(), symbol,
         _dataParams.adjust(), _dataParams.verify(),
                                                         _dataParams.volume0invalid(),
         _dataParams.invalidDataHandling(), _dataParams.volumeMultiplier(),
         retries ) );
            }
      */
      LOG(log_info, L"Exiting getData");
      return ypd;
    }
    catch (const PriceDataException& e) {
      if (m_errorSymbolsList != 0) {
        m_errorSymbolsList->add(symbol);
      }
      throw YahooErrorException(e.message());
    }
    catch (const SettingNotSupportedException& e) {
      throw YahooFatalErrorException(e.message());
    }
    catch (const RegexFormatException& e) {
      throw YahooFatalErrorException(e.message());
    }
    catch (const BadDataFormatException& e) {
      if (retries < 2) {
        //        MessageBox( 0, "retry", "retry", MB_OK );
        PriceDataPtr data = getData(symbol, dateRange, retries + 1);
        LOG(log_info, L"Exiting getData from BadDataFormatException catch block");
        return data;
      }
      else {
        LOG(log_error, L"Bad format exception: ", e.getData());
        throw YahooErrorException(L"Data in unknown format: "s + e.getData());
      }
    }
  }

 private:
  const std::wstring makeFileName(const std::wstring& symbol) const {
    FileNameCreateSubdir x(m_dataParams.createSubdirs());
    std::wstring fileName = x.makePath(m_dataParams.outputPath(), symbol, m_dataParams.getFileName(symbol));

    return fileName;
  }

  bool writeData(const std::wstring& symbol, const std::wstring& path, const PriceData* history, bool update, bool reload) {
    bool wroteHeader = false;
    // first handle the file header
    const std::wstring& fh(m_dataParams.fileHeader());
    if ((!update || reload) && !fh.empty()) {
      // for header, open as binary, because the
      // lines end in \r\n, so they don't need
      // the text transformation that happens
      // in text mode
      try {
        SpecialFileWrite f(path, false, true);

        std::wofstream& of(f);

        of << fh;
        if (*fh.rbegin() != L'\n') {
          of << L"\r\n";
        }
        // close the binary file
        wroteHeader = true;
      }
      catch (SpecialFileException&) {
        return false;
      }
    }

    SpecialFileWritePtr o;

    try {
      if (update || wroteHeader) {
        o = SpecialFileWritePtr(new SpecialFileWrite(path.c_str(), true));
      }
      else {
        o = SpecialFileWritePtr(new SpecialFileWrite(path.c_str()));
      }

      history->toString(*o, m_dataParams);
      return true;
    }
    catch (const SpecialFileException&) {
      return false;
    }
  }

 protected:
  void run(ThreadContext* context = 0);

 public:
  class Duration {
   private:
    TimeDuration m_duration;
    mutable std::mutex m_mx;

   public:
    Duration() : m_duration(0, 0, 0, 0) {}
    void add(double secs) {
      std::scoped_lock lock(m_mx);

      m_duration += Seconds(secs);
    }

    void reset() { m_duration = Seconds(0.0); }

    const TimeDuration& totalDuration() const { return m_duration; }
    const std::wstring toString() const { return m_duration.toString(); }
  };

 private:
  Date getOldest() {
    return yloader::Date(m_dataParams.dataSourcePlugin()->firstDate());
  }

  yloader::DateRange getRange() {
    Date oldest(getOldest());

    Date start(m_dataParams.allData() ? oldest : m_dataParams.start());
    // get the en date specified by the user. If download all data is selected,
    // get the current date
    Date end(m_dataParams.allData() ? LocalTimeSec().date() : m_dataParams.end());
    // if start date > end date error, throw exception

    /*	assert( !end.isNotADate() );
            assert( !start.isNotADate() );
    */
    if (!start.isNotADate() && !end.isNotADate() && start > end) {
      throw YahooErrorException(L"No bars loaded - start date > end date");
    }

    return yloader::DateRange(start, end);
  }

 protected:
  void downloadSymbol(const std::wstring& symbol, const std::wstring& path, bool updateCurrent, bool reload = false);
};

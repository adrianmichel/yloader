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

#define MAX_SYMBOLS_USER 100000000

#include "yahooeventsink.h"
#include "dataparams.h"
#include <shlwapi.h>
#include "filenames.h"
#include <thread.h>
using std::vector;
#include "yahoodata.h"
#include <miscfile.h>

using namespace yloader;

typedef ManagedPtr<DateTime> DateTimePtrX;

// used to get the first and last date in the file used when updating
class XFile {
 private:
  BarVectorPtr _lastBars;

 public:
  XFile(const std::wstring& fileName,
        const DataFormatSettings& dataFormatSettings) {
    //    _firstDate = getFirstDate( fileName, dataFormatSettings );
    _lastBars = makeLastBars(fileName, dataFormatSettings);
  }

 private:
  BarVectorPtr makeLastBars(const std::wstring& fileName,
                            const DataFormatSettings& dataFormatSettings);

  bool comment(const std::wstring& str) {
    return str.at(0) == _TCHAR('#') || str.length() > 1 &&
                                           str.at(0) == _TCHAR('/') &&
                                           str.at(1) == _TCHAR('/');
  }

  /**
   * Parse the file and return bars from lines
   *
   * @param _file input file stream. The current position
   * may be set anywhere
   * @param skip
   * @exception BarException
   */
  BarVectorPtr getBars(yloader::t_ifstream& _file, bool skip,
                       const DataFormatSettings& dataFormatSettings) {
    BarVectorPtr bbv(boost::make_shared<BarVector>());

    std::wstring str;
    do {
      if (skip) {
        std::getline(_file, str);
        skip = false;
      }
      std::getline(_file, str);

      if (!str.empty() && !comment(str)) {
        try {
          BarPtr bb(boost::make_shared<Bar>(str, dataFormatSettings));
          if (!bb->date().isNotADate()) bbv->push_back(bb);
        } catch (...) {
        }
      }
    } while (!_file.eof());

    return bbv;
  }

 private:
 public:
  Date getFirstDate() const {
    return _lastBars && !_lastBars->empty() ? (*_lastBars->begin())->date()
                                            : Date();
  }
  Date getLastDate() const {
    return _lastBars && !_lastBars->empty() ? (*_lastBars->rbegin())->date()
                                            : Date();
  }
  //  Date getFirstDate() const { return _firstDate; }
  const BarVectorPtr getLastBars() const { return _lastBars; }
};

class YahooException {
 private:
  const std::wstring _message;

 protected:
  YahooException(const std::wstring& message) : _message(message) {}

 public:
  const std::wstring& message() const { return _message; }
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
  StrList* _symbols;
  StrList::const_iterator _i;
  mutable yloader::Mutex _m;
  size_t _count;

 public:
  SymbolsListIterator(StrList* symbols)
      : _symbols(symbols), _i(symbols->begin()), _count(0) {
    assert(_symbols != 0);
  }

  const std::wstring* getNext() {
    assert(_symbols != 0);
    Lock lock(_m);

#ifdef _DEBUG
    std::wostringstream o;
    o << _T( "getNext returning symbol " ) << _count << std::endl;
    tsprint_debug(o);
#endif  //_DEBUG

    if (_i != _symbols->end()) {
      _count++;
      return &(*_i++);
    } else
      return 0;
  }

  const std::wstring* getCurrentSymbol() const {
    assert(_symbols != 0);
    Lock lock(_m);

    return _i != _symbols->end() ? &(*_i) : 0;
  }

  size_t getCurrentIndex() const {
    assert(_symbols != 0);
    Lock lock(_m);

    return _count;
  }

  unsigned int size() const {
    assert(_symbols != 0);
    return _symbols->size();
  }
};

typedef yloader::ManagedPtr<SymbolsListIterator> SymbolsListIteratorPtr;

typedef std::set<std::wstring> SymbolsListBase;

class SymbolsList : public SymbolsListBase {
 private:
  yloader::Mutex m_mx;

 public:
  void add(const std::wstring& symbol) {
    yloader::Lock lock(m_mx);

    if (!symbol.empty()) __super::insert(yloader::to_lower_case(symbol));
  }
};

typedef boost::shared_ptr<SymbolsList> SymbolsListPtr;

class WriteFileSymbolsList : public SymbolsList {
 private:
  const std::wstring m_fileName;

 public:
  WriteFileSymbolsList(const std::wstring& fileName) : m_fileName(fileName) {}

  bool write(bool append = false) {
    yloader::SpecialFileWrite file(m_fileName, append);

    yloader::t_ofstream& ofs(file);

    if (ofs) {
      for (SymbolsList::const_iterator i = __super::begin();
           i != __super::end(); ++i) {
        ofs << *i << _T( "\n" );
      }
      return true;
    } else
      return false;
  }
};

typedef boost::shared_ptr<WriteFileSymbolsList> WriteFileSymbolsListPtr;

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
  SymbolsListIterator& _symbolsIterator;
  const DataParams& _dataParams;
  YahooEventSink& _yahooEventSink;
  SymbolsList* _errorSymbolsList;
  yloader::UniqueSymbolsSetPtr _ignoreSymbols;

  std::wstring _currentSymbol;
  unsigned int _count;
  FileNames& _fileNames;

 public:
  YahooHistData(const std::wstring& threadName, const DataParams& dataParams,
                UniqueSymbolsSetPtr ignoreSymbols,
                SymbolsList* errorSymbolsList,
                SymbolsListIterator& symbolsIterator, YahooEventSink& yes,
                FileNames& fileNames) throw(YahooException)
      : Thread(threadName),
        _dataParams(dataParams),
        _symbolsIterator(symbolsIterator),
        _yahooEventSink(yes),
        _fileNames(fileNames),
        _errorSymbolsList(errorSymbolsList),
        _ignoreSymbols(ignoreSymbols) {}

  ~YahooHistData(void) {
#ifdef _DEBUG
    yloader::tsprint_debug(_T( "In destructor\n" ));
#endif
  }

  bool ignoreSymbol(const std::wstring& symbol) {
    return _ignoreSymbols && !_ignoreSymbols->empty() &&
           _ignoreSymbols->hasSymbol(symbol);
  }

  PriceDataPtr getData(const std::wstring& symbol,
                       const yloader::DateRange& dateRange,
                       unsigned int retries = 0) throw(YahooException) {
    try {
      //			PriceDataPtr ypd;
      //      if( _dataParams.dataSource() == 0 )
      //      {
      //			YahooData yd;
      PriceDataPtr ypd =
          PriceDataPtr(new PriceData(symbol, dateRange, retries, _dataParams));
      /*																											_dataParams.dataSourcePlugin().get(),
         _dataParams.adjust(), _dataParams.verify(),
                                                                                                                                                                                                       _dataParams.volume0invalid(), _dataParams.invalidDataHandling(), _dataParams.volumeMultiplier()) );
       */
      ypd->init(_dataParams.period());
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
      return ypd;
    } catch (const PriceDataException& e) {
      if (_errorSymbolsList != 0) _errorSymbolsList->add(symbol);
      throw YahooErrorException(e.message());
    } catch (const SettingNotSupportedException& e) {
      throw YahooFatalErrorException(e.message());
    } catch (const RegexFormatException& e) {
      throw YahooFatalErrorException(e.message());
    } catch (const BadDataFormatException& e) {
      if (retries < 2) {
        //        MessageBox( 0, "retry", "retry", MB_OK );
        return getData(symbol, dateRange, retries + 1);
      } else {
        LOG(log_error, _T( "Bad format exception: " ) << e.getData())
        throw YahooErrorException(std::wstring(_T( "Data in unknown format: " ))
                                  << e.getData());
      }
    }
  }

 private:
  const std::wstring makeFileName(const std::wstring& symbol) const {
    FileNameCreateSubdir x(_dataParams.createSubdirs());
    std::wstring fileName = x.makePath(_dataParams.outputPath(), symbol,
                                       _dataParams.getFileName(symbol));

    return fileName;
  }

  bool writeData(const std::wstring& symbol, const std::wstring& path,
                 const PriceData* history, bool update, bool reload) {
    bool wroteHeader = false;
    // first handle the file header
    const std::wstring& fh(_dataParams.fileHeader());
    if ((!update || reload) && !fh.empty()) {
      // for header, open as binary, because the
      // lines end in \r\n, so they don't need
      // the text transformation that happens
      // in text mode
      try {
        SpecialFileWrite f(path, false, true);

        yloader::t_ofstream& of(f);

        of << fh;
        if (*fh.rbegin() != _TCHAR('\n')) of << _T( "\r\n" );
        // close the binary file
        wroteHeader = true;
      } catch (SpecialFileException&) {
        return false;
      }
    }

    SpecialFileWritePtr o;

    try {
      if (update || wroteHeader)
        o = SpecialFileWritePtr(new SpecialFileWrite(path.c_str(), true));
      else
        o = SpecialFileWritePtr(new SpecialFileWrite(path.c_str()));

      history->toString(*o, _dataParams);
      return true;
    } catch (const SpecialFileException&) {
      return false;
    }
  }

 protected:
  void run(ThreadContext* context = 0);

 public:
  class Duration {
   private:
    TimeDuration _duration;
    mutable Mutex _m;

   public:
    Duration() : _duration(0, 0, 0, 0) {}
    void add(double secs) {
      Lock lock(_m);

      _duration += Seconds(secs);
    }

    void reset() { _duration = Seconds(0.0); }

    const TimeDuration& totalDuration() const { return _duration; }
    const std::wstring toString() const { return _duration.toString(); }
  };

 private:
  Date getOldest() {
    return yloader::Date(_dataParams.dataSourcePlugin()->firstDate());
  }

  yloader::DateRange getRange() {
    Date oldest(getOldest());

    Date start(_dataParams.allData() ? oldest : _dataParams.start());
    // get the en date specified by the user. If download all data is selected,
    // get the current date
    Date end(_dataParams.allData() ? LocalTimeSec().date() : _dataParams.end());
    // if start date > end date error, throw exception

    /*	assert( !end.isNotADate() );
            assert( !start.isNotADate() );
    */
    if (!start.isNotADate() && !end.isNotADate() && start > end)
      throw YahooErrorException(_T( "No bars loaded - start date > end date" ));

    return yloader::DateRange(start, end);
  }

 protected:
  void downloadSymbol(const std::wstring& symbol, const std::wstring& path,
                      bool updateCurrent, bool reload = false);
};

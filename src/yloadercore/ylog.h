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

#include <iostream>
#include "dataparams.h"
#include "yahooeventsink.h"
#include "yahoohistdata.h"

class LogFileException {
 private:
  const std::wstring _fileName;

 public:
  LogFileException(const std::wstring& fileName) : _fileName(fileName) {}

  const std::wstring& fileName() const { return _fileName; }
};

class YahooSessionLog : public YahooEventSink {
 private:
  const DataParams& _dataParams;
  const std::wstring& _logFileName;
  yloader::t_ofstream _ofs;
  bool _logging;
  bool _logOnlyErrors;
  Mutex _m;

 public:
  YahooSessionLog(const DataParams& dataParams) throw(LogFileException)
      : _dataParams(dataParams),
        _logFileName(_dataParams.logFile()),
        _logging(!_dataParams.logFile().empty()),
        _logOnlyErrors(dataParams.logOnlyErrors()) {
    if (_logging) {
      _ofs.open(_logFileName.c_str(),
                std::ios_base::out |
                    (_dataParams.appendToLogFile() ? std::ios_base::app : 0));
      if (!_ofs) throw LogFileException(_logFileName);

      _ofs
          << _T( "***********************************************************************************" )
          << std::endl;
      _ofs << _T( "**** " ) << LocalTimeSec().to_simple_string() << TAB
           << _T( "Start of session ****" ) << std::endl
           << std::endl;
      _ofs << _T( "Session parameters:" ) << std::endl;
      _ofs << _T( "Data source: " ) << DOUBLE_TAB
           << _dataParams.dataSourcePlugin()->name() << std::endl;
      _ofs << _T( "Symbols file:" ) << DOUBLE_TAB
           << dataParams.symbolsFileName() << std::endl;
      _ofs << _dataParams.toString();
      _ofs << std::endl;
      _ofs << LocalTimeSec().to_simple_string() << TAB
           << _T( "Start downloading data - " )
           << (_logOnlyErrors ? _T( "logging only error events" )
                              : _T( "logging all session events" ))
           << std::endl
           << std::endl;
    }
  }

  ~YahooSessionLog() {
    _ofs << std::endl
         << _T( "**** " ) << LocalTimeSec().to_simple_string() << TAB
         << _T( "End of session ****" ) << std::endl
         << std::endl;
  }

  virtual void event(const YahooEvent& event) {
    Lock lock(_m);
    if (_logging &&
        (_logOnlyErrors && event.type() > event_info || !_logOnlyErrors))
      _ofs << LocalTimeSec().to_simple_string() << TAB << event.toString()
           << std::endl;
  }

  void setStatistics(const Statistics& statistics, bool canceled) {
    _ofs << std::endl
         << _T( "Session statistics" ) << std::endl
         << std::endl
         << statistics.toString() << _T( "Session:\t\t\t" )
         << (canceled ? _T( "Canceled" ) : _T( "Completed" )) << std::endl;
  }

  /*  void setDuration( double duration )
    {
      _ofs << std::endl << _T( "Session duration: " ) << Seconds( duration
    ).toString() << std::endl;
    }
  */
};

typedef boost::shared_ptr<YahooSessionLog> YahooSessionLogPtr;

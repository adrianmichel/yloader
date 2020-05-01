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

#include <iostream>
#include "dataparams.h"
#include "yahooeventsink.h"
#include "yahoohistdata.h"

class LogFileException {
 private:
  const std::wstring m_fileName;

 public:
  LogFileException(const std::wstring& fileName) : m_fileName(fileName) {}

  const std::wstring& fileName() const { return m_fileName; }
};

class YahooSessionLog : public YahooEventSink {
 private:
  const DataParams& m_dataParams;
  const std::wstring& m_logFileName;
  std::wofstream m_ofs;
  bool m_logging;
  bool m_logOnlyErrors;
  std::mutex m_mx;

 public:
  YahooSessionLog(const DataParams& dataParams)
      : m_dataParams(dataParams),
        m_logFileName(m_dataParams.logFile()),
        m_logging(!m_dataParams.logFile().empty()),
        m_logOnlyErrors(dataParams.logOnlyErrors()) {
    if (m_logging) {
      m_ofs.open(m_logFileName.c_str(), std::ios_base::out | (m_dataParams.appendToLogFile() ? std::ios_base::app : 0));
      if (!m_ofs) {
        throw LogFileException(m_logFileName);
      }

      m_ofs << L"***********************************************************************************"  << std::endl;
      m_ofs << L"**** " << LocalTimeSec().to_simple_string() << TAB << L"Start of session ****" << std::endl << std::endl;
      m_ofs << L"Session parameters:" << std::endl;
      m_ofs << L"Data source: " << DOUBLE_TAB << m_dataParams.dataSourcePlugin()->name() << std::endl;
      m_ofs << L"Symbols file:" << DOUBLE_TAB << dataParams.symbolsFileName() << std::endl;
      m_ofs << m_dataParams.toString() << std::endl;
      m_ofs << LocalTimeSec().to_simple_string() << TAB << L"Start downloading data - ";
      m_ofs << (m_logOnlyErrors ? L"logging only error events" : L"logging all session events") << std::endl << std::endl;
    }
  }

  ~YahooSessionLog() {
    m_ofs << std::endl << L"**** " << LocalTimeSec().to_simple_string() << TAB;
    m_ofs << L"End of session ****" << std::endl << std::endl;
  }

  virtual void event(const YahooEvent& event) {
    std::scoped_lock lock(m_mx);
    if (m_logging && (m_logOnlyErrors && event.type() > event_info || !m_logOnlyErrors)) {
      m_ofs << LocalTimeSec().to_simple_string() << TAB << event.toString() << std::endl;
    }
  }

  void setStatistics(const Statistics& statistics, bool canceled) {
    m_ofs << std::endl
         << L"Session statistics" << std::endl
         << std::endl
         << statistics.toString() << L"Session:\t\t\t"
         << (canceled ? L"Canceled" : L"Completed") << std::endl;
  }
};

using YahooSessionLogPtr = std::shared_ptr<YahooSessionLog>;

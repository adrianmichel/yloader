/*
Copyright(C) 2017  YLoader.com

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <sys/timeb.h>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "misc.h"

#ifdef LOG_EXPORTS
#define LOG_API __declspec(dllexport)
#else
#define LOG_API __declspec(dllimport)
#endif

/**/

inline std::wstring hexstr(unsigned int n, unsigned int digits) {
  std::wostringstream os;
  os << _T( "0x" ) << std::setw(digits) << std::setfill(_TCHAR('0')) << std::hex
     << n;
  return os.str();
}

inline std::wstring hexstr(HWND n, unsigned int digits) {
  return hexstr(reinterpret_cast<unsigned int>(n), digits);
}

inline std::wstring hexstr(const void* n, unsigned int digits) {
  return hexstr((unsigned int)n, digits);
}

#define HEXSTR(n, digits) hexstr(n, digits).c_str()
#define HEXSTR4(n) HEXSTR(n, 4)
#define HEXSTR8(n) HEXSTR(n, 8)
#define HEXSTR5(n) HEXSTR(n, 5)

/**/

enum Level { log_debug, log_info, log_error, log_assert, log_none };

namespace yloader {
class LogException {};

// this is the diagnostic logging

typedef yloader::ManagedPtr<yloader::t_ostream> OstreamPtr;

#pragma message(__TODO__ "implement log file limit")

class LOG_API Log {
 private:
  static Level _globalLevel;
  Level _level;
  Level _thisLevel;

  static std::wstring _logToFile;
  static bool _logToDebugOutput;
  static bool _logToConsole;
  static yloader::Mutex _mutex;

 public:
  // level 0 - everything passes
  Log(Level level, TCHAR* func) : _level(level) {}

  // start session
  Log() : _level(log_none) {}

  ~Log() {}

  static std::wstring levelToString(Level level) {
    switch (level) {
      case log_debug:
        return _T( "L0" );
      case log_info:
        return _T("L1");
      case log_error:
        return _T("L2");
      case log_assert:
        return _T("L3");
      case log_none:
        return _T("");
      default:
        assert(false);
        return _T("");
    }
  }

  static bool checkLevel(Level level) {
    Lock lock(_mutex);
    return _globalLevel <= level;
  }

  static Level level() {
    Lock lock(_mutex);
    return _globalLevel;
  }

  static std::wstring header(Level level, TCHAR* func) {
    assert(func != 0);
    std::wostringstream o;
    o << _T( "[" ) << timeStamp(true) << _T( " " )
      << HEXSTR5(GetCurrentThreadId()) << _T( "," );
    o << HEXSTR5(GetCurrentProcessId()) << " " << levelToString(level)
      << _T( "] [" ) << func << _T( "] " );
    return o.str();
  }

  static void setLevel(Level level);

  static void setDebugLevel() {
    Lock lock(_mutex);
    _globalLevel = log_debug;
  }

  static void setNormalLevel() {
    Lock lock(_mutex);
    _globalLevel = log_info;
  }

  static bool isDebugLevel() {
    Lock lock(_mutex);

    return _globalLevel == log_debug;
  }

  static void flipLevel();

  static void setLogToFile(const std::wstring& file) {
    Lock lock(_mutex);
    _logToFile = file;
  }
  static void setLogToConsole(bool log = true) {
    Lock lock(_mutex);
    _logToConsole = log;
  }
  static void setLogToDebugOutput(bool log = true) {
    Lock lock(_mutex);
    _logToDebugOutput = log;
  }

  static bool logToFile() {
    Lock lock(_mutex);
    return !_logToFile.empty();
  }
  static const std::wstring& getLogFileName() {
    Lock lock(_mutex);
    return _logToFile;
  }
  static bool logToConsole() {
    Lock lock(_mutex);
    return _logToConsole;
  }
  static bool logToDebugOutput() {
    Lock lock(_mutex);
    return _logToDebugOutput;
  }

  static void xlog(Level level, TCHAR* function, const std::wstring& value);
  static void xfilelog(const std::wstring& fileName, Level level,
                       TCHAR* function, const std::wstring& value);

 protected:
  virtual t_ostream& getOstream() = 0;

 public:
  template <typename T>
  t_ostream& operator<<(const T& t) {
    t_ostream& os = getOstream();

    if (_globalLevel <= _level && os) os << t;

    return os;
  }
};

class FileLog : public Log {
 private:
  std::wofstream _ofs;

 public:
  FileLog(Level level, TCHAR* func)
      : _ofs(getLogFileName().c_str(),
             std::ios_base::ate | std::ios_base::out | std::ios_base::app),
        Log(level, func) {
    if (!_ofs) {
      throw LogException();
    }
  }

  FileLog(TCHAR* fileName, Level level, TCHAR* func)
      : _ofs(fileName,
             std::ios_base::ate | std::ios_base::out | std::ios_base::app),
        Log(level, func) {
    if (!_ofs) {
      throw LogException();
    }
  }

  FileLog(const std::wstring& fileName, Level level, TCHAR* func)
      : _ofs(fileName.c_str(),
             std::ios_base::ate | std::ios_base::out | std::ios_base::app),
        Log(level, func) {
    if (!_ofs) {
      throw LogException();
    }
  }

  virtual t_ostream& getOstream() { return _ofs; }
};

}  // namespace yloader

using yloader::operator<<;

#define LOG(level, value) \
  yloader::Log::xlog(level, _T(__FUNCTION__), std::wstring() << value);
#define FILE_LOG(fileName, level, value)                    \
  yloader::Log::xfilelog(fileName, level, _T(__FUNCTION__), \
                         std::wstring() << value);


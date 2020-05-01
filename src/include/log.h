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

template< typename T > std::wstring hexstr(T n, unsigned int digits) {
  std::wostringstream os;
  os << L"0x" << std::setw(digits) << std::setfill(L'0') << std::hex << n;
  return os.str();
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
#pragma message(__TODO__ "implement log file limit")

class LOG_API Log {
 private:
  static Level m_globalLevel;
  Level m_level;
  Level m_thisLevel;

  static std::wstring m_logToFile;
  static bool m_logToDebugOutput;
  static bool m_logToConsole;
  static std::mutex m_mutex;

 public:
  // level 0 - everything passes
  Log(Level level, const wchar_t* func) : m_level(level) {}

  // start session
  Log() : m_level(log_none) {}

  ~Log() {}

  static std::wstring levelToString(Level level) {
    switch (level) {
      case log_debug:
        return L"L0";
      case log_info:
        return L"L1";
      case log_error:
        return L"L2";
      case log_assert:
        return L"L3";
      case log_none:
        return L"";
      default:
        assert(false);
        return L"";
    }
  }

  static bool checkLevel(Level level) {
    return m_globalLevel <= level;
  }

  static Level level() {
    std::scoped_lock lock(m_mutex);
    return m_globalLevel;
  }

  static std::wstring header(Level level, const wchar_t* func) {
    assert(func != 0);
    std::wostringstream o;
    o << L"[" << timeStamp(true) << L" " << HEXSTR5(GetCurrentThreadId()) << L",";
    o << HEXSTR5(GetCurrentProcessId()) << L" " << levelToString(level) << L"] [" << func << L"] ";
    return o.str();
  }

  static void setLevel(Level level);

  static void setDebugLevel() {
    std::scoped_lock lock(m_mutex);
    m_globalLevel = log_debug;
  }

  static void setNormalLevel() {
    std::scoped_lock lock(m_mutex);
    m_globalLevel = log_info;
  }

  static bool isDebugLevel() {
    std::scoped_lock lock(m_mutex);

    return m_globalLevel == log_debug;
  }

  static void flipLevel();

  static void setLogToFile(const std::wstring& file) {
    std::scoped_lock lock(m_mutex);
    m_logToFile = file;
  }
  static void setLogToConsole(bool log = true) {
    std::scoped_lock lock(m_mutex);
    m_logToConsole = log;
  }
  static void setLogToDebugOutput(bool log = true) {
    std::scoped_lock lock(m_mutex);
    m_logToDebugOutput = log;
  }

  static bool logToFile() {
    return !m_logToFile.empty();
  }
  static const std::wstring& getLogFileName() {
    return m_logToFile;
  }
  static bool logToConsole() {
    return m_logToConsole;
  }
  static bool logToDebugOutput() {
    return m_logToDebugOutput;
  }

  static void xlog(Level level, const wchar_t* function, const std::wstring& value);
  static void xfilelog(const std::wstring& fileName, Level level, const wchar_t* function, const std::wstring& value);

 protected:
  virtual std::wostream& getOstream() = 0;

 public:
  template <typename T>
  std::wostream& operator<<(const T& t) {
    std::wostream& os = getOstream();

    if (m_globalLevel <= m_level && os) {
      os << t;
    }

    return os;
  }
};

class FileLog : public Log {
 private:
  std::wofstream m_ofs;

 public:
  FileLog(Level level, const wchar_t* func)
      : m_ofs(getLogFileName().c_str(),
             std::ios_base::ate | std::ios_base::out | std::ios_base::app),
        Log(level, func) {
    if (!m_ofs) {
      throw LogException();
    }
  }

  FileLog(TCHAR* fileName, Level level, const wchar_t* func)
      : m_ofs(fileName,
             std::ios_base::ate | std::ios_base::out | std::ios_base::app),
        Log(level, func) {
    if (!m_ofs) {
      throw LogException();
    }
  }

  FileLog(const std::wstring& fileName, Level level, const wchar_t* func)
      : m_ofs(fileName.c_str(),
             std::ios_base::ate | std::ios_base::out | std::ios_base::app),
        Log(level, func) {
    if (!m_ofs) {
      throw LogException();
    }
  }

  virtual std::wostream& getOstream() { return m_ofs; }
};

}  // namespace yloader

template<typename... T > std::wstring out(T...t) {
  std::wostringstream os;
  ((os << std::forward<T>(t)), ...);
  return os.str();
}


#define LOG( level, ... ) \
  [&](const wchar_t* function, auto... args)->void{ \
  yloader::Log::xlog(level, function, out( args...)); \
}(_T(__FUNCTION__), __VA_ARGS__)


#define FILE_LOG(fileName, level, value)                    \
  yloader::Log::xfilelog(fileName, level, _T(__FUNCTION__), \
                         std::wstring() << value);


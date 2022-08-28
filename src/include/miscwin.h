/*
Copyright(C) 2020  YLoader.com

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

#include <Windows.h>
#include <ostream>
#include <sstream>
#include <string>
#include <assert.h>
#include <tchar.h>
#include <string>
#include <vector>

#include <exceptions.h>
#include <misc.h>

/* @cond */
namespace yloader {
/* @endcond */

#ifdef MISCWIN_EXPORTS
#define MISCWIN_API __declspec(dllexport)
#else
#define MISCWIN_API __declspec(dllimport)
#endif

/* @cond */
// registry related APIs

using namespace std;

#include <memory>

/* @endcond */

///////////////////////////////////
// makes a file name from its components. Takes into account existing or non
// existing \ or / or . characters
MISCWIN_API const std::wstring makeFileName(const std::wstring& path, const std::wstring& name, const std::wstring& ext);

class RunProcessException {
 private:
  DWORD m_errorCode;
  const std::wstring m_processFileName;
  const std::wstring m_cmdLine;

 public:
  RunProcessException(const std::wstring& processFileName, const std::wstring& cmdLine, DWORD errorCode)
      : m_errorCode(errorCode) {}

  const DWORD errorCode() const { return m_errorCode; }

  const std::wstring& processFileName() const { return m_processFileName; }
  const std::wstring& cmdLine() const { return m_cmdLine; }

  const std::wstring message() const {
    std::wostringstream o;

    o << L"Attempting to run \"" << m_processFileName << L"\" with command line " << m_cmdLine << L" has failed with error " << m_errorCode;
    return o.str();
  }
};

class RunProcessResult {
 private:
  bool m_timeout;
  DWORD m_exitCode;

 public:
  RunProcessResult(bool timeout, DWORD exitCode)
      : m_timeout(timeout), m_exitCode(exitCode) {}

  bool timeout() const { return m_timeout; }
  DWORD exitCode() const { return m_exitCode; }
};

using StrPairVector = const std::vector<std::pair<std::wstring, std::wstring> >;

class Environment {
 private:
  mutable bool m_valid;
  mutable std::wstring m_envStr;

  StrVector m_envStrings;

 public:
  Environment() {}

  Environment(const Environment& env) {
    m_envStrings = env.m_envStrings;
    m_valid = env.m_valid;
    m_envStr = env.m_envStr;
  }

  Environment(const StrPairVector& env)
    : m_valid(false) {
    for (const auto& e : env) {
      add(e.first, e.second);
    }
  }

  Environment(const StrVector& env) : m_valid(false) { m_envStrings = env; }

  void add(LPCTSTR name, LPCTSTR value) {
    add(name, value);
  }

  void add(const std::wstring& name, const std::wstring& value) {
    m_valid = false;
    m_envStrings.push_back(name + L'=' + value);
  }

  operator LPVOID() const {
    return m_envStr.empty() && m_envStrings.empty() ? 0 : (LPVOID)get().data();
  }

  const std::wstring& get() const {
    if (!m_valid) {
      for (const auto& envStr : m_envStrings) {
        m_envStr += envStr + L'\0';
      }

      m_envStr += L'\0';
      m_valid = true;
    }

    return m_envStr;
  }

  const std::wstring toString() const {
    std::wstring str;
    for (const auto& envStr : m_envStrings) {
      str += envStr + L"\n";
    }

    return str;
  }
};

using EnvironmentPtr = std::shared_ptr<Environment>;

MISCWIN_API const RunProcessResult
runProcess(const std::wstring& processFileName, const std::wstring& cmdLine, bool waitForProcess, const std::wstring* startingDirectory = 0,
           const Environment& env = Environment(), unsigned long timeout = 0);


class basic_debugbuf : public std::basic_stringbuf<wchar_t, std::char_traits<wchar_t>> {
  using Base = std::basic_stringbuf<wchar_t, std::char_traits<wchar_t>>;
 public:
  virtual ~basic_debugbuf() { sync(); }

 protected:
  int sync() {
    output_debug_string(Base::str().c_str());
    Base::str(std::wstring());  // Clear the string buffer

    return 0;
  }

  void output_debug_string(const wchar_t* text) {
      ::OutputDebugStringW(text);
  }
};

class basic_dostream : public std::basic_ostream<wchar_t, std::char_traits<wchar_t>> {
  using Base = std::basic_ostream < wchar_t, std::char_traits<wchar_t>>;

 public:
  basic_dostream()
      : std::basic_ostream<wchar_t, std::char_traits<wchar_t>>(new basic_debugbuf{}) {}
  ~basic_dostream() { delete Base::rdbuf(); }
};

using t_dostream = basic_dostream;

//////////////////////////////////////
/// Named mutex

class NamedMutex {
 private:
  HANDLE m_mutex;

  bool m_alreadyExists;

  DWORD m_lastError;

 public:
  NamedMutex(const std::wstring& name) : m_mutex(0) {
    m_mutex = CreateMutex(NULL, true, name.c_str());

    m_mutex != 0 ? m_lastError = GetLastError() : 0;
  }

  ~NamedMutex() {
    if (m_mutex != 0) CloseHandle(m_mutex);
  }

  bool alreadyExists() const { return m_lastError == ERROR_ALREADY_EXISTS; }
};
}  // namespace yloader

MISCWIN_API void init();
MISCWIN_API void uninit();

class InitUninit {
public:
  InitUninit() {
    init();
  }

  ~InitUninit() {
    uninit();
  }
};

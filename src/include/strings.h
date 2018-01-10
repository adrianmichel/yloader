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

#include <tchar.h>
#include <algorithm>
#include <codecvt>
#include <sstream>
#include <string>

namespace yloader {

/**\defgroup String String related types
 * Definition of string related types
 * @{
 */

// turns the argument into a string
#define STRINGIZE(x) _T(#x)

// transforms s in lower case
inline const std::wstring to_upper_case(const std::wstring& str) {
  std::wstring s(str);

  std::transform(s.begin(), s.end(), s.begin(), toupper);
  return s;
}

inline const std::wstring to_lower_case(const std::wstring& str) {
  std::wstring s(str);
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

const std::wstring whiteSpaces(_T( " \t\n\r" ));

// removes leading and trailing white space
inline const std::wstring trim(const std::wstring& str) {
  std::wstring::size_type start = str.find_first_not_of(whiteSpaces);
  std::wstring::size_type end = str.find_last_not_of(whiteSpaces);

  if (start == std::wstring::npos)
    // empty string
    return _T( "" );
  else if (end == std::wstring::npos)
    return str.substr(start);
  else
    return str.substr(start, end - start + 1);
}

inline bool isBlanc(const std::wstring& str) {
  return str.find_first_not_of(whiteSpaces) == std::wstring::npos;
}

inline const std::wstring addExtension(const std::wstring& fileName,
                                       const std::wstring& ext) {
  std::wstring result(fileName);
  if (!ext.empty())
    result += std::wstring(ext[0] == TCHAR('.') ? _T( "" ) : _T( "." )) + ext;

  return result;
}

inline const std::wstring addTailIfNotPresent(const std::wstring& str, TCHAR c,
                                              const std::wstring& match) {
  return str.empty() || match.find(*str.rbegin()) != std::wstring::npos
             ? str
             : str + c;
}

inline const std::wstring addBackSlash(const std::wstring& str) {
  return addTailIfNotPresent(str, TCHAR('\\'), std::wstring(_T("\\/")));
}

inline const std::wstring addFwdSlash(const std::wstring& str) {
  return addTailIfNotPresent(str, TCHAR('/'), std::wstring(_T("/")));
}

inline const std::wstring quote(const std::wstring& str) {
  return _T( "\"" ) + str + _T( "\"" );
}

template <typename T>
std::wstring& operator<<(std::wstring& str, const T& t) {
  std::wostringstream os;
  os << t;
  str += os.str();
  return str;
}

inline const std::wstring unescape(const std::wstring& str) {
  std::wstring unescaped;
  bool escape = false;

  for (unsigned int n = 0; n < str.length(); n++) {
    TCHAR c = str[n];

    switch (c) {
      case TCHAR('\\'):
        if (escape) {
          unescaped += c;
          escape = false;
        } else
          escape = true;
        break;
      case TCHAR('*'):
        if (escape) {
          unescaped += TCHAR(' ');
          escape = false;
        } else
          unescaped += c;
        break;
      case TCHAR('n'):
        if (escape) {
          unescaped += TCHAR('\n');
          escape = false;
        } else
          unescaped += c;
        break;
      case TCHAR('t'):
        if (escape) {
          unescaped += TCHAR('\t');
          escape = false;
        } else
          unescaped += c;
        break;
      case TCHAR('r'):
        if (escape) {
          unescaped += TCHAR('\r');
          escape = false;
        } else
          unescaped += c;
        break;
      default:
        unescaped += c;
        escape = false;
        break;
    }
  }
  return unescaped;
}

inline const std::wstring unescape(const TCHAR* str) {
  return unescape(std::wstring(str));
}

inline const std::wstring escape(const std::wstring& str) {
  std::wstring escaped;

  for (unsigned int n = 0; n < str.length(); n++) {
    switch (str[n]) {
      case TCHAR('\\'):
        escaped += _T( "\\\\" );
        break;
      case TCHAR(' '):
        escaped += _T( "\\*" );
        break;
      case TCHAR('\n'):
        escaped += _T( "\\n" );
        break;
      case TCHAR('\t'):
        escaped += _T( "\\t" );
        break;
      case TCHAR('\r'):
        escaped += _T( "\\r" );
        break;
      default:
        escaped += str[n];
        break;
    }
  }
  return escaped;
}

inline std::wstring s2ws(const std::string& str) {
  typedef std::codecvt_utf8<TCHAR> convert_typeX;
  std::wstring_convert<convert_typeX, TCHAR> converterX;

  return converterX.from_bytes(str);
}

inline std::string ws2s(const std::wstring& wstr) {
  typedef std::codecvt_utf8<wchar_t> convert_typeX;
  std::wstring_convert<convert_typeX, wchar_t> converterX;

  return converterX.to_bytes(wstr);
}

// end string related classes
//@}
}  // namespace yloader
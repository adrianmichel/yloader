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

constexpr auto whiteSpaces = L" \t\n\r";

// removes leading and trailing white space
inline const std::wstring trim(const std::wstring& str) {
  std::wstring::size_type start = str.find_first_not_of(whiteSpaces);
  std::wstring::size_type end = str.find_last_not_of(whiteSpaces);

  if (start == std::wstring::npos) {
    // empty string
    return L"";
  }
  else if (end == std::wstring::npos) {
    return str.substr(start);
  }
  else {
    return str.substr(start, end - start + 1);
  }
}

inline bool isBlanc(const std::wstring& str) {
  return str.find_first_not_of(whiteSpaces) == std::wstring::npos;
}

inline const std::wstring addExtension(const std::wstring& fileName, const std::wstring& ext) {
  std::wstring result(fileName);
  if (!ext.empty()) {
    result += std::wstring(ext[0] == L'.' ? L"" : L".") + ext;
  }

  return result;
}

inline const std::wstring addTailIfNotPresent(const std::wstring& str, wchar_t c, const std::wstring& match) {
  return str.empty() || match.find(*str.rbegin()) != std::wstring::npos ? str : str + c;
}

inline const std::wstring addBackSlash(const std::wstring& str) {
  return addTailIfNotPresent(str, L'\\', L"\\/");
}

inline const std::wstring addFwdSlash(const std::wstring& str) {
  return addTailIfNotPresent(str, L'/', L"/");
}

inline const std::wstring quote(const std::wstring& str) {
  return L"\"" + str + L"\"";
}

inline const std::wstring unescape(const std::wstring& str) {
  std::wstring unescaped;
  bool escape = false;

  for (unsigned int n = 0; n < str.length(); n++) {
    TCHAR c = str[n];

    switch (c) {
      case L'\\':
        if (escape) {
          unescaped += c;
          escape = false;
        }
        else {
          escape = true;
        }
        break;
      case L'*':
        if (escape) {
          unescaped += L' ';
          escape = false;
        }
        else {
          unescaped += c;
        }
        break;
      case L'n':
        if (escape) {
          unescaped += L'\n';
          escape = false;
        } 
        else {
          unescaped += c;
        }
        break;
      case L't':
        if (escape) {
          unescaped += L'\t';
          escape = false;
        }
        else {
          unescaped += c;
        }
        break;
      case L'r':
        if (escape) {
          unescaped += L'\r';
          escape = false;
        } 
        else {
          unescaped += c;
        }
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
      case L'\\':
        escaped += L"\\\\";
        break;
      case L' ':
        escaped += L"\\*";
        break;
      case L'\n':
        escaped += L"\\n";
        break;
      case L'\t':
        escaped += L"\\t";
        break;
      case L'\r':
        escaped += L"\\r";
        break;
      default:
        escaped += str[n];
        break;
    }
  }
  return escaped;
}

  using shared_char_array = std::shared_ptr< char[] >;
  using shared_wchar_array = std::shared_ptr< wchar_t[] >;

  inline std::wstring s2ws(const char* str) {
    if (!str) {
      return std::wstring{};
    }

    int nLen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, NULL);
    if (nLen > 0) {
      auto lpszW = shared_wchar_array(new WCHAR[nLen + sizeof(WCHAR)], std::default_delete<WCHAR[]>());
      MultiByteToWideChar(CP_UTF8, 0, str, -1, lpszW.get(), nLen);
      return std::wstring{ lpszW.get() };
    }
    else {
      return std::wstring{};
    }
  }

  inline std::wstring s2ws(const std::string& str) {
    return s2ws(str.c_str());
  }

  inline std::string ws2s(const wchar_t* wstr) {
    if (!wstr) {
      return std::string{};
    }

    int nLen = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, NULL, NULL, NULL);
    if (nLen > 0) {
      auto lpsz = shared_char_array(new char[nLen + sizeof(char)], std::default_delete<char[]>());
      WideCharToMultiByte(CP_UTF8, 0, wstr, -1, lpsz.get(), nLen, NULL, NULL);
      return std::string{ lpsz.get() };
    }
    else {
      return std::string{};
    }
  }

  inline std::string ws2s(const std::wstring& wstr) {
    return ws2s(wstr.c_str());
  }

// end string related classes
//@}
}  // namespace yloader
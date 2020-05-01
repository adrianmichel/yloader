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

/** @file
 *  \brief contains a string tokenizer class
 */

#include <algorithm>
#include <functional>
#include <locale>
#include <string>
#include <vector>
#include <optional>

 // For the case the default is a space.
 // This is the default predicate for the Tokenize() function.
template <typename T>
class CIsSpace {
public:
  bool operator()(T c) const {
    // isspace<char> returns true if c is a white-space character (0x09-0x0D or
    // 0x20)
    return ::isspace<T>(c) != 0;
  }
};

// For the case the separator is a comma
template <typename T>
class CIsComma {
public:
  bool operator()(T c) const { return (TCHAR(',') == c); }
};

// For the case the separator is a character from a set of characters given in a
// string
template <typename T>
class CIsFromString {
private:
  std::basic_string<T> m_ostr;

public:
  // Constructor specifying the separators
  CIsFromString(const std::basic_string<T>& rostr) : m_ostr(rostr) {}

  CIsFromString(const T* rostr) : m_ostr(rostr) {}

  bool operator()(T c) const {
    return m_ostr.find(c) != std::basic_string<T>::npos;
  }
};

// String Tokenizer
template <typename T, class Pred>
class CTokenizer : public std::vector<std::basic_string<T> > {
public:
  // The predicate should evaluate to true when applied to a separator.
  CTokenizer(const std::basic_string<T>& str, const Pred& pred, std::optional< size_t > maxTokens = std::nullopt) {
    assert(!maxTokens || *maxTokens > 0);

    // First clear the results vector
    __super::clear();
    typename std::basic_string<T>::const_iterator it = str.begin();
    typename std::basic_string<T>::const_iterator itTokenEnd = str.begin();

    while (it != str.end()) {
      // Eat seperators
      while (it != str.end() && pred(*it)) {
        it++;
      }

      if (!maxTokens || __super::size() < (*maxTokens - 1)) {
        // Find next token
        itTokenEnd = find_if(it, str.end(), pred);
        // Append token to result
        if (it < itTokenEnd) {
          __super::push_back(std::basic_string<T>(it, itTokenEnd));
        }
        it = itTokenEnd;
      }
      else {
        __super::push_back(std::basic_string< T >(it, str.end()));
        return;
      }
    }
  }
};

using Sep = CIsFromString<wchar_t>;
using Tokenizer = CTokenizer<wchar_t, Sep>;

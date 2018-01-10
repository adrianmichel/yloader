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

// For the case the default is a space.
// This is the default predicate for the Tokenize() function.
template <class T>
class CIsSpace : public std::unary_function<T, bool> {
 public:
  bool operator()(T c) const {
    // isspace<char> returns true if c is a white-space character (0x09-0x0D or
    // 0x20)
    return ::isspace<T>(c) != 0;
  }
};

// For the case the separator is a comma
template <class T>
class CIsComma : public std::unary_function<T, bool> {
 public:
  bool operator()(T c) const { return (TCHAR(',') == c); }
};

// For the case the separator is a character from a set of characters given in a
// string
template <class T>
class CIsFromString : public std::unary_function<T, bool> {
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
template <class T, class Pred>
class CTokenizer : public std::vector<std::basic_string<T> > {
 public:
  // The predicate should evaluate to true when applied to a separator.
  CTokenizer(const std::basic_string<T>& rostr, const Pred& roPred) {
    // First clear the results vector
    __super::clear();
    std::basic_string<T>::const_iterator it = rostr.begin();
    std::basic_string<T>::const_iterator itTokenEnd = rostr.begin();
    while (it != rostr.end()) {
      // Eat seperators
      while (it != rostr.end() && roPred(*it)) it++;
      // Find next token
      itTokenEnd = find_if(it, rostr.end(), roPred);
      // Append token to result
      if (it < itTokenEnd)
        __super::push_back(std::basic_string<T>(it, itTokenEnd));
      it = itTokenEnd;
    }
  }
};

typedef CIsFromString<TCHAR> Sep;
typedef CTokenizer<TCHAR, Sep> Tokenizer;
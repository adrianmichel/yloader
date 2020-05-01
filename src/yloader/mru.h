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

#include <strings.h>
#include <deque>

// there can't be more than 8 MRU
#define MRU_MAX 8

enum MRUType { symbolsFile, outputPath, outputFile };

class MRU : public std::deque<std::wstring> {
 private:
  unsigned int m_max;
  MRUType m_type;
  const std::wstring m_mruBase;

 public:
  MRU(MRUType type)
      : m_max(MRU_MAX), m_type(type), m_mruBase(mruTypeToString(type)) {
    for (unsigned int n = 0; n < MRU_MAX; n++) {
      std::wostringstream o;

      o << n;

      std::wstring mru = m_mruBase + o.str();
      CString s = AfxGetApp()->GetProfileString(L"YahooData", mru.c_str(), L"");
      if (!s.IsEmpty()) {
        __super::push_back((LPCTSTR)s);
      }
    }
  }

  std::wstring mruTypeToString(MRUType type) {
    switch (type) {
      case symbolsFile:
        return L"mruSymbolsFile";
      case outputPath:
        return L"mruOutputPath";
      case outputFile:
        return L"mruOutputFile";
      default:
        assert(false);
        return L"";
    }
  }

  ~MRU() {
    for (unsigned int n = 0; n < MRU_MAX; n++) {
      std::wstring mru = m_mruBase + std::to_wstring(n);
      AfxGetApp()->WriteProfileString(L"YahooData", mru.c_str(), n < __super::size() ? (*this)[n].c_str() : L"");
    }
  }
  void add(const std::wstring& item) {
    __super::push_front(item);

    __super::iterator i = std::find(++__super::begin(), __super::end(), item);
    if (i != __super::end()) {
      __super::erase(i);
    }

    assert(m_max > 0);

    if (__super::size() > m_max) __super::pop_back();
  }
};

constexpr auto MRU_SYMBOLS_ID = 3000;
constexpr auto MRU_OUTPUT_PATH_ID = 3100;
constexpr auto MRU_OUTPUT_FILE_ID = 3200;
constexpr auto CLEAR_MRU_SYMBOLS_ID = 3020;
constexpr auto CLEAR_MRU_OUTPUT_PATH_ID = 3120;
constexpr auto CLEAR_MRU_OUTPUT_FILE_ID = 3220;

class CMenuButton : public CButton {
 private:
  MRU m_mru;

 protected:
  virtual unsigned int getMRUId() const = 0;
  virtual unsigned int getClearId() const = 0;

 public:
  CMenuButton(MRUType type) : m_mru(type) {}

  virtual void init() = 0;

  void clearMenu() {
  }

  void clear() {
    m_mru.clear();
    clearMenu();
  }

  void add(const std::wstring& item) {
    m_mru.add(item);
    setMenu();
  }

  void setMenu() {
  }

  const std::wstring& operator[](unsigned int ix) const { return m_mru[ix]; }
};

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

#include <misc.h>

using namespace std::string_literals;

class CharacterMappingException : public std::exception {
 public:
  CharacterMappingException(const std::wstring& message)
      : std::exception(yloader::ws2s(message).c_str()) {}

  std::wstring message() { return yloader::s2ws(__super::what()); }
};

using CMBase = std::map<TCHAR, std::wstring>;
using CharacterMapping = std::pair<std::wstring, std::wstring>;
using CharacterMappingPtr = std::shared_ptr<CharacterMapping>;

class CharactersMapping : private CMBase {
 public:
  CharactersMapping() {}

  CharactersMapping(const std::vector<std::string>& map) { set(map); }

  void add(wchar_t c1, wchar_t c2) {
    LOG(log_debug, L"c1: ", c1, L", c2: ", c2);
    static const std::wstring illegal(L"\\/:?\"<>|");
    if (illegal.find(c2) != std::wstring::npos) {
      add(c1);
    }
    else {
      if (c2 == L'*') {
        c2 = L' ';
      }
      __super::insert(__super::value_type(c1, std::wstring() + c2));
    }
  }

  void add(const TCHAR* c1, const TCHAR* c2) {
    assert(_tcslen(c1) == 0 || _tcslen(c1) == 1);
    assert(_tcslen(c2) == 0 || _tcslen(c2) == 1);

    if (_tcslen(c1) == 1) {
      if (_tcslen(c2) == 0) {
        add(*c1);
      }
      else {
        add(*c1, *c2);
      }
    }
  }

  // string must have exactly two chars
  void add(const std::string& map) {
    if (!map.empty()) {
      std::wstring wmap(yloader::s2ws(map));
      if (wmap.size() == 2) {
        add(wmap[0], wmap[1]);
      }
      else if (wmap.size() == 1) {
        add(wmap[0]);
      }
      else {
        throw CharacterMappingException(L"Mapping must be between at most two characters: \""s + wmap + L"\"");
      }
    }
  }

  void set(const std::vector<std::string>& map) {
    if (map.size() > 4) {
      throw CharacterMappingException(L"There can be at most four character mappings");
    }
    __super::clear();
    for (auto s : map) {
      add(s);
    }
  }

  void operator=(const std::vector<std::string>& map) { set(map); }

  void add(TCHAR c1) {
    LOG(log_debug, L"c1: ", c1);
    __super::insert(__super::value_type(c1, std::wstring()));
  }

  void set(const std::wstring& from, const std::wstring& to) {
    __super::clear();
    for (std::wstring::size_type i = 0; i < min(from.length(), to.length()); i++) {
      add(from[i], to[i]);
    }
  }

  std::wstring map(const std::wstring& source) const {
    std::wstring dest(source);

    std::wstring used;

    for (auto i : *this) {
      if (used.find(i.first) == std::wstring::npos) {
        for (std::wstring::size_type pos = dest.find(i.first); pos != std::wstring::npos; pos = dest.find(i.first)) {
          used += i.second;
          dest.replace(pos, 1, i.second);
        }
      }
    }

    return dest;
  }

  std::wstring getFromString() const {
    std::wstring from;
    for (auto i : *this) {
      from += i.first;
    }

    return from;
  }

  std::wstring getToString() const {
    std::wstring to;
    for (auto i : *this) {
      to += i.second.empty() ? L":" : handleSpecial(i.second);
    }

    LOG(log_debug, L"to: ", to);
    return to;
  }

  std::wstring handleSpecial(const std::wstring& t) const {
    assert(t.length() == 1);
    return t[0] == L' ' ? L"*"s : t;
  }

  CharacterMappingPtr getFirstMapping() const {
    if (__super::empty()) {
      return CharacterMappingPtr();
    }
    else {
      m_it = __super::begin();
      return std::make_shared< CharacterMapping >(std::wstring(1, m_it->first), m_it->second);
    }
  }

  CharacterMappingPtr getNextMapping() const {
    if (__super::empty()) {
      return CharacterMappingPtr();
    }
    else {
      if (m_it != __super::end()) {
        ++m_it;
        return m_it == __super::end() ? nullptr : std::make_shared< CharacterMapping >(std::wstring(1, m_it->first), m_it->second);
      }
      else {
        return 0;
      }
    }
  }

 private:
  mutable __super::const_iterator m_it;
};

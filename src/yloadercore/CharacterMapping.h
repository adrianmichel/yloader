/*
Copyright (C) 2017  YLoader.com

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

class CharacterMappingException : public std::exception {
 public:
  CharacterMappingException(const std::wstring& message)
      : std::exception(yloader::ws2s(message).c_str()) {}

  std::wstring message() { return yloader::s2ws(__super::what()); }
};

typedef std::map<TCHAR, std::wstring> CMBase;
typedef std::pair<std::wstring, std::wstring> CharacterMapping;
typedef boost::shared_ptr<CharacterMapping> CharacterMappingPtr;

class CharactersMapping : private CMBase {
 public:
  CharactersMapping() {}

  CharactersMapping(const std::vector<std::string>& map) { set(map); }

  void add(TCHAR c1, TCHAR c2) {
    LOG(log_debug, _T( "c1: " ) << c1 << _T( ", c2: " ) << c2);
    static const std::wstring illegal(_T( "\\/:?\"<>|" ));
    if (illegal.find(c2) != std::wstring::npos)
      add(c1);
    else {
      if (c2 == TCHAR('*')) c2 = TCHAR(' ');
      __super::insert(__super::value_type(c1, std::wstring() + c2));
    }
  }

  void add(const TCHAR* c1, const TCHAR* c2) {
    assert(_tcslen(c1) == 0 || _tcslen(c1) == 1);
    assert(_tcslen(c2) == 0 || _tcslen(c2) == 1);

    if (_tcslen(c1) == 1) {
      if (_tcslen(c2) == 0)
        add(*c1);
      else
        add(*c1, *c2);
    }
  }

  // string must have exactly two chars
  void add(const std::string& map) {
    if (!map.empty()) {
      std::wstring wmap(yloader::s2ws(map));
      if (wmap.size() == 2) {
        add(wmap[0], wmap[1]);
      } else if (wmap.size() == 1) {
        add(wmap[0]);
      } else {
        throw CharacterMappingException(
            std::wstring(
                _T("Mapping must be between at most two characters: \"")) +
            wmap + _T( "\""));
      }
    }
  }

  void set(const std::vector<std::string>& map) {
    if (map.size() > 4) {
      throw CharacterMappingException(
          _T("There can be at most four character mappings"));
    }
    __super::clear();
    for (std::vector<std::string>::const_iterator i = map.begin();
         i != map.end(); ++i) {
      add(*i);
    }
  }

  void operator=(const std::vector<std::string>& map) { set(map); }

  void add(TCHAR c1) {
    LOG(log_debug, _T( "c1: " ) << c1);
    __super::insert(__super::value_type(c1, std::wstring()));
  }

  void set(const std::wstring& from, const std::wstring& to) {
    __super::clear();
    for (std::wstring::size_type i = 0; i < min(from.length(), to.length());
         i++) {
      add(from[i], to[i]);
    }
  }

  std::wstring map(const std::wstring& source) const {
    std::wstring dest(source);

    std::wstring used;

    for (__super::const_iterator i = __super::begin(); i != __super::end();
         i++) {
      if (used.find(i->first) == std::wstring::npos) {
        for (std::wstring::size_type pos = dest.find(i->first);
             pos != std::wstring::npos; pos = dest.find(i->first)) {
          used += i->second;
          dest.replace(pos, 1, i->second);
        }
      }
    }

    return dest;
  }

  std::wstring getFromString() const {
    std::wstring from;
    for (__super::const_iterator i = __super::begin(); i != __super::end(); i++)
      from += i->first;

    return from;
  }

  std::wstring getToString() const {
    std::wstring to;
    for (__super::const_iterator i = __super::begin(); i != __super::end(); i++)
      to += i->second.empty() ? _T( ":" ) : handleSpecial(i->second);

    LOG(log_debug, _T( "to: " ) << to);
    return to;
  }

  std::wstring handleSpecial(const std::wstring& t) const {
    assert(t.length() == 1);
    return t[0] == TCHAR(' ') ? std::wstring(_T( "*" )) : t;
  }

  CharacterMappingPtr getFirstMapping() const {
    if (__super::empty())
      return CharacterMappingPtr();
    else {
      _i = __super::begin();
      return CharacterMappingPtr(
          new CharacterMapping(std::wstring() + _i->first, _i->second));
    }
  }

  CharacterMappingPtr getNextMapping() const {
    if (__super::empty())
      return CharacterMappingPtr();
    else {
      if (_i != __super::end()) {
        ++_i;
        return _i == __super::end()
                   ? 0
                   : CharacterMappingPtr(new CharacterMapping(
                         std::wstring() + _i->first, _i->second));
      } else
        return 0;
    }
  }

 private:
  mutable __super::const_iterator _i;
};

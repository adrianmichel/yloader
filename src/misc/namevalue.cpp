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

#include "stdafx.h"
#include <namevalue.h>

///////////////////////////////////////////////////
///
/// NameValueVector methods

using namespace yloader;

using Base = yloader::NameValueMapBase;
using BaseItPair = std::pair<Base::const_iterator, Base::const_iterator>;

MISC_API void NameValueMap::set(const NameValue& nameValue) {
  assert(!nameValue.name().empty());
  Base::size_type n = Base::count(nameValue.name());

  if (n == 0) {
    add(nameValue);
  }
  else if (n == 1) {
    Base::iterator i = Base::find(nameValue.name());
    assert(i != Base::end());

    *(i->second) = nameValue;
  }
  else {
    // can't set if there are multiple values with the same name for now
    assert(false);
  }
}

MISC_API void NameValueMap::add(const NameValue& nameValue) {
  add(nameValue.name(), nameValue.value());
}

MISC_API yloader::NameValue yloader::NameValueMap::operator[](size_t n) const {
  if (size() > 0 && n < size()) {
    NameValuePtr nameValue(m_vector[n]);

    return Base::count(nameValue->name()) > 1 ? NameValue(nameValue->name() + L"[]", nameValue->value()) : *nameValue;
  }
  else {
    return NameValue{};
  }
}

MISC_API void yloader::NameValueMap::push_back(const yloader::NameValue& nv) {
  add(nv);
}

MISC_API void NameValueMap::add(const std::wstring& name, const std::wstring& value) {
  assert(!name.empty());

  NameValuePtr nameValue(new NameValue(name, value));

  m_vector.push_back(nameValue);
  __super::insert(Base::value_type(name, nameValue));
}

MISC_API yloader::StringPtr yloader::NameValueMap::toString(bool phpStyle) const {
  yloader::StringPtr str(new std::wstring());

  for (Base::const_iterator i = Base::begin(); i != Base::end();) {
    std::wstring name((*i).first);
    BaseItPair its = Base::equal_range(name);

    if (Base::count(name) > 1 && phpStyle) {
      for (i = its.first; i != its.second; ++i) {
        if (!str->empty()) {
          (*str) += L"&";
        }

        (*str) += name + L"[]=" + i->second->value();
      }
    }
    else {
      if (!str->empty()) {
        (*str) += L"&";
      }

      (*str) += name + L"=" + i->second->value();
      ++i;
    }
  }

  return str;
}
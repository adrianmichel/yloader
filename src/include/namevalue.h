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

#pragma warning(disable : 4800 4275 4251 4244 4003)

#include <map>
#include "sharedptr.h"
#include "strings.h"

#include <boost/shared_ptr.hpp>

namespace yloader {
typedef std::pair<std::wstring, std::wstring> NameValueBase;

class NameValue : public NameValueBase {
 public:
  NameValue() {}

  template <typename T>
  NameValue(const std::wstring& name, const T& value)
      : NameValueBase(name, std::wstring() << value) {
    assert(!name.empty());
  }

  /*	NameValue( const std::wstring& name, const std::wstring& value )
          : std::pair< std::wstring, std::wstring >( name, value )
          {
                  assert( !name.empty() );
          }

          NameValue( const std::wstring& name, double value )
          {
                  assert( !name.empty() );
                  __super::first = name;
                  std::wostringstream os;
                  os << value;
                  __super::second = os.str();

          }
  */
  const std::wstring& name() const { return __super::first; }
  const std::wstring& value() const { return __super::second; }

  operator bool() const { return !NameValueBase::first.empty(); }

  yloader::StringPtr yloader::NameValue::toString() const {
    yloader::StringPtr str(new std::wstring());
    (*str) += name() + _T( "=" ) + value();
    return str;
  }
};

typedef boost::shared_ptr<NameValue> NameValuePtr;

typedef std::multimap<std::wstring, NameValuePtr> NameValueMapBase;

class MISC_API NameValueMap : public NameValueMapBase {
 private:
  typedef std::vector<NameValueMapBase::const_iterator> NameValueVector;
  std::vector<NameValuePtr> m_vector;

 public:
  template <typename T>
  void set(const std::wstring& name, const T& value) {
    set(NameValue(name, value));
  }
  void set(const NameValue& nameValue);
  void add(const NameValue& nameValue);
  NameValue operator[](unsigned int n) const;
  void push_back(const yloader::NameValue& nv);
  template <typename T>
  void add(const std::wstring& name, const T& value) {
    add(NameValue(name, value));
  }
  void add(const std::wstring& name, const std::wstring& value);

  // if php style is true, if there are multiple values with the same name
  // they will be represented as "name[]=v1&name[]=v2&...". This allows a php
  // script to automatically generate an array in the receiving variable
  yloader::StringPtr toString(bool phpStyle = true) const;
};

}  // namespace yloader

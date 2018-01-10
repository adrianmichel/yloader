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
#include <strings.h>
#include <boost/format.hpp>

namespace yloader {

template <typename T1>
std::wstring format(const std::wstring& f, T1 t1) {
  return (boost::wformat(f) % t1).str();
}

template <typename T1, typename T2>
std::wstring format(const std::wstring& f, T1 t1, T2 t2) {
  return (boost::wformat(f) % t1 % t2).str();
}

template <typename T1, typename T2, typename T3>
std::wstring format(const std::wstring& f, T1 t1, T2 t2, T3 t3) {
  return (boost::wformat(f) % t1 % t2 % t3).str();
}

template <typename T1, typename T2, typename T3, typename T4>
std::wstring format(const std::wstring& f, T1 t1, T2 t2, T3 t3, T4 t4) {
  return (boost::wformat(f) % t1 % t2 % t3 % t4).str();
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
std::wstring format(const std::wstring& f, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) {
  return (boost::wformat(f) % t1 % t2 % t3 % t4 % t5).str();
}

}  // namespace yloader
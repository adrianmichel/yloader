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

#include "stdafx.h"
#include <hinstance.h>

HInstance::HInstance(const std::wstring& fileName) throw(HInstanceException)
    : _hInstance(init(fileName)), _path(fileName) {
  if (!_hInstance) {
    throw HInstanceException();
  }
}

HInstance::~HInstance() {
  if (_hInstance != 0) AfxFreeLibrary(_hInstance);
}

HINSTANCE HInstance::init(const std::wstring& path) {
  return AfxLoadLibrary(path.c_str());
}

FARPROC HInstance::getProcAddress(const std::wstring& procName) const
    throw(HInstanceMethodException) {
  assert(_hInstance != 0);
  FARPROC proc = GetProcAddress(_hInstance, yloader::ws2s(procName).c_str());

  if (proc != 0)
    return proc;
  else
    throw HInstanceMethodException(procName);
}

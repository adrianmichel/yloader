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

class HInstanceException {
 private:
  DWORD _lastError;

 public:
  HInstanceException() : _lastError(::GetLastError()) {}

  DWORD getLastError() const { return _lastError; }
};

class HInstanceMethodException {
 private:
  const std::wstring _name;

 public:
  HInstanceMethodException(const std::wstring& name) : _name(name) {}

  const std::wstring& name() const { return _name; }
};

// a library instance class that deals correctly with loading/freeing the
// library
class HInstance {
 private:
  const std::wstring _path;
  HINSTANCE _hInstance;

 protected:
  HInstance(const std::wstring& fileName) throw(HInstanceException);
  ~HInstance();

  bool valid() const { return _hInstance != 0; }

  FARPROC getProcAddress(const std::wstring& procName) const
      throw(HInstanceMethodException);

 private:
  HINSTANCE init(const std::wstring& path);

 public:
  const std::wstring& path() const { return _path; }
};
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
  DWORD m_lastError;

 public:
  HInstanceException() : m_lastError(::GetLastError()) {}

  DWORD getLastError() const { return m_lastError; }
};

class HInstanceMethodException {
 private:
  const std::wstring m_name;

 public:
  HInstanceMethodException(const std::wstring& name) : m_name(name) {}

  const std::wstring& name() const { return m_name; }
};

// a library instance class that deals correctly with loading/freeing the
// library
class HInstance {
 private:
  const std::wstring m_path;
  HINSTANCE m_hInstance;

 protected:
  HInstance(const std::wstring& fileName);
  ~HInstance();

  bool valid() const { return m_hInstance != 0; }

  FARPROC getProcAddress(const std::wstring& procName) const;

 private:
  HINSTANCE init(const std::wstring& path);

 public:
  const std::wstring& path() const { return m_path; }
};

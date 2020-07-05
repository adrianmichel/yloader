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

#pragma once

#include <winbase.h>
#include <boost/regex.hpp>
#include <fstream>
#include "misc.h"
#include "sharedptr.h"
#include "strings.h"

namespace yloader {

inline DateTime toDateTime(const SYSTEMTIME& st) {
  return DateTime(Date(st.wYear, st.wMonth, st.wDay), TimeDuration(st.wHour, st.wMinute, st.wSecond, 0));
}

inline DateTime toDateTime(const FILETIME fileTime) {
  SYSTEMTIME st;
  FileTimeToSystemTime(&fileTime, &st);
  return toDateTime(st);
}

class FileException {};

MISCWIN_API yloader::DateTime getFileLastWriteTime(const std::wstring& fileName);

MISCWIN_API bool fileExists(const TCHAR* filepath);

inline bool fileExists(const std::wstring& filepath) {
  return fileExists(filepath.c_str());
}

MISCWIN_API bool DirectoryExists(const TCHAR* dir);
MISCWIN_API bool DirectoryExists(const std::wstring& dir);

// returns the path without the file name
MISCWIN_API std::wstring getModulePath();

inline std::wstring getModuleFileName() {
  TCHAR fileName[MAX_PATH + 1] = {0};

  GetModuleFileName(0, fileName, MAX_PATH);

  return std::wstring(fileName);
}

MISCWIN_API std::wstring getAppDataPath();

MISCWIN_API std::wstring getDirectory(const std::wstring& root, const std::wstring& dir);

using VersionPtr = std::shared_ptr<Version>;
MISCWIN_API VersionPtr getFileVersion(const std::wstring& fileName);
MISCWIN_API std::wstring getFileLanguage(const std::wstring& fileName);

MISCWIN_API std::wstring getFilePath(const std::wstring& file);

class SpecialFileException {};

using fstream_ptr = std::shared_ptr<std::fstream>;
template <typename file_type>
class SpecialFile {
 protected:
  using file_type_ptr = std::shared_ptr<file_type>;

 private:
  file_type_ptr m_fs;
  FILE* m_f;
  HANDLE m_h;
  int m_fd;
  const std::wstring m_fileName;

 protected:
  SpecialFile(const std::wstring& fileName)
      : m_f(0), m_h(0), m_fd(0), m_fileName(fileName) {}

  void setHandle(HANDLE h) {
      if (h != INVALID_HANDLE_VALUE) {
      m_h = h;
    }
    else {
      LOG(log_error, L"last error: ", GetLastError());
      throw SpecialFileException();
    }
  }
  void setFILE(FILE* f) {
    if (f != 0) {
      m_f = f;
    }
    else {
      LOG(log_error, L"last error: ", GetLastError());
      throw SpecialFileException();
    }
  }

  void setFStream(file_type_ptr fs) {
    if (fs && *fs) {
      m_fs = fs;
    }
    else {
      LOG(log_error, L"last error: ", GetLastError());
      throw SpecialFileException();
    }
  }

  void setFileDescriptor(int fd) {
    if (fd >= 0) {
      m_fd = fd;
    }
    else {
      LOG(log_error, L"last error: ", GetLastError());
      throw SpecialFileException();
    }
  }

  virtual ~SpecialFile() {
    if (m_fs && m_fs->is_open()) {
      m_fs->close();
    }

    if (m_f) {
      fclose(m_f);
    }
  }

 public:
  operator file_type&() {
    assert(this->operator bool());
    return *m_fs;
  }

  operator bool() const { return m_fs && *m_fs; }
};

class MISCWIN_API SpecialFileRead : public SpecialFile<std::wifstream> {
 private:
  using Base = SpecialFile<std::wifstream>;

 private:
  void createReadFile(const std::wstring& fileName);

 public:
  SpecialFileRead(const std::wstring& fileName);
};

class MISCWIN_API SpecialFileWrite : public SpecialFile<std::wofstream> {
 private:
  using Base = SpecialFile<std::wofstream>;

 private:
  void createWriteFile(const std::wstring& fileName, bool append, bool binary);

 public:
  SpecialFileWrite(const std::wstring& fileName, bool append = false,
                   bool binary = false);
};

using SpecialFileReadPtr = std::shared_ptr<SpecialFileRead>;
using SpecialFileWritePtr = std::shared_ptr<SpecialFileWrite>;

}  // namespace yloader

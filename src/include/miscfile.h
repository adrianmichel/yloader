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

#include <winbase.h>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include "misc.h"
#include "sharedptr.h"
#include "strings.h"

namespace yloader {

inline DateTime toDateTime(const SYSTEMTIME& st) {
  return DateTime(Date(st.wYear, st.wMonth, st.wDay),
                  TimeDuration(st.wHour, st.wMinute, st.wSecond, 0));
}

inline DateTime toDateTime(const FILETIME fileTime) {
  SYSTEMTIME st;
  FileTimeToSystemTime(&fileTime, &st);
  return toDateTime(st);
}

class FileException {};

MISCWIN_API yloader::DateTime getFileLastWriteTime(
    const std::wstring& fileName) throw(FileException);

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

MISCWIN_API std::wstring getDirectory(const std::wstring& root,
                                      const std::wstring& dir);

typedef ManagedPtr<Version> VersionPtr;
MISCWIN_API VersionPtr getFileVersion(const std::wstring& fileName);
MISCWIN_API std::wstring getFileLanguage(const std::wstring& fileName);

MISCWIN_API std::wstring getFilePath(const std::wstring& file);

class SpecialFileException {};

typedef boost::shared_ptr<std::fstream> fstream_ptr;
template <typename file_type>
class SpecialFile {
 protected:
  typedef boost::shared_ptr<file_type> file_type_ptr;

 private:
  file_type_ptr _fs;
  FILE* _f;
  HANDLE _h;
  int _fd;
  const std::wstring _fileName;

 protected:
  SpecialFile(const std::wstring& fileName)
      : _f(0), _h(0), _fd(0), _fileName(fileName) {}

  void setHandle(HANDLE h) {
    if (h != INVALID_HANDLE_VALUE)
      _h = h;
    else {
      LOG(log_error, _T( "last error: " ) << GetLastError());
      throw SpecialFileException();
    }
  }
  void setFILE(FILE* f) {
    if (f != 0)
      _f = f;
    else {
      LOG(log_error, _T( "last error: " ) << GetLastError());
      throw SpecialFileException();
    }
  }

  void setFStream(file_type_ptr fs) {
    if (fs && *fs)
      _fs = fs;
    else {
      LOG(log_error, _T( "last error: " ) << GetLastError());
      throw SpecialFileException();
    }
  }

  void setFileDescriptor(int fd) {
    if (fd >= 0)
      _fd = fd;
    else {
      LOG(log_error, _T( "last error: " ) << GetLastError());
      throw SpecialFileException();
    }
  }

  virtual ~SpecialFile() {
    if (_fs && _fs->is_open()) _fs->close();

    if (_f) fclose(_f);
  }

 public:
  operator file_type&() {
    assert(this->operator bool());
    return *_fs;
  }

  operator bool() const { return _fs && *_fs; }
};

class MISCWIN_API SpecialFileRead : public SpecialFile<yloader::t_ifstream> {
 private:
  typedef SpecialFile<yloader::t_ifstream> Base;

 private:
  void createReadFile(const std::wstring& fileName);

 public:
  SpecialFileRead(const std::wstring& fileName);
};

class MISCWIN_API SpecialFileWrite : public SpecialFile<yloader::t_ofstream> {
 private:
  typedef SpecialFile<yloader::t_ofstream> Base;

 private:
  void createWriteFile(const std::wstring& fileName, bool append, bool binary);

 public:
  SpecialFileWrite(const std::wstring& fileName, bool append = false,
                   bool binary = false);
};

typedef boost::shared_ptr<SpecialFileRead> SpecialFileReadPtr;
typedef boost::shared_ptr<SpecialFileWrite> SpecialFileWritePtr;

}  // namespace yloader
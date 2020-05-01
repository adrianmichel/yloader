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

#include "stdafx.h"
#include <curl/curl.h>
#include <io.h>
#include <miscfile.h>
#include <shlobj.h>
#include <sys/stat.h>   // For stat().
#include <sys/types.h>  // For stat().
#include <thread.h>
#include "miscwinapp.h"

#include <wininet.h>
#include <boost/shared_array.hpp>

using namespace yloader;

BEGIN_MESSAGE_MAP(CMiscWinApp, CWinApp)
END_MESSAGE_MAP()

// CFilePluginsApp construction

CMiscWinApp::CMiscWinApp() {
}

CMiscWinApp theApp;

BOOL CMiscWinApp::InitInstance() {
  CWinApp::InitInstance();

  return TRUE;
}

int CMiscWinApp::ExitInstance() {

  return CWinApp::ExitInstance();
}

MISCWIN_API void init() {
 curl_global_init(CURL_GLOBAL_ALL);
}

MISCWIN_API void uninit() {
 curl_global_cleanup();
}

MISCWIN_API const std::wstring yloader::makeFileName(const std::wstring& path, const std::wstring& name, const std::wstring& ext) {
  std::wstring fileName(path);

  // open the file with the data
  if (!fileName.empty() && fileName.back() != L'\\' && fileName.back() != L'/') {
      fileName += L'\\';
  }

  fileName += name;
  if (!ext.empty() && ext.front() != L'.') {
    fileName += L'.';
  }

  return fileName + ext;
}

MISCWIN_API const RunProcessResult yloader::runProcess(const std::wstring& processFileName, const std::wstring& cmdLine,
    bool waitForProcess, const std::wstring* startingDirectory, const Environment& env, unsigned long timeout) {
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  LPCTSTR startingDir = startingDirectory != 0 ? startingDirectory->c_str() : 0;

  if (CreateProcess(processFileName.empty() ? 0 : processFileName.c_str(),
                    const_cast<LPTSTR>(cmdLine.c_str()),  // Command line.
                    NULL,         // Process handle not inheritable.
                    NULL,         // Thread handle not inheritable.
                    TRUE,         // Set handle inheritance to FALSE.
                    0,            // No creation flags.
                    0,            // Use parent's environment block.
                    startingDir,  // Use parent's starting directory.
                    &si,          // Pointer to STARTUPINFO structure.
                    &pi)          // Pointer to PROCESS_INFORMATION structure.
  ) {
    DWORD exitCode(0);

    bool timedout(false);
    // Wait until child process exits.
    if (waitForProcess) {
      if (timeout == 0) {
        WaitForSingleObject(pi.hProcess, INFINITE);
      }
      else {
        if (WaitForSingleObject(pi.hProcess, timeout) == WAIT_TIMEOUT) {
          // if timeout, forcefully therminate the process
          TerminateProcess(pi.hProcess, -1);
          timedout = true;
        }
      }
    }
    GetExitCodeProcess(pi.hProcess, &exitCode);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return RunProcessResult(timedout, exitCode);
  }
  else {
    throw RunProcessException(processFileName, cmdLine, GetLastError());
  }
}

////////////////////////////////
// log related static variables
////////////////////////////////
Level yloader::Log::m_globalLevel;
std::wstring yloader::Log::m_logToFile;
bool yloader::Log::m_logToDebugOutput = false;
bool yloader::Log::m_logToConsole = false;
std::mutex Log::m_mutex;

class ConsoleLog : public Log {
 public:
  ConsoleLog(Level level, const wchar_t* func) : Log(level, func) {}

  virtual std::wostream& getOstream() { return std::wcout; }
};

class OutputDebugLog : public Log {
  yloader::t_dostream m_dos;

 public:
  OutputDebugLog(Level level, const wchar_t* func) : Log(level, func) {}

  virtual std::wostream& getOstream() { return m_dos; }
};

MISCWIN_API void yloader::Log::xfilelog(const std::wstring& fileName, Level level, const wchar_t* function, const std::wstring& value) {
  std::scoped_lock lock(m_mutex);

  if (Log::checkLevel(level)) {
    std::wstring h = Log::header(level, function);

    try {
      FileLog fileLog(fileName, level, function);
      fileLog << h << value << std::endl;
    }
    catch (const LogException&) {
      // could not open log file - now what?
    }
  }
}

LOG_API void yloader::Log::xlog(Level level, const wchar_t* function, const std::wstring& value) {
  std::scoped_lock lock(m_mutex);

  if (Log::checkLevel(level)) {
    std::wstring h = Log::header(level, function);

    if (Log::logToFile()) {
      try {
        FileLog fileLog(level, function);
        fileLog << h << value << std::endl;
      }
      catch (const LogException&) {
        // could not open log file - now what?
      }
    }

    if (Log::logToConsole()) {
      ConsoleLog consoleLog(level, function);
      consoleLog << h << value << std::endl;
    }

    if (Log::logToDebugOutput()) {
      OutputDebugLog odl(level, function);
      odl << h << value << std::endl;
    }
  }
}

void yloader::Log::flipLevel() {
  if (isDebugLevel()) {
    setNormalLevel();
  }
  else {
    setDebugLevel();
  }
}

MISCWIN_API void yloader::Log::setLevel(Level level) {
  std::scoped_lock lock(m_mutex);

  m_globalLevel = level;
}

//**************************
// Thread code
//**************************

void ThreadBase::start(ThreadContext* context) {
  if (!m_running) {
    startx(context);
  }
}

void ThreadBase::startSync(ThreadContext* context, unsigned long timeout) {
  if (!m_running) {
    startx(context, true, timeout);
  }
}

void ThreadBase::stopSync() {
  if (m_running) {
    stopx();
  }
}
void ThreadBase::stopAsync() {
  if (m_running) {
    stopy();
  }
}
void ThreadBase::restart(ThreadContext* context) {
  stopSync();
  start(context);
}
bool ThreadBase::isRunning() const {
  return m_running;
}

bool ThreadBase::stopping() {
  return !m_run;
}

unsigned int ThreadBase::threadProc(void* p) {
  std::shared_ptr<Context> context(static_cast<Context*>(p));

  try {
    LOG(log_info, L"Starting thread: \"", context->diagString(), L"\"");
    context->thread()->run(context->threadContext());
    context->thread()->m_run = false;
    context->thread()->m_running = false;
    LOG(log_info, L"Exiting thread: \"", context->diagString(), L"\"");
  }
  catch (...) {
    LOG(log_error, L"Unhandled exception in thread: \"", context->diagString(), L"\"");

    context->thread()->m_run = false;
    context->thread()->m_running = false;
    assert(false);
  }
  return 0;
}

HANDLE ThreadBase::create(ThreadContext* context, unsigned int (*proc)(void*)) {
  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(sa);
  sa.lpSecurityDescriptor = NULL;

  return (HANDLE)_beginthreadex((void*)&sa, 0, (unsigned(__stdcall*)(void*))proc, new Context(this, context), 0, (unsigned int*)&m_threadId);
}

void ThreadBase::startx(ThreadContext* context, bool sync, unsigned long timeout) {
  if (!m_running) {
    m_run = true;
    m_running = true;

    m_hThread = create(context, threadProc);

    if (m_hThread) {
      if (sync) {
        WaitForSingleObject(m_hThread, (timeout > 0 ? timeout : INFINITE));
      }

      if (m_priority) {
        BOOL b = ::SetThreadPriority(m_hThread, THREAD_PRIORITY_ABOVE_NORMAL);
      }
    }
    else {
      // error
    }
  }
}

void ThreadBase::stopx() {
  if (m_hThread != 0) {
    m_run = false;
    WaitForSingleObject(m_hThread, INFINITE);
    m_hThread = 0;
  }
}

void ThreadBase::stopy() {
  if (m_hThread != 0) {
    m_run = false;
    m_hThread = 0;
  }
}

MISCWIN_API std::wstring yloader::getAppDataPath() {
  TCHAR path[MAX_PATH];

  if (SUCCEEDED(SHGetFolderPath(0, CSIDL_LOCAL_APPDATA, 0, SHGFP_TYPE_CURRENT, path))) {
    return path;
  }
  else {
    return L"";
  }
}

MISCWIN_API std::wstring yloader::getDirectory(const std::wstring& root, const std::wstring& dir) {
  if (!root.empty()) {
    std::wstring path(addBackSlash(root) + dir);
    if (yloader::DirectoryExists(path.c_str()) || SUCCEEDED(CreateDirectory(path.c_str(), 0))) {
      return path;
    }
    else {
      return L"";
    }
  }
  else {
    return L"";
  }
}

MISCWIN_API yloader::VersionPtr yloader::getFileVersion(const std::wstring& fileName) {
  DWORD handle;
  DWORD size(::GetFileVersionInfoSize(fileName.c_str(), &handle));

  if (size > 0) {
    boost::shared_array<char> data(new char[size]);

    if (::GetFileVersionInfo(fileName.c_str(), 0, size, data.get())) {
      VS_FIXEDFILEINFO* pVS = NULL;
      unsigned int len;

      if (VerQueryValue(data.get(), L"\\", (void**)&pVS, &len) && (pVS != NULL)) {
        unsigned int n1, n2, n3, n4;
        n1 = pVS->dwProductVersionMS >> 16;
        n2 = pVS->dwProductVersionMS & 0xFFFF;
        n3 = pVS->dwProductVersionLS >> 16;
        n4 = pVS->dwProductVersionLS & 0xFFFF;

        return std::make_shared< yloader::Version >(n1, n2, n3, n4);
      }
      else {
        return nullptr;
      }
    }
    else {
      return nullptr;
    }
  }
  else {
    return nullptr;
  }
}

MISCWIN_API std::wstring yloader::getFileLanguage(const std::wstring& fileName) {
  return L"en";
}

MISCWIN_API bool yloader::DirectoryExists(const TCHAR* dir) {
  assert(dir != 0 && _tcslen(dir) > 0);

  if (dir == 0 || _tcslen(dir) == 0) {
    return false;
  }

  if (_taccess(dir, 0) == 0) {
    _WIN32_FILE_ATTRIBUTE_DATA info;
    GetFileAttributesEx(dir, GetFileExInfoStandard, &info);

    return info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
  }
  else {
    return false;
  }
}

MISCWIN_API bool yloader::DirectoryExists(const std::wstring& dir) {
  return DirectoryExists(dir.c_str());
}

MISCWIN_API std::wstring yloader::getModulePath() {
  std::wstring dir;
  TCHAR fileName[MAX_PATH + 1];
  CFileFind fileFinder;

  GetModuleFileName(0, fileName, MAX_PATH);
  if (fileFinder.FindFile(fileName)) {
    fileFinder.FindNextFile();
    dir = fileFinder.GetRoot();
    return dir;
  }
  else {
    assert(false);
    return L"";
  }
}

MISCWIN_API bool yloader::fileExists(const TCHAR* filepath) {
  HANDLE hFile = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE) return false;

  CloseHandle(hFile);
  return true;
}

MISCWIN_API DateTime yloader::getFileLastWriteTime(const std::wstring& fileName) {
  HANDLE file = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (file == INVALID_HANDLE_VALUE) {
    throw FileException();
  }
  else {
    FILETIME creation;
    FILETIME lastAccess;
    FILETIME lastWrite;

    GetFileTime(file, &creation, &lastAccess, &lastWrite);

    CloseHandle(file);

    return toDateTime(lastWrite);
  }
}
MISCWIN_API std::wstring yloader::getFilePath(const std::wstring& file) {
  CFileFind ff;

  ff.FindFile(file.c_str());
  ff.FindNextFile();

  return ff.GetRoot().GetString();
}

extern "C" {
_CRTIMP int __cdecl _open_osfhandle(intptr_t, int);
}

// reserved windows file names
boost::wregex expression( LR"((?:\`|\\|/)(?:CON|PRN|AUX|NUL|(?:COM[1-9])|(?:LPT[1-9]))(?:\'|\\|\.|/))", boost::regex::perl | boost::regex::icase);

boost::match_flag_type flags = boost::match_default;

bool hasReservedFileName(const std::wstring& fileName) {
  // a reserved sring at the begining of string, or preceded by

  boost::match_results<const TCHAR*> what;

  return boost::regex_search(fileName.c_str(), what, expression, flags);
}

#define FILE_PREFIX std::wstring(LR"(\\?\)")

SpecialFileRead::SpecialFileRead(const std::wstring& fileName)
    : Base(fileName) {
  if (hasReservedFileName(fileName)) {
    createReadFile(FILE_PREFIX + fileName);
  }
  else {
    __super::setFStream(std::make_shared< std::wifstream>(fileName.c_str()));
  }
}

SpecialFileWrite::SpecialFileWrite(const std::wstring& fileName, bool append, bool binary)
    : Base(fileName) {
  if (hasReservedFileName(fileName)) {
    createWriteFile(FILE_PREFIX + fileName, append, binary);
  }
  else {
    file_type_ptr f(std::make_shared< std::wofstream >());

    int mode = (append ? std::ios_base::app : 0) | (binary ? std::ios_base::binary : 0);

    if (mode != 0) {
      f->open(fileName.c_str(), mode);
    }
    else {
      f->open(fileName.c_str());
    }

    f->precision(12);
    __super::setFStream(f);
  }
}

MISCWIN_API void yloader::SpecialFileRead::createReadFile(const std::wstring& fileName) {
  HANDLE file_handle = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  __super::setHandle(file_handle);
  int file_descriptor = _open_osfhandle((intptr_t)file_handle, 0);

  setFileDescriptor(file_descriptor);
  FILE* file = _fdopen(file_descriptor, "r");

  __super::setFILE(file);
  __super::setFStream(file_type_ptr(new std::wifstream(file)));
}

MISCWIN_API void yloader::SpecialFileWrite::createWriteFile(const std::wstring& fileName, bool append, bool binary) {
  HANDLE file_handle = CreateFile(fileName.c_str(), append ? FILE_APPEND_DATA : GENERIC_WRITE, 0, NULL, append ? OPEN_ALWAYS : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  __super::setHandle(file_handle);

  int file_descriptor = _open_osfhandle((intptr_t)file_handle, 0);

  setFileDescriptor(file_descriptor);
  FILE* file = _fdopen(file_descriptor, "w");

  __super::setFILE(file);

  file_type_ptr fs(new std::wofstream(file));

  __super::setFStream(fs);
}

MISCWIN_API std::wstring urlEncode(const std::wstring& url) {
  if (url.empty()) return std::wstring();

  TCHAR buff[1];
  DWORD buffSize(1);

  if (!InternetCanonicalizeUrl(url.c_str(), buff, &buffSize, 0)) {
    return std::wstring();
  }
  else if (buffSize > 0) {
    boost::shared_array<TCHAR> buffer(new TCHAR[buffSize]);

    if (!InternetCanonicalizeUrl(url.c_str(), buffer.get(), &buffSize, 0)) {
      return std::wstring();
    }
    else {
      return std::wstring(buffer.get());
    }

  }
  else {
    return std::wstring();
  }
}

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
#include <plugin.h>

void yloader::PluginExplorer::initIgnoreModulesSet(std::set<std::wstring>& ignoreModulesSet) {
  ignoreModulesSet.insert(yloader::to_lower_case(L"misc.dll"));
  ignoreModulesSet.insert(yloader::to_lower_case(L"miscwin.dll"));
}

bool yloader::PluginExplorer::ignoreModule(const std::wstring& m_fileName) {
  static std::set<std::wstring> ignoreModules;

  initIgnoreModulesSet(ignoreModules);

  const std::wstring fileName(yloader::to_lower_case(m_fileName));

  return ignoreModules.find(fileName) != ignoreModules.end();
}

void yloader::PluginExplorer::explore(const std::wstring& p, const std::wstring& ext, bool recursive,
    PluginLoadingStatusHandler* loadingStatusHandler, std::vector<InfoPtr>& duplicates) {
  std::wstring path = p.length() == 0 ? L"." : p;
  {
    std::wstring fileName(yloader::addBackSlash(path) + L"*.*");

    CFileFind finder;

    BOOL bWorking = finder.FindFile(fileName.c_str());
    while (bWorking) {
      bWorking = finder.FindNextFile();

      // skip . and .. files; otherwise, we'd
      // recur infinitely!
      if (finder.IsDots()) {
        continue;
      }

      // if it's a directory, recursively search it
      if (finder.IsDirectory() && recursive) {
        explore(std::wstring(finder.GetFilePath()), ext, recursive, loadingStatusHandler, duplicates);
      }
    }
    finder.Close();
  }
  {
    std::wstring fileName(yloader::addBackSlash(path));

    if (ext.length() == 0) {
      fileName += L"*.*";
    }
    else if (fileName.at(0) == L'.') {
      fileName += std::wstring(L"*") + ext;
    }
    else {
      fileName += std::wstring(L"*.") + ext;
    }

    CFileFind finder;

    BOOL bWorking = finder.FindFile(fileName.c_str());
    while (bWorking) {
      bWorking = finder.FindNextFile();

      // skip . and .. files; otherwise, we'd
      // recur infinitely!
      if (finder.IsDots()) continue;

      if (finder.IsDirectory() || ignoreModule((LPCTSTR)(finder.GetFileName()))) {
        continue;
      }
      else {
        std::wstring filePath(finder.GetFilePath());

        process(filePath, loadingStatusHandler, duplicates);
      }
    }
    finder.Close();
  }
}

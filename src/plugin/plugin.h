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

#pragma once

#include <hinstance.h>
class PluginMethodException : public yloader::PluginException {
 private:
  const std::wstring _methodName;

 public:
  PluginMethodException(const std::wstring& PluginName,
                        const std::wstring& methodName)
      : _methodName(methodName),
        PluginException(PluginName, _T( "Method exception" )) {}

  const std::wstring& getMethodName() const { return _methodName; }
};

class PluginInstanceException : public yloader::PluginException {
 public:
  PluginInstanceException(const std::wstring& pluginName)
      : PluginException(pluginName, _T( "Plugin instance exception" )) {}
};

template <class T>
class PluginInstanceBase : protected HInstance {
 protected:
  typedef yloader::Plugin<T>* (*GET_PLUGIN)();
  yloader::Plugin<T>* _plugin;

 public:
  PluginInstanceBase(const std::wstring& path) : HInstance(path), _plugin(0) {}

  yloader::Plugin<T>* operator->() {
    assert(_plugin != 0);
    return _plugin;
  }

  yloader::Plugin<T>& operator*() {
    if (_plugin != 0)
      return *_plugin;
    else
      throw PluginInstanceException(path());
  }
};

/**
 * A generic Plugin instance
 * loads a specific plugin based on it's path (including file name)
 * calls the getPlugin method which each plugin must implement
 * if this method is not implemented, or the path doesn't point to a valid
 * module an exception is thrown
 */
template <class T>
class PluginInstance : public PluginInstanceBase<T> {
 public:
  // the complete path, including name and extension
  PluginInstance(const std::wstring& path) throw(PluginMethodException,
                                                 PluginInstanceException) try
      : PluginInstanceBase
    <T>(path) {
      {
        try {
          try {
            //					LOG( log_debug, _T( "Potential plugin: ") << path
            //);
            GET_PLUGIN getPlugin =
                reinterpret_cast<GET_PLUGIN>(getProcAddress(procName));
            _plugin = (*getPlugin)();
          } catch (const HInstanceMethodException&) {
            //					LOG( log_debug, _T( "Not a plugin: ") << path << ", " <<
            //procName << " was not found" );
          }
        } catch (const HInstanceException&) {
          LOG(log_debug, _T( "HInstanceException caught" ));
          throw PluginException(_T( "???" ), __super::path());
        } catch (const HInstanceMethodException& e) {
          LOG(log_debug, _T( "HInstanceMethodException caught" ));
          throw PluginMethodException(__super::path(), e.name());
        }
      }
    }
  catch (const HInstanceException&) {
    throw PluginInstanceException(path);
  }

  typedef void (*RELEASE_PLUGIN)();
  ~PluginInstance() {
    try {
      RELEASE_PLUGIN releasePlugin =
          reinterpret_cast<RELEASE_PLUGIN>(getProcAddress(releaseProcName));
      (*releasePlugin)();
    } catch (const HInstanceMethodException&) {
    }
  }

 private:
  static const std::wstring procName;
  static const std::wstring releaseProcName;
};
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

// to generate compiler warnings with location, that are clickable in the
// compiler output window ex: #pragma message(__LOC__"Need to do exception
// handling")

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__) ") : "

#define __TODO__ __LOC__ "warning Todo: "
#define todo __TODO__

#ifdef MISC_EXPORTS
#define MISC_API __declspec(dllexport)
#else
#define MISC_API __declspec(dllimport)
#endif

/** @file
 *  \brief Contains various classes, typedefs, defines
 */

#include <assert.h>
#include <math.h>
#include <tchar.h>
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <vector>
#include <mutex>
#include <optional>

#include "rpc.h"
#include "rpcdce.h"

#ifdef MISC_EXPORTS
#define MISC_API __declspec(dllexport)
#else
#define MISC_API __declspec(dllimport)
#endif


#include "datetime.h"
#include "sharedptr.h"
#include "strings.h"
#include "versionno.h"

using namespace std::string_literals;

namespace yloader {
/**\defgroup Uitilities Utility definitions
 * @{
 */

//@}

/**
 * a unique id, currently implemented as a string
 */

class UniqueIdException {
 private:
  const std::wstring m_id;

 public:
  UniqueIdException(const TCHAR* id) : m_id(id) {}

  UniqueIdException(const std::wstring& id) : m_id(id) {}

  const std::wstring& id() const { return m_id; }
};

class UniqueId : public std::wstring {
 public:
  UniqueId::UniqueId()
    : std::wstring(MakeUuidString()) {
  }

  UniqueId::UniqueId(const TCHAR* str){
    UUID uuid;
    if (UuidFromString(reinterpret_cast<RPC_WSTR>(const_cast<TCHAR*>(str)), &uuid) != RPC_S_OK)
      throw UniqueIdException(str);

    std::wstring::operator=(reinterpret_cast<LPCTSTR>(str));
  }

  UniqueId(const UniqueId& id) : std::wstring(id) {}

  const UniqueId& operator=(const UniqueId& id) {
    if (this != &id) std::wstring::operator=(id);
    return *this;
  }

  inline std::wstring MakeUuidString(UUID* pUUID = NULL) {
    std::wstring sUUID;
    RPC_WSTR sTemp;
    BOOL bAllocated = FALSE;

    if (pUUID == NULL) {
      pUUID = new UUID;
      bAllocated = TRUE;
    }
    if (pUUID != NULL) {
      HRESULT hr;
      hr = UuidCreateSequential(pUUID);
      if (hr == RPC_S_OK) {
        hr = UuidToString(pUUID, &sTemp);
        if (hr == RPC_S_OK) {
          sUUID = (LPCTSTR)sTemp;
          RpcStringFree(&sTemp);
        }
      }
      if (bAllocated) {
        delete pUUID;
        pUUID = NULL;
      }
    }
    return sUUID;
  }
};

class UniqueIdVector : public std::vector<UniqueId> {
 public:
  UniqueIdVector(const std::vector<std::wstring>& ids) {
    for (const auto& id : ids) {
      push_back(UniqueId(id.c_str()));
    }
  }

  UniqueIdVector() {}
};

using UniqueIdPtrVector = std::vector<const UniqueId*>;

/**\defgroup Multithreading Multithreading functions and classes
 *
 * @{
 */

MISC_API std::wostream& tsprint(const std::wstring& str, std::wostream& os);
MISC_API std::wostream& tsprint(const std::wostringstream& o, std::wostream& os);

/**
 * This mutex is used internally by print and dump calls.
 *
 * It is made available so the user code can synchronize its own threads with
 * these functions if necessary
 *
 * @see std::wstring
 * @see PositionsManager::dump
 */
extern MISC_API std::mutex m;

//@}

/**\defgroup Collections Various collection classes
 * @{
 */

// TODO: these pointer collections should be implemented using a different
// allocator leave like this for now not all methods will work as expected, such
// as erase or remove, depending on the container

/**
 * A vector of pointers
 *
 * A regular STL vector of pointers does not delete the pointers in the
 * destructor. At the same time it is not a good idea to use smart pointers in a
 * vector because of assignment and copy issues so PtrVector implements a vector
 * of regular pointers whose destructor deletes all pointers
 */
template <class T>
class PtrVector : public std::vector<T*> {
 private:
  using PtrVectorBase = std::vector<T*>;

 public:
  virtual ~PtrVector() { deleteAll(); }

  void deleteAll() {
    for (unsigned int n = 0; n < size(); n++) delete at(n);
  }

  void clear() {
    deleteAll();
    PtrVectorBase::clear();
  }
};

/**
 * A vector of pointers to constant objects
 *
 * A regular STL vector of pointers does not delete the pointers in the
 * destructor. At the same time it is not a good idea to use smart pointers in a
 * vector because of assignment and copy issues so PtrVector implements a vector
 * of regular pointers whose destructor deletes all pointers
 */
template <class T>
class ConstPtrVector : public PtrVector<const T> {};

// note: erase has not been overriden, so it will not delete the pointer!
template <class T, class U>
class PtrMap : public std::map<T, U*> {
 private:
  using PtrMapBase = std::map<T, U*>;

 public:
  virtual ~PtrMap() { deleteAll(); }

  void deleteAll() {
    for (PtrMapBase::iterator i = begin(); i != end(); i++) delete i->second;
  }

  void clear() {
    deleteAll();
    PtrMapBase::clear();
  }
};

/**
 * A map of an arbitrary type T to pointers to constant objects of type U
 *
 * A regular STL map of pointers does not delete the pointers in the destructor.
 * At the same time it is not a good idea to use smart pointers because of
 * assignment and copy issues so PtrMap implements a map of regular pointers
 * whose destructor deletes all pointers
 */
template <class T, class U>
class ConstPtrMap : public PtrMap<T, const U> {};

template <class T>
class PtrSet : public std::set<T*> {
 private:
  using PtrSetBase = std::set<T*>;

 public:
  virtual ~PtrSet() { deleteAll(); }

  void deleteAll() {
    for (PtrSetBase::iterator i = begin(); i != end(); i++) delete *i;
  }

  void clear() {
    deleteAll();
    PtrSetBase::clear();
  }
};

template <class T>
class ConstPtrSet : public PtrSet<const T> {};

/** \brief a vector of strings
 *
 * Can be used as a base class for a polymorphic multiply derived class, because
 * of the virtual destructor
 */

using StrVectorBase = std::vector<std::wstring>;

class StrVector : public StrVectorBase {
 public:
  StrVector(const StrVectorBase& v) : StrVectorBase(v) {}

  StrVector() {}

  virtual ~StrVector() {}

  StrVector& operator=(const StrVector& sv) {
    if (this != &sv) __super::operator=(sv);

    return *this;
  }
};

using StrSet = std::set<std::wstring>;

/**
 * Generic Info that all plug-ins and plug-in configurations, as well as other
 * classes must have.
 *
 * Info consists of a UniqueId, a name and a description
 */
class Info {
 private:
  const UniqueId m_id;
  const std::wstring m_name;
  const std::wstring m_description;

 public:
  /**
   * Constructor taking 3 arguments: UniqueId, and the name and description,
   * which can be any arbitrary strings
   *
   * @param id     the unique id
   * @param name   The name
   * @param description
   *               The description
   */
  Info(const UniqueId& id, const std::wstring& name,
       const std::wstring& description)
      : m_id(id), m_name(name), m_description(description) {}
  /**
   * Copy constructor - takes another Info as argument
   *
   * @param info   The source Info for the copy operation
   */
  Info(const Info& info)
      : m_id(info.id()), m_name(info.name()), m_description(info.description()) {}
  /**
   * default constructor - generates a new unique id and sets the name and
   * description to empty strings
   */
  Info() {}

  /**
   * Returns the unique id
   *
   * @return the unique id
   */
  const UniqueId& id() const { return m_id; }
  /**
   * Returns the name
   *
   * @return the name
   */
  const std::wstring& name() const { return m_name; }
  /**
   * Returns the description
   *
   * @return the description
   */
  const std::wstring& description() const { return m_description; }
};

using InfoPtr = std::shared_ptr<Info>;
using UniqueIdPtr = std::shared_ptr<UniqueId>;

class Range;
class DataRequester;

class VersionException {
 private:
  const std::wstring m_message;

 public:
  VersionException(const std::wstring& message) : m_message(message) {}

  const std::wstring& message() const { return m_message; }
};
/**
 * Version contains:
 *	- major
        - minor
        - revision
        - build
        - beta (optional)

        When comparing two version, everything else being equal,
        the one without beta is higher than the one with beta
 */
class MISC_API Version {
 private:
  unsigned int m_major;
  unsigned int m_minor;
  unsigned int m_revision;
  unsigned int m_build;
#define VERSION_SEPARATOR L"."
#define VERSION_SEPARATORS VERSION_SEPARATOR L","

 private:
  void parse(const TCHAR* version);

 public:
  static const Version CURRENT;
  /**
   * Default constructor - sets the current version from the package version
   * string
   */
  Version();
  /**
   * Constructor - takes the 4 values that define a version as parameters
   *
   * @param major
   * @param minor
   * @param build
   * @param revision
   */
  Version(unsigned int major, unsigned int minor, unsigned int revision,
          unsigned int build);

  /**
   * Constructor from string. Ex: "2.0.25.10"
   * accepts anything in between 1 and 4 fields:
   * 1
   * 1.2
   * 1.2.3
   * 1.2.3.4
   */
  Version(const std::wstring& version);
  Version(const TCHAR* version);

  /**
   * Returns the major number
   *
   * @return the major number
   */
  unsigned int major() const { return m_major; }

  /**
   * Returns the minor number
   *
   * @return the minor number
   */
  unsigned int minor() const { return m_minor; }

  /**
   * returns the revision number
   *
   * @return the revision number
   */
  unsigned int revision() const { return m_revision; }
  /**
   * returns the build number
   *
   * @return the build number
   */
  unsigned int build() const { return m_build; }

  bool operator>(const Version& version) {
    return m_major > version.major()
               ? true
               : m_major < version.major()
                     ? false
                     : m_minor > version.minor()
                           ? true
                           : m_minor < version.minor()
                                 ? false
                                 : m_revision > version.revision()
                                       ? true
                                       : m_revision < version.revision()
                                             ? false
                                             : m_build > version.build();
  }

  operator bool() const { return m_major > 0; }

  std::wstring toShortString() const {
    std::wostringstream o;
    o << major() << VERSION_SEPARATOR << minor();
    return o.str();
  }

  std::wstring toString() const {
    std::wostringstream o;
    o << toShortString() << VERSION_SEPARATOR << revision() << VERSION_SEPARATOR
      << build();
    return o.str();
  }
};

#if defined(_WIN64)
constexpr auto BUILD_TYPE = L"64 bit";
#else
constexpr auto BUILD_TYPE = L"32 bit";
#endif

inline void printLine(const std::wstring& str) {
  yloader::tsprint(str + L"\n", std::wcout);
}

inline void printLine() { printLine(std::wstring()); }

inline std::wstring getCaption(const std::wstring& productName) {
  return productName + L" v" + Version::CURRENT.toString() + L", " + BUILD_TYPE;
}

inline void printDashedLine(const std::wstring& str) {
  printLine(L"------"s + L" " + str + L" " + L"------"s);
}

inline void printDashedLine(const TCHAR* str) {
  printDashedLine(std::wstring(str));
}

/**
 * Generates file name for a symbol file.
 *
 * Handles flat data (all symbols files in the same directory), or hierarchical
 * data (symbol files in nested directories).
 *
 * By default, this class generates the file name, but derived classes can also
 * generate the necessary subdirectories, by overrideing the the virtual method
 * createDirectory.
 *
 * In case of nested directories, there is a base dir, under which 2 levels are
 * created: The first is the first char of the symbol ( with transformation for
 * invalid file characters), the second level being the second character of the
 * symbol. In case of one char symbols, the two levels are both the symbol.
 */
class FileName {
 private:
  const bool m_flatData;

  wchar_t transformInvalidFileChars(wchar_t c) const {
    if (c == L'\\')
      return L'a';
    else if (c == L'/')
      return L'b';
    else if (c == L':')
      return L'c';
    else if (c == L'*')
      return L'd';
    else if (c == L'?')
      return L'e';
    else if (c == L'\"')
      return L'f';
    else if (c == L'<')
      return L'g';
    else if (c == L'>')
      return L'h';
    else if (c == L'|')
      return L'i';
    else
      return c;
  }

 public:
  FileName(bool flatData) : m_flatData(flatData) {}

  virtual ~FileName() {}

  // path is the root path, symbol is used to generated nested subdirs using its
  // first and second char depending on the "flatData" member, and filename is
  // the name part including the extension
  const std::wstring makePath(const std::wstring& p, const std::wstring& symbol,
                              const std::wstring& fileName) const {
    std::wostringstream path;
    path << addBackSlash(p);

    if (!m_flatData) {
      std::wstring first(1, transformInvalidFileChars(symbol[0]));
      std::wstring second(1, symbol.length() == 1 ? symbol[0] : transformInvalidFileChars(symbol[1]));

      path << first;

      createDir(path.str());

      path << L"\\" << second;

      createDir(path.str());

      path << L"\\";
    }

    path << fileName;

    return path.str();
  }

  virtual void createDir(const std::wstring& path) const {}
};

MISC_API std::vector<std::string> cmdLineSplitter(const std::wstring& line);

}  // end namespace yloader

// class used by command line
template <class T> class Settable {
 private:
   std::optional< T > m_value;
 public:
   const Settable() = default;
   const Settable(const T& t)
     : m_value(t) {
   }
  const Settable& operator=(const T& t) {
    *m_value = t;

    return *this;
  }

  void set(const T& t) {
    m_value = t;
  }

  bool isSet() const { return m_value.has_value(); }
  const T& getValue() const { return *m_value; }
  operator const T&() const { return *m_value; }
  const T& getValue(const T& def) const { return isSet() ? *m_value : def; }
};

using StringSettable = Settable<std::wstring>;
using BoolSettable = Settable<bool>;
using UIntSettable = Settable<unsigned int>;
using DoubleSettable = Settable<double>;
using ULongSettable = Settable<unsigned long>;
using IntSettable = Settable<int>;
using StrVectorSettable = Settable<std::vector<std::string> >;

////////////////////
///
inline double round(double x) {
  double intpart;
  double frac = modf(x, &intpart);
  return frac >= 0.5 || (frac < 0 && frac > -0.5) ? ceil(x) : floor(x);
}

inline std::wstring char2hex(char dec) {
  wchar_t dig1 = (dec & 0xF0) >> 4;
  wchar_t dig2 = (dec & 0x0F);
  if (0 <= dig1 && dig1 <= 9) dig1 += 48;         // 0,48inascii
  if (10 <= dig1 && dig1 <= 15) dig1 += 97 - 10;  // a,97inascii
  if (0 <= dig2 && dig2 <= 9) dig2 += 48;
  if (10 <= dig2 && dig2 <= 15) dig2 += 97 - 10;

  std::wstring r;
  r.append(&dig1, 1);
  r.append(&dig2, 1);
  return r;
}

inline std::wstring urlencode(const std::wstring& url) {
  std::wstring escaped;
  for (wchar_t c : url) {
    if ((L'0' <= c && c <= L'9') ||   // 0-9
        (L'a' <= c && c <= L'z') ||   // abc...xyz
        (L'A' <= c && c <= L'Z') ||  // ABC...XYZ
        (c == L'~' || c == L'!' || c == L'*' ||
         c == L'(' || c == L')' || c == L'\'')) {
      escaped.append(&c, 1);
    }
    else {
      escaped.append(L"%");
      escaped.append(char2hex(c));  // converts char 255 to string "ff"
    }
  }
  return escaped;
}

//@}

// using namespace yloader

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

// to generate compiler warnings with location, that are clickable in the
// compiler output window ex: #pragma message(__LOC__"Need to do exception
// handling")

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__) ") : "

#define __TODO__ __LOC__ "warning Todo: "
#define todo __TODO__

#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#pragma warning(disable : 4800)
#pragma warning(disable : 4290)

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

#include "rpc.h"
#include "rpcdce.h"

#include "datetime.h"
#include "sharedptr.h"
#include "strings.h"
#include "threadsync.h"
#include "versionno.h"

namespace yloader {
/**\defgroup Uitilities Utility definitions
 * @{
 */

/** \defgroup MinMax Min and Max
 * Definitions of min and max template functions with 2 respective 3 operators
 * @{
 */
template <class T>
const T& max2(const T& x, const T& y) {
  return x > y ? x : y;
}
template <class T>
const T& min2(const T& x, const T& y) {
  return x < y ? x : y;
}
template <class T>
const T& max3(const T& x, const T& y, const T& z) {
  return max2(x, max2(y, z));
}
template <class T>
const T& min3(const T& x, const T& y, const T& z) {
  return min2(x, min2(y, z));
}
//@}

/*typedef std::wstring std::wstring;
typedef std::wostringstream std::wostringstream;
typedef std::wistringstream std::wistringstream;
*/

using yloader::operator<<;

/**
 * a unique id, currently implemented as a string
 */

class UniqueIdException {
 private:
  const std::wstring _id;

 public:
  UniqueIdException(const TCHAR* id) : _id(id) {}

  UniqueIdException(const std::wstring& id) : _id(id) {}

  const std::wstring& id() const { return _id; }
};

inline std::wstring s2ws(const char* str) { return s2ws(std::string(str)); }

class UniqueId : public std::wstring {
 public:
  UniqueId::UniqueId() : std::wstring(MakeUuidString()) {
    //	TSPRINT_DEBUG( _T( "Unique id: " ) );
    //	TSPRINT_DEBUG( *this );
    //	TSPRINT_DEBUG( _T( "\n" ) );
  }

  UniqueId::UniqueId(const TCHAR* str) throw(UniqueIdException) {
    UUID uuid;
    if (UuidFromString(reinterpret_cast<RPC_WSTR>(const_cast<TCHAR*>(str)),
                       &uuid) != RPC_S_OK)
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
    for (size_t n = 0; n < ids.size(); n++) push_back(UniqueId(ids[n].c_str()));
  }

  UniqueIdVector() {}
};

typedef std::vector<const UniqueId*> UniqueIdPtrVector;

#if defined UNICODE
// typedef std::wifstream t_ifstream;
typedef std::wostream t_ostream;
typedef std::wofstream t_ofstream;
typedef std::wifstream t_ifstream;
#else
// typedef std::ifstream t_ifstream;
typedef std::basic_ostream<TCHAR> t_ostream;
#endif

/**\defgroup Multithreading Multithreading functions and classes
 *
 * @{
 */

MISC_API t_ostream& tsprint(const std::wstring& str, t_ostream& os);
MISC_API t_ostream& tsprint(const std::wostringstream& o, t_ostream& os);
// this prints to the debug output stream (on Windows this uses
// OuptputDebugString

#ifdef _DEBUG
MISC_API void tsprint_debug(const std::wostringstream& o);
MISC_API void tsprint_debug(const std::wstring& str);
#define TSPRINT_DEBUG(str) tsprint_debug(str)
#else
#define TSPRINT_DEBUG(str)
#endif

/**
 * This mutex is used internally by print and dump calls.
 *
 * It is made available so the user code can synchronize its own threads with
 * these functions if necessary
 *
 * @see std::wstring
 * @see PositionsManager::dump
 */
extern MISC_API Mutex m;

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
  typedef std::vector<T*> PtrVectorBase;

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
  typedef std::map<T, U*> PtrMapBase;

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
  typedef std::set<T*> PtrSetBase;

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

typedef std::vector<std::wstring> StrVectorBase;

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

typedef std::set<std::wstring> StrSet;

/**
 * Generic Info that all plug-ins and plug-in configurations, as well as other
 * classes must have.
 *
 * Info consists of a UniqueId, a name and a description
 */
class Info {
 private:
  const UniqueId _id;
  const std::wstring _name;
  const std::wstring _description;

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
      : _id(id), _name(name), _description(description) {}
  /**
   * Copy constructor - takes another Info as argument
   *
   * @param info   The source Info for the copy operation
   */
  Info(const Info& info)
      : _id(info.id()), _name(info.name()), _description(info.description()) {}
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
  const UniqueId& id() const { return _id; }
  /**
   * Returns the name
   *
   * @return the name
   */
  const std::wstring& name() const { return _name; }
  /**
   * Returns the description
   *
   * @return the description
   */
  const std::wstring& description() const { return _description; }
};

typedef ManagedPtr<Info> InfoPtr;
typedef ManagedPtr<UniqueId> UniqueIdPtr;

class Range;
class DataRequester;

class VersionException {
 private:
  const std::wstring _message;

 public:
  VersionException(const std::wstring& message) : _message(message) {}

  const std::wstring& message() const { return _message; }
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
#define VERSION_SEPARATOR _T( ".")
#define VERSION_SEPARATORS VERSION_SEPARATOR _T( ",")

 private:
  void parse(const TCHAR* version) throw(VersionException);

 public:
  static const Version CURRENT;
  /**
   * Default constructor - sets the current version from the package version
   * string
   */
  Version() throw(VersionException);
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
  Version(const std::wstring& version) throw(VersionException);
  Version(const TCHAR* version) throw(VersionException);

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
#define BUILD_TYPE _T( "64 bit")
#else
#define BUILD_TYPE _T( "32 bit")
#endif

inline void printLine(const std::wstring& str) {
  yloader::tsprint(str + _T("\n"), std::wcout);
}

inline void printLine() { printLine(std::wstring()); }

inline std::wstring getCaption(const std::wstring& productName) {
  return productName + _T(" v") + Version::CURRENT.toString() + _T( ", " ) +
         BUILD_TYPE;
}

inline void printDashedLine(const std::wstring& str) {
  const std::wstring DASHES(_T("------"));
  const std::wstring SPACE(_T(" "));
  printLine(DASHES + SPACE + str + SPACE + DASHES);
}

inline void printDashedLine(const TCHAR* str) {
  printDashedLine(std::wstring(str));
}

#pragma warning(default : 4800)
/**
 * Diagnostic class - counts instances of a class as they are created and
 * destroyed -
 *
 * can be used to make sure that all instances of a class are destroyed, or to
 * see how many instances there are at any given moment
 *
 * By default _os is initialized with std::cout, but it can be assigned another
 * reference at runtime
 */
class ObjCount {
 private:
  MISC_API static int _objCount;
  MISC_API static t_ostream& _os;
  MISC_API static int _totalObjects;

  const std::wstring _name;

 public:
  ObjCount(const std::wstring& name = _T( "" )) : _name(name) {
    _objCount++;
    _totalObjects++;
    std::wostringstream o;
    o << _name << _T( " constructor, count: " ) << _objCount
      << _T( ", total objects: " ) << _totalObjects << std::endl;
    TSPRINT_DEBUG(o);
  }

  virtual ~ObjCount() {
    _objCount--;
    std::wostringstream o;
    o << _name << _T( " destructor, count: " ) << _objCount << std::endl;
    TSPRINT_DEBUG(o);
  }
};

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
  const bool _flatData;

  TCHAR transformInvalidFileChars(TCHAR c) const {
    if (c == _TCHAR('\\'))
      return _TCHAR('a');
    else if (c == _TCHAR('/'))
      return _TCHAR('b');
    else if (c == _TCHAR(':'))
      return _TCHAR('c');
    else if (c == _TCHAR('*'))
      return _TCHAR('d');
    else if (c == _TCHAR('?'))
      return _TCHAR('e');
    else if (c == _TCHAR('\"'))
      return _TCHAR('f');
    else if (c == _TCHAR('<'))
      return _TCHAR('g');
    else if (c == _TCHAR('>'))
      return _TCHAR('h');
    else if (c == _TCHAR('|'))
      return _TCHAR('i');
    else
      return c;
  }

 public:
  FileName(bool flatData) : _flatData(flatData) {}

  virtual ~FileName() {}

  // path is the root path, symbol is used to generated nested subdirs using its
  // first and second char depending on the "flatData" member, and filename is
  // the name part including the extension
  const std::wstring makePath(const std::wstring& p, const std::wstring& symbol,
                              const std::wstring& fileName) const {
    std::wostringstream path;
    path << addBackSlash(p);

    if (!_flatData) {
      std::wstring first(1, transformInvalidFileChars(symbol[0]));
      std::wstring second(1, symbol.length() == 1
                                 ? symbol[0]
                                 : transformInvalidFileChars(symbol[1]));

      path << first;

      createDir(path.str());

      path << _T( "\\" ) << second;

      createDir(path.str());

      path << _T( "\\" );
    }

    path << fileName;

    return path.str();
  }

  virtual void createDir(const std::wstring& path) const {}
};

/*
template< class Key, class Value > class Element
{
}


template< class Key, class Value >class Map
{
  private:
    typedef std::map< Key, Value > _Map;

    _Map _map;

  public:
    void insert( Key key, Value value )
    {
    }

*/

#define COUT std::cout << _T( "[" ) << __FUNCTION__ << _T( "] " )

MISC_API std::vector<std::string> cmdLineSplitter(const std::wstring& line);

}  // end namespace yloader
// utility macro - define a basic exception class
#define BASIC_EXCEPTION(name)                                             \
  \
class name : public std::exception\
{\
\
public : name(const std::wstring& message) : std::exception(              \
                 yloader::ws2s(message).c_str()){} std::wstring message() \
                 const {return yloader::s2ws(__super::what());            \
  }                                                                       \
  \
}                                                                      \
  ;

#define BASIC_EXCEPTION_DEF_CONSTRUCTOR(name) \
  \
BASIC_EXCEPTION(name##Base)                   \
  \
class name : public name##Base\
{              \
    \
public : name() : name##Base(""){}            \
  \
};

// class used by command line

template <class T>
class Settable {
 private:
  bool _set;
  T _value;

 public:
  Settable() : _set(false) {}

  const Settable& operator=(const T& t) {
    _set = true;
    _value = t;

    return *this;
  }

  void set(const T& t) {
    _set = true;
    _value = t;
  }

  bool isSet() const { return _set; }
  const T& getValue() const { return _value; }
  operator const T&() const { return _value; }
  const T& getValue(const T& def) const { return isSet() ? _value : def; }
};

typedef Settable<std::wstring> StringSettable;
typedef Settable<bool> BoolSettable;
typedef Settable<unsigned int> UIntSettable;
typedef Settable<double> DoubleSettable;
typedef Settable<unsigned long> ULongSettable;
typedef Settable<int> IntSettable;
typedef Settable<std::vector<std::string> > StrVectorSettable;

////////////////////
///
inline double round(double x) {
  double intpart;
  double frac = modf(x, &intpart);
  return frac >= 0.5 || (frac < 0 && frac > -0.5) ? ceil(x) : floor(x);
}

inline std::wstring char2hex(char dec) {
  TCHAR dig1 = (dec & 0xF0) >> 4;
  TCHAR dig2 = (dec & 0x0F);
  if (0 <= dig1 && dig1 <= 9) dig1 += 48;         // 0,48inascii
  if (10 <= dig1 && dig1 <= 15) dig1 += 97 - 10;  // a,97inascii
  if (0 <= dig2 && dig2 <= 9) dig2 += 48;
  if (10 <= dig2 && dig2 <= 15) dig2 += 97 - 10;

  std::wstring r;
  r.append(&dig1, 1);
  r.append(&dig2, 1);
  return r;
}

inline std::wstring urlencode(const std::wstring& c) {
  std::wstring escaped;
  int max = c.length();
  for (int i = 0; i < max; i++) {
    if ((48 <= c[i] && c[i] <= 57) ||   // 0-9
        (65 <= c[i] && c[i] <= 90) ||   // abc...xyz
        (97 <= c[i] && c[i] <= 122) ||  // ABC...XYZ
        (c[i] == TCHAR('~') || c[i] == TCHAR('!') || c[i] == TCHAR('*') ||
         c[i] == TCHAR('(') || c[i] == TCHAR(')') || c[i] == TCHAR('\''))) {
      escaped.append(&c[i], 1);
    } else {
      escaped.append(_T("%"));
      escaped.append(char2hex(c[i]));  // converts char 255 to string "ff"
    }
  }
  return escaped;
}

//@}

// using namespace yloader

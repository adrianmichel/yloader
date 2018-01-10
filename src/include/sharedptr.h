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

#pragma warning(disable : 4800 4275 4251 4244 4003)

#include <assert.h>
#include <list>
#include <vector>
#include "strings.h"
#include "threadsync.h"

namespace yloader {
/**
 * @}
 */

/**\defgroup Pointers Various pointer classes
 * @{
 */

/**
 * Smart pointer with a special assignment operator
 *
 * This class is used the same way std::auto_ptr is used with the difference
 * that when applying the assignment operator AutoPtrX deletes the pointer being
 * replaced, while std::auto_ptr doesn't.
 *
 * Certain YLoader types are exposed to the user as AutoPtrX pointers, which
 * makes the tedious aspects of keeping track of pointers and deleting them
 * transparent to the user.
 *
 * For example:
 * \code
 * AutoPtrX< Some_type> p1;
 * AutoPtrX< Some_type> p2;
 *
 * // initialize the two pointer
 *
 * // applying the assignment operator will first delete the pointer pointed to
 * // by p1 and then assign p2 to p1, and p2 will lose ownership of the pointer
 * it points to p1 = p2; \endcode
 */

/**
 * Reference counter class
 *
 * Used by ManagedPtr to count references to a pointer
 *
 * This class is normally only used internally by the library, and never
 * instantiated explicitly by user code.
 *
 * id is here just for diagnostic purposes - it shows the id of the data item
 * that's being counted.
 *
 * RefCountable is thread safe
 *
 * @see ManagedPtr
 */
class RefCountable {
 protected:
  MISC_API static int _totalCount;
  mutable int ref_count;
  mutable Mutex _m;
  std::wstring _id;

 public:
  RefCountable() : ref_count(1) {
#ifdef _DEBUG
    if (false) {
      std::wostringstream o;
      o << _T( "Ref countable - in one param constructor- " )
        << _T( ": " ) << _totalCount << std::endl;
      //			tsprint( o, std::cout );
    }
#endif
  };

  // copy constructor
  RefCountable(const RefCountable& rc) : ref_count(rc.getCount()) {
#ifdef _DEBUG
    if (false) {
      std::wostringstream o;
      o << _T( "Ref countable - in copy constructor - " )
        << _T( ": " ) << ref_count << std::endl;
      //			tsprint( o, std::cout );
    }
#endif
  }

  ~RefCountable() {
    // when destroying the RefCountable, all references to T must have been
    // released cannot destroy a RefCountable which has the reference count != 0
    assert(ref_count == 0);
#ifdef _DEBUG
    if (false) {
      std::wostringstream o;
      o << _T( "Ref countable - in destructor - " )
        << _T( ": " ) << ref_count << std::endl;
      //			tsprint( o, std::cout );
    }
#endif
  }

  void addRef() const {
    Lock lock(_m);
    ++_totalCount;
    ++ref_count;

#ifdef _DEBUG
    if (false) {
      std::wostringstream o;
      //			o << _T( "addRef - " ) << _id << _T( ": " ) <<
      //_totalCount << std::endl;
      o << _T( "addRef - " ) << ref_count << std::endl;
      //			tsprint( o, std::cout );
    }
#endif
  };

  bool release() {
    Lock lock(_m);
    --_totalCount;
    --ref_count;
    // cannot release more times than addRef-ed

    assert(getCount() >= 0);
#ifdef _DEBUG
    if (false) {
      std::wostringstream o;
      //			o << _T( "release - " ) << _id << _T( ": " ) <<
      //_totalCount << std::endl;
      o << _T( "release - " ) << ref_count << std::endl;
      //			tsprint( o, std::cout );
    }
#endif

    return ref_count == 0;
  };

  // assignment operator
  /*  RefCountable& operator=( const RefCountable& rc )
  {
  Lock lock( _m );
  tsprint( _T( "in RefCountable op=\n" ), std::cout );

  if ( this != &rc )
  {
  _count = rc.getCount();
  _id = rc._id;
  }
  if ( false )
  {
  std::wostringstream o;
  o << _T( "Ref countable - in operator = - " ) << _id << _T( ": " ) << _count
  << std::endl; tsprint( o, std::cout );
  }
  return *this;
  }
  */
  int getCount() const {
    Lock lock(_m);

    return ref_count;
  }
};

/**
 * Implements a smart pointer with reference counting.
 *
 * Multiple instances of ManagedPtr share one pointer to an object of type T and
 * a pointer to a reference counter (normally stored in a cache). This way, the
 * same object can be used and released in different places, without the code
 * using them having to do anything explicitly to add/release. Whenever the
 * ManagedPtr goes out of scope the reference counter is decreased. Also,
 * assignment operator and copy constructor correctly handle the reference
 * counting.
 *
 *
 * This makes the process of keeping track of objects and pointers to them
 * transparent to the user
 *
 *
 */

// todo: parametrize the delete operation
template <class T>
class PtrDeleter {
 public:
  virtual ~PtrDeleter() {}

  virtual void operator()(T* t) { delete t; }
};

template <class T>
class ManagedPtr {
 public:
  typedef void (*Deleter)(T* t);

 private:
  // the payload (the pointer that is being managed)
  T* _t;
  // the reference counter object
  mutable RefCountable* _rc;
  Deleter _deleter;

 public:
  ManagedPtr() : _t(0), _rc(new RefCountable()), _deleter(0) {
#ifdef _DEBUG
    if (false) {
      std::wostringstream o;
      o << _T( "ManagedPtr constr 1" ) << std::endl;
      //			tsprint( o, std::cout );
    }
#endif
  }

  ManagedPtr(T* t, Deleter deleter = 0)
      : _t(t), _rc(new RefCountable()), _deleter(deleter) {}

  ManagedPtr(T* t, RefCountable* rc) : _t(t), _rc(rc), _deleter(0) {
    assert(_rc != 0);
    // this object constitutes another reference to _t, so increase refcount
    _rc->addRef();

#ifdef _DEBUG
    if (false) {
      std::wostringstream o;
      o << _T( "ManagedPtr constr 2" ) << std::endl;
      //			tsprint( o, std::cout );
    }
#endif
  }

  ManagedPtr(const ManagedPtr<T>& mp)
      : _t(mp.get()), _rc(mp.getRC()), _deleter(mp._deleter) {
    assert(_rc != 0);
    // this object constitutes another reference to _t, so increase refcount
    _rc->addRef();

#ifdef _DEBUG
    if (false) {
      std::wostringstream o;
      o << _T( "ManagedPtr copy constr" ) << std::endl;
      //			tsprint( o, std::cout );
    }
#endif
  }

  ~ManagedPtr() {
    assert(_rc != 0);
    // release the pointer, and if ref count is 0 and auto destroy is true, then
    // delete the payload and the reference counter
    if (_rc->release()) {
      if (_deleter == 0)
        delete _t;
      else
        _deleter(_t);
      delete _rc;
    }

#ifdef _DEBUG
    if (false) {
      std::wostringstream o;
      o << _T( "ManagedPtr destr: " ) << std::endl;
      //			tsprint( o, std::cout );
    }
#endif
  }

  ManagedPtr<T>& operator=(const ManagedPtr<T>& mp) {
    assert(mp.getRC() != 0);
    assert(_rc != 0);
#ifdef _DEBUG
    if (false) {
      std::wostringstream o;
      o << _T( "ManagedPtr operator=: " ) << std::endl;
      //			tsprint( o, std::cout );
    }
#endif

    if (this != &mp) {
      // when assigning, release the old pointer
      // release the current pointer as it will be replaced by the new one, and
      // thus it will not be referenced by the current object any more also, if
      // the ref count is 0 and autoDestroy flag is true, delete the payload _t
      // and the reference counter
      if (_rc->release()) {
        if (_deleter == 0)
          delete _t;
        else
          _deleter(_t);
        delete _rc;
      }
      // finally assign the new values and increment the reference counter
      _t = mp.get();
      _rc = mp.getRC();
      _rc->addRef();
      _deleter = mp._deleter;
    }
    return *this;
  }

  RefCountable* getRC() const { return _rc; }

  T* operator->() { return _t; }

  const T* operator->() const { return _t; }

  T* get() const { return _t; }

  const T& operator*() const { return *_t; }

  T& operator*() { return *_t; }

  /*	const T* get() const
          {
                  return _t;
          }
  */

  bool operator==(const ManagedPtr<T> ptr) { return _t == ptr.get(); }
  bool operator==(const T* t) const { return _t == t; }

  bool operator!=(const T* t) const { return _t != t; }

  bool isReferred() const { return _rc->getCount() > 1; }

  int count() const { return _rc->getCount(); }

  void reset() { *this = ManagedPtr<T>(); }

  operator bool() const { return _t != 0; }
};

typedef ManagedPtr<std::wstring> StringPtr;
/** \brief pointer to a list of strings
 *
 */

typedef std::list<std::wstring> StrList;
typedef ManagedPtr<StrList> StrListPtr;
typedef ManagedPtr<const std::wstring> ConstStringPtr;
typedef std::vector<StringPtr> StringPtrVector;

}  // namespace yloader
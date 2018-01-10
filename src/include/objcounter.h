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

#ifdef _DEBUG

#include <fstream>
#include <set>

namespace yloader {

class Counter {
 private:
  int m_count;
  std::wstring m_name;
  std::set<int> m_indexes;
  // total number of instances created
  int m_total;
  int m_max;
  yloader::Mutex m_mx;

  void setName(const std::wstring& name) {
    if (m_name.empty()) m_name = name;
  }

 public:
  Counter() : m_count(0), m_total(0), m_max(0) {}

  ~Counter() {
    yloader::Lock lock(m_mx);
    assert(m_indexes.size() == m_count);
    if (m_count > 0) {
      LOG(log_error, "\n!! " << m_name << "leak\n\tTotal created:\t " << m_total
                             << "\n\tMax created:\t " << m_max
                             << "\n\tLeaked count:\t " << m_count
                             << "\n\tLeaked indexes:\t "
                             << leakedIxsAsString());
    } else if (m_count < 0)
      assert(false);
    else {
      // reached count 0 - all instances have been destroyed
      // todo: we may want to log this when  debugging memory/object leaks
    }
  }

  int inc(const std::wstring& name, int breakIndex) {
    yloader::Lock lock(m_mx);
    setName(name);
    ++m_total;
    ++m_count;
    // latest removed is used to ensure that we don't get a collision
    // in case an earlier index is removed before a later index
    int ix = m_total;

    m_max = m_total;
    m_indexes.insert(ix);
    if (ix == breakIndex) {
      __asm int 3;
    }
    //		LOG( log_debug, "ObjCounter - adding \"" << m_name << "\", index " <<
    //ix << ", total " << m_total );
    return ix;
  }

  void removeIndex(int index) {
    yloader::Lock lock(m_mx);
    --m_count;
    if (m_indexes.find(index) == m_indexes.end()) {
      // this indes has already been deleted - error
      assert(false);
      //			LOG( log_debug, "ObjCounter - \"" << m_name <<
      //"\" attempting to remove not found index (deleting twice?)\n\tindex:\t "
      //<< index << "\n\tcurrent ix map size:\t" << m_indexes.size() );
    } else {
      m_indexes.erase(index);
      //			LOG( log_debug, "ObjCounter - removing \"" <<
      //m_name << "\", index " <<  index << ", remaining " << m_indexes.size()
      //);
    }
  }

 private:
#define MAX_LEAKED_INDEXES_DISPLAYED 20
  std::wstring leakedIxsAsString() {
    std::wstring leakedIxs;
    int n = 0;
    for (std::set<int>::const_iterator i = m_indexes.begin();
         i != m_indexes.end() && n < MAX_LEAKED_INDEXES_DISPLAYED; ++i, ++n)
      leakedIxs << (n == 0 ? _T( "" ) : _T( ", " )) << *i;

    if (n >= MAX_LEAKED_INDEXES_DISPLAYED) leakedIxs << _T( ", ..." );

    return leakedIxs;
  }
};

template <typename T>
class ObjCounterBase {
  int m_objIx;
  static Counter m_count;

 public:
  ObjCounterBase(const std::wstring& name, int breakIndex) {
    m_objIx = m_count.inc(name, breakIndex);
  }

  // this is to make sure that when assigning an object of one class to another
  // it won't change the object index (otherwise the default operator= would set
  // the m_objIx)
  ObjCounterBase<T>& operator=(const ObjCounterBase& c) { return *this; }

  virtual ~ObjCounterBase() { m_count.removeIndex(m_objIx); }
};

}  // namespace yloader

template <typename T>
yloader::Counter yloader::ObjCounterBase<T>::m_count;

#define OBJ_CTR(COUNTER_STR, BREAK_INDEX)                                     \
  \
\
class ObjCounter : public yloader::ObjCounterBase<COUNTER_STR>\
{              \
    \
public : ObjCounter() :                                                       \
        yloader::ObjCounterBase<COUNTER_STR>(_T(#COUNTER_STR), BREAK_INDEX){} \
  \
};

#define OBJ_COUNTER(COUNTER_STR) \
  \
OBJ_CTR(COUNTER_STR, 0)          \
  \
ObjCounter m_objCounter;

#define OBJ_COUNTER_BREAK_ON_INDEX(COUNTER_STR, BREAK_INDEX) \
  \
OBJ_CTR(COUNTER_STR, BREAK_INDEX)                            \
  \
ObjCounter m_objCounter;

#else
#define OBJ_COUNTER(COUNTER_STR)
#define OBJ_COUNTER_BREAK_ON_INDEX(COUNTER_STR, BREAK_INDEX)
#endif

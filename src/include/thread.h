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

#include <misc.h>
#include <process.h>
namespace yloader

#ifdef THREAD_EXPORTS
#define THREAD_API __declspec(dllexport)
#else
#define THREAD_API __declspec(dllimport)
#endif

{
/**\brief Encapsulation of a thread
 *
 */

class THREAD_API ThreadBase {
 private:
  HANDLE _hThread;
  DWORD _threadId;
  const std::wstring _diag_string;
  bool _priority;
  bool _run;
  bool _running;

 public:
  /**
   * thread context base class - to be derived if necessary
   *
   * A pointer to a thread context object or derived is passed to the run method
   */
  class ThreadContext {
   public:
    virtual ~ThreadContext() {}
  };

  /**
   * starts the thread
   */
  virtual void start(ThreadContext* context = 0);
  virtual void startSync(ThreadContext* context = 0, unsigned long timeout = 0);

  /**
   * stops the thread synchronously
   */
  virtual void stopSync();
  /**
   * stops the thread asynchronously
   */
  virtual void stopAsync();

  /**
   * restarts the thread (stops and starts it)
   */
  void restart(ThreadContext* context = 0);
  bool isRunning() const;
  bool stopping();
  const std::wstring& name() const { return _diag_string; }
  bool postMessage(unsigned int message, WPARAM wParam = 0, LPARAM lParam = 0) {
    assert(_threadId != 0);
    return ::PostThreadMessage(_threadId, message, wParam, lParam) != 0;
  }

  bool isThreadActive() {
    return ((_hThread != NULL) &&
            (::WaitForSingleObject(_hThread, 0) != WAIT_OBJECT_0));
  }

  // returns false if timeout
  // true if the objet has become signaled
  bool waitForThread(DWORD timeout = INFINITE) {
    return ::WaitForSingleObject(_hThread, timeout) != WAIT_TIMEOUT;
  }

  HANDLE handle() { return _hThread; }

 protected:
  ThreadBase(const std::wstring& diag_string, bool priority = false)
      : _hThread(0),
        _run(false),
        _running(false),
        _diag_string(diag_string),
        _priority(priority),
        _threadId(0) {}

 public:
  virtual ~ThreadBase() {
    if (_hThread != 0) CloseHandle(_hThread);
  }

 protected:
  class Context {
   private:
    ThreadBase* _thread;
    std::auto_ptr<ThreadBase::ThreadContext> _context;

   public:
    Context(ThreadBase* thread, ThreadBase::ThreadContext* context = 0)
        : _thread(thread), _context(context) {}

    virtual ~Context() {}

    ThreadBase* thread() { return _thread; }

    ThreadBase::ThreadContext* threadContext() { return _context.get(); }

    const std::wstring& diagString() const {
      assert(_thread != 0);
      return _thread->_diag_string;
    }
  };

  /**
   * the thread method
   *
   * receives a pointer to a generic ThreadContext - must cast it to the
   * specific derived context defined by the caller
   */
  virtual void run(ThreadContext* context) = 0;
  HANDLE create(ThreadContext* context, unsigned int (*proc)(void*));

 private:
  void startx(ThreadContext* context = 0, bool sync = false,
              unsigned long timeout = 0);
  void stopx();
  void stopy();
  void runx(ThreadContext* context);
  static unsigned int threadProc(void* p);
};

class THREAD_API Thread : public ThreadBase {
 public:
  Thread(const std::wstring& diag_string, bool priority = false)
      : ThreadBase(diag_string, priority) {}
};

}  // namespace yloader
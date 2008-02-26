/*
 *  cMutex.h
 *  Avida
 *
 *  Created by David on 3/2/07.
 *  Copyright 2007-2008 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cMutex_h
#define cMutex_h

#ifndef platform_h
#include "platform.h"
#endif


#if AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(UNIX)

// Use POSIX Threads
# include <pthread.h>

#ifdef DEBUG
# define PTHREAD_MUTEX_CHKRTN(OP) \
  { \
    int ret = OP; \
    ASSERT_MSG(ret == 0, "OP failed"); \
  }
#else
# define PTHREAD_MUTEX_CHKRTN(OP) OP
#endif

class cMutex
{
  friend class cConditionVariable;
private:
  pthread_mutex_t m_mutex;
#ifdef DEBUG
  bool m_locked;
#endif
  
  cMutex(const cMutex&); // @not_implemented
  cMutex& operator=(const cMutex&); // @not_implemented
  

public:
#ifdef DEBUG
  cMutex() : m_locked(false) { PTHREAD_MUTEX_CHKRTN(pthread_mutex_init(&m_mutex, NULL)); }
#else
  inline cMutex() { PTHREAD_MUTEX_CHKRTN(pthread_mutex_init(&m_mutex, NULL)); }
#endif
  
  inline ~cMutex()
  {
    ASSERT_MSG(!m_locked, "destroying locked mutex");
    PTHREAD_MUTEX_CHKRTN(pthread_mutex_destroy(&m_mutex));
  }

#ifdef DEBUG
  void Lock()
  {
    PTHREAD_MUTEX_CHKRTN(pthread_mutex_lock(&m_mutex));
    m_locked = true;
  }
#else
  inline void Lock() { PTHREAD_MUTEX_CHKRTN(pthread_mutex_lock(&m_mutex)); }
#endif

#ifdef DEBUG
  void Unlock()
  {
    ASSERT_MSG(m_locked, "mutex not locked");
    m_locked = false;
    PTHREAD_MUTEX_CHKRTN(pthread_mutex_unlock(&m_mutex));
  }
#else
  inline void Unlock() { PTHREAD_MUTEX_CHKRTN(pthread_mutex_unlock(&m_mutex)); }
#endif
};

#elif AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(WINDOWS)

// Use Windows Threading
# include <windows.h>

class cMutex
{
  friend class cConditionVariable;
private:
  CRITICAL_SECTION m_mutex;
#ifdef DEBUG
  bool m_locked;
#endif
  
  cMutex(const cMutex&); // @not_implemented
  cMutex& operator=(const cMutex&); // @not_implemented
  
  
public:
#ifdef DEBUG
  cMutex() : m_locked(false) { InitializeCriticalSection(&m_mutex); }
#else
  inline cMutex() { InitializeCriticalSection(&m_mutex); }
#endif
  
  inline ~cMutex()
  {
    ASSERT_MSG(!m_locked, "destroying locked mutex");
    DeleteCriticalSection(&m_mutex);
  }
  
#ifdef DEBUG
  void Lock()
  {
    EnterCriticalSection(&m_mutex);
    m_locked = true;
  }
#else
  inline void Lock() { EnterCriticalSection(&m_mutex); }
#endif
  
#ifdef DEBUG
  void Unlock()
  {
    ASSERT_MSG(m_locked, "mutex not locked");
    m_locked = false;
    LeaveCriticalSection(&m_mutex);
  }
#else
  inline void Unlock() { LeaveCriticalSection(&m_mutex); }
#endif
};


#else

// Disable Threading
class cMutex
{
private:
#ifdef DEBUG
  bool m_locked;
#endif
  
  cMutex(const cMutex&); // @not_implemented
  cMutex& operator=(const cMutex&); // @not_implemented
  
  
public:
#ifdef DEBUG
  cMutex() : m_locked(false) { ; }
#else
  inline cMutex() { ; }
#endif
  
  inline ~cMutex() { ASSERT_MSG(!m_locked, "destroying locked mutex"); }
  
#ifdef DEBUG
  void Lock() { m_locked = true; }
#else
  inline void Lock() { ; }
#endif
  
#ifdef DEBUG
  void Unlock()
  {
    ASSERT_MSG(m_locked, "mutex not locked");
    m_locked = false;
  }
#else
  inline void Unlock() { ; }
#endif  
};

#endif


class cMutexAutoLock
{
private:
  cMutex& m_mutex;
  
  cMutexAutoLock(); // @not_implemented
  cMutexAutoLock(const cMutexAutoLock&); // @not_implemented
  cMutexAutoLock& operator=(const cMutexAutoLock&); // @not_implemented
  
public:
  inline explicit cMutexAutoLock(cMutex& mutex) : m_mutex(mutex) { m_mutex.Lock(); }
  inline ~cMutexAutoLock() { m_mutex.Unlock(); }
};
  
#endif

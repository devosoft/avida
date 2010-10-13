/*
 *  cConditionVariable.h
 *  Avida
 *
 *  Created by David on 3/3/07.
 *  Copyright 2007-2010 Michigan State University. All rights reserved.
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

#ifndef cConditionVariable_h
#define cConditionVariable_h

#ifndef cMutex_h
#include "cMutex.h"
#endif
#ifndef platform_h
#include "Platform.h"
#endif


#if AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(UNIX)

// Use POSIX Threads
# include <pthread.h>

class cConditionVariable
{
private:
  pthread_cond_t m_cond;
  
  cConditionVariable(const cConditionVariable&); // @not_implemented
  cConditionVariable& operator=(const cConditionVariable&); // @not_implemented
  
  
public:
  inline cConditionVariable() { pthread_cond_init(&m_cond, NULL); }
  inline ~cConditionVariable() { pthread_cond_destroy(&m_cond); }

#ifdef DEBUG
  void Wait(cMutex& mutex)
  {
    mutex.m_locked = false;
    pthread_cond_wait(&m_cond, &(mutex.m_mutex));
    mutex.m_locked = true;
  }
#else
  inline void Wait(cMutex& mutex) { pthread_cond_wait(&m_cond, &(mutex.m_mutex)); }
#endif
  
  inline void Signal() { pthread_cond_signal(&m_cond); }
  inline void Broadcast() { pthread_cond_broadcast(&m_cond); }
};

#elif AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(WINDOWS)

// Use Windows Threading
# include <windows.h>
class cConditionVariable
{
private:
  CONDITION_VARIABLE m_cond;
  
  cConditionVariable(const cConditionVariable&); // @not_implemented
  cConditionVariable& operator=(const cConditionVariable&); // @not_implemented
  
  
public:
  inline cConditionVariable() { InitializeConditionVariable(&m_cond); }
  inline ~cConditionVariable() { ; }

#ifdef DEBUG
  void Wait(cMutex& mutex)
  {
    mutex.m_locked = false;
    SleepConditionVariableCS(&m_cond, &(mutex.m_mutex));
    mutex.m_locked = true;
  }
#else
  inline void Wait(cMutex& mutex) { SleepConditionVariableCS(&m_cond, &(mutex.m_mutex)); }
#endif
  
  inline void Signal() { WakeConditionVariable(&m_cond); }
  inline void Broadcast() { WakeAllConditionVariable(&m_cond); }
};


#else

// Disable Threading
class cConditionVariable
{
private:
  cConditionVariable(const cConditionVariable&); // @not_implemented
  cConditionVariable& operator=(const cConditionVariable&); // @not_implemented
  
public:
  inline cConditionVariable() { ; }
  inline ~cConditionVariable() { ; }
  
  inline void Wait(cMutex& mutex) { ; }
  
  inline void Signal() { ; }
  inline void Broadcast() { ; }
};

#endif

#endif

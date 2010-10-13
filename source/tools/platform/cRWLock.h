/*
 *  cRWLock.h
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

#ifndef cRWLock_h
#define cRWLock_h

#ifndef platform_h
#include "Platform.h"
#endif


#if AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(UNIX)

// Use POSIX Threads
# include <pthread.h>

class cRWLock
{
private:
  pthread_rwlock_t m_rwlock;
  
  cRWLock(const cRWLock&); // @not_implemented
  cRWLock& operator=(const cRWLock&); // @not_implemented
  
public:
  inline cRWLock() { pthread_rwlock_init(&m_rwlock, NULL); }
  inline ~cRWLock() { pthread_rwlock_destroy(&m_rwlock); }

  inline void ReadLock() { pthread_rwlock_rdlock(&m_rwlock); }
  inline void ReadUnlock() { pthread_rwlock_unlock(&m_rwlock); }  

  inline void WriteLock() { pthread_rwlock_wrlock(&m_rwlock); }
  inline void WriteUnlock() { pthread_rwlock_unlock(&m_rwlock); }  
};

#elif AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(WINDOWS)

// Use Windows Threading
# include <windows.h>

class cRWLock
{
private:
  SRWLOCK m_rwlock;
  
  cRWLock(const cRWLock&); // @not_implemented
  cRWLock& operator=(const cRWLock&); // @not_implemented
  
public:
  inline cRWLock() { InitializeSRWLock(&m_rwlock); }
  inline ~cRWLock() { ; }
  
  inline void ReadLock() { AcquireSRWLockShared(&m_rwlock); }
  inline void ReadUnlock() { ReleaseSRWLockShared(&m_rwlock); }  

  inline void WriteLock() { AcquireSRWLockExclusive(&m_rwlock); }
  inline void WriteUnlock() { ReleaseSRWLockExclusive(&m_rwlock); }  
};



#else

class cRWLock
{
private:
  cRWLock(const cRWLock&); // @not_implemented
  cRWLock& operator=(const cRWLock&); // @not_implemented
  
public:
  inline cRWLock() { ; }
  inline ~cRWLock() { ; }
  
  inline void ReadLock() { ; }
  inline void ReadUnlock() { ; }  
  
  inline void WriteLock() { ; }
  inline void WriteUnlock() { ; }  
};


#endif

#endif

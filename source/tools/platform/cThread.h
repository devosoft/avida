/*
 *  cThread.h
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 2006-2007 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cThread_h
#define cThread_h

#ifndef cMutex_h
#include "cMutex.h"
#endif


#if AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(UNIX)

// Use POSIX Threads
#include <pthread.h>

class cThread
{
protected:
  pthread_t m_thread;
  cMutex m_mutex;
  bool m_running;

  void Stop();
  
  virtual void Run() = 0;
  
  static void* EntryPoint(void* arg);

  cThread(const cThread&); // @not_implemented
  cThread& operator=(const cThread&); // @not_implemented

public:
  cThread() : m_running(false) { ; }
  virtual ~cThread();
  
  bool Start();
  void Join();
};

#elif AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(WINDOWS)

// Use Windows Threading
#include <windows.h>

class cThread
{
protected:
  HANDLE m_thread;
  cMutex m_mutex;
  
  void Stop();

  virtual void Run() = 0;
  
  static unsigned int __stdcall EntryPoint(void* arg);
  
  cThread(const cThread&); // @not_implemented
  cThread& operator=(const cThread&); // @not_implemented
  
public:
  cThread() : m_thread(0) { ; }
  virtual ~cThread();
  
  bool Start();
  void Join();
};

#else

// Disable Threading
class cThread
{
protected:
  virtual void Run() = 0;
  
  cThread(const cThread&); // @not_implemented
  cThread& operator=(const cThread&); // @not_implemented
  
public:
  inline cThread() { ; }
  virtual ~cThread() { ; }
  
  inline bool Start() { Run(); return true; }
  inline void Join() { ; }
};

#endif


#endif

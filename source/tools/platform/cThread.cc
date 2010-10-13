/*
 *  cThread.cc
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#include "cThread.h"

#if AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(UNIX)

cThread::~cThread() { if (m_running) pthread_detach(m_thread); }

bool cThread::Start()
{
  m_mutex.Lock();
  if (!m_running) {
    m_running = true;
    m_mutex.Unlock();

    // Create thread, mark as running when successful
    int rval = pthread_create(&m_thread, NULL, &cThread::EntryPoint, this);
    if (rval) m_running = false;
  } else {
    m_mutex.Unlock();
  }
  
  return m_running;
}

void cThread::Stop()
{
  if (m_running) {
    // Close and clean up thread, set running to false
    pthread_cancel(m_thread);
    pthread_join(m_thread, NULL);
    m_running = false;
  }
}

void cThread::Join()
{
  if (m_running) {
    pthread_join(m_thread, NULL);
    m_running = false;
  }
}

void* cThread::EntryPoint(void* arg)
{
  // Common entry point to start cThread objects
  // Calls Run method of appropriate subclass to do the actual work
  cThread* thread = static_cast<cThread*>(arg);
  thread->Run();
  
  pthread_exit(NULL);
  return NULL;
}


#elif AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(WINDOWS)

cThread::~cThread() { if (m_thread) CloseHandle(m_thread); }

bool cThread::Start()
{
  cMutexAutoLock lock(m_mutex);

  if (m_thread == 0) {
    unsigned long tid = 0;
    m_thread = (HANDLE)_beginthreadex(0, 0, &cThread::EntryPoint, this, 0, &tid);

    if (m_thread) return true;
    return false;
  }
  
  return true;
}

void cThread::Stop()
{
  if (m_thread) {
    if (CloseHandle(m_thread) == TRUE) {
      m_thread = 0;
    }
  }
  
}

void cThread::Join()
{
  if (m_thread) WaitForSingleObject(m_thread, INFINITE);
}

unsigned int __stdcall cThread::EntryPoint(void* arg)
{
  // Common entry point to start cThread objects
  // Calls Run method of appropriate subclass to do the actual work
  cThread* thread = static_cast<cThread*>(arg);
  thread->Run();

  _endthreadex(0);
  return 0;
}


#endif

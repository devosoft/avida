/*
 *  cThread.h
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef cThread_h
#define cThread_h

#ifndef NULL
#define NULL 0
#endif

#include <pthread.h>

class cThread
{
protected:
  pthread_t m_thread;
  pthread_mutex_t m_mutex;
  bool m_running;

  virtual void Run() = 0;
  
  static void* EntryPoint(void* arg);

  cThread(const cThread&); // @not_implemented
  cThread& operator=(const cThread&); // @not_implemented

public:
  cThread() : m_running(false) { pthread_mutex_init(&m_mutex, NULL); }
  virtual ~cThread();
  
  int Start();
  void Stop();
  void Join();
};


#ifdef ENABLE_UNIT_TESTS
namespace nThread {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

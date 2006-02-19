/*
 *  cThread.h
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cThread_h
#define cThread_h

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
  cThread() : m_running(false) { ; }
  virtual ~cThread();
  
  int Start();
  void Stop();
  void Join();
};

#endif

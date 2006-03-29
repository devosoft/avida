/*
 *  cThread.cc
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "cThread.h"

cThread::~cThread()
{
#ifndef WIN32_PTHREAD_HACK
  if (m_running) pthread_detach(m_thread);
#endif
}


int cThread::Start()
{
  int rval = 0;
  
  pthread_mutex_lock(&m_mutex);
  if (!m_running) {
    m_running = true;
    pthread_mutex_unlock(&m_mutex);

#ifndef WIN32_PTHREAD_HACK
    // Create thread, mark as running when successful
    rval = pthread_create(&m_thread, NULL, &cThread::EntryPoint, this);
    if (rval) m_running = false;
#else
    Run();
#endif
  } else {
    pthread_mutex_unlock(&m_mutex);
  }
  
  return rval;
}

void cThread::Stop()
{
#ifndef WIN32_PTHREAD_HACK
  if (m_running) {
    // Close and clean up thread, set running to false
    pthread_cancel(m_thread);
    pthread_join(m_thread, NULL);
    m_running = false;
  }
#endif
}

void cThread::Join()
{
#ifndef WIN32_PTHREAD_HACK
  if (m_running) {
    pthread_join(m_thread, NULL);
    m_running = false;
  }
#endif
}

void* cThread::EntryPoint(void* arg)
{
  // Common entry point to start cThread objects
  // Calls Run method of appropriate subclass to do the actual work
  cThread* thread = static_cast<cThread*>(arg);
  thread->Run();
  
  return NULL;
}

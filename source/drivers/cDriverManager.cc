/*
 *  cDriverManager.cpp
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#include "cDriverManager.h"

#include "cActionLibrary.h"
#include "cAvidaDriver.h"
#include "cWorldDriver.h"

#include <assert.h>
#include <stdlib.h>


cDriverManager* cDriverManager::m_dm = NULL;

cDriverManager::cDriverManager()
{
  pthread_mutex_init(&m_mutex, NULL);
  m_actlib = cActionLibrary::ConstructDefaultActionLibrary();
}

cDriverManager::~cDriverManager()
{
  cAvidaDriver* adrv;
  while (adrv = m_adrvs.Pop()) {
    delete adrv;
  }
  
  cWorldDriver* wdrv;
  while (wdrv = m_wdrvs.Pop()) {
    delete wdrv;
  }
  
  delete m_actlib;
  
  pthread_mutex_destroy(&m_mutex);
}

void cDriverManager::Initialize()
{
  if (m_dm == NULL) {
    m_dm = new cDriverManager();
    if (atexit(cDriverManager::Destroy)) {
      // Failed to register with atexit, this is bad, very bad.
      exit(-1);
    }
  }
}

void cDriverManager::Destroy()
{
  delete m_dm;
}

void cDriverManager::Register(cAvidaDriver* drv)
{
  assert(m_dm);
  pthread_mutex_lock(&m_dm->m_mutex);
  m_dm->m_adrvs.Push(drv);
  pthread_mutex_unlock(&m_dm->m_mutex);
}

void cDriverManager::Register(cWorldDriver* drv)
{
  assert(m_dm);
  pthread_mutex_lock(&m_dm->m_mutex);
  m_dm->m_wdrvs.Push(drv);
  pthread_mutex_unlock(&m_dm->m_mutex);
}

void cDriverManager::Unregister(cAvidaDriver* drv)
{
  assert(m_dm);
  pthread_mutex_lock(&m_dm->m_mutex);
  m_dm->m_adrvs.Remove(drv);
  pthread_mutex_unlock(&m_dm->m_mutex);
}

void cDriverManager::Unregister(cWorldDriver* drv)
{
  assert(m_dm);
  pthread_mutex_lock(&m_dm->m_mutex);
  m_dm->m_wdrvs.Remove(drv);
  pthread_mutex_unlock(&m_dm->m_mutex);
}

cActionLibrary* cDriverManager::GetActionLibrary()
{
  assert(m_dm);
  return m_dm->m_actlib;
}

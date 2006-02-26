/*
 *  cDriverManager.cpp
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#include "cDriverManager.h"

#include "cAvidaDriver.h"
#include "cWorldDriver.h"

#include <stdlib.h>


cDriverManager* cDriverManager::m_dm = NULL;

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
}

void cDriverManager::Initialize()
{
  if (m_dm == NULL)
  {
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
  if (m_dm) m_dm->m_adrvs.Push(drv);
}

void cDriverManager::Register(cWorldDriver* drv)
{
  if (m_dm) m_dm->m_wdrvs.Push(drv);
}

void cDriverManager::Unregister(cAvidaDriver* drv)
{
  if (m_dm) m_dm->m_adrvs.Remove(drv);
}

void cDriverManager::Unregister(cWorldDriver* drv)
{
  if (m_dm) m_dm->m_wdrvs.Remove(drv);
}


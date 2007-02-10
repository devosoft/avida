/*
 *  cDriverManager.cpp
 *  Avida
 *
 *  Created by David on 12/11/05.
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
  while ((adrv = m_adrvs.Pop())) {
    delete adrv;
  }
  
  cWorldDriver* wdrv;
  while ((wdrv = m_wdrvs.Pop())) {
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

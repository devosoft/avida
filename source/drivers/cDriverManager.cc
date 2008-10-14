/*
 *  cDriverManager.cpp
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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
#include "cDriverStatusConduit.h"
#include "cWorldDriver.h"

#include <cassert>
#include <cstdlib>


cDriverManager* cDriverManager::m_dm = NULL;

cDriverManager::cDriverManager()
{
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
  m_dm->m_mutex.Lock();
  m_dm->m_adrvs.Push(drv);
  m_dm->m_mutex.Unlock();
}

void cDriverManager::Register(cWorldDriver* drv)
{
  assert(m_dm);
  m_dm->m_mutex.Lock();
  m_dm->m_wdrvs.Push(drv);
  m_dm->m_mutex.Unlock();
}

void cDriverManager::Unregister(cAvidaDriver* drv)
{
  assert(m_dm);
  m_dm->m_mutex.Lock();
  m_dm->m_adrvs.Remove(drv);
  m_dm->m_mutex.Unlock();
}

void cDriverManager::Unregister(cWorldDriver* drv)
{
  assert(m_dm);
  m_dm->m_mutex.Lock();
  m_dm->m_wdrvs.Remove(drv);
  m_dm->m_mutex.Unlock();
}

cDriverStatusConduit& cDriverManager::Status()
{
  cDriverStatusConduit* conduit = m_dm->m_conduit.Get();
  if (!conduit) {
    conduit = new cDriverStatusConduit;
    m_dm->m_conduit.Set(conduit);
  }
  return *conduit;
}

void cDriverManager::SetConduit(cDriverStatusConduit* conduit)
{
  m_dm->m_conduit.Set(conduit);
}


cActionLibrary* cDriverManager::GetActionLibrary()
{
  assert(m_dm);
  return m_dm->m_actlib;
}

/*
 *  cDriverManager.cpp
 *  Avida
 *
 *  Created by David on 12/11/05.
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

#include "cDriverManager.h"

#include "cDriverStatusConduit.h"
#include "cDMObject.h"

#include <cassert>
#include <cstdlib>


cDriverManager* cDriverManager::s_dm = NULL;

cDriverManager::~cDriverManager()
{
  cDMObject* obj;
  while ((obj = m_objs.Pop())) delete obj;
}

void cDriverManager::Initialize()
{
  if (s_dm == NULL) {
    s_dm = new cDriverManager();
    if (atexit(cDriverManager::Destroy)) {
      // Failed to register with atexit, this is bad, very bad.
      exit(-1);
    }
  }
}

void cDriverManager::Destroy()
{
  delete s_dm;
}

void cDriverManager::Register(cDMObject* obj)
{
  assert(s_dm);
  s_dm->m_mutex.Lock();
  s_dm->m_objs.Push(obj);
  s_dm->m_mutex.Unlock();
}

void cDriverManager::Unregister(cDMObject* obj)
{
  assert(s_dm);
  s_dm->m_mutex.Lock();
  s_dm->m_objs.Remove(obj);
  s_dm->m_mutex.Unlock();
}

cDriverStatusConduit& cDriverManager::Status()
{
  cDriverStatusConduit* conduit = s_dm->m_conduit.Get();
  if (!conduit) {
    conduit = new cDriverStatusConduit;
    s_dm->m_conduit.Set(conduit);
  }
  return *conduit;
}

void cDriverManager::SetConduit(cDriverStatusConduit* conduit)
{
  assert(s_dm);
  s_dm->m_conduit.Set(conduit);
}

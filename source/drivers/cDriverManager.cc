/*
 *  cDriverManager.cpp
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#include "cDriverManager.h"

#include "avida/core/cDriverManagerObject.h"

#include "cDriverStatusConduit.h"

#include <cassert>
#include <cstdlib>


cDriverManager* cDriverManager::s_dm = NULL;

cDriverManager::~cDriverManager()
{
  cDriverManagerObject* obj;
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

void cDriverManager::Register(cDriverManagerObject* obj)
{
  assert(s_dm);
  s_dm->m_mutex.Lock();
  s_dm->m_objs.Push(obj);
  s_dm->m_mutex.Unlock();
}

void cDriverManager::Unregister(cDriverManagerObject* obj)
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

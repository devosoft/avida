/*
 *  core/GlobalObject.cc
 *  avida-core
 *
 *  Created by David on 12/11/05.
 *  Copyright 2005-2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 */

#include "avida/core/GlobalObject.h"

#include "tList.h"
#include "cMutex.h"
#include "tThreadSpecific.h"

#include <cassert>
#include <cstdlib>


Avida::cGlobalObject::~cGlobalObject() { ; }


struct GlobalObjectData
{
  tList<Avida::cGlobalObject> objs;
  
  cMutex mutex;
  tThreadSpecific<cDriverStatusConduit> conduit;
  
  bool initialized;
  
  GlobalObjectData() : initialized(false) { ; } 
} global_obj_data;


static void destroyGlobalObjectData()
{
  Avida::cGlobalObject* obj;
  while ((obj = global_obj_data.objs.Pop())) delete obj;
}


void Avida::GlobalObjectManager::Initialize()
{
  if (!global_obj_data.initialized) {
    if (atexit(destroyGlobalObjectData)) {
      // Failed to register with atexit, this is bad, very bad.
      exit(-1);
    }
    global_obj_data.initialized = true;
  }
}

void Avida::GlobalObjectManager::Register(cGlobalObject* obj)
{
  global_obj_data.mutex.Lock();
  global_obj_data.objs.Push(obj);
  global_obj_data.mutex.Unlock();
}

void Avida::GlobalObjectManager::Unregister(cGlobalObject* obj)
{
  global_obj_data.mutex.Lock();
  global_obj_data.objs.Remove(obj);
  global_obj_data.mutex.Unlock();
}

cDriverStatusConduit& Avida::GlobalObjectManager::Status()
{
  cDriverStatusConduit* conduit = global_obj_data.conduit.Get();
  if (!conduit) {
    conduit = new cDriverStatusConduit;
    global_obj_data.conduit.Set(conduit);
  }
  return *conduit;
}

void Avida::GlobalObjectManager::SetConduit(cDriverStatusConduit* conduit)
{
  global_obj_data.conduit.Set(conduit);
}

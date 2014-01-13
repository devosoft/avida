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
 *
 */

#include "avida/core/GlobalObject.h"

#include "apto/core/List.h"
#include "apto/core/Mutex.h"

#include <cassert>
#include <cstdlib>


Avida::GlobalObject::~GlobalObject() { ; }


struct GlobalObjectData
{
  Apto::List<Avida::GlobalObject*> objs;
  
  Apto::Mutex mutex;
  bool initialized;
  
  GlobalObjectData() : initialized(false) { ; } 
} global_obj_data;


static void destroyGlobalObjectData()
{
  Avida::GlobalObject* obj;
  while (global_obj_data.objs.GetSize() && (obj = global_obj_data.objs.Pop())) delete obj;
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

void Avida::GlobalObjectManager::Register(GlobalObject* obj)
{
  global_obj_data.mutex.Lock();
  global_obj_data.objs.Push(obj);
  global_obj_data.mutex.Unlock();
}

void Avida::GlobalObjectManager::Unregister(GlobalObject* obj)
{
  global_obj_data.mutex.Lock();
  global_obj_data.objs.Remove(obj);
  global_obj_data.mutex.Unlock();
}

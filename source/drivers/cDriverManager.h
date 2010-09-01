/*
 *  cDriverManager.h
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

#ifndef cDriverManager_h
#define cDriverManager_h

#include "tList.h"
#include "cMutex.h"
#include "cDriverStatusConduit.h"
#include "tThreadSpecific.h"

class cDMObject;


class cDriverManager
{
private:
  static cDriverManager* s_dm;
  
  tList<cDMObject> m_objs;
  
  cMutex m_mutex;
  tThreadSpecific<cDriverStatusConduit> m_conduit;
  
  cDriverManager() { ; }
  ~cDriverManager();

  cDriverManager(const cDriverManager&); // @not_implemented
  cDriverManager& operator=(const cDriverManager&); // @not_implemented

  static void Destroy();    // destory the driver manager, and all registered drivers.  Registered with atexit(). 

public:
  static void Initialize(); // initialize static driver manager.  This method is NOT thread-safe.

  static void Register(cDMObject* obj);
  static void Unregister(cDMObject* obj);
  
  static cDriverStatusConduit& Status();
  static void SetConduit(cDriverStatusConduit* conduit);
};

#endif

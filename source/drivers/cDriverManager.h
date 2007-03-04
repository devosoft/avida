/*
 *  cDriverManager.h
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

#ifndef cDriverManager_h
#define cDriverManager_h

#ifndef tList_h
#include "tList.h"
#endif
#ifndef cMutex_h
#include "cMutex.h"
#endif

class cActionLibrary;
class cAvidaDriver;
class cWorldDriver;


class cDriverManager
{
private:
  static cDriverManager* m_dm;
  
  tList<cAvidaDriver> m_adrvs;
  tList<cWorldDriver> m_wdrvs;
  
  cMutex m_mutex;
  cActionLibrary* m_actlib;
  
  cDriverManager();
  ~cDriverManager();

  cDriverManager(const cDriverManager&); // @not_implemented
  cDriverManager& operator=(const cDriverManager&); // @not_implemented

  static void Destroy();    // destory the driver manager, and all registered drivers.  Registered with atexit(). 

public:
  static void Initialize(); // initialize static driver manager.  This method is NOT thread-safe.

  static void Register(cAvidaDriver* drv);
  static void Register(cWorldDriver* drv);

  static void Unregister(cAvidaDriver* drv);
  static void Unregister(cWorldDriver* drv);
  
  static cActionLibrary* GetActionLibrary();
};


#ifdef ENABLE_UNIT_TESTS
namespace nDriverManager {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

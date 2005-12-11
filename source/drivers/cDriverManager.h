/*
 *  cDriverManager.h
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef cDriverManager_h
#define cDriverManager_h

#ifndef tList_h
#include "tList.h"
#endif

class cAvidaDriver;
class cWorldDriver;

class cDriverManager
{
private:
  static cDriverManager* m_dm;
  
  tList<cAvidaDriver> m_adrvs;
  tList<cWorldDriver> m_wdrvs;
  
  cDriverManager() { ; }
  ~cDriverManager();

  // not implemented
  cDriverManager(const cDriverManager&);

public:
  static void Initialize(); // initialize static driver manager.  This method is NOT thread-safe.
  static void Destroy();    // destory the driver manager, and all registered drivers.  Registered with atexit(). 

  static void Register(cAvidaDriver* drv) { if (m_dm) m_dm->m_adrvs.Push(drv); }
  static void Register(cWorldDriver* drv) { if (m_dm) m_dm->m_wdrvs.Push(drv); }

  static void Unregister(cAvidaDriver* drv) { if (m_dm) m_dm->m_adrvs.Remove(drv); }
  static void Unregister(cWorldDriver* drv) { if (m_dm) m_dm->m_wdrvs.Remove(drv); }
};

#endif

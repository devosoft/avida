/*
 *  cDriverManager.h
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
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

  cDriverManager(const cDriverManager&); // @not_implemented
  cDriverManager& operator=(const cDriverManager&); // @not_implemented

public:
  static void Initialize(); // initialize static driver manager.  This method is NOT thread-safe.
  static void Destroy();    // destory the driver manager, and all registered drivers.  Registered with atexit(). 

  static void Register(cAvidaDriver* drv);
  static void Register(cWorldDriver* drv);

  static void Unregister(cAvidaDriver* drv);
  static void Unregister(cWorldDriver* drv);
};

#endif

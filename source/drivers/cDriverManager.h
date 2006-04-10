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

#include <pthread.h>

class cAvidaDriver;
class cWorldDriver;


class cDriverManager
{
private:
  static cDriverManager* m_dm;
  
  tList<cAvidaDriver> m_adrvs;
  tList<cWorldDriver> m_wdrvs;
  
  pthread_mutex_t m_mutex;
  
  cDriverManager() { pthread_mutex_init(&m_mutex, NULL); }
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
};

#endif

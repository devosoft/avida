/*
 *  cEventManager.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cEventManager_h
#define cEventManager_h

#ifndef tList_h
#include "tList.h"
#endif
#ifndef tObjectFactory_h
#include "tObjectFactory.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cEvent;
class cWorld;

class cEventManager : private tObjectFactory<cEvent* ()> {
private:
  cWorld* m_world;
  
public:
  cEventManager(cWorld* world);
  ~cEventManager() { ; }

  cEvent* ConstructEvent(const cString name, const cString& args);
  
  void PrintAllEventDescriptions();
};

#endif

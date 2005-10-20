//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cEventManager_h
#define cEventManager_h

#ifndef TLIST_HH
#include "tList.h"
#endif
#ifndef TOBJECTFACTORY_H
#include "tObjectFactory.h"
#endif
#ifndef STRING_HH
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

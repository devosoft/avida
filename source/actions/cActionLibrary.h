/*
 *  cActionLibrary.h
 *  Avida
 *
 *  Created by David on 4/8/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cActionLibrary_h
#define cActionLibrary_h

#ifndef cAction_h
#include "cAction.h"
#endif
#ifndef tObjectFactory_h
#include "tObjectFactory.h"
#endif

class cWorld;
class cString;

class cActionLibrary : public tObjectFactory<cAction* (cWorld*, const cString&)>
{
public:
  cActionLibrary() { ; }
};

#ifdef ENABLE_UNIT_TESTS
namespace nActionLibrary {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

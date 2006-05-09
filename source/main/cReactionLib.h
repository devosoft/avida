/*
 *  cReactionLib.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 */

#ifndef cReactionLib_h
#define cReactionLib_h

#ifndef tArray_h
#include "tArray.h"
#endif

class cReaction;
class cString;

class cReactionLib
{
private:
  tArray<cReaction*> reaction_array;

  cReactionLib(const cReactionLib&); // @not_implemented
  cReactionLib& operator=(const cReactionLib&); // @not_implemented

public:
  cReactionLib() { ; }
  ~cReactionLib();

  int GetSize() const { return reaction_array.GetSize(); }
  
  cReaction* AddReaction(const cString& name);
  cReaction* GetReaction(const cString& name) const;
  cReaction* GetReaction(int id) const;
};


#ifdef ENABLE_UNIT_TESTS
namespace nReactionLib {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif

#endif

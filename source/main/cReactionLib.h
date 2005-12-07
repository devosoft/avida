/*
 *  cReactionLib.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
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

class cReactionLib {
private:
  tArray<cReaction *> reaction_array;
public:
  cReactionLib() { ; }
  ~cReactionLib();

  int GetSize() const { return reaction_array.GetSize(); }
  
  cReaction * AddReaction(const cString & name);
  cReaction * GetReaction(const cString & name) const;
  cReaction * GetReaction(int id) const;
};

#endif

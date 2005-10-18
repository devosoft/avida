//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef REACTION_LIB_HH
#define REACTION_LIB_HH

#ifndef TARRAY_HH
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

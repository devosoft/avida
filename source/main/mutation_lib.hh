//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MUTATION_LIB_HH
#define MUTATION_LIB_HH

#ifndef TARRAY_HH
#include "tArray.hh"
#endif
#ifndef TLIST_HH
#include "tList.hh"
#endif

class cMutation;
class cString;
class cMutationLib {
private:
  tArray<cMutation *> mutation_array;
  tArray< tList<cMutation> > trigger_list_array;
public:
  cMutationLib();
  ~cMutationLib();

  int GetSize() const { return mutation_array.GetSize(); }

  cMutation * AddMutation(const cString & name, int trigger, int scope,
			  int type, double rate);

  const tArray<cMutation *> & GetMutationArray() const
    { return mutation_array; }
  const tList<cMutation> & GetMutationList(int trigger) const
    { return trigger_list_array[trigger]; }
};

#endif

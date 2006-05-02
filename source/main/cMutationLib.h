/*
 *  cMutationLib.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cMutationLib_h
#define cMutationLib_h

#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

class cMutation;
class cString;

class cMutationLib
{
private:
  tArray<cMutation*> mutation_array;
  tArray< tList<cMutation> > trigger_list_array;

  
  cMutationLib(const cMutationLib&); // @not_implemented
  cMutationLib& operator=(const cMutationLib&); // @not_implemented
  
public:
  cMutationLib();
  ~cMutationLib();

  int GetSize() const { return mutation_array.GetSize(); }

  cMutation* AddMutation(const cString & name, int trigger, int scope, int type, double rate);

  const tArray<cMutation*>& GetMutationArray() const { return mutation_array; }
  const tList<cMutation>& GetMutationList(int trigger) const { return trigger_list_array[trigger]; }


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif

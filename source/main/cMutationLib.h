/*
 *  cMutationLib.h
 *  Avida
 *
 *  Called "mutation_lib.hh" prior to 12/5/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
};


#ifdef ENABLE_UNIT_TESTS
namespace nMutationLib {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

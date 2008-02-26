/*
 *  cLocalMutations.h
 *  Avida
 *
 *  Called "local_mutations.hh" prior to 12/5/05.
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

#ifndef cLocalMutations_h
#define cLocalMutations_h

#ifndef tArray_h
#include "tArray.h"
#endif

class cMutationLib;
class cLocalMutations
{
private:
  const cMutationLib& mut_lib;
  tArray<double> rates;
  tArray<int> counts;
  
  
  cLocalMutations(); // @not_implemented
  cLocalMutations(const cLocalMutations&); // @not_implemented
  cLocalMutations& operator=(const cLocalMutations&); // @not_implemented
  
public:
  cLocalMutations(const cMutationLib& _lib, int genome_length);
  ~cLocalMutations() { ; }

  const cMutationLib& GetMutationLib() const { return mut_lib; }
  double GetRate(int id) const { return rates[id]; }
  int GetCount(int id) const { return counts[id]; }

  void IncCount(int id) { counts[id]++; }
  void IncCount(int id, int num_mut) { counts[id] += num_mut; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nLocalMutations {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

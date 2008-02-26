/*
 *  cInjectGenotypeQueue.h
 *  Avida
 *
 *  Called "inject_genotype_queue.hh" prior to 11/30/05.
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

#ifndef cInjectGenotypeQueue_h
#define cInjectGenotypeQueue_h

#ifndef cInjectGenotypeElement_h
#include "cInjectGenotypeElement.h"
#endif

class cInjectGenotype;
class cGenome;

class cInjectGenotypeQueue {
private:
  int size;
  cInjectGenotypeElement root;

  void Remove(cInjectGenotypeElement* in_element);
  
  cInjectGenotypeQueue(const cInjectGenotypeQueue&); // @not_implemented
  cInjectGenotypeQueue& operator=(const cInjectGenotypeQueue&); // @not_implemented
  
public:
  cInjectGenotypeQueue();
  ~cInjectGenotypeQueue();

  bool OK();

  void Insert(cInjectGenotype & in_inject_genotype);
  void Remove(cInjectGenotype & in_inject_genotype);
  cInjectGenotype * Find(const cGenome & in_genome) const;
};


#ifdef ENABLE_UNIT_TESTS
namespace nInjectGenotypeQueue {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif

#endif

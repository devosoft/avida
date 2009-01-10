/*
 *  cInjectGenotypeControl.h
 *  Avida
 *
 *  Called "inject_genotype_control.hh" prior to 11/15/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#ifndef cInjectGenotypeControl_h
#define cInjectGenotypeControl_h

#ifndef nInjectGenotype_h
#include "nInjectGenotype.h"
#endif

class cGenome;
class cInjectGenotype;
class cWorld;

class cInjectGenotypeControl {
private:
  cWorld* m_world;
  int size;
  cInjectGenotype* best;
  cInjectGenotype* coalescent;
  cInjectGenotype* threads[nInjectGenotype::THREADS];

  cInjectGenotype* historic_list;
  int historic_count;

  void Insert(cInjectGenotype& in_inject_genotype, cInjectGenotype* prev_inject_genotype);
  bool CheckPos(cInjectGenotype& in_inject_genotype);
  
  cInjectGenotypeControl(); // @not_implemented
  cInjectGenotypeControl(const cInjectGenotypeControl&); // @not_implemented
  cInjectGenotypeControl& operator=(const cInjectGenotypeControl&); // @not_implemented

public:
  cInjectGenotypeControl(cWorld* world);
  ~cInjectGenotypeControl();

  bool OK();
  void Remove(cInjectGenotype & in_inject_genotype);
  void Insert(cInjectGenotype & new_inject_genotype);
  bool Adjust(cInjectGenotype & in_inject_genotype);

  void RemoveHistoric(cInjectGenotype & in_inject_genotype);
  void InsertHistoric(cInjectGenotype & in_inject_genotype);
  int GetHistoricCount() { return historic_count; }

  inline int GetSize() const { return size; }
  inline cInjectGenotype * GetBest() const { return best; }
  inline cInjectGenotype * GetCoalescent() const { return coalescent; }

  cInjectGenotype * Find(const cGenome & in_genome) const;
  int FindPos(cInjectGenotype & in_inject_genotype, int max_depth = -1);

  inline cInjectGenotype * Get(int thread) const { return threads[thread]; }
  inline cInjectGenotype * Reset(int thread)
    { return threads[thread] = best; }
  inline cInjectGenotype * ResetHistoric(int thread)
    { return threads[thread] = historic_list; }
  cInjectGenotype * Next(int thread);
  cInjectGenotype * Prev(int thread);
};


#ifdef ENABLE_UNIT_TESTS
namespace nInjectGenotypeControl {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif

#endif

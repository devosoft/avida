/*
 *  cGenotypeControl.h
 *  Avida
 *
 *  Called "genotype_control.hh" prior to 11/30/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#ifndef cGenotypeControl_h
#define cGenotypeControl_h

#ifndef nGenotype_h
#include "nGenotype.h"
#endif

class cGenome;
class cGenotype;
class cWorld;

class cGenotypeControl {
private:
  cWorld* m_world;
  int size;
  cGenotype* best;
  cGenotype* coalescent;
  cGenotype* threads[nGenotype::THREADS];

  cGenotype * historic_list;
  int historic_count;

  void Insert(cGenotype & in_genotype, cGenotype * prev_genotype);
  bool CheckPos(cGenotype & in_genotype);
  
  cGenotypeControl(); // @not_implemented
  cGenotypeControl(const cGenotypeControl&); // @not_implemented
  cGenotypeControl& operator=(const cGenotypeControl&); // @not_implemented
  
public:
  cGenotypeControl(cWorld* world);
  ~cGenotypeControl();

  bool OK();
  void Remove(cGenotype & in_genotype);
  void Insert(cGenotype & new_genotype);
  bool Adjust(cGenotype & in_genotype);

  void RemoveHistoric(cGenotype & in_genotype);
  void InsertHistoric(cGenotype & in_genotype);
  int GetHistoricCount() { return historic_count; }

  int UpdateCoalescent();

  inline int GetSize() const { return size; }
  inline cGenotype * GetBest() const { return best; }
  inline cGenotype * GetCoalescent() const { return coalescent; }

  cGenotype * Find(const cGenome & in_genome) const;
  int FindPos(cGenotype & in_genotype, int max_depth = -1);

  inline cGenotype * Get(int thread) const { return threads[thread]; }
  inline cGenotype * Reset(int thread)
    { return threads[thread] = best; }
  inline cGenotype * ResetHistoric(int thread)
    { return threads[thread] = historic_list; }
  cGenotype * Next(int thread);
  cGenotype * Prev(int thread);
};


#ifdef ENABLE_UNIT_TESTS
namespace nGenotypeControl {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

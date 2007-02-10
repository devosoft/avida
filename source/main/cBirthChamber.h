/*
 *  cBirthChamber.h
 *  Avida
 *
 *  Called "birth_chamber.hh" prior to 12/2/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef cBirthChamber_h
#define cBirthChamber_h

#ifndef cCPUMemory_h
#include "cCPUMemory.h"
#endif
#ifndef cMerit_h
#include "cMerit.h"
#endif

/**
 * All genome-based organisms must go through the birth chamber, which will
 * handle any special modifications during the birth process, such as divide
 * mutations reversions, sterilization, and crossover for sex.  If sex is
 * turned on, this class will hold organisms that are waiting to be crossed
 * over before they are born.
 **/

class cAvidaContext;
class cGenome;
class cGenotype;
class cOrganism;
template <class T> class tArray;
class cWorld;

class cBirthChamber {
private:

  class cBirthEntry {
  public:
    cCPUMemory genome;
    cMerit merit;
    cGenotype * parent_genotype;
    int update_in;  // Update entry was created; Set to -1 if entry is empty.

    cBirthEntry() : parent_genotype(NULL), update_in(-1) { ; }
  };
  
  cWorld* m_world;

  cBirthEntry global_wait_entry;
  tArray<cBirthEntry> local_wait_entry;
  tArray<cBirthEntry> deme_wait_entry;
  tArray<cBirthEntry> size_wait_entry;
  tArray<cBirthEntry> mate_select_wait_entry;

  // mark whether that instruction has already been swapped 
  // between two genomes; used in modular recombination
  tArray<int> swapped0;
  tArray<int> swapped1;


  // Private methods...
  bool EvaluateEntry(const cBirthEntry & entry) const;

  bool RegionSwap(cCPUMemory& genome0, cCPUMemory& genome1, int start0, int end0, int start1, int end1);
  void GenomeSwap(cCPUMemory& genome0, cCPUMemory& genome1, double& merit0, double& merit1);

  bool DoAsexBirth(cAvidaContext& ctx, const cGenome& child_genome, cOrganism& parent,
                   tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array);
  bool DoPairAsexBirth(cAvidaContext& ctx, const cBirthEntry& old_entry, const cGenome& new_genome, cOrganism& parent,
                       tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array);
  cBirthEntry* FindSexLocalWaiting(cAvidaContext& ctx, const cGenome& child_genome, cOrganism& parent);
  cBirthEntry* FindSexDemeWaiting(const cGenome& child_genome, cOrganism& parent);
  cBirthEntry* FindSexSizeWaiting(const cGenome & child_genome, cOrganism& parent);
  cBirthEntry* FindSexMateSelectWaiting(const cGenome & child_genome, cOrganism& parent);
  cBirthEntry* FindSexGlobalWaiting(const cGenome & child_genome, cOrganism& parent);

  void DoBasicRecombination(cAvidaContext& ctx, cCPUMemory& genome0, cCPUMemory& genome1,
                            double& merit0, double& merit1);
  void DoModularContRecombination(cAvidaContext& ctx, cCPUMemory& genome0, cCPUMemory& genome1,
                                  double& merit0, double& merit1);
  void DoModularNonContRecombination(cAvidaContext& ctx, cCPUMemory& genome0, cCPUMemory& genome1,
                                     double& merit0, double& merit1);
  void DoModularShuffleRecombination(cAvidaContext& ctx, cCPUMemory& genome0, cCPUMemory& genome1,
                                     double& merit0, double& merit1);

  void SetupGenotypeInfo(cOrganism* organism, cGenotype* parent0_genotype, cGenotype* parent1_genotype);

  // Pick a random waiting genome from the nehighborhood for recombination
  int PickRandRecGenome(cAvidaContext& ctx, const int & parent_id, int world_x, int world_y);
  

  cBirthChamber(); // @not_implemented
  cBirthChamber(const cBirthChamber&); // @not_implemented
  cBirthChamber& operator=(const cBirthChamber&); // @not_implemented
  
public:
  cBirthChamber(cWorld* world);
  ~cBirthChamber() { ; }

  // Handle manipulations & tests of genome.  Return false if divide process
  // should halt.  Place offspring in child_array.
  bool SubmitOffspring(cAvidaContext& ctx, const cGenome& child_genome, cOrganism& parent,
                       tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array);

  // Check the neighborhood for waiting genomes
  bool GetNeighborWaiting(const int & parent_id, int world_x, int world_y);
};


#ifdef ENABLE_UNIT_TESTS
namespace nBirthChamber {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

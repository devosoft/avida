/*
 *  cBirthChamber.h
 *  Avida
 *
 *  Called "birth_chamber.hh" prior to 12/2/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cBirthEntry_h
#include "cBirthEntry.h"
#endif
#ifndef tArrayMap_h
#include "tArrayMap.h"
#endif

/**
 * All genome-based organisms must go through the birth chamber, which will
 * handle any special modifications during the birth process, such as divide
 * mutations reversions, sterilization, and crossover for sex.  If sex is
 * turned on, this class will hold organisms that are waiting to be crossed
 * over before they are born.
 **/

class cAvidaContext;
class cBirthSelectionHandler;
class cSequence;
class cOrganism;
class cWorld;

class cBirthChamber
{
private:
  cWorld* m_world;
  tArrayMap<int, cBirthSelectionHandler*> m_handler_map;


  cBirthChamber(); // @not_implemented
  cBirthChamber(const cBirthChamber&); // @not_implemented
  cBirthChamber& operator=(const cBirthChamber&); // @not_implemented
  

public:
  cBirthChamber(cWorld* world) : m_world(world) { ; }
  ~cBirthChamber();

  // Handle manipulations & tests of genome.  Return false if divide process
  // should halt.  Place offspring in child_array.
  bool SubmitOffspring(cAvidaContext& ctx, const cGenome& offspring_genome, cOrganism* parent,
                       tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array);  

  bool ValidBirthEntry(const cBirthEntry& entry) const;
  void StoreAsEntry(const cGenome& offspring_genome, cOrganism* parent, cBirthEntry& entry) const;
  void ClearEntry(cBirthEntry& entry);

private:
  cBirthSelectionHandler* getSelectionHandler(int hw_type);
  
  bool RegionSwap(cSequence& genome0, cSequence& genome1, int start0, int end0, int start1, int end1);
  void GenomeSwap(cSequence& genome0, cSequence& genome1, double& merit0, double& merit1);
  
  bool DoAsexBirth(cAvidaContext& ctx, const cGenome& offspring_genome, cOrganism& parent,
                   tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array);
  bool DoPairAsexBirth(cAvidaContext& ctx, const cBirthEntry& old_entry, const cGenome& new_genome, cOrganism& parent,
                       tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array);
  

  void DoBasicRecombination(cAvidaContext& ctx, cSequence& genome0, cSequence& genome1, double& merit0, double& merit1);
  void DoModularContRecombination(cAvidaContext& ctx, cSequence& genome0, cSequence& genome1,
                                  double& merit0, double& merit1);
  void DoModularNonContRecombination(cAvidaContext& ctx, cSequence& genome0, cSequence& genome1,
                                     double& merit0, double& merit1);
  void DoModularShuffleRecombination(cAvidaContext& ctx, cSequence& genome0, cSequence& genome1,
                                     double& merit0, double& merit1);
  
  void SetupGenotypeInfo(cOrganism* organism, const tArray<cBioGroup*>* p0grps, const tArray<cBioGroup*>* p1grps = NULL);
};


#endif

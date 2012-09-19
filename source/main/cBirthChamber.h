/*
 *  cBirthChamber.h
 *  Avida
 *
 *  Called "birth_chamber.hh" prior to 12/2/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cBirthChamber_h
#define cBirthChamber_h

#include "avida/systematics/Group.h"

#include "cBirthEntry.h"

/**
 * All genome-based organisms must go through the birth chamber, which will
 * handle any special modifications during the birth process, such as divide
 * mutations reversions, sterilization, and crossover for sex.  If sex is
 * turned on, this class will hold organisms that are waiting to be crossed
 * over before they are born.
 **/

class cAvidaContext;
class cBirthSelectionHandler;
class cOrganism;
class cWorld;

using namespace Avida;


class cBirthChamber
{
private:
  cWorld* m_world;
  Apto::Map<int, cBirthSelectionHandler*> m_handler_map;


  cBirthChamber(); // @not_implemented
  cBirthChamber(const cBirthChamber&); // @not_implemented
  cBirthChamber& operator=(const cBirthChamber&); // @not_implemented
  

public:
  cBirthChamber(cWorld* world) : m_world(world) { ; }
  ~cBirthChamber();

  // Handle manipulations & tests of genome.  Return false if divide process
  // should halt.  Place offspring in child_array.
  bool SubmitOffspring(cAvidaContext& ctx, const Genome& offspring_genome, cOrganism* parent,
                       Apto::Array<cOrganism*>& child_array, Apto::Array<cMerit>& merit_array);

  bool ValidBirthEntry(const cBirthEntry& entry) const;
  bool ValidateBirthEntry(cBirthEntry& entry); //@CHC: Same as ValidBirthEntry() but may modify the entry if it has died due to old age
  void StoreAsEntry(const Genome& offspring_genome, cOrganism* parent, cBirthEntry& entry) const;
  void ClearEntry(cBirthEntry& entry);
  
  int GetWaitingOffspringNumber(int which_mating_type, int hw_type);
  void PrintBirthChamber(const cString& filename, int hw_type);

private:
  cBirthSelectionHandler* getSelectionHandler(int hw_type);
  
  bool RegionSwap(InstructionSequence& genome0, InstructionSequence& genome1, int start0, int end0, int start1, int end1);
  void GenomeSwap(InstructionSequence& genome0, InstructionSequence& genome1, double& merit0, double& merit1);
  
  bool DoAsexBirth(cAvidaContext& ctx, const Genome& offspring_genome, cOrganism& parent,
                   Apto::Array<cOrganism*>& child_array, Apto::Array<cMerit>& merit_array);
  bool DoPairAsexBirth(cAvidaContext& ctx, const cBirthEntry& old_entry, const Genome& new_genome, cOrganism& parent,
                       Apto::Array<cOrganism*>& child_array, Apto::Array<cMerit>& merit_array);
  

  void DoBasicRecombination(cAvidaContext& ctx, InstructionSequence& genome0, InstructionSequence& genome1, double& merit0, double& merit1);
  void DoModularContRecombination(cAvidaContext& ctx, InstructionSequence& genome0, InstructionSequence& genome1,
                                  double& merit0, double& merit1);
  void DoModularNonContRecombination(cAvidaContext& ctx, InstructionSequence& genome0, InstructionSequence& genome1,
                                     double& merit0, double& merit1);
  void DoModularShuffleRecombination(cAvidaContext& ctx, InstructionSequence& genome0, InstructionSequence& genome1,
                                     double& merit0, double& merit1);
  
  void SetupGenotypeInfo(cOrganism* organism, Systematics::ConstGroupMembershipPtr p0grps, Systematics::ConstGroupMembershipPtr p1grps = Systematics::ConstGroupMembershipPtr(NULL));
};


#endif

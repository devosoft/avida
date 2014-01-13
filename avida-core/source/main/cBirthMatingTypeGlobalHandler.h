/*
 *  cBirthMatingTypeGlobalHandler.h
 *  Avida
 *
 *  Created by David Bryson on 4/1/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#ifndef cBirthMatingTypeGlobalHandler_h
#define cBirthMatingTypeGlobalHandler_h

#include "cBirthEntry.h"
#include "cBirthSelectionHandler.h"

class cBirthChamber;


class cBirthMatingTypeGlobalHandler : public cBirthSelectionHandler
{
private:
  cWorld* m_world;
  cBirthChamber* m_bc;
  Apto::Array<cBirthEntry> m_entries;

  int getTaskID(cString task_name, cWorld* world);
  void storeOffspring(cAvidaContext& ctx, const Genome& offspring, cOrganism* parent);
  cBirthEntry* selectMate(cAvidaContext& ctx, const Genome& offspring, cOrganism* parent, int which_mating_type, int mate_choice_method);
  int getWaitingOffspringMostTask(int which_mating_type, int task_id);
  bool compareBirthEntries(cAvidaContext& ctx, int mate_choice_method, const cBirthEntry& entry1, const cBirthEntry& entry2);
  
public:
  cBirthMatingTypeGlobalHandler(cWorld* world, cBirthChamber* bc) : m_world(world), m_bc(bc) { ; }
  ~cBirthMatingTypeGlobalHandler();
  
  cBirthEntry* SelectOffspring(cAvidaContext& ctx, const Genome& offspring, cOrganism* parent);
  
  int GetWaitingOffspringNumber(int which_mating_type);
  void PrintBirthChamber(const cString& filename);
  //void GetWaitingOffspringTaskData(int task_id, float results_array[]); //CHC Note: Overridden functionality still needs to be implemented
};

#endif

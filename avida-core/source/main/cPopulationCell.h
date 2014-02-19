/*
 *  cPopulationCell.h
 *  Avida
 *
 *  Called "pop_cell.hh" prior to 12/5/05.
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

#ifndef cPopulationCell_h
#define cPopulationCell_h

#include "avida/core/InstructionSequence.h"

#include "cMutationRates.h"

class cHardwareBase;
class cPopulation;
class cOrganism;
class cWorld;

using namespace Avida;


class cPopulationCell
{
  friend class cPopulation;

private:
  cWorld* m_world;

  cOrganism* m_organism;
  cHardwareBase* m_hardware;

  cMutationRates* m_mut_rates;           // Mutation rates at this cell.

  int m_cell_id;           // Unique id for position of cell in population.

  void InsertOrganism(cOrganism* new_org, cAvidaContext& ctx);
  cOrganism* RemoveOrganism(cAvidaContext& ctx); 


public:
  cPopulationCell() : m_world(NULL), m_organism(NULL), m_hardware(NULL), m_mut_rates(NULL) { ; }
  cPopulationCell(const cPopulationCell& in_cell);
  ~cPopulationCell() { delete m_mut_rates; }

  void operator=(const cPopulationCell& in_cell);

  void Setup(cWorld* world, int in_id, const cMutationRates& in_rates, int x, int y);

  
  inline cOrganism* GetOrganism() const { return m_organism; }
  inline cHardwareBase* GetHardware() const { return m_hardware; }

  inline const cMutationRates& MutationRates() const { assert(m_mut_rates); return *m_mut_rates; }
  inline cMutationRates& MutationRates() { assert(m_mut_rates); return *m_mut_rates; }

  inline int GetID() const { return m_cell_id; }

  inline bool IsOccupied() const { return m_organism != NULL; }
};

#endif

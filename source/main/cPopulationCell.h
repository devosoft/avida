/*
 *  cPopulationCell.h
 *  Avida
 *
 *  Called "pop_cell.hh" prior to 12/5/05.
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

#ifndef cPopulationCell_h
#define cPopulationCell_h

#include <fstream>

#ifndef cMutationRates_h
#include "cMutationRates.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

class cHardwareBase;
class cPopulation;
class cOrganism;
class cPopulationCell;
class cWorld;

class cPopulationCell
{
  friend class cPopulation;

private:
  cWorld* m_world;
  
  cOrganism* m_organism;                    // The occupent of this cell.
  cHardwareBase* m_hardware;
  
  tList<cPopulationCell> m_connections;  // A list of neighboring cells.
  cMutationRates* m_mut_rates;           // Mutation rates at this cell.
  tArray<int> m_inputs;                 // Environmental Inputs...

  int m_cell_id;           // Unique id for position of cell in population.
  int m_deme_id;           // ID of the deme that this cell is part of.
  int m_cell_data;         // "data" that is local to the cell and can be retrieaved by the org.
  int m_spec_state;

  bool m_migrant; //@AWC -- does the cell contain a migrant genome?

  // location in population
  int m_x; //!< The x-coordinate of the position of this cell in the environment.
  int m_y; //!< The y-coordinate of the position of this cell in the environment.

  
  void InsertOrganism(cOrganism* new_org);
  cOrganism* RemoveOrganism();

  
public:
  cPopulationCell() : m_world(NULL), m_organism(NULL), m_hardware(NULL), m_mut_rates(NULL), m_migrant(false) { ; }
  cPopulationCell(const cPopulationCell& in_cell);
  ~cPopulationCell() { delete m_mut_rates; }

  void operator=(const cPopulationCell& in_cell);

  void Setup(cWorld* world, int in_id, const cMutationRates& in_rates, int x, int y);
  void SetDemeID(int in_id) { m_deme_id = in_id; }
  void Rotate(cPopulationCell& new_facing);

  //@AWC -- This is, admittatidly, a hack to get migration between demes working under local copy...
  void SetMigrant() {m_migrant = true;} //@AWC -- this cell will contain a migrant genome
  void UnsetMigrant() {m_migrant = false;} //@AWC -- unset the migrant flag
  bool IsMigrant() {return m_migrant;} //@AWC -- does this contain a migrant genome?

  inline cOrganism* GetOrganism() const { return m_organism; }
  inline cHardwareBase* GetHardware() const { return m_hardware; }
  inline tList<cPopulationCell>& ConnectionList() { return m_connections; }
  inline cPopulationCell& GetCellFaced() { return *(m_connections.GetFirst()); }
  int GetFacing();  // Returns the facing of this cell.
  inline void GetPosition(int& x, int& y) { x = m_x; y = m_y; } // Retrieves the position (x,y) coordinates of this cell.
  inline const cMutationRates& MutationRates() const { assert(m_mut_rates); return *m_mut_rates; }
  inline cMutationRates& MutationRates() { assert(m_mut_rates); return *m_mut_rates; }
  
  inline int GetInput(int) const { return m_inputs[m_cell_id]; }
  inline const tArray<int>& GetInputs() const { return m_inputs; }
  inline int GetInputAt(int& input_pointer);
  inline int GetInputSize() { return m_inputs.GetSize(); }
  void ResetInputs(cAvidaContext& ctx);

  inline int GetID() const { return m_cell_id; }
  inline int GetDemeID() const { return m_deme_id; }
  inline int GetCellData() const { return m_cell_data; }
  void SetCellData(const int data) { m_cell_data = data; }
  
  inline int GetSpeculativeState() const { return m_spec_state; }
  inline void SetSpeculativeState(int count) { m_spec_state = count; }
  inline void DecSpeculative() { m_spec_state--; }

  inline bool IsOccupied() const { return m_organism != NULL; }

  double UptakeCellEnergy(double frac_to_uptake);

  bool OK();
};

inline int cPopulationCell::GetInputAt(int& input_pointer)
{
  input_pointer %= m_inputs.GetSize();
  return m_inputs[input_pointer++];
}



#endif

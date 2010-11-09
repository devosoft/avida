/*
 *  cPopulationCell.h
 *  Avida
 *
 *  Called "pop_cell.hh" prior to 12/5/05.
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

#ifndef cPopulationCell_h
#define cPopulationCell_h

#include <fstream>
#include <set>
#include <deque>

#ifndef cMutationRates_h
#include "cMutationRates.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif
#include "cSequence.h"
#include "cGenomeUtil.h"

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
  
  struct {
    int contents;
    int org_id;
    int update;
  } m_cell_data;         // "data" that is local to the cell and can be retrieaved by the org.
  
  int m_spec_state;

  bool m_migrant; //@AWC -- does the cell contain a migrant genome?

  // location in population
  int m_x; //!< The x-coordinate of the position of this cell in the environment.
  int m_y; //!< The y-coordinate of the position of this cell in the environment.

  // @WRE: Statistic for movement
  int m_visits; // The number of times Avidians move into the cell

  void InsertOrganism(cOrganism* new_org);
  cOrganism* RemoveOrganism();


public:
	typedef std::set<cPopulationCell*> neighborhood_type; //!< Type for cell neighborhoods.

  cPopulationCell() : m_world(NULL), m_organism(NULL), m_hardware(NULL), m_mut_rates(NULL), m_migrant(false), m_hgt(0) { ; }
  cPopulationCell(const cPopulationCell& in_cell);
  ~cPopulationCell() { delete m_mut_rates; delete m_hgt; }

  void operator=(const cPopulationCell& in_cell);

  void Setup(cWorld* world, int in_id, const cMutationRates& in_rates, int x, int y);
  void SetDemeID(int in_id) { m_deme_id = in_id; }
  void Rotate(cPopulationCell& new_facing);

  //@AWC -- This is, admittedly, a hack to get migration between demes working under local copy...
  void SetMigrant() {m_migrant = true;} //@AWC -- this cell will contain a migrant genome
  void UnsetMigrant() {m_migrant = false;} //@AWC -- unset the migrant flag
  bool IsMigrant() {return m_migrant;} //@AWC -- does this contain a migrant genome?

  inline cOrganism* GetOrganism() const { return m_organism; }
  inline cHardwareBase* GetHardware() const { return m_hardware; }
  inline tList<cPopulationCell>& ConnectionList() { return m_connections; }
	//! Recursively build a set of cells that neighbor this one, out to the given depth.
  void GetNeighboringCells(std::set<cPopulationCell*>& cell_set, int depth) const;
	//! Recursively build a set of occupied cells that neighbor this one, out to the given depth.
  void GetOccupiedNeighboringCells(std::set<cPopulationCell*>& occupied_cell_set, int depth) const;
  inline cPopulationCell& GetCellFaced() { return *(m_connections.GetFirst()); }
  int GetFacing();  // Returns the facing of this cell.
  inline void GetPosition(int& x, int& y) const { x = m_x; y = m_y; } // Retrieves the position (x,y) coordinates of this cell.
	inline std::pair<int,int> GetPosition() const { return std::make_pair(m_x,m_y); }
  inline int GetVisits() { return m_visits; } // @WRE: Retrieves the number of visits for this cell.
  inline void IncVisits() { m_visits++; } // @WRE: Increments the visit count for a cell
  inline const cMutationRates& MutationRates() const { assert(m_mut_rates); return *m_mut_rates; }
  inline cMutationRates& MutationRates() { assert(m_mut_rates); return *m_mut_rates; }

  inline int GetInput(int input_cell) const { return m_inputs[input_cell]; }
  inline const tArray<int>& GetInputs() const { return m_inputs; }
  inline int GetInputAt(int& input_pointer);
  inline int GetInputSize() { return m_inputs.GetSize(); }
  void ResetInputs(cAvidaContext& ctx);

  inline int GetID() const { return m_cell_id; }
  inline int GetDemeID() const { return m_deme_id; }
  inline int GetCellData() const { return m_cell_data.contents; }
  inline int GetCellDataOrgID() const { return m_cell_data.org_id; }
  inline int GetCellDataUpdate() const { return m_cell_data.update; }
  void SetCellData(int data, int org_id = -1);

  inline int GetSpeculativeState() const { return m_spec_state; }
  inline void SetSpeculativeState(int count) { m_spec_state = count; }
  inline void DecSpeculative() { m_spec_state--; }

  inline bool IsOccupied() const { return m_organism != NULL; }

  double UptakeCellEnergy(double frac_to_uptake);

  bool OK();

	// -------- HGT support --------
public:
	typedef cGenomeUtil::fragment_list_type fragment_list_type; //!< Type for the list of genome fragments.
	//! Diffuse genome fragments from this cell to its neighbors.
	void DiffuseGenomeFragments();
	//! Add fragments from the passed-in genome to the HGT fragments contained in this cell.
	void AddGenomeFragments(const cSequence& genome);
	//! Retrieve the number of genome fragments currently found in this cell.
	unsigned int CountGenomeFragments() const;
	//! Remove and return the front genome fragment.
	cSequence PopGenomeFragment();
	//! Retrieve the list of fragments from this cell.
	fragment_list_type& GetFragments();
	//! Clear all fragments from this cell, adjust resources as required.
	void ClearFragments();

private:
	//! Contains HGT-related information for this cell.
	struct HGTSupport {
		// WARNING: the default operator= is used in cPopulationCell's copy ctor and operator=.
		fragment_list_type fragments; //!< Fragments located in this cell.
	};
	HGTSupport* m_hgt; //!< Lazily-initialized pointer to the HGT support struct.
	//! Initialize HGT support in this cell.
	inline void InitHGTSupport() { if(!m_hgt) { m_hgt = new HGTSupport(); } }
	//! Is HGT initialized?
	inline bool IsHGTInitialized() const { return m_hgt != 0; }
};

inline int cPopulationCell::GetInputAt(int& input_pointer)
{
  input_pointer %= m_inputs.GetSize();
  return m_inputs[input_pointer++];
}

#endif

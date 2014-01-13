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

#include <fstream>
#include <set>
#include <deque>

#include "cMutationRates.h"
#include "tList.h"
#include "cGenomeUtil.h"

class cHardwareBase;
class cPopulation;
class cOrganism;
class cPopulationCell;
class cWorld;

using namespace Avida;


class cPopulationCell
{
  friend class cPopulation;

private:
  cWorld* m_world;

  cOrganism* m_organism;                    // The occupent of this cell.
  cHardwareBase* m_hardware;

  tList<cPopulationCell> m_connections;  // A list of neighboring cells.
  cMutationRates* m_mut_rates;           // Mutation rates at this cell.
  Apto::Array<int> m_inputs;                 // Environmental Inputs...

  int m_cell_id;           // Unique id for position of cell in population.
  int m_deme_id;           // ID of the deme that this cell is part of.  

  struct {
    int contents;
    int org_id;
    int update;
    int territory;
    int current;
    int forager;
  } m_cell_data;         // "data" that is local to the cell and can be retrieaved by the org.

  int m_spec_state;

  bool m_migrant; //@AWC -- does the cell contain a migrant genome?

  // location in population
  int m_x; //!< The x-coordinate of the position of this cell in the environment.
  int m_y; //!< The y-coordinate of the position of this cell in the environment.

  // @WRE: Statistic for movement
  int m_visits; // The number of times Avidians move into the cell

  void InsertOrganism(cOrganism* new_org, cAvidaContext& ctx); 
  cOrganism* RemoveOrganism(cAvidaContext& ctx); 


public:
  typedef std::set<cPopulationCell*> neighborhood_type; //!< Type for cell neighborhoods.

  cPopulationCell() : m_world(NULL), m_organism(NULL), m_hardware(NULL), m_mut_rates(NULL), m_migrant(false), m_can_input(false), m_can_output(false), m_hgt(0) { ; }
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
  void GetOccupiedNeighboringCells(Apto::Array<cPopulationCell*>& occupied_cells) const;
  inline cPopulationCell& GetCellFaced() { return *(m_connections.GetFirst()); }
  int GetFacing();  // Returns the facing of this cell.
  int GetFacedDir(); // Returns the human interpretable facing of this org.
  inline void GetPosition(int& x, int& y) const { x = m_x; y = m_y; } // Retrieves the position (x,y) coordinates of this cell.
  inline std::pair<int,int> GetPosition() const { return std::make_pair(m_x,m_y); }
  inline int GetVisits() { return m_visits; } // @WRE: Retrieves the number of visits for this cell.
  inline void IncVisits() { m_visits++; } // @WRE: Increments the visit count for a cell
  inline const cMutationRates& MutationRates() const { assert(m_mut_rates); return *m_mut_rates; }
  inline cMutationRates& MutationRates() { assert(m_mut_rates); return *m_mut_rates; }

  inline int GetInput(int input_cell) const { return m_inputs[input_cell]; }
  inline const Apto::Array<int>& GetInputs() const { return m_inputs; }
  inline int GetInputAt(int& input_pointer);
  inline int GetInputSize() { return m_inputs.GetSize(); }
  void ResetInputs(cAvidaContext& ctx);

  inline int GetID() const { return m_cell_id; }
  inline int GetDemeID() const { return m_deme_id; }
  inline int GetCellData() const { return m_cell_data.contents; }
  inline int GetCellDataOrgID() const { return m_cell_data.org_id; }
  inline int GetCellDataUpdate() const { return m_cell_data.update; }
  inline int GetCellDataTerritory() const { return m_cell_data.territory; }
  inline int GetCellDataForagerType() const { return m_cell_data.forager; }
  void UpdateCellDataExpired();
  void SetCellData(int data, int org_id = -1);
  void ClearCellData();

  inline int GetSpeculativeState() const { return m_spec_state; }
  inline void SetSpeculativeState(int count) { m_spec_state = count; }
  inline void DecSpeculative() { m_spec_state--; }

  inline bool IsOccupied() const { return m_organism != NULL; }

  double UptakeCellEnergy(double frac_to_uptake, cAvidaContext& ctx); 
  
// -------- Avatar support -------- 
private:
  Apto::Array<cOrganism*, Apto::Smart>  m_av_prey;
  Apto::Array<cOrganism*, Apto::Smart>  m_av_pred;

public:
  inline int GetNumAVInputs() const { return GetNumPredAV(); }
  inline int GetNumAVOutputs() const { return GetNumPreyAV(); }
  inline int GetNumAV() const { return GetNumPreyAV() + GetNumPredAV(); }
  inline int GetNumPredAV() const { return m_av_pred.GetSize(); }
  inline int GetNumPreyAV() const { return m_av_prey.GetSize(); }
  void AddPredAV(cAvidaContext& ctx, cOrganism* org);
  void AddPreyAV(cAvidaContext& ctx, cOrganism* org);
  void RemovePredAV(cOrganism* org);
  void RemovePreyAV(cOrganism* org);
  inline bool HasInputAV() const { return GetNumAVInputs() > 0; }
  inline bool HasOutputAV() const { return GetNumAVOutputs() > 0; }
  inline bool HasAV() const { return (GetNumAVOutputs() > 0 || GetNumAVInputs() > 0); }
  inline bool HasPredAV() const { return GetNumPredAV() > 0; }
  inline bool HasPreyAV() const { return GetNumPreyAV() > 0; }
  bool HasOutputAV(cOrganism* org);
  cOrganism* GetRandAV(cAvidaContext& ctx) const;
  cOrganism* GetRandPredAV() const;
  cOrganism* GetRandPreyAV() const;
  Apto::Array<cOrganism*> GetCellInputAVs();
  Apto::Array<cOrganism*> GetCellOutputAVs();
  Apto::Array<cOrganism*> GetCellAVs();

// -------- Neural support -------- 
private:
  bool m_can_input;
  bool m_can_output;
public:
  void SetCanInput(bool input) { m_can_input = input; }
  void SetCanOutput(bool output) { m_can_output = output; }
  bool GetCanInput() { return m_can_input; }
  bool GetCanOutput() { return m_can_output; }

	// -------- HGT support --------
public:
	typedef cGenomeUtil::fragment_list_type fragment_list_type; //!< Type for the list of genome fragments.
	//! Diffuse genome fragments from this cell to its neighbors.
	void DiffuseGenomeFragments();
	//! Add fragments from the passed-in genome to the HGT fragments contained in this cell.
	void AddGenomeFragments(cAvidaContext& ctx, const InstructionSequence& genome);
	//! Retrieve the number of genome fragments currently found in this cell.
	unsigned int CountGenomeFragments() const;
	//! Remove and return the front genome fragment.
	InstructionSequence PopGenomeFragment(cAvidaContext& ctx);
	//! Retrieve the list of fragments from this cell.
	fragment_list_type& GetFragments();
	//! Clear all fragments from this cell, adjust resources as required.
	void ClearFragments(cAvidaContext& ctx);

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

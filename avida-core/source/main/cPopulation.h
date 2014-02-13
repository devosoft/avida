/*
 *  cPopulation.h
 *  Avida
 *
 *  Called "population.hh" prior to 12/5/05.
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

#ifndef cPopulation_h
#define cPopulation_h

#include "avida/data/Provider.h"

#include "cBirthChamber.h"
#include "cOrgInterface.h"
#include "cPopulationInterface.h"
#include "cWorld.h"

#include <fstream>
#include <map>

class cAvidaContext;
class cEnvironment;
class cLineage;
class cOrganism;
class cPopulationCell;

using namespace Avida;


class cPopulationOrgStatProvider : public Data::ArgumentedProvider
{
public:
  ~cPopulationOrgStatProvider();

  virtual void UpdateReset() = 0;
  virtual void HandleOrganism(cOrganism* org) = 0;
};

typedef Apto::SmartPtr<cPopulationOrgStatProvider, Apto::InternalRCObject> cPopulationOrgStatProviderPtr;


class cPopulation : public Data::ArgumentedProvider
{
private:
  // Components...
  cWorld* m_world;
  Apto::PriorityScheduler* m_scheduler;                // Handles allocation of CPU cycles
  Apto::Array<cPopulationCell> cell_array;  // Local cells composing the population
  Apto::Array<int> empty_cell_id_array;     // Used for PREFER_EMPTY birth methods
  cBirthChamber birth_chamber;         // Global birth chamber.
  
  // Keep list of live organisms
  Apto::Array<cOrganism*, Apto::Smart> live_org_list;
  
  Apto::Array<cPopulationOrgStatProviderPtr> m_org_stat_providers;
  
  
  // Data Tracking...
  Apto::List<cPopulationCell*> reaper_queue; // Death order in some mass-action runs
  
  // Default organism setups...
  cEnvironment& environment;          // Physics & Chemistry description

  // Other data...
  int world_x;                         // Structured population width.
  int world_y;                         // Structured population height.
  int num_organisms;                   // Cell count with living organisms
  
  int num_prey_organisms;
  int num_pred_organisms;
  int num_top_pred_organisms;
  
  // Outside interactions...
  bool sync_events;   // Do we need to sync up the event list with population?
	 
  
public:
  cPopulation(cWorld* world);
  ~cPopulation();


  // Data::Provider
  Data::ConstDataSetPtr Provides() const;
  void UpdateProvidedValues(Update current_update);
  Apto::String DescribeProvidedValue(const Apto::String& data_id) const;
  bool SupportsConcurrentUpdate() const;
  
  // Data::ArgumentedProvider
  void SetActiveArguments(const Data::DataID& data_id, Data::ConstArgumentSetPtr args);
  Data::ConstArgumentSetPtr GetValidArguments(const Data::DataID& data_id) const;
  bool IsValidArgument(const Data::DataID& data_id, Data::Argument arg) const;
  
  Data::PackagePtr GetProvidedValueForArgument(const Data::DataID& data_id, const Data::Argument& arg) const;

  
  // cPopulation
  void AttachOrgStatProvider(cPopulationOrgStatProviderPtr provider) { m_org_stat_providers.Push(provider); }

  
  void InjectGenome(int cell_id, Systematics::Source src, const Genome& genome, cAvidaContext& ctx, bool assign_group = true, Systematics::RoleClassificationHints* hints = NULL);

  
  // Activate the offspring of an organism in the population
  bool ActivateOffspring(cAvidaContext& ctx, const Genome& offspring_genome, cOrganism* parent_organism);
  
  // Inject an organism from the outside world.
  void Inject(const Genome& genome, Systematics::Source src, cAvidaContext& ctx, int cell_id = -1, double merit = -1, double neutral_metric = 0, bool inject_with_group = false, int group_id = -1, int forager_type = -1, int trace = 0);
  void InjectGroup(const Genome& genome, Systematics::Source src, cAvidaContext& ctx, int cell_id = -1, double merit = -1, double neutral_metric = 0, int group_id = -1, int forager_type = -1, int trace = 0);
  
  // Deactivate an organism in the population (required for deactivations)
  void KillOrganism(cPopulationCell& in_cell, cAvidaContext& ctx); 
  void KillOrganism(cAvidaContext& ctx, int in_cell) { KillOrganism(cell_array[in_cell], ctx); } 
  
  // Specialized functionality
  void SwapCells(int cell_id1, int cell_id2, cAvidaContext& ctx); 

  
  // Process a single organism one instruction...
  int ScheduleOrganism();          // Determine next organism to be processed.
  void ProcessStep(cAvidaContext& ctx, double step_size, int cell_id);
  void ProcessStepSpeculative(cAvidaContext& ctx, double step_size, int cell_id);

  // Calculate the statistics from the most recent update.
  void ProcessPostUpdate(cAvidaContext& ctx);
  void ProcessPreUpdate();
  void ProcessUpdateCellActions(cAvidaContext& ctx);

  // Saving and loading...
  bool SavePopulation(const cString& filename, bool save_historic, bool save_group_info = false, bool save_avatars = false,
                      bool save_rebirth = false);
  bool SaveStructuredSystematicsGroup(const Systematics::RoleID& role, const cString& filename);
  bool LoadStructuredSystematicsGroup(cAvidaContext& ctx, const Systematics::RoleID& role, const cString& filename);
  bool LoadPopulation(const cString& filename, cAvidaContext& ctx, int cellid_offset=0, int lineage_offset=0,
                      bool load_groups = false, bool load_birth_cells = false, bool load_avatars = false, bool load_rebirth = false, bool load_parent_dat = false);
  
  int GetSize() const { return cell_array.GetSize(); }
  int GetWorldX() const { return world_x; }
  int GetWorldY() const { return world_y; }

  cPopulationCell& GetCell(int in_num) { return cell_array[in_num]; }

  cBirthChamber& GetBirthChamber(int id) { (void) id; return birth_chamber; }

  void ResetInputs(cAvidaContext& ctx);

  cEnvironment& GetEnvironment() { return environment; }
  int GetNumOrganisms() { return num_organisms; }
  int GetNumPreyOrganisms() { return num_prey_organisms; }
  int GetNumPredOrganisms() { return num_pred_organisms; }
  int GetNumTopPredOrganisms() { return num_top_pred_organisms; }

  void DecNumPreyOrganisms() { num_prey_organisms--; }
  void DecNumPredOrganisms() { num_pred_organisms--; }
  void DecNumTopPredOrganisms() { num_top_pred_organisms--; }

  void IncNumPreyOrganisms() { num_prey_organisms++; }
  void IncNumPredOrganisms() { num_pred_organisms++; }
  void IncNumTopPredOrganisms() { num_top_pred_organisms++; }
  
  void RemovePredators(cAvidaContext& ctx);
  void InjectPreyClone(cAvidaContext& ctx, cOrganism* org_to_clone);
  
  bool GetSyncEvents() { return sync_events; }
  void SetSyncEvents(bool _in) { sync_events = _in; }

  bool UpdateMerit(int cell_id, double new_merit);

  // Add an org to live org list
  void AddLiveOrg(cOrganism* org);  
  // Remove an org from live org list
  void RemoveLiveOrg(cOrganism* org); 
  const Apto::Array<cOrganism*, Apto::Smart>& GetLiveOrgList() const { return live_org_list; }
	
  //Kill random member of the group (but not self!!!)
  void AttackFacedOrg(cAvidaContext& ctx, int loser);
  void KillRandPred(cAvidaContext& ctx, cOrganism* org);
  void KillRandPrey(cAvidaContext& ctx, cOrganism* org);
  

private:
  void SetupCellGrid();
  void BuildTimeSlicer(); // Build the schedule object
  
  // Methods to place offspring in the population.
  cPopulationCell& PositionOffspring(cPopulationCell& parent_cell, cAvidaContext& ctx, bool parent_ok = true); 
  void PositionAge(cPopulationCell& parent_cell, Apto::List<cPopulationCell*>& found_list, bool parent_ok);
  void PositionMerit(cPopulationCell & parent_cell, Apto::List<cPopulationCell*>& found_list, bool parent_ok);
  Apto::Array<int>& GetEmptyCellIDArray() { return empty_cell_id_array; }
  void FindEmptyCell(Apto::List<cPopulationCell*>& cell_list, Apto::List<cPopulationCell*>& found_list);
  int FindRandEmptyCell(cAvidaContext& ctx);
  
  // Update statistics collecting...
  void UpdateOrganismStats(cAvidaContext& ctx);
  void UpdateFTOrgStats(cAvidaContext& ctx); 
  void UpdateMaleFemaleOrgStats(cAvidaContext& ctx);
  
  void InjectClone(int cell_id, cOrganism& orig_org, Systematics::Source src);
  void CompeteOrganisms_ConstructOffspring(int cell_id, cOrganism& parent);
  
	
  // Must be called to activate *any* organism in the population.
  bool ActivateOrganism(cAvidaContext& ctx, cOrganism* in_organism, cPopulationCell& target_cell, bool assign_group = true, bool is_inject = false);
  
  
  int PlaceAvatar(cAvidaContext& ctx, cOrganism* parent);
  
  inline void AdjustSchedule(const cPopulationCell& cell, const cMerit& merit);
};

#endif

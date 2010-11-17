/*
 *  cPopulation.h
 *  Avida
 *
 *  Called "population.hh" prior to 12/5/05.
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

#ifndef cPopulation_h
#define cPopulation_h

#include "cBirthChamber.h"
#include "cDeme.h"
#include "cOrgInterface.h"
#include "cPopulationInterface.h"
#include "cResourceCount.h"
#include "cString.h"
#include "cWorld.h"
#include "tArray.h"
#include "tList.h"
#include "tVector.h"

#include <fstream>
#include <map>


class cAvidaContext;
class cBioUnit;
class cCodeLabel;
class cChangeList;
class cEnvironment;
class cSequence;
class cLineage;
class cOrganism;
class cPopulationCell;
class cSchedule;
class cSaleItem;



class cPopulation
{
private:
  // Components...
  cWorld* m_world;
  cSchedule* schedule;                // Handles allocation of CPU cycles
  tArray<cPopulationCell> cell_array;  // Local cells composing the population
  tArray<int> empty_cell_id_array;     // Used for PREFER_EMPTY birth methods
  cResourceCount resource_count;       // Global resources available
  cBirthChamber birth_chamber;         // Global birth chamber.
  tArray<tList<cSaleItem> > market;   // list of lists of items for sale, each list goes with 1 label

  tVector<pair<int,int> > *sleep_log;
  
  // Data Tracking...
  tList<cPopulationCell> reaper_queue; // Death order in some mass-action runs

  // Default organism setups...
  cEnvironment & environment;          // Physics & Chemistry description

  // Other data...
  int world_x;                         // Structured population width.
  int world_y;                         // Structured population height.
	int world_z; //!< Population depth.
  int num_organisms;                   // Cell count with living organisms
  tArray<cDeme> deme_array;            // Deme structure of the population.
 
  // Outside interactions...
  bool sync_events;   // Do we need to sync up the event list with population?
	
	// Group formation information
	std::map<int, int> m_groups; //<! Maps the group id to the number of orgs in the group

  int m_hgt_resid; //!< HGT resource ID.
  

  cPopulation(); // @not_implemented
  cPopulation(const cPopulation&); // @not_implemented
  cPopulation& operator=(const cPopulation&); // @not_implemented
  
  
public:
  cPopulation(cWorld* world);
  ~cPopulation();

  bool InitiatePop(cUserFeedback* errors = NULL);

  void InjectGenome(int cell_id, eBioUnitSource src, const cGenome& genome, int lineage_label = 0);

  // Activate the offspring of an organism in the population
  bool ActivateOffspring(cAvidaContext& ctx, const cGenome& offspring_genome, cOrganism* parent_organism);
  bool ActivateParasite(cOrganism* host, cBioUnit* parent, const cString& label, const cSequence& injected_code);
  
  // Inject an organism from the outside world.
  void Inject(const cGenome& genome, eBioUnitSource src, int cell_id = -1, double merit = -1, int lineage_label = 0, double neutral_metric = 0);
  void InjectParasite(const cString& label, const cSequence& injected_code, int cell_id);
  
  // Deactivate an organism in the population (required for deactivations)
  void KillOrganism(cPopulationCell& in_cell);
  
  // @WRE 2007/07/05 Helper function to take care of side effects of Avidian 
  // movement that cannot be directly handled in cHardwareCPU.cc
  void MoveOrganisms(cAvidaContext& ctx, int src_cell_id, int dest_cell_id);

  // Specialized functionality
  void Kaboom(cPopulationCell& in_cell, int distance=0);
  void AddSellValue(const int data, const int label, const int sell_price, const int org_id, const int cell_id);
  int BuyValue(const int label, const int buy_price, const int cell_id);
  void SwapCells(int cell_id1, int cell_id2);

  // Deme-related methods
  //! Compete all demes with each other based on the given competition type.
  void CompeteDemes(int competition_type);
  
  //! Compete all demes with each other based on the given vector of fitness values.
  void CompeteDemes(const std::vector<double>& calculated_fitness);

  //! Replicate all demes based on the given replication trigger.
  void ReplicateDemes(int rep_trigger);

  //! Helper method to replicate deme
  void ReplicateDeme(cDeme & source_deme);

  //! Helper method that replaces a target deme with the given source deme.
  void ReplaceDeme(cDeme& source_deme, cDeme& target_deme);
  
  //! Helper method that seeds a deme from the given genome.
  void SeedDeme(cDeme& deme, cGenome& genome, eBioUnitSource src);

  //! Helper method that seeds a deme from the given genotype.
  void SeedDeme(cDeme& _deme, cBioGroup* bg, eBioUnitSource src);
  
  //! Helper method that seeds a target deme from the organisms in the source deme.
  bool SeedDeme(cDeme& source_deme, cDeme& target_deme);

  //! Helper method that determines the cell into which an organism will be placed during deme replication.
  int DemeSelectInjectionCell(cDeme& deme, int sequence=0);
  
  //! Helper method that performs any post-injection fixups on the cell in the given deme.
  void DemePostInjection(cDeme& deme, cPopulationCell& cell);
  
  void DivideDemes();
  void ResetDemes();
  void CopyDeme(int deme1_id, int deme2_id);
  void SpawnDeme(int deme1_id, int deme2_id=-1);
  void AddDemePred(cString type, int times);

  void CheckImplicitDemeRepro(cDeme& deme);
  
  // Deme-related stats methods
  void PrintDemeAllStats();
  void PrintDemeTestamentStats(const cString& filename);
	void PrintCurrentMeanDemeDensity(const cString& filename);
  void PrintDemeEnergySharingStats();
  void PrintDemeEnergyDistributionStats();
  void PrintDemeOrganismEnergyDistributionStats();
  void PrintDemeDonor();
  void PrintDemeFitness();
  void PrintDemeGestationTime();
  void PrintDemeInstructions();
  void PrintDemeLifeFitness();
  void PrintDemeMerit();
  void PrintDemeMutationRate();
  void PrintDemeReceiver();
  void PrintDemeResource();
  void PrintDemeGlobalResources();
  void PrintDemeSpatialResData(const cResourceCount& res, const int i, const int deme_id) const;
  void PrintDemeSpatialEnergyData() const;
  void PrintDemeSpatialSleepData() const;
  void PrintDemeTasks();
	void PrintDemeTotalAvgEnergy();
  
  // Print deme founders
  void DumpDemeFounders(ofstream& fp);
  
  // Print donation stats
  void PrintDonationStats();


  // Process a single organism one instruction...
  int ScheduleOrganism();          // Determine next organism to be processed.
  void ProcessStep(cAvidaContext& ctx, double step_size, int cell_id);
  void ProcessStepSpeculative(cAvidaContext& ctx, double step_size, int cell_id);

  // Calculate the statistics from the most recent update.
  void ProcessPostUpdate(cAvidaContext& ctx);
  void ProcessUpdateCellActions(cAvidaContext& ctx);

  // Clear all but a subset of cells...
  void SerialTransfer(int transfer_size, bool ignore_deads);

  // Saving and loading...
  bool SavePopulation(const cString& filename);
  bool LoadPopulation(const cString& filename, int cellid_offset=0, int lineage_offset=0);
  bool DumpMemorySummary(std::ofstream& fp);

  bool OK();

  int GetSize() const { return cell_array.GetSize(); }
  int GetWorldX() const { return world_x; }
  int GetWorldY() const { return world_y; }
  int GetNumDemes() const { return deme_array.GetSize(); }
  cDeme& GetDeme(int i) { return deme_array[i]; }

  cPopulationCell& GetCell(int in_num) { return cell_array[in_num]; }
  const tArray<double>& GetResources() const { return resource_count.GetResources(); }
  const tArray<double>& GetCellResources(int cell_id) const { return resource_count.GetCellResources(cell_id); }
  const tArray<double>& GetDemeResources(int deme_id) { return GetDeme(deme_id).GetDemeResourceCount().GetResources(); }
  const tArray<double>& GetDemeCellResources(int deme_id, int cell_id) { return GetDeme(deme_id).GetDemeResourceCount().GetCellResources( GetDeme(deme_id).GetRelativeCellID(cell_id) ); }
  const tArray< tArray<int> >& GetCellIdLists() const { return resource_count.GetCellIdLists(); }

  cBirthChamber& GetBirthChamber(int id) { (void) id; return birth_chamber; }

  void UpdateResources(const tArray<double>& res_change);
  void UpdateResource(int id, double change);
  void UpdateCellResources(const tArray<double>& res_change, const int cell_id);
  void UpdateDemeCellResources(const tArray<double>& res_change, const int cell_id);

  void SetResource(int id, double new_level);
  double GetResource(int id) const { return resource_count.Get(id); }
  cResourceCount& GetResourceCount() { return resource_count; }

  void ResetInputs(cAvidaContext& ctx);

  cEnvironment& GetEnvironment() { return environment; }
  int GetNumOrganisms() { return num_organisms; }

  bool GetSyncEvents() { return sync_events; }
  void SetSyncEvents(bool _in) { sync_events = _in; }
  void PrintPhenotypeData(const cString& filename);
  void PrintHostPhenotypeData(const cString& filename);
  void PrintParasitePhenotypeData(const cString& filename);
  void PrintPhenotypeStatus(const cString& filename);

  bool UpdateMerit(int cell_id, double new_merit);

  void SetChangeList(cChangeList* change_list);
  cChangeList* GetChangeList();
  
  void AddBeginSleep(int cellID, int start_time);
  void AddEndSleep(int cellID, int end_time);
 
  tVector<pair<int,int> > getCellSleepLog(int i) { return sleep_log[i]; }

  // Trials and genetic algorithm @JEB
  void NewTrial(cAvidaContext& ctx);
  void CompeteOrganisms(cAvidaContext& ctx, int competition_type, int parents_survive);
  
  // Let users change environmental variables durning the run @BDB 22-Feb-2008
  void UpdateResourceCount(const int Verbosity);
	
	// Adds an organism to a group
	void JoinGroup(int group_id);
	// Removes an organism from a group
	void LeaveGroup(int group_id);
	// Identifies the number of organisms in a group
  int NumberOfOrganismsInGroup(int group_id);
	// Get the group information
	map<int, int> GetFormedGroups() { return m_groups; }
	
	// -------- HGT support --------
	//! Modify current level of the HGT resource.
	void AdjustHGTResource(double delta);
	
	// -------- Population mixing support --------
	//! Mix all organisms in the population.
	void MixPopulation();

private:
  void BuildTimeSlicer(cChangeList* change_list); // Build the schedule object
  
  // Methods to place offspring in the population.
  cPopulationCell& PositionOffspring(cPopulationCell& parent_cell, bool parent_ok = true);
  void PositionAge(cPopulationCell& parent_cell, tList<cPopulationCell>& found_list, bool parent_ok);
  void PositionMerit(cPopulationCell & parent_cell, tList<cPopulationCell>& found_list, bool parent_ok);
  void PositionEnergyUsed(cPopulationCell & parent_cell, tList<cPopulationCell>& found_list, bool parent_ok);
  cPopulationCell& PositionDemeMigration(cPopulationCell& parent_cell, bool parent_ok = true);
  cPopulationCell& PositionDemeRandom(int deme_id, cPopulationCell& parent_cell, bool parent_ok = true);
  int UpdateEmptyCellIDArray(int deme_id = -1);
  void FindEmptyCell(tList<cPopulationCell>& cell_list, tList<cPopulationCell>& found_list);
  
  // Update statistics collecting...
  void UpdateDemeStats();
  void UpdateOrganismStats();
  
  void InjectClone(int cell_id, cOrganism& orig_org, eBioUnitSource src);
  void CompeteOrganisms_ConstructOffspring(int cell_id, cOrganism& parent);
  
  //! Helper method that adds a founder organism to a deme, and sets up its phenotype
  void SeedDeme_InjectDemeFounder(int _cell_id, cBioGroup* bg, cPhenotype* _phenotype = NULL);
  
  void CCladeSetupOrganism(cOrganism* organism); 
	
  // Must be called to activate *any* organism in the population.
  void ActivateOrganism(cAvidaContext& ctx, cOrganism* in_organism, cPopulationCell& target_cell);
  
  inline void AdjustSchedule(const cPopulationCell& cell, const cMerit& merit);
};

#endif

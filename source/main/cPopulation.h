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


namespace Avida {
  class Sequence;
};

class cAvidaContext;
class cBioUnit;
class cCodeLabel;
class cEnvironment;
class cLineage;
class cOrganism;
class cPopulationCell;
class cSchedule;
class cSaleItem;

using namespace Avida;


class cPopulation : public Data::ArgumentedProvider
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
  //Keeps track of which organisms are in which group.
  tArrayMap<int, tSmartArray<cOrganism*> > group_list;
  //std::map<int, std::vector<cOrganism*> > group_list;
  
  // Keep list of live organisms
  tSmartArray<cOrganism* > live_org_list;
  
  tVector<pair<int,int> > *sleep_log;
  
  // Data Tracking...
  tList<cPopulationCell> reaper_queue; // Death order in some mass-action runs
  tSmartArray<cBioGroup*> minitrace_queue;
  bool print_mini_trace_genomes;
  
  // Default organism setups...
  cEnvironment & environment;          // Physics & Chemistry description

  // Other data...
  int world_x;                         // Structured population width.
  int world_y;                         // Structured population height.
  int world_z;                         //!< Population depth.
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


  // Data::Provider
  Data::ConstDataSetPtr Provides() const;
  void UpdateProvidedValues(Update current_update);
  Apto::String DescribeProvidedValue(const Apto::String& data_id) const;
  
  // Data::ArgumentedProvider
  void SetActiveArguments(const Data::DataID& data_id, Data::ConstArgumentSetPtr args);
  Data::ConstArgumentSetPtr GetValidArguments(const Data::DataID& data_id) const;
  bool IsValidArgument(const Data::DataID& data_id, Data::Argument arg) const;
  
  Data::PackagePtr GetProvidedValueForArgument(const Data::DataID& data_id, const Data::Argument& arg) const;

  
  
  
  // cPopulation
  
  bool InitiatePop(cUserFeedback* errors = NULL);

  void InjectGenome(int cell_id, eBioUnitSource src, const Genome& genome, cAvidaContext& ctx, int lineage_label = 0, bool assign_group = true); 

  // Activate the offspring of an organism in the population
  bool ActivateOffspring(cAvidaContext& ctx, const Genome& offspring_genome, cOrganism* parent_organism);
  bool ActivateParasite(cOrganism* host, cBioUnit* parent, const cString& label, const InstructionSequence& injected_code);
  
  // Helper function for ActivateParasite - returns if the parasite from the infected host should infect the target host
  bool TestForParasiteInteraction(cOrganism* infected_host, cOrganism* target_host);
  
  // Inject an organism from the outside world.
  void Inject(const Genome& genome, eBioUnitSource src, cAvidaContext& ctx, int cell_id = -1, double merit = -1, int lineage_label = 0, double neutral_metric = 0, bool inject_with_group = false, int group_id = -1, int forager_type = -1); 
  void InjectGroup(const Genome& genome, eBioUnitSource src, cAvidaContext& ctx, int cell_id = -1, double merit = -1, int lineage_label = 0, double neutral_metric = 0, int group_id = -1, int forager_type = -1);   
  void InjectParasite(const cString& label, const InstructionSequence& injected_code, int cell_id);
  
  // Deactivate an organism in the population (required for deactivations)
  void KillOrganism(cPopulationCell& in_cell, cAvidaContext& ctx); 
  
  // @WRE 2007/07/05 Helper function to take care of side effects of Avidian 
  // movement that cannot be directly handled in cHardwareCPU.cc
  bool MoveOrganisms(cAvidaContext& ctx, int src_cell_id, int dest_cell_id);

  // Specialized functionality
  void Kaboom(cPopulationCell& in_cell, cAvidaContext& ctx, int distance=0); 
  void AddSellValue(const int data, const int label, const int sell_price, const int org_id, const int cell_id);
  int BuyValue(const int label, const int buy_price, const int cell_id);
  void SwapCells(int cell_id1, int cell_id2, cAvidaContext& ctx); 

  // Deme-related methods
  //! Compete all demes with each other based on the given competition type.
  void CompeteDemes(cAvidaContext& ctx, int competition_type);
  
  //! Compete all demes with each other based on the given vector of fitness values.
  void CompeteDemes(const std::vector<double>& calculated_fitness, cAvidaContext& ctx); 

  //! Replicate all demes based on the given replication trigger.
  void ReplicateDemes(int rep_trigger, cAvidaContext& ctx);

  //! Helper method to replicate deme
  void ReplicateDeme(cDeme & source_deme, cAvidaContext& ctx);

  //! Helper method that replaces a target deme with the given source deme.
  void ReplaceDeme(cDeme& source_deme, cDeme& target_deme, cAvidaContext& ctx); 
  
  //! Helper method that seeds a deme from the given genome.
  void SeedDeme(cDeme& deme, Genome& genome, eBioUnitSource src, cAvidaContext& ctx); 

  //! Helper method that seeds a deme from the given genotype.
  void SeedDeme(cDeme& _deme, cBioGroup* bg, eBioUnitSource src, cAvidaContext& ctx); 
  
  //! Helper method that seeds a target deme from the organisms in the source deme.
  bool SeedDeme(cDeme& source_deme, cDeme& target_deme, cAvidaContext& ctx); 

  //! Helper method that determines the cell into which an organism will be placed during deme replication.
  int DemeSelectInjectionCell(cDeme& deme, int sequence=0);
  
  //! Helper method that performs any post-injection fixups on the cell in the given deme.
  void DemePostInjection(cDeme& deme, cPopulationCell& cell);
  
  void DivideDemes(cAvidaContext& ctx); 
  void ResetDemes();
  void CopyDeme(int deme1_id, int deme2_id, cAvidaContext& ctx); 
  void SpawnDeme(int deme1_id, cAvidaContext& ctx, int deme2_id=-1); 
  void AddDemePred(cString type, int times);

  void CheckImplicitDemeRepro(cDeme& deme, cAvidaContext& ctx);
  
  // Deme-related stats methods
  void PrintDemeAllStats(cAvidaContext& ctx); 
  void PrintDemeTestamentStats(const cString& filename);
	void PrintCurrentMeanDemeDensity(const cString& filename);
  void PrintDemeEnergySharingStats();
  void PrintDemeEnergyDistributionStats(cAvidaContext& ctx); 
  void PrintDemeOrganismEnergyDistributionStats();
  void PrintDemeDonor();
  void PrintDemeFitness();
  void PrintDemeGestationTime();
  void PrintDemeInstructions();
  void PrintDemeLifeFitness();
  void PrintDemeMerit();
  void PrintDemeMutationRate();
  void PrintDemeReceiver();
  void PrintDemeResource(cAvidaContext& ctx); 
  void PrintDemeGlobalResources(cAvidaContext& ctx); 
  void PrintDemeSpatialResData(const cResourceCount& res, const int i, const int deme_id, cAvidaContext& ctx) const; 
  void PrintDemeSpatialEnergyData() const;
  void PrintDemeSpatialSleepData() const;
  void PrintDemeTasks();
	void PrintDemeTotalAvgEnergy(cAvidaContext& ctx); 
  
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
  void UpdateResStats(cAvidaContext& ctx);
  void ProcessUpdateCellActions(cAvidaContext& ctx);

  // Clear all but a subset of cells...
  void SerialTransfer(int transfer_size, bool ignore_deads, cAvidaContext& ctx); 

  // Saving and loading...
  bool SavePopulation(const cString& filename, bool save_historic, bool save_group_info = false);
  bool LoadPopulation(const cString& filename, cAvidaContext& ctx, int cellid_offset=0, int lineage_offset=0, bool load_groups = false, bool load_birth_cells = false); 
  bool DumpMemorySummary(std::ofstream& fp);
  bool SaveFlameData(const cString& filename);
  
  void SetMiniTraceQueue(tSmartArray<cBioGroup*> new_queue, bool print_genomes);
  tSmartArray<cBioGroup*> GetMiniTraceQueue() const { return minitrace_queue; }
  
  int GetSize() const { return cell_array.GetSize(); }
  int GetWorldX() const { return world_x; }
  int GetWorldY() const { return world_y; }
  int GetNumDemes() const { return deme_array.GetSize(); }
  cDeme& GetDeme(int i) { return deme_array[i]; }

  cPopulationCell& GetCell(int in_num) { return cell_array[in_num]; }
  const tArray<double>& GetResources(cAvidaContext& ctx) const { return resource_count.GetResources(ctx); } 
  const tArray<double>& GetCellResources(int cell_id, cAvidaContext& ctx) const { return resource_count.GetCellResources(cell_id, ctx); } 
  const tArray<double>& GetFrozenResources(cAvidaContext& ctx, int cell_id) const { return resource_count.GetFrozenResources(ctx, cell_id); }
  const tArray<double>& GetDemeResources(int deme_id, cAvidaContext& ctx) { return GetDeme(deme_id).GetDemeResourceCount().GetResources(ctx); }  
  const tArray<double>& GetDemeCellResources(int deme_id, int cell_id, cAvidaContext& ctx) { return GetDeme(deme_id).GetDemeResourceCount().GetCellResources( GetDeme(deme_id).GetRelativeCellID(cell_id), ctx ); } 
  void TriggerDoUpdates(cAvidaContext& ctx) { resource_count.UpdateGlobalResources(ctx); }
  const tArray< tArray<int> >& GetCellIdLists() const { return resource_count.GetCellIdLists(); }

  int GetCurrPeakX(cAvidaContext& ctx, int res_id) const { return resource_count.GetCurrPeakX(ctx, res_id); } 
  int GetCurrPeakY(cAvidaContext& ctx, int res_id) const { return resource_count.GetCurrPeakY(ctx, res_id); } 
  int GetFrozenPeakX(cAvidaContext& ctx, int res_id) const { return resource_count.GetFrozenPeakX(ctx, res_id); } 
  int GetFrozenPeakY(cAvidaContext& ctx, int res_id) const { return resource_count.GetFrozenPeakY(ctx, res_id); } 

  cBirthChamber& GetBirthChamber(int id) { (void) id; return birth_chamber; }

  void UpdateResources(cAvidaContext& ctx, const tArray<double>& res_change);
  void UpdateResource(cAvidaContext& ctx, int id, double change);
  void UpdateCellResources(cAvidaContext& ctx, const tArray<double>& res_change, const int cell_id);
  void UpdateDemeCellResources(cAvidaContext& ctx, const tArray<double>& res_change, const int cell_id);
  
  void SetResource(cAvidaContext& ctx, int id, double new_level);
  void SetResource(cAvidaContext& ctx, const cString res_name, double new_level);
  double GetResource(cAvidaContext& ctx, int id) const { return resource_count.Get(ctx, id); }
  cResourceCount& GetResourceCount() { return resource_count; }
  void SetResourceInflow(const cString res_name, double new_level);
  void SetResourceOutflow(const cString res_name, double new_level);
  
  void SetDemeResource(cAvidaContext& ctx, const cString res_name, double new_level);
  void SetSingleDemeResourceInflow(int deme_id, const cString res_name, double new_level);
  void SetDemeResourceInflow(const cString res_name, double new_level);
  void SetSingleDemeResourceOutflow(int deme_id, const cString res_name, double new_level);
  void SetDemeResourceOutflow(const cString res_name, double new_level);
  
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

  void AddBeginSleep(int cellID, int start_time);
  void AddEndSleep(int cellID, int end_time);
 
  tVector<pair<int,int> > getCellSleepLog(int i) { return sleep_log[i]; }

  // Trials and genetic algorithm @JEB
  void NewTrial(cAvidaContext& ctx);
  void CompeteOrganisms(cAvidaContext& ctx, int competition_type, int parents_survive);
  
  // Let users change environmental variables durning the run @BDB 22-Feb-2008
  void UpdateResourceCount(const int Verbosity, cWorld* world);        
  
  // Let users change Gradient Resource variables during the run JW
  void UpdateGradientCount(const int Verbosity, cWorld* world, const cString res_name);
 
  // Add an org to live org list
  void AddLiveOrg(cOrganism* org);  
  // Remove an org from live org list
  void RemoveLiveOrg(cOrganism* org); 
  tSmartArray<cOrganism*> GetLiveOrgList() const { return live_org_list; }
	
  // Adds an organism to a group  
  void JoinGroup(cOrganism* org, int group_id);
  void MakeGroup(cOrganism* org);
  // Removes an organism from a group 
  void LeaveGroup(cOrganism* org, int group_id);

  //Kill random member of the group (but not self!!!) 
  void KillGroupMember(cAvidaContext& ctx, int group_id, cOrganism* org);
  //Attack organism faced by this one, if there is an organism in front.
  void AttackFacedOrg(cAvidaContext& ctx, int loser);
  // Identifies the number of organisms in a group
  int NumberOfOrganismsInGroup(int group_id);
  // Get the group information
  map<int, int> GetFormedGroups() { return m_groups; }

  // -------- Tolerance support --------
  // Calculate tolerance of group towards immigrants @JJB
  int CalcGroupToleranceImmigrants(int group_id);
  // Calculate tolerance of group towards offspring (not including parent) @JJB
  int CalcGroupToleranceOffspring(cOrganism* parent_organism);
  // Calculates the odds (out of 1) for immigrants based on group's tolerance @JJB
  double CalcGroupOddsImmigrants(int group_id);
  bool AttemptImmigrateGroup(int group_id, cOrganism* org);
  // Calculates the odds (out of 1) for offspring to be born into the group @JJB
  double CalcGroupOddsOffspring(int group_id);
  double CalcGroupOddsOffspring(cOrganism* parent);
  bool AttemptOffspringParentGroup(cAvidaContext& ctx, cOrganism* parent, cOrganism* offspring);
  // Calculates the standard deviation for group tolerance to immigrants
  double CalcGroupAveImmigrants(int group_id);
  double CalcGroupSDevImmigrants(int group_id);
  // Calculates the standard deviation for group tolerance to their own offspring
  double CalcGroupAveOwn(int group_id);
  double CalcGroupSDevOwn(int group_id);
  // Calculates the standard deviation for group tolerance to other group offspring
  double CalcGroupAveOthers(int group_id);
  double CalcGroupSDevOthers(int group_id);

  // -------- HGT support --------
  //! Modify current level of the HGT resource.
  void AdjustHGTResource(cAvidaContext& ctx, double delta);

  // -------- Population mixing support --------
  //! Mix all organisms in the population.
  void MixPopulation(cAvidaContext& ctx); 

private:
  void BuildTimeSlicer(); // Build the schedule object
  
  // Methods to place offspring in the population.
  cPopulationCell& PositionOffspring(cPopulationCell& parent_cell, cAvidaContext& ctx, bool parent_ok = true); 
  void PositionAge(cPopulationCell& parent_cell, tList<cPopulationCell>& found_list, bool parent_ok);
  void PositionMerit(cPopulationCell & parent_cell, tList<cPopulationCell>& found_list, bool parent_ok);
  void PositionEnergyUsed(cPopulationCell & parent_cell, tList<cPopulationCell>& found_list, bool parent_ok);
  cPopulationCell& PositionDemeMigration(cPopulationCell& parent_cell, bool parent_ok = true);
  cPopulationCell& PositionDemeRandom(int deme_id, cPopulationCell& parent_cell, bool parent_ok = true);
  int UpdateEmptyCellIDArray(int deme_id = -1);
  void FindEmptyCell(tList<cPopulationCell>& cell_list, tList<cPopulationCell>& found_list);
  
  // Update statistics collecting...
  void UpdateDemeStats(cAvidaContext& ctx); 
  void UpdateOrganismStats(cAvidaContext& ctx); 
  
  void InjectClone(int cell_id, cOrganism& orig_org, eBioUnitSource src);
  void CompeteOrganisms_ConstructOffspring(int cell_id, cOrganism& parent);
  
  //! Helper method that adds a founder organism to a deme, and sets up its phenotype
  void SeedDeme_InjectDemeFounder(int _cell_id, cBioGroup* bg, cAvidaContext& ctx, cPhenotype* _phenotype = NULL); 
  
  void CCladeSetupOrganism(cOrganism* organism); 
	
  // Must be called to activate *any* organism in the population.
  void ActivateOrganism(cAvidaContext& ctx, cOrganism* in_organism, cPopulationCell& target_cell, bool assign_group = true);
  void TestForMiniTrace(cAvidaContext& ctx, cOrganism* in_organism);
  void SetupMiniTrace(cAvidaContext& ctx, cOrganism* in_organism);
  void PrintMiniTraceGenome(cAvidaContext& ctx, cOrganism* in_organism, cString& filename);
  
  inline void AdjustSchedule(const cPopulationCell& cell, const cMerit& merit);
};

#endif

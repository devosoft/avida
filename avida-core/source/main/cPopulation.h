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
#include "tList.h"

#include <fstream>
#include <map>


class cAvidaContext;
class cCodeLabel;
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
  cResourceCount resource_count;       // Global resources available
  cBirthChamber birth_chamber;         // Global birth chamber.
  //Keeps track of which organisms are in which group.
  Apto::Map<int, Apto::Array<cOrganism*, Apto::Smart> > m_group_list;
  Apto::Map<int, Apto::Array<pair<int,int> > > m_group_intolerances;
  Apto::Map<int, Apto::Array<pair<int,int> > > m_group_intolerances_females;
  Apto::Map<int, Apto::Array<pair<int,int> > > m_group_intolerances_males;
  Apto::Map<int, Apto::Array<pair<int,int> > > m_group_intolerances_juvs;
  
  // Keep list of live organisms
  Apto::Array<cOrganism*, Apto::Smart> live_org_list;
  
  Apto::Array<cPopulationOrgStatProviderPtr> m_org_stat_providers;
  
  
  Apto::Array<pair<int,int>, Apto::Smart>* sleep_log;
  
  Apto::Array<GeneticRepresentationPtr> parasite_genotype_list;
  Apto::Array<GeneticRepresentationPtr> host_genotype_list;
  
  // Data Tracking...
  tList<cPopulationCell> reaper_queue; // Death order in some mass-action runs
  Apto::Array<int, Apto::Smart> minitrace_queue;
  bool print_mini_trace_genomes;
  bool print_mini_trace_reacs;
  bool use_micro_traces;
  int m_next_prey_q;
  int m_next_pred_q;
  
  Apto::Array<cOrganism*, Apto::Smart> repro_q;
  Apto::Array<cOrganism*, Apto::Smart> topnav_q;
  
  // Default organism setups...
  cEnvironment& environment;          // Physics & Chemistry description

  // Other data...
  int world_x;                         // Structured population width.
  int world_y;                         // Structured population height.
  int num_organisms;                   // Cell count with living organisms
  int num_prey_organisms;
  int num_pred_organisms;
  int num_top_pred_organisms;
  
  Apto::Array<cDeme> deme_array;            // Deme structure of the population.
 
  // Outside interactions...
  bool sync_events;   // Do we need to sync up the event list with population?
	
  // Group formation information
  std::map<int, int> m_groups; //<! Maps the group id to the number of orgs in the group
  std::map<int, int> m_group_females; //<! Maps the group id to the number of females in the group
  std::map<int, int> m_group_males; //<! Maps the group id to the number of males in the group

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
  bool SupportsConcurrentUpdate() const;
  
  // Data::ArgumentedProvider
  void SetActiveArguments(const Data::DataID& data_id, Data::ConstArgumentSetPtr args);
  Data::ConstArgumentSetPtr GetValidArguments(const Data::DataID& data_id) const;
  bool IsValidArgument(const Data::DataID& data_id, Data::Argument arg) const;
  
  Data::PackagePtr GetProvidedValueForArgument(const Data::DataID& data_id, const Data::Argument& arg) const;

  // cPopulation
  
  void AttachOrgStatProvider(cPopulationOrgStatProviderPtr provider) { m_org_stat_providers.Push(provider); }
  
  void ResizeCellGrid(int x, int y);
    
  void InjectGenome(int cell_id, Systematics::Source src, const Genome& genome, cAvidaContext& ctx, int lineage_label = 0, bool assign_group = true, Systematics::RoleClassificationHints* hints = NULL);

  // Activate the offspring of an organism in the population
  bool ActivateOffspring(cAvidaContext& ctx, const Genome& offspring_genome, cOrganism* parent_organism);
  bool ActivateParasite(cOrganism* host, Systematics::UnitPtr parent, const cString& label, const InstructionSequence& injected_code);
  
  // Helper function for ActivateParasite - returns if the parasite from the infected host should infect the target host
  bool TestForParasiteInteraction(cOrganism* infected_host, cOrganism* target_host);
  
  void UpdateQs(cOrganism* parent, bool reproduced = false);
  
  // Inject an organism from the outside world.
  void Inject(const Genome& genome, Systematics::Source src, cAvidaContext& ctx, int cell_id = -1, double merit = -1, int lineage_label = 0, double neutral_metric = 0, bool inject_with_group = false, int group_id = -1, int forager_type = -1, int trace = 0); 
  void InjectGroup(const Genome& genome, Systematics::Source src, cAvidaContext& ctx, int cell_id = -1, double merit = -1, int lineage_label = 0, double neutral_metric = 0, int group_id = -1, int forager_type = -1, int trace = 0);
  void InjectParasite(const cString& label, const InstructionSequence& injected_code, int cell_id);
  
  // Deactivate an organism in the population (required for deactivations)
  void KillOrganism(cPopulationCell& in_cell, cAvidaContext& ctx); 
  void KillOrganism(cAvidaContext& ctx, int in_cell) { KillOrganism(cell_array[in_cell], ctx); } 
  void InjureOrg(cAvidaContext& ctx, cPopulationCell& in_cell, double injury, bool ding_reacs = true);
  
  // @WRE 2007/07/05 Helper function to take care of side effects of Avidian 
  // movement that cannot be directly handled in cHardwareCPU.cc
  bool MoveOrganisms(cAvidaContext& ctx, int src_cell_id, int dest_cell_id, int avatar_cell);

  // Specialized functionality
  void Kaboom(cPopulationCell& in_cell, cAvidaContext& ctx, int distance=0); 
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
  
  //! Helper method that replaces a target deme with a given source deme using
  // the germ line flagged by the organisms. 
  void ReplaceDemeFlaggedGermline(cDeme& source_deme, cDeme& target_deme, cAvidaContext& ctx);
  
  //! Helper method that seeds a deme from the given genome.
  void SeedDeme(cDeme& deme, Genome& genome, Systematics::Source src, cAvidaContext& ctx); 

  //! Helper method that seeds a deme from the given genotype.
  void SeedDeme(cDeme& _deme, Systematics::GroupPtr bg, Systematics::Source src, cAvidaContext& ctx); 
  
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
  void PrintDemesMeritsData(); //@JJB**
  
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
  void ProcessPreUpdate();
  void UpdateResStats(cAvidaContext& ctx);
  void ProcessUpdateCellActions(cAvidaContext& ctx);

  // Clear all but a subset of cells...
  void SerialTransfer(int transfer_size, bool ignore_deads, cAvidaContext& ctx); 

  // Saving and loading...
  bool LoadParasiteGenotypeList(const cString& filename, cAvidaContext& ctx);
  bool LoadHostGenotypeList(const cString& filename, cAvidaContext& ctx);

  bool SavePopulation(const cString& filename, bool save_historic, bool save_group_info = false, bool save_avatars = false,
                      bool save_rebirth = false);
  bool SaveStructuredSystematicsGroup(const Systematics::RoleID& role, const cString& filename);
  bool LoadStructuredSystematicsGroup(cAvidaContext& ctx, const Systematics::RoleID& role, const cString& filename);
  bool LoadPopulation(const cString& filename, cAvidaContext& ctx, int cellid_offset=0, int lineage_offset=0,
                      bool load_groups = false, bool load_birth_cells = false, bool load_avatars = false, bool load_rebirth = false, bool load_parent_dat = false, int traceq = 0);
  bool SaveFlameData(const cString& filename);
  
  void SetMiniTraceQueue(Apto::Array<int, Apto::Smart> new_queue, const bool print_genomes, const bool print_reacs, const bool use_micro = false);
  void AppendMiniTraces(Apto::Array<int, Apto::Smart> new_queue, const bool print_genomes, const bool print_reacs, const bool use_micro = false);
  void LoadMiniTraceQ(const cString& filename, int orgs_per, bool print_genomes, bool print_reacs);
  Apto::Array<int, Apto::Smart> SetRandomTraceQ(int max_samples);
  Apto::Array<int, Apto::Smart> SetRandomPreyTraceQ(int max_samples);
  Apto::Array<int, Apto::Smart> SetRandomPredTraceQ(int max_samples);
  void SetNextPreyQ(int num_prey, bool print_genomes, bool print_reacs, bool use_micro);
  void SetNextPredQ(int num_pred, bool print_genomes, bool print_reacs, bool use_micro);
  Apto::Array<int, Apto::Smart> SetTraceQ(int save_dominants, int save_groups, int save_foragers, int orgs_per, int max_samples);
  const Apto::Array<int, Apto::Smart>& GetMiniTraceQueue() const { return minitrace_queue; }
  void AppendRecordReproQ(cOrganism* new_org);
  void SetTopNavQ();
  Apto::Array<cOrganism*, Apto::Smart>& GetTopNavQ() { return topnav_q; }
  
  int GetSize() const { return cell_array.GetSize(); }
  int GetWorldX() const { return world_x; }
  int GetWorldY() const { return world_y; }
  int GetNumDemes() const { return deme_array.GetSize(); }
  cDeme& GetDeme(int i) { return deme_array[i]; }

  cPopulationCell& GetCell(int in_num) { assert(in_num >=0); assert(in_num < cell_array.GetSize()); return cell_array[in_num]; }
  const Apto::Array<double>& GetResources(cAvidaContext& ctx) const { return resource_count.GetResources(ctx); }
  const Apto::Array<double>& GetCellResources(int cell_id, cAvidaContext& ctx) const { return resource_count.GetCellResources(cell_id, ctx); } 
  const Apto::Array<double>& GetFrozenResources(cAvidaContext& ctx, int cell_id) const { return resource_count.GetFrozenResources(ctx, cell_id); }
  double GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const { return resource_count.GetFrozenCellResVal(ctx, cell_id, res_id); }
  double GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const { return resource_count.GetCellResVal(ctx, cell_id, res_id); }
  const Apto::Array<double>& GetDemeResources(int deme_id, cAvidaContext& ctx) { return GetDeme(deme_id).GetDemeResourceCount().GetResources(ctx); }  
  const Apto::Array<double>& GetDemeCellResources(int deme_id, int cell_id, cAvidaContext& ctx) { return GetDeme(deme_id).GetDemeResourceCount().GetCellResources( GetDeme(deme_id).GetRelativeCellID(cell_id), ctx ); } 
  void TriggerDoUpdates(cAvidaContext& ctx) { resource_count.UpdateResources(ctx); }
  const Apto::Array< Apto::Array<int> >& GetCellIdLists() const { return resource_count.GetCellIdLists(); }

  int GetCurrPeakX(cAvidaContext& ctx, int res_id) const { return resource_count.GetCurrPeakX(ctx, res_id); } 
  int GetCurrPeakY(cAvidaContext& ctx, int res_id) const { return resource_count.GetCurrPeakY(ctx, res_id); } 
  int GetFrozenPeakX(cAvidaContext& ctx, int res_id) const { return resource_count.GetFrozenPeakX(ctx, res_id); } 
  int GetFrozenPeakY(cAvidaContext& ctx, int res_id) const { return resource_count.GetFrozenPeakY(ctx, res_id); } 
  Apto::Array<int>* GetWallCells(int res_id) { return resource_count.GetWallCells(res_id); }

  cBirthChamber& GetBirthChamber(int id) { (void) id; return birth_chamber; }

  void UpdateResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void UpdateResource(cAvidaContext& ctx, int id, double change);
  void UpdateCellResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int cell_id);
  void UpdateDemeCellResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int cell_id);
  
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
  void PrintPhenotypeData(const cString& filename);
  void PrintHostPhenotypeData(const cString& filename);
  void PrintParasitePhenotypeData(const cString& filename);
  void PrintPhenotypeStatus(const cString& filename);

  bool UpdateMerit(cAvidaContext& ctx, int cell_id, double new_merit);

  void AddBeginSleep(int cellID, int start_time);
  void AddEndSleep(int cellID, int end_time);
 
  const Apto::Array<pair<int,int>, Apto::Smart>& getCellSleepLog(int i) const { return sleep_log[i]; }

  // Trials and genetic algorithm @JEB
  void NewTrial(cAvidaContext& ctx);
  void CompeteOrganisms(cAvidaContext& ctx, int competition_type, int parents_survive);
  
  // Let users change environmental variables durning the run @BDB 22-Feb-2008
  void UpdateResourceCount(const int Verbosity, cWorld* world);        
  
  // Let users change Gradient Resource variables during the run JW
  void UpdateGradientCount(cAvidaContext& ctx, const int Verbosity, cWorld* world, const cString res_name);
  void UpdateGradientPlatInflow(const cString res_name, const double inflow);
  void UpdateGradientPlatOutflow(const cString res_name, const double outflow);
  void UpdateGradientConeInflow(const cString res_name, const double inflow);
  void UpdateGradientConeOutflow(const cString res_name, const double outflow);
  void UpdateGradientInflow(const cString res_name, const double inflow);
  void SetGradPlatVarInflow(cAvidaContext& ctx, const cString res_name, const double mean, const double variance, const int type);
  void SetPredatoryResource(const cString res_name, const double odds, const int juvsper, const double detection_prob);
  void SetProbabilisticResource(cAvidaContext& ctx, const cString res_name, const double initial, const double inflow, 
                                const double outflow, const double lambda, const double theta, const int x, const int y, const int count);
  void UpdateInflow(const cString& res_name, const double change);

  void ExecutePredatoryResource(cAvidaContext& ctx, const int cell_id, const double pred_odds, const int juvs_per, const bool hammer);
  void ExecuteDeadlyResource(cAvidaContext& ctx, const int cell_id, const double odds, const bool hammer);
  void ExecuteDamagingResource(cAvidaContext& ctx, const int cell_id, const double damage, const bool hammer);

  // Add an org to live org list
  void AddLiveOrg(cOrganism* org);  
  // Remove an org from live org list
  void RemoveLiveOrg(cOrganism* org); 
  const Apto::Array<cOrganism*, Apto::Smart>& GetLiveOrgList() const { return live_org_list; }
	
  // Adds an organism to a group  
  void JoinGroup(cOrganism* org, int group_id);
  void MakeGroup(cOrganism* org);
  // Removes an organism from a group 
  void LeaveGroup(cOrganism* org, int group_id);

  //Kill random member of the group (but not self!!!) 
  void KillGroupMember(cAvidaContext& ctx, int group_id, cOrganism* org);
  void AttackFacedOrg(cAvidaContext& ctx, int loser);
  void KillRandPred(cAvidaContext& ctx, cOrganism* org);
  void KillRandPrey(cAvidaContext& ctx, cOrganism* org);
  cOrganism* GetRandPrey(cAvidaContext& ctx, cOrganism* org);
  // Identifies the number of organisms in a group
  int NumberOfOrganismsInGroup(int group_id);
  int NumberGroupFemales(int group_id);
  int NumberGroupMales(int group_id);
  int NumberGroupJuvs(int group_id);
  void ChangeGroupMatingTypes(cOrganism* org, int group_id, int old_type, int new_type);
  // Get the group information
  map<int, int> GetFormedGroups() { return m_groups; }
  Apto::Array<int> GetFormedGroupArray();

  // -------- Tolerance support --------
  int CalcGroupToleranceImmigrants(int group_id, int mating_type = -1);
  int CalcGroupToleranceOffspring(cOrganism* parent_organism);
  double CalcGroupOddsImmigrants(int group_id, int mating_type  = -1);
  bool AttemptImmigrateGroup(cAvidaContext& ctx, int group_id, cOrganism* org);
  double CalcGroupOddsOffspring(int group_id);
  double CalcGroupOddsOffspring(cOrganism* parent);
  bool AttemptOffspringParentGroup(cAvidaContext& ctx, cOrganism* parent, cOrganism* offspring);
  double CalcGroupAveImmigrants(int group_id, int mating_type = -1);
  double CalcGroupSDevImmigrants(int group_id, int mating_type = -1);
  double CalcGroupAveOwn(int group_id);
  double CalcGroupSDevOwn(int group_id);
  double CalcGroupAveOthers(int group_id);
  double CalcGroupSDevOthers(int group_id);
  int& GetGroupIntolerances(int group_id, int tol_num, int mating_type);

  // -------- HGT support --------
  //! Modify current level of the HGT resource.
  void AdjustHGTResource(cAvidaContext& ctx, double delta);

  // -------- Population mixing support --------
  //! Mix all organisms in the population.
  void MixPopulation(cAvidaContext& ctx); 

private:
  void SetupCellGrid();
  void ClearCellGrid();
  void BuildTimeSlicer(); // Build the schedule object
  
  // Methods to place offspring in the population.
  cPopulationCell& PositionOffspring(cPopulationCell& parent_cell, cAvidaContext& ctx, bool parent_ok = true); 
  void PositionAge(cPopulationCell& parent_cell, tList<cPopulationCell>& found_list, bool parent_ok);
  void PositionMerit(cPopulationCell & parent_cell, tList<cPopulationCell>& found_list, bool parent_ok);
  void PositionEnergyUsed(cPopulationCell & parent_cell, tList<cPopulationCell>& found_list, bool parent_ok);
  cPopulationCell& PositionDemeMigration(cPopulationCell& parent_cell, bool parent_ok = true);
  cPopulationCell& PositionDemeRandom(int deme_id, cPopulationCell& parent_cell, bool parent_ok = true);
  int UpdateEmptyCellIDArray(int deme_id = -1);
  Apto::Array<int>& GetEmptyCellIDArray() { return empty_cell_id_array; }
  void FindEmptyCell(tList<cPopulationCell>& cell_list, tList<cPopulationCell>& found_list);
  int FindRandEmptyCell(cAvidaContext& ctx);
  
  // Update statistics collecting...
  void UpdateDemeStats(cAvidaContext& ctx); 
  void UpdateOrganismStats(cAvidaContext& ctx); 
  void UpdateFTOrgStats(cAvidaContext& ctx); 
  void UpdateMaleFemaleOrgStats(cAvidaContext& ctx);
  
  void InjectClone(int cell_id, cOrganism& orig_org, Systematics::Source src);
  void CompeteOrganisms_ConstructOffspring(int cell_id, cOrganism& parent);
  
  //! Helper method that adds a founder organism to a deme, and sets up its phenotype
  void SeedDeme_InjectDemeFounder(int _cell_id, Systematics::GroupPtr bg, cAvidaContext& ctx, cPhenotype* _phenotype = NULL, int lineage_label=0, bool reset=false); 
  
  void CCladeSetupOrganism(cOrganism* organism); 
	
  // Must be called to activate *any* organism in the population.
  bool ActivateOrganism(cAvidaContext& ctx, cOrganism* in_organism, cPopulationCell& target_cell, bool assign_group = true, bool is_inject = false);
  
  void TestForMiniTrace(cOrganism* in_organism);
  void SetupMiniTrace(cOrganism* in_organism);
  void PrintMiniTraceGenome(cOrganism* in_organism, cString& filename);
  void PrintMiniTraceSuccess(const int exec_success);

  int PlaceAvatar(cAvidaContext& ctx, cOrganism* parent);
  
  inline void AdjustSchedule(const cPopulationCell& cell, const cMerit& merit);
  
  bool LoadGenotypeList(const cString& filename, cAvidaContext& ctx, Apto::Array<GeneticRepresentationPtr>& list_obj);
};

#endif

/*
 *  cDeme.h
 *  Avida
 *
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#ifndef cDeme_h
#define cDeme_h

#include <set>
#include <vector>

#include "avida/systematics/Types.h"
#include "cDemeCellEvent.h"
#include "cGermline.h"
#include "cPhenotype.h"
#include "cMerit.h"
#include "cDemeNetwork.h"
#include "tBuffer.h"
#include "cResourceCount.h"
#include "cStringList.h"
#include "cDoubleSum.h"

class cResource;
class cWorld;
class cPopulationCell;
class cOrganism;
class cOrgMovementPredicate;
class cOrgMessagePredicate;
class cDemePredicate;
class cReactionResult; //@JJB**
class cTaskState; //@JJB**

/*! Demes are groups of cells in the population that are somehow bound together
as a unit.  The deme object is used from within cPopulation to manage these 
groups. */

class cDeme
{
private:
  cWorld* m_world;
  int _id; //!< ID of this deme (position in cPopulation::deme_array).
  Apto::Array<int> cell_ids;
  int width; //!< Width of this deme.

  bool replicateDeme;
  bool treatable;
  std::set<int> treatment_ages;

// The following should be moved to cDemePhenotype / cPopulationPhenotype
  int cur_birth_count; //!< Number of organisms that have been born into this deme since reset.
  int last_birth_count;
  int cur_org_count; //!< Number of organisms are currently in this deme.
  int last_org_count; 
  int injected_count; //<! Number of organisms that have been injected into this deme
  int birth_count_perslot;
  int _age; //!< Age of this deme, in updates.
  int generation; //!< Generation of this deme
  double total_org_energy; //! amount of energy in organisms in this deme
  int time_used; //!< number of cpu cycles this deme has used
  int gestation_time; // Time used during last generation
  double cur_normalized_time_used; // normalized by merit and number of orgs
  double last_normalized_time_used; 
  unsigned int MSG_sendFailed;
  unsigned int MSG_dropped;
  unsigned int MSG_SuccessfullySent;
  double energyInjectedIntoOrganisms; //! total amount of energy injected into seed organisms
  double energyRemainingInDemeAtReplication; //! total amount of energy remaining in deme when deme was last replicated.
  double total_energy_testament; //! total amount of energy from suicide organisms for offspring deme
  int eventsTotal;
  unsigned int eventsKilled;
  unsigned int eventsKilledThisSlot;
  unsigned int eventKillAttempts;
  unsigned int eventKillAttemptsThisSlot;
  unsigned int consecutiveSuccessfulEventPeriods;
  int sleeping_count; //!< Number of organisms currently sleeping
  cDoubleSum energyUsage;
  
  double total_energy_donated;
  double total_energy_received;
  double total_energy_applied;
  
  Apto::Array<int> cur_task_exe_count;
  Apto::Array<int> cur_reaction_count;
  Apto::Array<int> last_task_exe_count;
  Apto::Array<int> last_reaction_count;
  
  Apto::Array<int> cur_org_task_count;
  Apto::Array<int> cur_org_task_exe_count;
  Apto::Array<int> cur_org_reaction_count;
  Apto::Array<int> last_org_task_count;
  Apto::Array<int> last_org_task_exe_count;
  Apto::Array<int> last_org_reaction_count;
  
  double avg_founder_generation;  //Average generation of current founders                                    
  double generations_per_lifetime; //Generations between current founders and founders of parent  

  // End of phenotypic traits
  
  cGermline _germline; //!< The germline for this deme, if used.

  cDeme(const cDeme&); // @not_implemented
  
  cResourceCount deme_resource_count; //!< Resources available to the deme
  Apto::Array<int> energy_res_ids; //!< IDs of energy resources
  
  Apto::Array<cDemeCellEvent, Apto::Smart> cell_events;
  std::vector<std::pair<int, int> > event_slot_end_points; // (slot end point, slot flow rate)
  
  int         m_germline_genotype_id; // Genotype id of germline (if in use)
  Apto::Array<int> m_founder_genotype_ids; // List of genotype ids used to found deme.
                                      // Keep a lease on these genotypes for the deme's lifetime.
  Apto::Array<cPhenotype> m_founder_phenotypes; // List of phenotypes of founder organsisms
                                      
  cMerit _current_merit; //!< Deme merit applied to all organisms living in this deme.
  cMerit _next_merit; //!< Deme merit that will be inherited upon deme replication.

  Apto::Array<cDemePredicate*, Apto::Smart> deme_pred_list; // Deme Predicates
  Apto::Array<cOrgMessagePredicate*, Apto::Smart> message_pred_list; // Message Predicates
  Apto::Array<cOrgMovementPredicate*, Apto::Smart> movement_pred_list;  // Movement Predicates
	
	// For the points infrastructure
	double points; 
	unsigned int migrations_out; 
	unsigned int migrations_in;
	unsigned int suicides;
	
public:
	//! Constructor.
  cDeme();
	
	//! Destructor.
  ~cDeme();
  
  cDeme& operator=(const cDeme&); //@JJB**
  void Setup(int id, const Apto::Array<int>& in_cells, int in_width = -1, cWorld* world = NULL);

  int GetID() const { return _id; }
  int GetSize() const { return cell_ids.GetSize(); }
  int GetCellID(int pos) const { return cell_ids[pos]; }
  int GetCellID(int x, int y) const;
  int GetDemeID() const { return _id; }
  //! Returns an (x,y) pair for the position of the passed-in cell ID.
  std::pair<int, int> GetCellPosition(int cellid) const;
  cPopulationCell& GetCell(int pos) const;
  cPopulationCell& GetCell(int x, int y) const;
  cOrganism* GetOrganism(int pos) const;
  
  std::vector<int> GetGenotypeIDs();

  int GetWidth() const { return width; }
  int GetHeight() const { return cell_ids.GetSize() / width; }

  void Reset(cAvidaContext& ctx, bool resetResources = true, double deme_energy = 0.0); //! used to pass energy to offspring deme
  void DivideReset(cAvidaContext& ctx, cDeme& parent_deme, bool resetResources = true, double deme_energy = 0.0);

  //! Kills all organisms currently in this deme.
  void KillAll(cAvidaContext& ctx);

  void UpdateStats();
  
  int GetBirthCount() const { return cur_birth_count; }
  int GetLastBirthCount() const { return last_birth_count; }
  void IncBirthCount() { cur_birth_count++; birth_count_perslot++;}

  int GetOrgCount() const { return cur_org_count; }
  int GetLastOrgCount() const { return last_org_count; }

  double GetDensity() const { return static_cast<double>(cur_org_count) / static_cast<double>(GetSize()); }
  int GetNumOrgsWithOpinion() const;
	
  void IncOrgCount() { cur_org_count++; }
  void DecOrgCount() { cur_org_count--; }

  int GetSleepingCount() const { return sleeping_count; }
  void IncSleepingCount() { sleeping_count++; }
  void DecSleepingCount() { sleeping_count--; }
  
  int GetGeneration() const { return generation; }

  int GetInjectedCount() const { return injected_count; }
  void IncInjectedCount() { injected_count++; }

  bool IsEmpty() const { return cur_org_count == 0; }
  bool IsFull() const { return cur_org_count == cell_ids.GetSize(); }

  bool TestReplication() const { return replicateDeme; }
  void ReplicateDeme() { replicateDeme = true; }

  bool isTreatable() const { return treatable; }
  void setTreatable(bool value) { treatable = value; }
  void AddTreatmentAge(const int age) { treatment_ages.insert(age); }
  bool IsTreatableAtAge(const int age);
  bool IsTreatableNow() { return IsTreatableAtAge(_age); }
  std::set<int> GetTreatmentAges() const { return treatment_ages; }

  int GetSlotFlowRate() const;
  int GetEventsTotal() const { return eventsTotal; }
  int GetEventsKilled() const { return eventsKilled; }
  int GetEventsKilledThisSlot() const { return eventsKilledThisSlot;}
  int GetEventKillAttempts() const { return eventKillAttempts; }
  int GetEventKillAttemptsThisSlot() const { return eventKillAttemptsThisSlot; }
  int GetConsecutiveSuccessfulEventPeriods() const { return consecutiveSuccessfulEventPeriods;}
  
  // -= Germline =-
  //! Returns this deme's germline.
  cGermline& GetGermline() { return _germline; }
  //! Replaces this deme's germline.
  void ReplaceGermline(const cGermline& germline);
  
  //! Update this deme's merit by rotating the heritable merit to the current merit.
  void UpdateDemeMerit();
  //! Update this deme's merit from the given source; merit will be applied to organisms now.
  void UpdateDemeMerit(cDeme& source);
  //! Update the heritable merit; will be applied to this deme and it's offspring upon replication.
  void UpdateHeritableDemeMerit(double value) { _next_merit = value; }
  //! Retrieve this deme's current merit; to be applied to organisms living in this deme now.
  const cMerit& GetDemeMerit() const { return _current_merit; }
  //! Retrieve this deme's heritable merit.
  const cMerit& GetHeritableDemeMerit() const { return _next_merit; }


  void AddCurTask(int task_num) { cur_task_exe_count[task_num]++; }
  void AddCurReaction (int reaction_num) { cur_reaction_count[reaction_num]++; }

  const Apto::Array<int>& GetCurTaskExeCount() const { return cur_task_exe_count; } //**
  const Apto::Array<int>& GetLastTaskExeCount() const { return last_task_exe_count; } //**
  const Apto::Array<int>& GetCurReactionCount() const { return cur_reaction_count; } //**
  const Apto::Array<int>& GetLastReactionCount() const { return last_reaction_count; } //**

  const Apto::Array<int>& GetCurOrgTaskCount() const { return cur_org_task_count; }
  const Apto::Array<int>& GetLastOrgTaskCount() const { return last_org_task_count; }
  const Apto::Array<int>& GetCurOrgTaskExeCount() const { return cur_org_task_exe_count; }
  const Apto::Array<int>& GetLastOrgTaskExeCount() const { return last_org_task_exe_count; }
  const Apto::Array<int>& GetCurOrgReactionCount() const { return cur_org_reaction_count; }
  const Apto::Array<int>& GetLastOrgReactionCount() const { return last_org_reaction_count; }

  bool HasDemeMerit() const { return _current_merit.GetDouble() != 1.0; }

  // -= Update support =-
  void ProcessPreUpdate(); 
  //! Called once, at the end of every update.
  void ProcessUpdate(cAvidaContext& ctx); 
  //! Returns the age of this deme in updates, where age is defined as the number of updates since the last time Reset() was called.
  int GetAge() const { return _age; }
  //! Called when an organism living in a cell in this deme is about to be killed.
  void OrganismDeath(cPopulationCell& cell);
  
  const cResourceCount& GetDemeResourceCount() const { return deme_resource_count; }
  cResourceCount& GetDemeResources() { return deme_resource_count; }
  void SetResource(cAvidaContext& ctx, int id, double new_level) { deme_resource_count.Set(ctx, id, new_level); }
  double GetSpatialResource(int rel_cellid, int resource_id, cAvidaContext& ctx) const;
  void AdjustSpatialResource(cAvidaContext& ctx, int rel_cellid, int resource_id, double amount);
  void AdjustResource(cAvidaContext& ctx, int resource_id, double amount);
  void SetDemeResourceCount(const cResourceCount in_res) { deme_resource_count = in_res; }
  void ResizeSpatialGrids(const int in_x, const int in_y) { deme_resource_count.ResizeSpatialGrids(in_x, in_y); }
  void ModifyDemeResCount(cAvidaContext& ctx, const Apto::Array<double> & res_change, const int absolute_cell_id);
  double GetCellEnergy(int absolute_cell_id, cAvidaContext& ctx) const; 
  double GetAndClearCellEnergy(int absolute_cell_id, cAvidaContext& ctx); 
  void GiveBackCellEnergy(int absolute_cell_id, double value, cAvidaContext& ctx); 
  void SetupDemeRes(int id, cResource * res, int verbosity, cWorld* world);                 
  void UpdateDemeRes(cAvidaContext& ctx) { deme_resource_count.GetResources(ctx); } 
  void Update(double time_step) { deme_resource_count.Update(time_step); }
  int GetRelativeCellID(int absolute_cell_id) const { return absolute_cell_id % GetSize(); } //!< assumes all demes are the same size
  int GetAbsoluteCellID(int relative_cell_id) const { return relative_cell_id + (_id * GetSize()); } //!< assumes all demes are the same size
	
  void SetCellEventGradient(int x1, int y1, int x2, int y2, int delay, int duration, bool static_pos, int time_to_live);
  int GetNumEvents();
  void SetCellEvent(int x1, int y1, int x2, int y2, int delay, int duration, bool static_position, int total_events);
  void SetCellEventSlots(int x1, int y1, int x2, int y2, int delay, int duration, 
                         bool static_position, int m_total_slots, int m_total_events_per_slot_max, 
                         int m_total_events_per_slot_min, int m_tolal_event_flow_levels);

  bool KillCellEvent(const int eventID);
  cDemeCellEvent* GetCellEvent(const int i) { return &cell_events[i]; };
  
  double CalculateTotalEnergy(cAvidaContext& ctx) const; 
  double CalculateTotalInitialEnergyResources() const;
  double GetEnergyInjectedIntoOrganisms() const { return energyInjectedIntoOrganisms; }
  void SetEnergyInjectedIntoOrganisms(double energy) { energyInjectedIntoOrganisms = energy; }
  double GetEnergyRemainingInDemeAtReplication() const { return energyRemainingInDemeAtReplication; }
  void SetEnergyRemainingInDemeAtReplication(double energy) { energyRemainingInDemeAtReplication = energy; }
  double GetTotalEnergyTestament() { return total_energy_testament; }
  void IncreaseTotalEnergyTestament(double increment) { total_energy_testament += increment; }
  
  void IncTimeUsed(double merit) 
    { time_used++; cur_normalized_time_used += 1.0/merit/(double)cur_org_count; }
  int GetTimeUsed() { return time_used; }
  int GetGestationTime() { return gestation_time; }
  double GetNormalizedTimeUsed() { return cur_normalized_time_used; }
  double GetLastNormalizedTimeUsed() { return last_normalized_time_used; }

  // --- Founder list management --- //
  void ClearFounders();
  void AddFounder(Systematics::GroupPtr bg, cPhenotype * _in_phenotype = NULL);
  Apto::Array<int>& GetFounderGenotypeIDs() { return m_founder_genotype_ids; }
  Apto::Array<cPhenotype>& GetFounderPhenotypes() { return m_founder_phenotypes; }
  double GetAvgFounderGeneration() { return avg_founder_generation; }        
  void UpdateGenerationsPerLifetime(double old_avg_founder_generation, Apto::Array<cPhenotype>& new_founder_phenotypes);
  double GetGenerationsPerLifetime() { return generations_per_lifetime; }  

  // --- Germline management --- //
  void ReplaceGermline(Systematics::GroupPtr bg);
  int GetGermlineGenotypeID() { return m_germline_genotype_id; }

  // --- Deme/Message/Movement predicates --- //
  bool DemePredSatisfiedPreviously();
  bool MsgPredSatisfiedPreviously();
  bool MovPredSatisfiedPreviously();
  int GetNumDemePredicates();
  int GetNumMessagePredicates();
  int GetNumMovementPredicates();
  cDemePredicate* GetDemePredicate(int i);
  cOrgMessagePredicate* GetMsgPredicate(int i);
  cOrgMovementPredicate* GetMovPredicate(int i);

  void AddDemeResourceThresholdPredicate(cString resourceName, cString comparisonOperator, double threasholdValue);
  void AddEventReceivedCenterPred(int times);
  void AddEventReceivedLeftSidePred(int times);
  void AddEventMoveCenterPred(int times);
  void AddEventMoveBetweenTargetsPred(int times);
  void AddEventMigrateToTargetsPred(int times);
  void AddEventEventNUniqueIndividualsMovedIntoTargetPred(int times);
	
  // --- Messaging stats --- //
  void MessageSuccessfullySent() { ++MSG_SuccessfullySent; }
  void messageDropped() { ++MSG_dropped; }
  void messageSendFailed() { ++MSG_sendFailed; }
  unsigned int GetMessageSuccessfullySent() { return MSG_SuccessfullySent; }
  unsigned int GetMessageDropped() { return MSG_dropped; }
  unsigned int GetMessageSendFailed() { return MSG_sendFailed; }
  
  // --- Pheromones --- //
  void AddPheromone(int absolute_cell_id, double value, cAvidaContext& ctx); 
	
  // --- Points --- //
  double GetNumberOfPoints() { return points; }
  void AddNumberOfPoints(double num_points) { points += num_points; }
  void SubtractNumberOfPoints(double num_points) { if (num_points > points) points = 0; }
  int GetMigrationsOut()  { return migrations_out; }
  int GetMigrationsIn()  { return migrations_in; }
  int GetSuicides()  { return suicides; }
  void AddMigrationOut() { migrations_out++; }
  void AddMigrationIn() { migrations_in++; }
  void AddSuicide() { suicides++; }
  void ClearMigrationOut() { migrations_out = 0; }
  void ClearMigrationIn() { migrations_in = 0; }
  void ClearSuicides() { suicides = 0; }
  
  // --- Energy Sharing --- //
  double GetEnergyDonated() const { return total_energy_donated; }
  double GetEnergyReceived() const { return total_energy_received; }
  double GetEnergyApplied() const { return total_energy_applied; }
  void IncreaseEnergyDonated(double amount) { assert(amount >=0); total_energy_donated += amount; }
  void IncreaseEnergyReceived(double amount) { assert(amount >=0); total_energy_received += amount; }
  void IncreaseEnergyApplied(double amount) { assert(amount >=0); total_energy_applied += amount; }
	
  // -= Network creation support =-
private:
  //! Lazily-initialized pointer to the network creation support struct.
  cDemeNetwork* m_network;

  //! Initialize network creation support.
  inline void InitNetworkCreation() { if(!m_network) m_network = cDemeNetwork::DemeNetworkFactory(m_world, *this); }
  //! Test for initialization of the network.
  inline bool IsNetworkInitialized() { return m_network != 0; }
public:
  //! Retrieve this deme's network.
  cDemeNetwork& GetNetwork();

  // -------- Deme Input and Output --------
private:
  int m_input_pointer;
  Apto::Array<int> m_inputs;
  tBuffer<int> m_input_buf;
  tBuffer<int> m_output_buf;
  Apto::Map<void*, cTaskState*> m_task_states;
  cReactionResult* m_reaction_result;
  Apto::Array<int> m_task_count;               // Total times each task was performed (resetable during the life of the deme)
  Apto::Array<int> m_last_task_count;
  Apto::Array<int> m_reaction_count;
  Apto::Array<double> m_cur_reaction_add_reward;
  double m_cur_bonus;
  cMerit m_cur_merit;
public:
  bool HasDoneInput() { return (m_input_buf.GetNumStored() > 0); }
  bool HasDoneOutput() { return (m_input_buf.GetNumStored() > 0); }
  void ResetInputs(cAvidaContext& ctx);
  void ResetInput() { m_input_pointer = 0; m_input_buf.Clear(); }
  int GetNextDemeInput(cAvidaContext& ctx);
  void DoDemeInput(int value);
  void DoDemeOutput(cAvidaContext& ctx, int value);
  double GetCurBonus() const { return m_cur_bonus; }
  void ResetMeritBonus() { m_cur_bonus = m_world->GetConfig().DEFAULT_BONUS.Get(); }
  const cMerit& GetCurMerit() { return m_cur_merit; }
  void UpdateCurMerit();
  cMerit CalcCurMerit();
  const Apto::Array<int>& GetTaskCount() const { return m_task_count; }
  const Apto::Array<int>& GetReactionCount() const { return m_reaction_count; }


	// --- Division of Labor --- //
public: 	
	int MinNumTimesReactionPerformed();
	void IncNumSwitchingPenalties(int pen_count) { m_switch_penalties += pen_count; }
	int GetNumSwitchingPenalties() { return m_switch_penalties; }
	double GetShannonMutualInformation();
	double GetNumOrgsPerformedReaction();
	double GetTotalResourceAmountConsumed() const;  
	void AddResourcesConsumed(double amt){	m_total_res_consumed += amt; }
	void UpdateShannon(cPopulationCell& cell);
	void UpdateShannonAll();
  double GetPercentReproductives();
	void ClearShannonInformationStats();
  std::pair<double, double> GetAveVarGermMut();
  std::pair<double, double> GetAveVarSomaMut();
  std::pair<double, double> GetGermlineNumPercent();
  std::pair<double, double> GetAveVarGermWorkLoad();
  std::pair<double, double> GetAveVarSomaWorkLoad();
  std::pair<double, double> GetAveVarWorkLoad();

	void ClearTotalResourceAmountConsumed() {m_total_res_consumed = 0;}
    
private:
  double m_total_res_consumed; //! Amount of resources consumed by deme.
  //! get total amount of resources used
  int m_switch_penalties; //! number of task switching penalties accumulated
  std::vector< std::vector<double> > m_shannon_matrix;
  int m_num_active; // number of active organisms in the lifetime of the deme
  int m_num_reproductives; // number of organisms that reproduced during the lifetime of the deme
	
	
};

#endif

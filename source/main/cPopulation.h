/*
 *  cPopulation.h
 *  Avida
 *
 *  Called "population.hh" prior to 12/5/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#include <fstream>

#ifndef cBirthChamber_h
#include "cBirthChamber.h"
#endif
#ifndef cDeme_h
#include "cDeme.h"
#endif
#ifndef cOrgInterface_h
#include "cOrgInterface.h"
#endif
#ifndef cPopulationInterface_h
#include "cPopulationInterface.h"
#endif
#ifndef cResourceCount_h
#include "cResourceCount.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef cWorld_h
#include "cWorld.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif
#ifndef tVector_h
#include "tVector.h"
#endif

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif

#include "cInstSet.h"

class cAvidaContext;
class cCodeLabel;
class cChangeList;
class cEnvironment;
class cGenome;
class cGenotype;
class cLineage;
class cOrganism;
class cPopulationCell;
class cSchedule;
class cSaleItem;


class cPopulation
{
#if USE_tMemTrack
  tMemTrack<cPopulation> mt;
#endif
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

  ///////////////// Private Methods ////////////////////
  void BuildTimeSlicer(cChangeList* change_list); // Build the schedule object

  // Methods to place offspring in the population.
  cPopulationCell& PositionChild(cPopulationCell& parent_cell, bool parent_ok = true);
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
  void UpdateGenotypeStats();
  void UpdateSpeciesStats();
  void UpdateDominantStats();
  void UpdateDominantParaStats();

  /**
   * Attention: InjectGenotype does *not* add the genotype to the archive.
   * It assumes that's where you got the genotype from.
   **/
  void InjectGenotype(int cell_id, cGenotype* genotype);
  void InjectGenome(int cell_id, const cGenome& genome, int lineage_label);
  void InjectClone(int cell_id, cOrganism& orig_org);
  void InjectChild(int cell_id, cOrganism& parent);

  void LineageSetupOrganism(cOrganism* organism, cLineage* lineage, int lin_label, cGenotype* parent_genotype = NULL);
  void CCladeSetupOrganism(cOrganism* organism); 
	
  // Must be called to activate *any* organism in the population.
  void ActivateOrganism(cAvidaContext& ctx, cOrganism* in_organism, cPopulationCell& target_cell);
  
  inline void AdjustSchedule(const cPopulationCell& cell, const cMerit& merit);
  

  cPopulation(); // @not_implemented
  cPopulation(const cPopulation&); // @not_implemented
  cPopulation& operator=(const cPopulation&); // @not_implemented
  
public:
  cPopulation(cWorld* world);
  ~cPopulation();

  void InitiatePop();

  // Activate the offspring of an organism in the population
  bool ActivateOffspring(cAvidaContext& ctx, const cMetaGenome& offspring_genome, cOrganism& parent_organism);
  bool ActivateParasite(cOrganism& parent, const cCodeLabel& label, const cGenome& injected_code);
  
  // Inject an organism from the outside world.
  void Inject(const cGenome& genome, int cell_id = -1, double merit = -1, int lineage_label = 0,
              double neutral_metric = 0);
  void InjectParasite(const cCodeLabel& label, const cGenome& injected_code, int cell_id);
  
  // Deactivate an organism in the population (required for deactivations)
  void KillOrganism(cPopulationCell& in_cell);
  
  // @WRE 2007/07/05 Helper function to take care of side effects of Avidian 
  // movement that cannot be directly handled in cHardwareCPU.cc
  void MoveOrganisms(cAvidaContext& ctx, cPopulationCell& src_cell, cPopulationCell& dest_cell);

  // Specialized functionality
  void Kaboom(cPopulationCell& in_cell, int distance=0);
  void AddSellValue(const int data, const int label, const int sell_price, const int org_id, const int cell_id);
  int BuyValue(const int label, const int buy_price, const int cell_id);
  void SwapCells(cPopulationCell & cell1, cPopulationCell & cell2);

  // Deme-related methods
  //! Compete all demes with each other based on the given competition type.
  void CompeteDemes(int competition_type);
  
  //! Compete all demes with each other based on the given vector of fitness values.
  void CompeteDemes(const std::vector<double>& fitness);

  //! Replicate all demes based on the given replication trigger.
  void ReplicateDemes(int rep_trigger);

  //! Helper method to replicate deme
  void ReplicateDeme(cDeme & source_deme);

  //! Helper method that replaces a target deme with the given source deme.
  void ReplaceDeme(cDeme& source_deme, cDeme& target_deme);
  
  //! Helper method that seeds a deme from the given genome.
  void SeedDeme(cDeme& deme, cGenome& genome);

  //! Helper method that seeds a deme from the given genotype.
  void SeedDeme(cDeme& _deme, cGenotype& _genotype);
  
  //! Helper method that seeds a target deme from the organisms in the source deme.
  bool SeedDeme(cDeme& source_deme, cDeme& target_deme);

  //! Helper method that adds a founder organism to a deme, and sets up its phenotype
  void InjectDemeFounder(int _cell_id, cGenotype& _genotype, cPhenotype* _phenotype = NULL);
  
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
  void CalcUpdateStats();

  // Clear all but a subset of cells...
  void SerialTransfer(int transfer_size, bool ignore_deads);

  // Saving and loading...
  bool SaveClone(std::ofstream& fp);
  bool LoadClone(std::ifstream& fp);
  bool LoadDumpFile(cString filename, int update);
  bool DumpMemorySummary(std::ofstream& fp);

  bool OK();

  int GetSize() const { return cell_array.GetSize(); }
  int GetWorldX() const { return world_x; }
  int GetWorldY() const { return world_y; }
  int GetNumDemes() const { return deme_array.GetSize(); }
  cDeme& GetDeme(int i) { return deme_array[i]; }

  cPopulationCell& GetCell(int in_num);
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
};


#ifdef ENABLE_UNIT_TESTS
namespace nPopulation {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

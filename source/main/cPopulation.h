/*
 *  cPopulation.h
 *  Avida
 *
 *  Called "population.hh" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif


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
  cResourceCount resource_count;       // Global resources available
  cBirthChamber birth_chamber;         // Global birth chamber.
  tArray<tList<cSaleItem> > market;   // list of lists of items for sale, each list goes with 1 label

  // Data Tracking...
  tList<cPopulationCell> reaper_queue; // Death order in some mass-action runs

  // Default organism setups...
  cEnvironment & environment;          // Physics & Chemestry description

  // Other data...
  int world_x;                         // Structured population width.
  int world_y;                         // Structured population
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
  void FindEmptyCell(tList<cPopulationCell>& cell_list, tList<cPopulationCell>& found_list);

  // Update statistics collecting...
  void UpdateOrganismStats();
  void UpdateGenotypeStats();
  void UpdateSpeciesStats();
  void UpdateDominantStats();
  void UpdateDominantParaStats();

  /**
   * Attention: InjectGenotype does *not* add the genotype to the archive.
   * It assumes thats where you got the genotype from.
   **/
  void InjectGenotype(int cell_id, cGenotype* genotype);
  void InjectGenome(int cell_id, const cGenome& genome, int lineage_label);
  void InjectClone(int cell_id, cOrganism& orig_org);

  void LineageSetupOrganism(cOrganism* organism, cLineage* lineage, int lin_label, cGenotype* parent_genotype = NULL);

  // Must be called to activate *any* organism in the population.
  void ActivateOrganism(cAvidaContext& ctx, cOrganism* in_organism, cPopulationCell& target_cell);

  cPopulation(); // @not_implemented
  cPopulation(const cPopulation&); // @not_implemented
  cPopulation& operator=(const cPopulation&); // @not_implemented
  
public:
  cPopulation(cWorld* world);
  ~cPopulation();

  // Extra Setup...
  bool SetupDemes();

  // Activate the offspring of an organism in the population
  bool ActivateOffspring(cAvidaContext& ctx, cGenome& child_genome, cOrganism& parent_organism);
  bool ActivateParasite(cOrganism& parent, const cGenome& injected_code);
  
  // Inject an organism from the outside world.
  void Inject(const cGenome& genome, int cell_id = -1, double merit = -1, int lineage_label = 0,
              double neutral_metric = 0);
  void InjectParasite(const cCodeLabel& label, const cGenome& injected_code, int cell_id);
  
  // Deactivate an organism in the population (required for deactivations)
  void KillOrganism(cPopulationCell& in_cell);
  void Kaboom(cPopulationCell& in_cell, int distance=0);
  void AddSellValue(const int data, const int label, const int sell_price, const int org_id, const int cell_id);
  int BuyValue(const int label, const int buy_price, const int cell_id);

  // Deme-related methods
  void CompeteDemes(int competition_type);
  void ReplicateDemes(int rep_trigger);
  void DivideDemes();
  void ResetDemes();
  void CopyDeme(int deme1_id, int deme2_id);
  void SpawnDeme(int deme1_id, int deme2_id=-1);
  void PrintDemeStats();

  // Process a single organism one instruction...
  int ScheduleOrganism();          // Determine next organism to be processed.
  void ProcessStep(cAvidaContext& ctx, double step_size, int cell_id);
  void ProcessStep(cAvidaContext& ctx, double step_size) { ProcessStep(ctx, step_size, ScheduleOrganism()); }

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

  int GetSize() { return cell_array.GetSize(); }
  int GetWorldX() { return world_x; }
  int GetWorldY() { return world_y; }
  int GetNumDemes() { return deme_array.GetSize(); }

  cPopulationCell& GetCell(int in_num);
  const tArray<double>& GetResources() const { return resource_count.GetResources(); }
  const tArray<double>& GetCellResources(int cell_id) const { return resource_count.GetCellResources(cell_id); }
  cBirthChamber& GetBirthChamber(int id) { (void) id; return birth_chamber; }

  void UpdateResources(const tArray<double>& res_change);
  void UpdateResource(int id, double change);
  void UpdateCellResources(const tArray<double>& res_change, const int cell_id);
  void SetResource(int id, double new_level);
  double GetResource(int id) const { return resource_count.Get(id); }
  cResourceCount& GetResourceCount() { return resource_count; }

  cEnvironment& GetEnvironment() { return environment; }
  int GetNumOrganisms() { return num_organisms; }

  bool GetSyncEvents() { return sync_events; }
  void SetSyncEvents(bool _in) { sync_events = _in; }
  void PrintPhenotypeData(const cString& filename);
  void PrintPhenotypeStatus(const cString& filename);

  bool UpdateMerit(int cell_id, double new_merit);

  void SetChangeList(cChangeList* change_list);
  cChangeList* GetChangeList();
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

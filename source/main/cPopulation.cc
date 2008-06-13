/*
 *  cPopulation.cc
 *  Avida
 *
 *  Called "population.cc" prior to 12/5/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

#include "cPopulation.h"

#include "cAvidaContext.h"
#include "cChangeList.h"
#include "cClassificationManager.h"
#include "cCodeLabel.h"
#include "cConstBurstSchedule.h"
#include "cConstSchedule.h"
#include "cDataFile.h"
#include "cDemeProbSchedule.h"
#include "cEnvironment.h"
#include "functions.h"
#include "cGenome.h"
#include "cGenomeUtil.h"
#include "cGenotype.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInitFile.h"
#include "cInjectGenotype.h"
#include "cInstSet.h"
#include "cIntegratedSchedule.h"
#include "cLineage.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulationCell.h"
#include "cProbSchedule.h"
#include "cProbDemeProbSchedule.h"
#include "cResource.h"
#include "cResourceCount.h"
#include "cSaleItem.h"
#include "cSpecies.h"
#include "cStats.h"
#include "cTopology.h"
#include "cWorld.h"
#include "cTopology.h"
#include "cTestCPU.h"
#include "cCPUTestInfo.h"

#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <set>
#include <cfloat>
#include <cmath>
#include <climits>

using namespace std;


cPopulation::cPopulation(cWorld* world)
: m_world(world)
, schedule(NULL)
//, resource_count(world->GetEnvironment().GetResourceLib().GetSize())
, birth_chamber(world)
, environment(world->GetEnvironment())
, num_organisms(0)
, sync_events(false)
{
  // Avida specific information.
  world_x = world->GetConfig().WORLD_X.Get();
  world_y = world->GetConfig().WORLD_Y.Get();
  int num_demes = m_world->GetConfig().NUM_DEMES.Get();
  const int num_cells = world_x * world_y;
  const int geometry = world->GetConfig().WORLD_GEOMETRY.Get();
  
  if(m_world->GetConfig().ENERGY_CAP.Get() == -1) {
    m_world->GetConfig().ENERGY_CAP.Set(INT_MAX);
  }
  
  if(m_world->GetConfig().LOG_SLEEP_TIMES.Get() == 1)  {
    sleep_log = new tVector<pair<int,int> >[world_x*world_y];
  }
  // Print out world details
  if (world->GetVerbosity() > VERBOSE_NORMAL) {
    cout << "Building world " << world_x << "x" << world_y << " = " << num_cells << " organisms." << endl;
    switch(geometry) {
    case nGeometry::GRID: { cout << "Geometry: Bounded grid" << endl; break; }
    case nGeometry::TORUS: { cout << "Geometry: Torus" << endl; break; }
    case nGeometry::CLIQUE: { cout << "Geometry: Clique" << endl; break; }
    case nGeometry::HEX: { cout << "Geometry: Hex" << endl; break; }
    default:
      cout << "Unknown geometry!" << endl;
      assert(false);
    }
  }
  
  // Invalid settings should be changed to one deme
  if (num_demes <= 0) {
    num_demes = 1; // One population == one deme.
  }

  // The following combination of options creates an infinite rotate-loop:
  assert(!((m_world->GetConfig().DEMES_ORGANISM_PLACEMENT.Get()==0)
           && (m_world->GetConfig().DEMES_ORGANISM_FACING.Get()==1)
           && (m_world->GetConfig().WORLD_GEOMETRY.Get()==1)));
  
  // Incompatible deme replication strategies:
  assert(!(m_world->GetConfig().DEMES_REPLICATE_SIZE.Get()
           && (m_world->GetConfig().DEMES_PROB_ORG_TRANSFER.Get()>0.0)));
  assert(!(m_world->GetConfig().DEMES_USE_GERMLINE.Get()
           && (m_world->GetConfig().DEMES_PROB_ORG_TRANSFER.Get()>0.0)));
  assert(!(m_world->GetConfig().DEMES_USE_GERMLINE.Get()
           && (m_world->GetConfig().MIGRATION_RATE.Get()>0.0)));
  
#ifdef DEBUG
  const int birth_method = m_world->GetConfig().BIRTH_METHOD.Get();

  if(num_demes > 1) {
    assert(birth_method != POSITION_CHILD_FULL_SOUP_ELDEST);
  }
#endif
  
  // Allocate the cells, resources, and market.
  cell_array.ResizeClear(num_cells);
  empty_cell_id_array.ResizeClear(cell_array.GetSize());
  market.Resize(MARKET_SIZE);
  
  // Setup the cells.  Do things that are not dependent upon topology here.
  for(int i=0; i<num_cells; ++i) {
    cell_array[i].Setup(world, i, environment.GetMutRates(), i%world_x, i/world_x);
    // Setup the reaper queue.
    if (world->GetConfig().BIRTH_METHOD.Get() == POSITION_CHILD_FULL_SOUP_ELDEST) {
      reaper_queue.Push(&(cell_array[i]));
    }
  }                         
  
  // What are the sizes of the demes that we're creating?
  const int deme_size_x = world_x;
  const int deme_size_y = world_y / num_demes;
  const int deme_size = deme_size_x * deme_size_y;
  deme_array.Resize(num_demes);
  
  // Broken setting:
  assert(m_world->GetConfig().DEMES_REPLICATE_SIZE.Get() <= deme_size);         
  
  // Setup the deme structures.
  tArray<int> deme_cells(deme_size);
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    for (int offset = 0; offset < deme_size; offset++) {
      int cell_id = deme_id * deme_size + offset;
      deme_cells[offset] = cell_id;
      cell_array[cell_id].SetDemeID(deme_id);
    }
    deme_array[deme_id].Setup(deme_id, deme_cells, deme_size_x, m_world);
  }
  
  // Setup the topology.
  // What we're doing here is chopping the cell_array up into num_demes pieces.
  // Note that having 0 demes (one population) is the same as having 1 deme.  Then
  // we send the cells that comprise each deme into the topology builder.
  for(int i=0; i<num_cells; i+=deme_size) {
    // We're cheating here; we're using the random access nature of an iterator
    // to index beyond the end of the cell_array.
    switch(geometry) {
    case nGeometry::GRID:
      build_grid(&cell_array.begin()[i], &cell_array.begin()[i+deme_size], deme_size_x, deme_size_y);
      break;
    case nGeometry::TORUS:
      build_torus(&cell_array.begin()[i], &cell_array.begin()[i+deme_size], deme_size_x, deme_size_y);
      break;
    case nGeometry::CLIQUE:
      build_clique(&cell_array.begin()[i], &cell_array.begin()[i+deme_size], deme_size_x, deme_size_y);
      break;
    case nGeometry::HEX:
      build_hex(&cell_array.begin()[i], &cell_array.begin()[i+deme_size], deme_size_x, deme_size_y);
      break;
    default:
      assert(false);
    }
  }
	
  BuildTimeSlicer(0);
  
  // Setup the resources...
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  int deme_res_index = -1;
  int num_deme_res = 0;

  //setting size of global and deme-level resources
  for(int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if(res->GetDemeResource())
      num_deme_res++;
  }
  
  cResourceCount tmp_res_count(resource_lib.GetSize() - num_deme_res);
  resource_count = tmp_res_count;
  resource_count.ResizeSpatialGrids(world_x, world_y);
  
  for(int i = 0; i < GetNumDemes(); i++) {
    cResourceCount tmp_deme_res_count(num_deme_res);
    GetDeme(i).SetDemeResourceCount(tmp_deme_res_count);
    GetDeme(i).ResizeSpatialGrids(deme_size_x, deme_size_y);
  }

  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (!res->GetDemeResource()) {
      global_res_index++;
      const double decay = 1.0 - res->GetOutflow();
      resource_count.Setup(global_res_index, res->GetName(), res->GetInitial(), 
                           res->GetInflow(), decay,
                           res->GetGeometry(), res->GetXDiffuse(),
                           res->GetXGravity(), res->GetYDiffuse(), 
                           res->GetYGravity(), res->GetInflowX1(), 
                           res->GetInflowX2(), res->GetInflowY1(), 
                           res->GetInflowY2(), res->GetOutflowX1(), 
                           res->GetOutflowX2(), res->GetOutflowY1(), 
                           res->GetOutflowY2(), res->GetCellListPtr(),
                           world->GetVerbosity() );
      m_world->GetStats().SetResourceName(global_res_index, res->GetName());
    } else if(res->GetDemeResource()) {
      deme_res_index++;
      for(int j = 0; j < GetNumDemes(); j++) {
        GetDeme(j).SetupDemeRes(deme_res_index, res, world->GetVerbosity());
        // could add deme resources to global resource stats here
      }
    } else {
      cerr<< "ERROR: Resource \"" << res->GetName() <<"\"is not a global or deme resource.  Exit";
      exit(1);
    }
  }

}

void cPopulation::InitiatePop() {

  // Load a clone if one is provided, otherwise setup start organism.
  if (m_world->GetConfig().CLONE_FILE.Get() == "-" || m_world->GetConfig().CLONE_FILE.Get() == "") {
    cGenome start_org(0);
    const cString& filename = m_world->GetConfig().START_CREATURE.Get();

    if (filename != "-" && filename != "") {
      if (!cGenomeUtil::LoadGenome(filename, m_world->GetHardwareManager().GetInstSet(), start_org)) {
        cerr << "Error: Unable to load start creature" << endl;
        exit(-1);
      }
      if (start_org.GetSize() != 0) {
        Inject(start_org);
      }
      else cerr << "Warning: Zero length start organism, not injecting into initial population." << endl;
    } else {
      cerr << "Warning: No start organism specified." << endl;
    }
  } else {
    ifstream fp(m_world->GetConfig().CLONE_FILE.Get());
    LoadClone(fp);
  }
}


cPopulation::~cPopulation()
{
  for (int i = 0; i < cell_array.GetSize(); i++) KillOrganism(cell_array[i]);
  delete schedule;
}


inline void cPopulation::AdjustSchedule(const cPopulationCell& cell, const cMerit& merit)
{
  const int deme_id = cell.GetDemeID();
  const cDeme& deme = deme_array[deme_id];
  schedule->Adjust(cell.GetID(), deme.HasDemeMerit() ? (merit * deme.GetDemeMerit()) : merit, cell.GetDemeID());
}



// Activate the child, given information from the parent.
// Return true if parent lives through this process.

bool cPopulation::ActivateOffspring(cAvidaContext& ctx, cGenome& child_genome, cOrganism& parent_organism)
{
  if (m_world->GetConfig().FASTFORWARD_NUM_ORGS.Get() > 0 && GetNumOrganisms() >= m_world->GetConfig().FASTFORWARD_NUM_ORGS.Get())
  {
	  return true;
  }
  assert(&parent_organism != NULL);
  
  tArray<cOrganism*> child_array;
  tArray<cMerit> merit_array;
  
  // Update the parent's phenotype.
  // This needs to be done before the parent goes into the birth chamber
  // or the merit doesn't get passed onto the child correctly
  cPhenotype& parent_phenotype = parent_organism.GetPhenotype();
  parent_phenotype.DivideReset(parent_organism.GetGenome());
  
  birth_chamber.SubmitOffspring(ctx, child_genome, parent_organism, child_array, merit_array);
  
  // First, setup the genotype of all of the offspring.
  cGenotype* parent_genotype = parent_organism.GetGenotype();
  const int parent_id = parent_organism.GetOrgInterface().GetCellID();
  assert(parent_id >= 0 && parent_id < cell_array.GetSize());
  cPopulationCell& parent_cell = cell_array[parent_id];
  
  tArray<int> target_cells(child_array.GetSize());
  
  // Loop through choosing the later placement of each child in the population.
  bool parent_alive = true;  // Will the parent live through this process?
  for (int i = 0; i < child_array.GetSize(); i++) {
    target_cells[i] = PositionChild(parent_cell, m_world->GetConfig().ALLOW_PARENT.Get()).GetID();
    
    // If we replaced the parent, make a note of this.
    if (target_cells[i] == parent_cell.GetID()) parent_alive = false;      
    
    const int mut_source = m_world->GetConfig().MUT_RATE_SOURCE.Get();
    if (mut_source == 1) {
      // Update the mutation rates of each child from the environment....
      child_array[i]->MutationRates().Copy(GetCell(target_cells[i]).MutationRates());
    } else {
      // Update the mutation rates of each child from its parent.
      child_array[i]->MutationRates().Copy(parent_organism.MutationRates());
      // If there is a meta-mutation rate, do tests for it.
      if (child_array[i]->MutationRates().GetMetaCopyMutProb() > 0.0) {
        child_array[i]->MutationRates().DoMetaCopyMut(ctx);
      }    
    }
    
    // Update the phenotypes of each child....
    const cGenome & child_genome = child_array[i]->GetGenome();
    child_array[i]->GetPhenotype().SetupOffspring(parent_phenotype,child_genome);
    child_array[i]->GetPhenotype().SetMerit(merit_array[i]);
    
    // Do lineage tracking for the new organisms.
    LineageSetupOrganism(child_array[i], parent_organism.GetLineage(),
                         parent_organism.GetLineageLabel(), parent_genotype);
    
    //By default, store the parent cclade, this may get modified in ActivateOrgansim (@MRR)
    child_array[i]->SetCCladeLabel(parent_organism.GetCCladeLabel());
  }
  
  // If we're not about to kill the parent, do some extra work on it.
  if (parent_alive == true) {
    // Reset inputs and re-calculate merit if required
    if (m_world->GetConfig().RESET_INPUTS_ON_DIVIDE.Get() > 0){
      environment.SetupInputs(ctx, parent_cell.m_inputs);

      int pc_phenotype = m_world->GetConfig().PRECALC_PHENOTYPE.Get();
      if (pc_phenotype){
        cCPUTestInfo test_info;
        cTestCPU* test_cpu = m_world->GetHardwareManager().CreateTestCPU();
        test_info.UseManualInputs(parent_cell.GetInputs()); // Test using what the environment will be
        test_cpu->TestGenome(ctx, test_info, parent_organism.GetHardware().GetMemory()); // Use the true genome
        if (pc_phenotype & 1) { // If we must update the merit
          parent_phenotype.SetMerit(test_info.GetTestPhenotype().GetMerit());
	}
        if (pc_phenotype & 2) {   // If we must update the gestation time
          parent_phenotype.SetGestationTime(test_info.GetTestPhenotype().GetGestationTime());
	}
        parent_phenotype.SetFitness(parent_phenotype.GetMerit().CalcFitness(parent_phenotype.GetGestationTime())); //Update fitness
        delete test_cpu;
      }
    }
    AdjustSchedule(parent_cell, parent_phenotype.GetMerit());
    
    // In a local run, face the child toward the parent. 
    const int birth_method = m_world->GetConfig().BIRTH_METHOD.Get();
    if (birth_method < NUM_LOCAL_POSITION_CHILD ||
        birth_method == POSITION_CHILD_PARENT_FACING) {
      for (int i = 0; i < child_array.GetSize(); i++) {
        GetCell(target_cells[i]).Rotate(parent_cell);
      }
    }
  }
  
  // Do any statistics on the parent that just gave birth...
  parent_genotype->AddGestationTime( parent_phenotype.GetGestationTime() );
  parent_genotype->AddFitness(       parent_phenotype.GetFitness()       );
  parent_genotype->AddMerit(         parent_phenotype.GetMerit()         );
  parent_genotype->AddCopiedSize(    parent_phenotype.GetCopiedSize()    );
  parent_genotype->AddExecutedSize(  parent_phenotype.GetExecutedSize()  );
  
  
  // Place all of the offspring...
  for (int i = 0; i < child_array.GetSize(); i++) {
    ActivateOrganism(ctx, child_array[i], GetCell(target_cells[i]));
    
    //@JEB - we may want to pass along some state information from parent to child
    if ( (m_world->GetConfig().EPIGENETIC_METHOD.Get() == EPIGENETIC_METHOD_OFFSPRING) 
      || (m_world->GetConfig().EPIGENETIC_METHOD.Get() == EPIGENETIC_METHOD_BOTH) ) {
      child_array[i]->GetHardware().InheritState(parent_organism.GetHardware());
    }
    
    cGenotype* child_genotype = child_array[i]->GetGenotype();
    child_genotype->DecDeferAdjust();
    m_world->GetClassificationManager().AdjustGenotype(*child_genotype);
  }
  
  return parent_alive;
}

bool cPopulation::ActivateParasite(cOrganism& parent, const cCodeLabel& label, const cGenome& injected_code)
{
  assert(&parent != NULL);
  
  if (injected_code.GetSize() == 0) return false;
  
  cHardwareBase& parent_cpu = parent.GetHardware();
  cInjectGenotype* parent_genotype = parent_cpu.ThreadGetOwner();
  
  const int parent_id = parent.GetOrgInterface().GetCellID();
  assert(parent_id >= 0 && parent_id < cell_array.GetSize());
  cPopulationCell& parent_cell = cell_array[ parent_id ];
  
  int num_neighbors = parent.GetNeighborhoodSize();
  cOrganism* target_organism = 
    parent_cell.ConnectionList().GetPos(m_world->GetRandom().GetUInt(num_neighbors))->GetOrganism();
  
  if (target_organism == NULL) return false;
  
  cHardwareBase& child_cpu = target_organism->GetHardware();
  
  if (child_cpu.GetNumThreads() == m_world->GetConfig().MAX_CPU_THREADS.Get()) return false;
  
  
  if (target_organism->InjectHost(label, injected_code)) {
    cInjectGenotype* child_genotype = parent_genotype;
    
    // If the parent genotype is not correct for the child, adjust it.
    if (parent_genotype == NULL || parent_genotype->GetGenome() != injected_code) {
      child_genotype = m_world->GetClassificationManager().GetInjectGenotype(injected_code, parent_genotype);
    }
    
    target_organism->AddParasite(child_genotype);
    child_genotype->AddParasite();
    child_cpu.ThreadSetOwner(child_genotype);
    m_world->GetClassificationManager().AdjustInjectGenotype(*child_genotype);
  }
  else
    return false;
  
  return true;
}

void cPopulation::ActivateOrganism(cAvidaContext& ctx, cOrganism* in_organism, cPopulationCell& target_cell)
{
  assert(in_organism != NULL);
  assert(in_organism->GetGenome().GetSize() >= 1);

  in_organism->SetOrgInterface(new cPopulationInterface(m_world));
  
  // If the organism does not have a genotype, give it one!  No parent
  // information is provided so we must set parents to NULL.
  if (in_organism->GetGenotype() == NULL) {
    cGenotype* new_genotype = m_world->GetClassificationManager().GetGenotype(in_organism->GetGenome(), NULL, NULL);
    in_organism->SetGenotype(new_genotype);
  }
  cGenotype* in_genotype = in_organism->GetGenotype();
  
  // Save the old genotype from this cell...
  cGenotype* old_genotype = NULL;
  if (target_cell.IsOccupied()) {
    old_genotype = target_cell.GetOrganism()->GetGenotype();
    
    // Sometimes a new organism will kill off the last member of its genotype
    // in the population.  Normally this would remove the genotype, so we 
    // want to defer adjusting that genotype until the new one is placed.
    old_genotype->IncDeferAdjust();
  }
  
  // Update the contents of the target cell.
  KillOrganism(target_cell);
  target_cell.InsertOrganism(in_organism);
  
  // Setup the inputs in the target cell.
  environment.SetupInputs(ctx, target_cell.m_inputs);
    
  // Precalculate the phenotype if requested
  int pc_phenotype = m_world->GetConfig().PRECALC_PHENOTYPE.Get();
  if (pc_phenotype){
    cCPUTestInfo test_info;
    cTestCPU* test_cpu = m_world->GetHardwareManager().CreateTestCPU();
    test_info.UseManualInputs(target_cell.GetInputs()); // Test using what the environment will be
    test_cpu->TestGenome(ctx, test_info, in_organism->GetHardware().GetMemory());  // Use the true genome
    
    if (pc_phenotype & 1)
      in_organism->GetPhenotype().SetMerit(test_info.GetTestPhenotype().GetMerit()); 
    if (pc_phenotype & 2)
      in_organism->GetPhenotype().SetGestationTime(test_info.GetTestPhenotype().GetGestationTime());
    in_organism->GetPhenotype().SetFitness(in_organism->GetPhenotype().GetMerit().CalcFitness(in_organism->GetPhenotype().GetGestationTime()));
    delete test_cpu;
  }
  // Update the archive...
      
  in_genotype->AddOrganism();
  
  if (old_genotype != NULL) {
    old_genotype->DecDeferAdjust();
    m_world->GetClassificationManager().AdjustGenotype(*old_genotype);
  }
  m_world->GetClassificationManager().AdjustGenotype(*in_genotype);
  
  // Initialize the time-slice for this new organism.
  AdjustSchedule(target_cell, in_organism->GetPhenotype().GetMerit());
  
  // Special handling for certain birth methods.
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_CHILD_FULL_SOUP_ELDEST) {
    reaper_queue.Push(&target_cell);
  }
  
  // Keep track of statistics for organism counts...
  num_organisms++;

  if (deme_array.GetSize() > 0) {
    deme_array[target_cell.GetDemeID()].IncOrgCount();
  }
  
  // Statistics...
  m_world->GetStats().RecordBirth(target_cell.GetID(), in_genotype->GetID(),
                                  in_organism->GetPhenotype().ParentTrue());
  
  // @MRR Do coalescence clade set up for new organisms.
  CCladeSetupOrganism(in_organism ); 
  
  //count how many times MERIT_BONUS_INST (rewarded instruction) is in the genome
  //only relevant if merit is proportional to # times MERIT_BONUS_INST is in the genome
  int rewarded_instruction = m_world->GetConfig().MERIT_BONUS_INST.Get();
  int num_rewarded_instructions = 0;
  int genome_length = in_organism->GetGenome().GetSize();
  
  if(rewarded_instruction == -1){
    //no key instruction, so no bonus 
    in_organism->GetPhenotype().SetCurBonusInstCount(0);
  }
  else{
    for(int i = 1; i <= genome_length; i++){
      if(in_organism->GetGenome()[i-1].GetOp() == rewarded_instruction){
        num_rewarded_instructions++;
      }  
    } 
    in_organism->GetPhenotype().SetCurBonusInstCount(num_rewarded_instructions);
  }
}

// @WRE 2007/07/05 Helper function to take care of side effects of Avidian 
// movement that cannot be directly handled in cHardwareCPU.cc
void cPopulation::MoveOrganisms(cAvidaContext& ctx, cPopulationCell& src_cell, cPopulationCell& dest_cell)
{
  // Declarations
  int actualNeighborhoodSize, fromFacing, destFacing, newFacing, success;
#ifdef DEBBUG
  int sID, dID, xx1, yy1, xx2, yy2;
#endif

  // Swap inputs between cells to fix bus error when Avidian moves into an unoccupied cell
  environment.SwapInputs(ctx, src_cell.m_inputs, dest_cell.m_inputs);
  
  // Find neighborhood size for facing 
  if (NULL != dest_cell.GetOrganism()) {
    actualNeighborhoodSize = dest_cell.GetOrganism()->GetNeighborhoodSize();
  } else {
    if (NULL != src_cell.GetOrganism()) {
      actualNeighborhoodSize = src_cell.GetOrganism()->GetNeighborhoodSize();
    } else {
      // Punt
      actualNeighborhoodSize = 8;
    }
  }
 
  // Swap cell facings between cells, so that if movement is directed, it continues to be associated with
  // the same organism
  // Determine absolute facing for each cell
  fromFacing = src_cell.GetFacing();
  destFacing = dest_cell.GetFacing();

  // Set facing in source cell
  success = 0;
  newFacing = destFacing;
  for(int i = 0; i < actualNeighborhoodSize; i++) {
    if (src_cell.GetFacing() != newFacing) {
      src_cell.ConnectionList().CircNext();
      //cout << "MO: src_cell facing not yet at " << newFacing << endl;
    } else {
      //cout << "MO: src_cell facing successfully set to " << newFacing << endl;
      success = 1;
      break;
    }
  }
// @DMB this doesn't compile properly -- #ifdef DEBUG
#if 0
  if (!success) {
    sID = src_cell.GetID();
    dID = dest_cell.GetID();
    src_cell.GetPosition(xx1,yy1);
    dest_cell.GetPosition(xx2,yy2);
    //Conditional for examining only neighbor move without swap in facing
    //if (1 == abs(xx2-xx1)+abs(yy2-yy1)) {
    cout << "MO: src: " << sID << "@ (" << xx1 << "," << yy1 << ") dest: " << dID << "@ (" << xx2 << "," << yy2 << "), FAILED to set src_cell facing to " << newFacing << endl;
    for (int j=0; j < actualNeighborhoodSize; j++) {
      src_cell.ConnectionList().CircNext();
      src_cell.GetCellFaced().GetPosition(xx2,yy2);
      cout << "connlist for " << sID << ": facing " << src_cell.GetFacing() << " -> (" << xx2 << "," << yy2 << ")" << endl;
    }
    //}
  }
#endif 

  // Set facing in destinatiion cell
  success = 0;
  newFacing = fromFacing;
  for(int i = 0; i < actualNeighborhoodSize; i++) {
    if (dest_cell.GetFacing() != newFacing) {
      dest_cell.ConnectionList().CircNext();
      // cout << "MO: dest_cell facing not yet at " << newFacing << endl;
    } else {
      // cout << "MO: dest_cell facing successfully set to " << newFacing << endl;
      success = 1;
      break;
    }
  }
// @DMB this doesn't compile properly -- #ifdef DEBUG
#if 0
  if (!success) {
    sID = src_cell.GetID();
    dID = dest_cell.GetID();
    src_cell.GetPosition(xx1,yy1);
    dest_cell.GetPosition(xx2,yy2);
    if (1 == abs(xx2-xx1)+abs(yy2-yy1)) {
      cout << "MO: src: " << sID << "@ (" << xx1 << "," << yy1 << ") dest: " << dID << "@ (" << xx2 << "," << yy2 << "), FAILED to set dest_cell facing to " << newFacing << endl;
    }
  }
#endif
  
}

void cPopulation::KillOrganism(cPopulationCell& in_cell)
{
  // do we actually have something to kill?
  if (in_cell.IsOccupied() == false) return;
  
  // Statistics...
  cOrganism* organism = in_cell.GetOrganism();
  cGenotype* genotype = organism->GetGenotype();
  m_world->GetStats().RecordDeath();
  
  int cellID = in_cell.GetID();

  if(GetCell(cellID).GetOrganism()->IsSleeping()) {
    GetCell(cellID).GetOrganism()->SetSleeping(false);
    m_world->GetStats().decNumAsleep(GetCell(cellID).GetDemeID());
  }
  if(m_world->GetConfig().LOG_SLEEP_TIMES.Get() == 1) {
    if(sleep_log[cellID].Size() > 0) {
      pair<int,int> p = sleep_log[cellID][sleep_log[cellID].Size()-1];
      if(p.second == -1) {
        AddEndSleep(cellID,m_world->GetStats().GetUpdate());
      }
    }
  }
  
  
  tList<tListNode<cSaleItem> >* sold_items = organism->GetSoldItems();
  if (sold_items)
  {
	  tListIterator<tListNode<cSaleItem> > sold_it(*sold_items);
	  tListNode<cSaleItem> * test_node;
    
	  while ( (test_node = sold_it.Next()) != NULL)
	  {
      tListIterator<cSaleItem> market_it(market[test_node->data->GetLabel()]);
      market_it.Set(test_node);
      delete market_it.Remove();
	  }
  }
  // Do the lineage handling
  if (m_world->GetConfig().LOG_LINEAGES.Get()) { m_world->GetClassificationManager().RemoveLineageOrganism(organism); }
  
  // Update count statistics...
  num_organisms--;
  
  if (deme_array.GetSize() > 0) {
    deme_array[in_cell.GetDemeID()].DecOrgCount();
  }
  genotype->RemoveOrganism();
  
  organism->ClearParasites();
  
  // And clear it!
  in_cell.RemoveOrganism();
  if (!organism->IsRunning()) delete organism;
  else organism->GetPhenotype().SetToDelete();
  
  // Alert the scheduler that this cell has a 0 merit.
  AdjustSchedule(in_cell, cMerit(0));
  
  // Update the archive (note: genotype adjustment may be defered)
  m_world->GetClassificationManager().AdjustGenotype(*genotype);
}

void cPopulation::Kaboom(cPopulationCell & in_cell, int distance)
{
  cOrganism * organism = in_cell.GetOrganism();
  cGenotype * genotype = organism->GetGenotype();
  cGenome genome = genotype->GetGenome();
  int id = genotype->GetID();
  
  int radius = 2;
  int count = 0;
  
  for (int i=-1*radius; i<=radius; i++) {
    for (int j=-1*radius; j<=radius; j++) {
      cPopulationCell & death_cell =
      cell_array[GridNeighbor(in_cell.GetID(), world_x, world_y, i, j)];
      //do we actually have something to kill?
      if (death_cell.IsOccupied() == false) continue;
      
      cOrganism * org_temp = death_cell.GetOrganism();
      cGenotype * gene_temp = org_temp->GetGenotype();
      
      if (distance == 0) {
        int temp_id = gene_temp->GetID();
        if (temp_id != id) {
          KillOrganism(death_cell);
          count++;
        }
      }
      else {	
        cGenome genome_temp = gene_temp->GetGenome();
        int diff=0;
        for (int i=0; i<genome_temp.GetSize(); i++)
          if (genome_temp.AsString()[i] != genome.AsString()[i])
            diff++;
        if (diff > distance)
        {
          KillOrganism(death_cell);
          count++;
        }
      }
    }
  }
  KillOrganism(in_cell);
  // @SLG my prediction = 92% and, 28 get equals
}

void cPopulation::AddSellValue(const int data, const int label, const int sell_price, const int org_id, const int cell_id)
{
	// find list under appropriate label, labels more than 8 nops long are simply the same
	// as a smaller label modded by the market size
	//int pos = label % market.GetSize();
  
	//// id of genotype currently residing in cell that seller live(d) in compared to 
	//// id of genotype of actual seller, if different than seller is dead, remove item from list
	//while ( market[pos].GetSize() > 0 && 
	//	(!GetCell(market[pos].GetFirst()->GetCellID()).IsOccupied() ||
	//	GetCell(market[pos].GetFirst()->GetCellID()).GetOrganism()->GetID()
	//	!= 	market[pos].GetFirst()->GetOrgID()) )
	//{
	//	market[pos].Pop();
	//}
    
	// create sale item
	cSaleItem *new_item = new cSaleItem(data, label, sell_price, org_id, cell_id);
  
	// place into array by label, array is big enough for labels up to 8 nops long
	tListNode<cSaleItem>* sell_node = market[label].PushRear(new_item);
	tListNode<tListNode<cSaleItem> >* org_node = GetCell(cell_id).GetOrganism()->AddSoldItem(sell_node);
	sell_node->data->SetNodePtr(org_node);
  
	//:7 for Kolby
}

int cPopulation::BuyValue(const int label, const int buy_price, const int cell_id)
{
	// find list under appropriate label, labels more than 8 nops long are simply the same
	// as a smaller label modded by the market size
	//int pos = label % market.GetSize();
  
	//// id of genotype currently residing in cell that seller live(d) in compared to 
	//// id of genotype of actual seller, if different than seller is dead, remove item from list
	//while ( market[pos].GetSize() > 0 && 
	//	(!GetCell(market[pos].GetFirst()->GetCellID()).IsOccupied() ||
	//	GetCell(market[pos].GetFirst()->GetCellID()).GetOrganism()->GetID()
	//	!= 	market[pos].GetFirst()->GetOrgID()) )
	//{
	//	market[pos].Pop();
	//}
    
	// if there's nothing in the list don't bother with rest
	if (market[label].GetSize() <= 0)
		return 0;
  
	// if the sell price is higher than we're willing to pay no purchase made
	if (market[label].GetFirst()->GetPrice() > buy_price)
		return 0;
  
	// if the buy price is higher than buying org's current merit no purchase made
	if (GetCell(cell_id).GetOrganism()->GetPhenotype().GetMerit().GetDouble() < buy_price)
		return 0;
  
	// otherwise transaction should be completed!
	cSaleItem* chosen = market[label].Pop();
	tListIterator<tListNode<cSaleItem> > sold_it(*GetCell(chosen->GetCellID()).GetOrganism()->GetSoldItems());
	sold_it.Set(chosen->GetNodePtr());
	sold_it.Remove();
  
	// first update sellers merit
	double cur_merit = GetCell(chosen->GetCellID()).GetOrganism()->GetPhenotype().GetMerit().GetDouble();
	cur_merit += buy_price;
	
	GetCell(chosen->GetCellID()).GetOrganism()->UpdateMerit(cur_merit);
	
	// next remove sold item from list in market 
	//market[pos].Remove(chosen);
  
  
	// finally return recieve value, buyer merit will be updated if return a valid value here
	int receive_value = chosen->GetData();
	return receive_value;
}

void cPopulation::SwapCells(cPopulationCell & cell1, cPopulationCell & cell2)
{
  // Sanity checks: Don't process if the cells are the same and 
  // don't bother trying to move when given a cell that isn't there
  if ((&cell1 == NULL) || (&cell2 == NULL)) return;
  if (cell1.GetID() == cell2.GetID()) return;
  // Clear current contents of cells
  cOrganism * org1 = cell1.RemoveOrganism();
  cOrganism * org2 = cell2.RemoveOrganism();
  if (org2 != NULL) {
    cell1.InsertOrganism(org2);
    AdjustSchedule(cell1, org2->GetPhenotype().GetMerit());
  } else {
    AdjustSchedule(cell1, cMerit(0));
  }
  if (org1 != NULL) {
    cell2.InsertOrganism(org1);
    // Increment visit count
    cell2.IncVisits();
    // Adjust for movement factor if needed
    if (1.0 != m_world->GetConfig().BIOMIMETIC_MOVEMENT_FACTOR.Get()) {
      double afterfit = org1->GetPhenotype().GetCurBonus() * m_world->GetConfig().BIOMIMETIC_MOVEMENT_FACTOR.Get();
      org1->GetPhenotype().SetCurBonus(afterfit); //Update fitness
    }
    // Trigger evaluation for task completion
    if (0 < m_world->GetConfig().BIOMIMETIC_EVAL_ON_MOVEMENT.Get()) {
      cAvidaContext& ctx = m_world->GetDefaultContext();
      org1->DoOutput(ctx,0);
    }
    AdjustSchedule(cell2, org1->GetPhenotype().GetMerit());
  } else {
    AdjustSchedule(cell2, cMerit(0));
  }
}

// CompeteDemes  probabilistically copies demes into the next generation
// based on their fitness. How deme fitness is estimated is specified by 
// competition_type input argument as:
/*
 0: deme fitness = 1 (control, random deme selection)
 1: deme fitness = number of births since last competition (default) 
 2: deme fitness = average organism fitness at the current update (uses parent's fitness, so
                                                                   does not work with donations)
 3: deme fitness = average mutation rate at the current update
 4: deme fitness = strong rank selection on (parents) fitness (2^-deme fitness rank)
 5: deme fitness = average organism life (current, not parents) fitness (works with donations)
 6: deme fitness = strong rank selection on life (current, not parents) fitness
 */
//  For ease of use, each organism 
// is setup as if it we just injected into the population.

void cPopulation::CompeteDemes(int competition_type)
{
  const int num_demes = deme_array.GetSize();
  
  double total_fitness = 0; 
  tArray<double> deme_fitness(num_demes); 
  
  switch(competition_type) {
    case 0:    // deme fitness = 1; 
      total_fitness = (double) num_demes;
      deme_fitness.SetAll(1); 
      break; 
    case 1:     // deme fitness = number of births
                // Determine the scale for fitness by totaling births across demes.
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        double cur_fitness = (double) deme_array[deme_id].GetBirthCount();
        deme_fitness[deme_id] = cur_fitness;
        total_fitness += cur_fitness;
      }
      break; 
    case 2:    // deme fitness = average organism fitness at the current update
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        cDoubleSum single_deme_fitness;
        const cDeme & cur_deme = deme_array[deme_id];
        for (int i = 0; i < cur_deme.GetSize(); i++) {
          int cur_cell = cur_deme.GetCellID(i);
          if (cell_array[cur_cell].IsOccupied() == false) continue;
          cPhenotype & phenotype =
            GetCell(cur_cell).GetOrganism()->GetPhenotype();
          single_deme_fitness.Add(phenotype.GetFitness());
        } 
        deme_fitness[deme_id] = single_deme_fitness.Ave();
        total_fitness += deme_fitness[deme_id];
      }
      break; 
    case 3: 	// deme fitness = average mutation rate at the current update 
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        cDoubleSum single_deme_div_type;
        const cDeme & cur_deme = deme_array[deme_id];
        for (int i = 0; i < cur_deme.GetSize(); i++) {
          int cur_cell = cur_deme.GetCellID(i);
          if (cell_array[cur_cell].IsOccupied() == false) continue;
          cPhenotype & phenotype =
            GetCell(cur_cell).GetOrganism()->GetPhenotype();
          assert(phenotype.GetDivType()>0);
          single_deme_div_type.Add(1/phenotype.GetDivType());
        }
        deme_fitness[deme_id] = single_deme_div_type.Ave();
        total_fitness += deme_fitness[deme_id];
      }
      break; 
    case 4: 	// deme fitness = 2^(-deme fitness rank) 
              // first find all the deme fitness values ...
    {      
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        cDoubleSum single_deme_fitness;
        const cDeme & cur_deme = deme_array[deme_id];
        for (int i = 0; i < cur_deme.GetSize(); i++) {
          int cur_cell = cur_deme.GetCellID(i);
          if (cell_array[cur_cell].IsOccupied() == false) continue;
          cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
          single_deme_fitness.Add(phenotype.GetFitness());
        }  
        deme_fitness[deme_id] = single_deme_fitness.Ave();
      }
      // ... then determine the rank of each deme based on its fitness
      tArray<double> deme_rank(num_demes);
      deme_rank.SetAll(1);
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        for (int test_deme = 0; test_deme < num_demes; test_deme++) {
          if (deme_fitness[deme_id] < deme_fitness[test_deme]) {
            deme_rank[deme_id]++;
          } 
        } 
      } 
      // ... finally, make deme fitness 2^(-deme rank)
      deme_fitness.SetAll(1);	
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        for (int i = 0; i < deme_rank[deme_id]; i++) { 
          deme_fitness[deme_id] = deme_fitness[deme_id]/2;
        } 
        total_fitness += deme_fitness[deme_id]; 
      } 
    }
      break; 
    case 5:    // deme fitness = average organism life fitness at the current update
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        cDoubleSum single_deme_life_fitness;
        const cDeme & cur_deme = deme_array[deme_id];
        for (int i = 0; i < cur_deme.GetSize(); i++) {
          int cur_cell = cur_deme.GetCellID(i);
          if (cell_array[cur_cell].IsOccupied() == false) continue;
          cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
          single_deme_life_fitness.Add(phenotype.GetLifeFitness());
        }
        deme_fitness[deme_id] = single_deme_life_fitness.Ave();
        total_fitness += deme_fitness[deme_id];
      }
      break; 
    case 6:     // deme fitness = 2^(-deme life fitness rank) (same as 4, but with life fitness)
                // first find all the deme fitness values ...
    {
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        cDoubleSum single_deme_life_fitness;
        const cDeme & cur_deme = deme_array[deme_id];
        for (int i = 0; i < cur_deme.GetSize(); i++) {
          int cur_cell = cur_deme.GetCellID(i);
          if (cell_array[cur_cell].IsOccupied() == false) continue;
          cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
          single_deme_life_fitness.Add(phenotype.GetLifeFitness());
        }
        deme_fitness[deme_id] = single_deme_life_fitness.Ave();
      }
      // ... then determine the rank of each deme based on its fitness
      tArray<double> deme_rank(num_demes);
      deme_rank.SetAll(1);
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        for (int test_deme = 0; test_deme < num_demes; test_deme++) {
          if (deme_fitness[deme_id] < deme_fitness[test_deme]) {
            deme_rank[deme_id]++;
          }
        }
      }
      // ... finally, make deme fitness 2^(-deme rank)
      deme_fitness.SetAll(1);
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        for (int i = 0; i < deme_rank[deme_id]; i++) {
          deme_fitness[deme_id] = deme_fitness[deme_id]/2;
        }
        total_fitness += deme_fitness[deme_id];
      }
    }
      break;
  } 
  
  // Pick which demes should be in the next generation.
  tArray<int> new_demes(num_demes);
  for (int i = 0; i < num_demes; i++) {
    double birth_choice = (double) m_world->GetRandom().GetDouble(total_fitness);
    double test_total = 0;
    for (int test_deme = 0; test_deme < num_demes; test_deme++) {
      test_total += deme_fitness[test_deme];
      if (birth_choice < test_total) {
        new_demes[i] = test_deme;
        break;
      }
    }
  }
  
  // Track how many of each deme we should have.
  tArray<int> deme_count(num_demes);
  deme_count.SetAll(0);
  for (int i = 0; i < num_demes; i++) {
    deme_count[new_demes[i]]++;
  }
  
  tArray<bool> is_init(num_demes); 
  is_init.SetAll(false);
  
  // Copy demes until all deme counts are 1.
  while (true) {
    // Find the next deme to copy...
    int from_deme_id, to_deme_id;
    for (from_deme_id = 0; from_deme_id < num_demes; from_deme_id++) {
      if (deme_count[from_deme_id] > 1) break;
    }
    
    // Stop If we didn't find another deme to copy
    if (from_deme_id == num_demes) break;
    
    for (to_deme_id = 0; to_deme_id < num_demes; to_deme_id++) {
      if (deme_count[to_deme_id] == 0) break;
    }
    
    // We now have both a from and a to deme....
    deme_count[from_deme_id]--;
    deme_count[to_deme_id]++;
    
    cDeme & from_deme = deme_array[from_deme_id];
    cDeme & to_deme   = deme_array[to_deme_id];
    
    // Do the actual copy!
    for (int i = 0; i < from_deme.GetSize(); i++) {
      int from_cell_id = from_deme.GetCellID(i);
      int to_cell_id = to_deme.GetCellID(i);
      if (cell_array[from_cell_id].IsOccupied() == true) {
        InjectClone( to_cell_id, *(cell_array[from_cell_id].GetOrganism()) );
      }
    }
    is_init[to_deme_id] = true;
  }
  
  // Now re-inject all remaining demes into themselves to reset them.
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    if (is_init[deme_id] == true) continue;
    cDeme & cur_deme = deme_array[deme_id];
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell_id = cur_deme.GetCellID(i);
      if (cell_array[cur_cell_id].IsOccupied() == false) continue;
      InjectClone( cur_cell_id, *(cell_array[cur_cell_id].GetOrganism()) );
    }
  }
  
  // Reset all deme stats to zero.
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    deme_array[deme_id].Reset(deme_array[deme_id].GetGeneration()); // increase deme generation by 1
  }
}


/*! Compete all demes with each other based on the given vector of fitness values.
 
 This form of compete demes uses fitness-proportional selection on a vector of deme
 fitnesses for group selection.  It integrates with the various deme replication options
 used in ReplicateDemes.
 
 Note: New deme competition fitness functions are added in PopulationActions.cc by subclassing
 cActionAbstractCompeteDemes and overriding cActionAbstractCompeteDemes::Fitness(cDeme&).  (Don't forget
 to register the action and add it to the events file).
 
 Another note: To mimic the behavior of the other version of CompeteDemes (which is kept around
 for backwards compatibility), change the config option DEMES_REPLICATE_SIZE to be the size of 
 each deme.
 */
void cPopulation::CompeteDemes(const std::vector<double>& fitness) {
  // Each deme must have a fitness:
  assert((int)fitness.size() == deme_array.GetSize());
  
  // Stat-tracking:
  m_world->GetStats().CompeteDemes(fitness);
  
  // Now, select the demes to live.  Each deme has a probability to replicate that is
  // equal to its fitness / total fitness.
  const double total_fitness = std::accumulate(fitness.begin(), fitness.end(), 0.0);
  assert(total_fitness > 0.0); // Must have *some* positive fitnesses...
  std::vector<unsigned int> deme_counts(deme_array.GetSize(), 0); // Number of demes (at index) which should wind up in the next generation.
  
  // What we're doing here is summing up the fitnesses until we reach or exceed the target fitness.
  // Then we're marking that deme as being part of the next generation.
  for(int i=0; i<deme_array.GetSize(); ++i) {
    double running_sum = 0.0;
    double target_sum = m_world->GetRandom().GetDouble(total_fitness);
    for(int j=0; j<deme_array.GetSize(); ++j) {
      running_sum += fitness[j];
      if(running_sum >= target_sum) {
        // j'th deme will be replicated.
        ++deme_counts[j];
        break;
      }
    }
  }
  
  // Now, while we can find both a source deme (one with a count greater than 1)
  // and a target deme (one with a count of 0), replace the target with the source.
  while(true) {
    int source_id=0;
    for(; source_id<(int)deme_counts.size(); ++source_id) {
      if(deme_counts[source_id] > 1) {
        --deme_counts[source_id];
        break;
      }
    }
    
    if(source_id == (int)deme_counts.size()) {
      break; // All done.
    }
    
    int target_id=0;
    for(; target_id<(int)deme_counts.size(); ++target_id) {
      if(deme_counts[target_id] == 0) {
        ++deme_counts[target_id];
        break;
      }
    }
    
    assert(source_id < deme_array.GetSize());
    assert(target_id < deme_array.GetSize());
    assert(source_id != target_id);
    
    // Replace the target with a copy of the source:
    ReplaceDeme(deme_array[source_id], deme_array[target_id]);
  }  
}


/* Check if any demes have met the critera to be replicated and do so.
There are several bases this can be checked on:

0: 'all'       - ...all non-empty demes in the population.
1: 'full_deme' - ...demes that have been filled up.
2: 'corners'   - ...demes with upper left and lower right corners filled.
3: 'deme-age'  - ...demes who have reached their maximum age
4: 'birth-count' ...demes that have had a certain number of births.
5: 'sat-mov-pred'...demes whose movement predicate was previously satisfied

*/

void cPopulation::ReplicateDemes(int rep_trigger)
{
  assert(GetNumDemes()>1); // Sanity check.
  
  // Loop through all candidate demes...
  const int num_demes = GetNumDemes();
  for(int deme_id=0; deme_id<num_demes; ++deme_id) {
    cDeme& source_deme = deme_array[deme_id];
    
    // Test this deme to determine if it should be replicated.  If not,
    // continue on to the next deme.
    switch (rep_trigger) {
      case 0: {
        // Replicate all non-empty demes.
        if(source_deme.IsEmpty()) continue;
        break;
      }
      case 1: {
        // Replicate all full demes.
        if(!source_deme.IsFull()) continue;
        break;
      }
      case 2: {
        // Replicate all demes with the corners filled in.
        // The first and last IDs represent the two corners.
        const int id1 = source_deme.GetCellID(0);
        const int id2 = source_deme.GetCellID(source_deme.GetSize() - 1);
        if(cell_array[id1].IsOccupied() == false ||
           cell_array[id2].IsOccupied() == false) continue;
        break;
      }
      case 3: {
        // Replicate old demes.
        if(source_deme.GetAge() < m_world->GetConfig().DEMES_MAX_AGE.Get()) continue;
        break;
      }
      case 4: {
        // Replicate demes that have had a certain number of births.
        if(source_deme.GetBirthCount() < m_world->GetConfig().DEMES_MAX_BIRTHS.Get()) continue;
        break;
      }
      case 5: {
        if(!(source_deme.MovPredSatisfiedPreviously())) continue;
        break;
      }
      default: {
        cerr << "ERROR: Invalid replication trigger " << rep_trigger
        << " in cPopulation::ReplicateDemes()" << endl;
        assert(false);
      }
    }
    
    ReplicateDeme(source_deme);
  }
}

/*! ReplicateDeme is a helper method for replicating a source deme.
*/
void cPopulation::ReplicateDeme(cDeme & source_deme)
{
    // Doesn't make sense to try and replicate a deme that *has no organisms*.
    if(source_deme.IsEmpty()) return;
    
    // Prevent sterile demes from replicating.
    if(m_world->GetConfig().DEMES_PREVENT_STERILE.Get() && (source_deme.GetBirthCount() == 0)) {
      return;
    }
    
    //Option to bridge between kin and group selection.
    if (m_world->GetConfig().DEMES_REPLICATION_ONLY_RESETS.Get())
    {
      //Reset deme (resources and births, among other things)
      bool source_deme_resource_reset = m_world->GetConfig().DEMES_RESET_RESOURCES.Get() == 0;
      source_deme.DivideReset(source_deme, source_deme_resource_reset);
      
      //Reset all organisms in deme, by re-injecting them?
      if (m_world->GetConfig().DEMES_REPLICATION_ONLY_RESETS.Get() == 2) {
        for (int i=0; i<source_deme.GetSize(); i++) {
          int cellid = source_deme.GetCellID(i);
          if (GetCell(cellid).IsOccupied()) {          
            int lineage = GetCell(cellid).GetOrganism()->GetLineageLabel();
            cGenome genome = GetCell(cellid).GetOrganism()->GetGenome();
            InjectGenome(cellid, genome, lineage);
          }
        }
      }
      return;
    }
    
    // Pick a target deme to replicate to, making sure that 
    // we don't try to replicate over ourself, i.e. DEMES_REPLACE_PARENT 0
    
    int target_id = -1;
    if (m_world->GetConfig().DEMES_PREFER_EMPTY.Get()) {
    
      //@JEB -- use empty_cell_id_array to hold empty demes
      //so we don't have to allocate a list
      int num_empty = 0;
      for (int i=0; i<GetNumDemes(); i++) {
        if (GetDeme(i).IsEmpty()) {
          empty_cell_id_array[num_empty] = i;
          num_empty++;
        }
      }  
      if (num_empty > 0) {
        target_id = empty_cell_id_array[m_world->GetRandom().GetUInt(num_empty)];
      }
    }
    
    // if we haven't found one yet, choose a random one
    if (target_id == -1) {
      target_id = source_deme.GetID();
      const int num_demes = GetNumDemes();
      while(target_id == source_deme.GetID()) {
        target_id = m_world->GetRandom().GetUInt(num_demes);
      }
    }
    ReplaceDeme(source_deme, deme_array[target_id]);
}

/*! ReplaceDeme is a helper method that handles all the different configuration
options related to the replacement of a target deme by a source.  It works with
both CompeteDemes and ReplicateDemes (and can be called directly via an event if
so desired).

@refactor Replace manual mutation with strategy pattern.
*/
void cPopulation::ReplaceDeme(cDeme& source_deme, cDeme& target_deme) 
{
  // Stats tracking; pre-replication hook.
  m_world->GetStats().DemePreReplication(source_deme, target_deme);
  
  // used to pass energy to offspring demes (set to zero if energy model is not enabled)
  double source_deme_energy(0.0), deme_energy_decay(0.0), parent_deme_energy(0.0), offspring_deme_energy(0.0);
  if(m_world->GetConfig().ENERGY_ENABLED.Get()) {
    source_deme_energy = source_deme.CalculateTotalEnergy();
    deme_energy_decay = 1.0 - m_world->GetConfig().FRAC_ENERGY_DECAY_AT_DEME_BIRTH.Get();
    parent_deme_energy = source_deme_energy * deme_energy_decay * (1.0 - m_world->GetConfig().FRAC_PARENT_ENERGY_GIVEN_TO_DEME_AT_BIRTH.Get());
    offspring_deme_energy = source_deme_energy * deme_energy_decay * m_world->GetConfig().FRAC_PARENT_ENERGY_GIVEN_TO_DEME_AT_BIRTH.Get();
  }
  
  bool target_successfully_seeded = true;
  
  // Are we using germlines?  If so, we need to mutate the germline to get the
  // genome that we're going to seed the target with.
  if(m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 1) {
    // @JEB Original germlines
    cCPUMemory next_germ(source_deme.GetGermline().GetLatest());
    const cInstSet& instset = m_world->GetHardwareManager().GetInstSet();
    cAvidaContext ctx(m_world->GetRandom());
    
    if(m_world->GetConfig().GERMLINE_COPY_MUT.Get() > 0.0) {
      for(int i=0; i<next_germ.GetSize(); ++i) {
        if(m_world->GetRandom().P(m_world->GetConfig().GERMLINE_COPY_MUT.Get())) {
          next_germ[i] = instset.GetRandomInst(ctx);
        }
      }
    }
    
    if((m_world->GetConfig().GERMLINE_INS_MUT.Get() > 0.0)
       && m_world->GetRandom().P(m_world->GetConfig().GERMLINE_INS_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(next_germ.GetSize() + 1);
      next_germ.Insert(mut_line, instset.GetRandomInst(ctx));
    }
    
    if((m_world->GetConfig().GERMLINE_DEL_MUT.Get() > 0.0)
       && m_world->GetRandom().P(m_world->GetConfig().GERMLINE_DEL_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(next_germ.GetSize());
      next_germ.Remove(mut_line);
    }
    
    // Replace the target deme's germline with the source deme's, and add the newly-
    // mutated germ to ONLY the target's germline.  The source remains unchanged.
    target_deme.ReplaceGermline(source_deme.GetGermline());
    target_deme.GetGermline().Add(next_germ);
        
    // Germline stats tracking.
    m_world->GetStats().GermlineReplication(source_deme.GetGermline(), target_deme.GetGermline());
    
    // All done with the germline manipulation; seed each deme.
    SeedDeme(source_deme, source_deme.GetGermline().GetLatest());
    SeedDeme(target_deme, target_deme.GetGermline().GetLatest());
    
  } else if(m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 2) {
    // @JEB -- New germlines using cGenotype
    
    // get germline genotype
    int germline_genotype_id = source_deme.GetGermlineGenotypeID();
    cGenotype * germline_genotype = m_world->GetClassificationManager().FindGenotype(germline_genotype_id);
    assert(germline_genotype);
    
    // create a new genome by mutation
    cCPUMemory new_genome(germline_genotype->GetGenome());
    const cInstSet& instset = m_world->GetHardwareManager().GetInstSet();
    cAvidaContext ctx(m_world->GetRandom());
    
    if(m_world->GetConfig().GERMLINE_COPY_MUT.Get() > 0.0) {
      for(int i=0; i<new_genome.GetSize(); ++i) {
        if(m_world->GetRandom().P(m_world->GetConfig().GERMLINE_COPY_MUT.Get())) {
          new_genome[i] = instset.GetRandomInst(ctx);
        }
      }
    }
    
    if((m_world->GetConfig().GERMLINE_INS_MUT.Get() > 0.0)
       && m_world->GetRandom().P(m_world->GetConfig().GERMLINE_INS_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(new_genome.GetSize() + 1);
      new_genome.Insert(mut_line, instset.GetRandomInst(ctx));
    }
    
    if((m_world->GetConfig().GERMLINE_DEL_MUT.Get() > 0.0)
       && m_world->GetRandom().P(m_world->GetConfig().GERMLINE_DEL_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(new_genome.GetSize());
      new_genome.Remove(mut_line);
    }
    
    //Create a new genotype which is daughter to the old one.
    cGenotype * new_germline_genotype = m_world->GetClassificationManager().GetGenotype(new_genome, germline_genotype, NULL);
    source_deme.ReplaceGermline(*new_germline_genotype);
    target_deme.ReplaceGermline(*new_germline_genotype);
    SeedDeme(source_deme, *new_germline_genotype);
    SeedDeme(target_deme, *new_germline_genotype);

  } else {
    // Not using germlines; things are much simpler.  Seed the target from the source.
    target_successfully_seeded = SeedDeme(source_deme, target_deme);
  }

  // The source's merit must be transferred to the target, and then the source has
  // to rotate its heritable merit to its current merit.
  if (target_successfully_seeded) target_deme.UpdateDemeMerit(source_deme);
  source_deme.UpdateDemeMerit();
  
  bool source_deme_resource_reset(true), target_deme_resource_reset(true);
  switch(m_world->GetConfig().DEMES_RESET_RESOURCES.Get()) {
    case 0:
      // reset resource in both demes
      source_deme_resource_reset = target_deme_resource_reset = true;
      break;
    case 1:
      // reset resource only in target deme
      source_deme_resource_reset = false;
      target_deme_resource_reset = true;
      break;
    case 2:
      // do not reset either deme resource
      source_deme_resource_reset = target_deme_resource_reset = false;
      break;
    default:
      cout << "Undefined value " << m_world->GetConfig().DEMES_RESET_RESOURCES.Get() << " for DEMES_RESET_RESOURCES\n";
      exit(1);
  }
  
  // Reset both demes, in case they have any cleanup work to do.
  // Must reset target first for stats to be correctly updated!
  if(m_world->GetConfig().ENERGY_ENABLED.Get()) {
    // Transfer energy from source to target if we're using the energy model.
    if (target_successfully_seeded) target_deme.DivideReset(source_deme, target_deme_resource_reset, offspring_deme_energy);
    source_deme.DivideReset(source_deme, source_deme_resource_reset, parent_deme_energy);
  } else {
    // Default; reset both source and target.
    if (target_successfully_seeded) target_deme.DivideReset(source_deme, target_deme_resource_reset);
    source_deme.DivideReset(source_deme, source_deme_resource_reset);
  }

  
  // do our post-replication stats tracking.
  m_world->GetStats().DemePostReplication(source_deme, target_deme);
}


/*! Helper method to seed a deme from the given genome.
If the passed-in deme is populated, all resident organisms are terminated.  The
deme's germline is left unchanged.

@todo Fix lineage label on injected genomes.
@todo Different strategies for non-random placement.
*/
void cPopulation::SeedDeme(cDeme& deme, cGenome& genome) {
  // Kill all the organisms in the deme.
  deme.KillAll();
  
  // Create the specified number of organisms in the deme.
  for(int i=0; i< m_world->GetConfig().DEMES_REPLICATE_SIZE.Get(); ++i) {
    int cellid = DemeSelectInjectionCell(deme, i);
    InjectGenome(cellid, genome, 0);
    DemePostInjection(deme, cell_array[cellid]);
  }
}

void cPopulation::SeedDeme(cDeme& _deme, cGenotype& _genotype) {
  // Kill all the organisms in the deme.
  _deme.KillAll();
  _deme.ClearFounders();
  
  // Create the specified number of organisms in the deme.
  for(int i=0; i< m_world->GetConfig().DEMES_REPLICATE_SIZE.Get(); ++i) {
    int cellid = DemeSelectInjectionCell(_deme, i);
    InjectGenotype(cellid, &_genotype);
    DemePostInjection(_deme, cell_array[cellid]);
    _deme.AddFounder(_genotype);
  }
  
}

/*! Helper method to seed a target deme from the organisms in the source deme.
All organisms in the target deme are terminated, and a subset of the organisms in
the source will be cloned to the target. Returns whether target deme was successfully seeded.
*/
bool cPopulation::SeedDeme(cDeme& source_deme, cDeme& target_deme) {
  cRandom& random = m_world->GetRandom();

  bool successfully_seeded = true;

  // Check to see if we're doing probabilistic organism replication from source
  // to target deme.
  if(m_world->GetConfig().DEMES_PROB_ORG_TRANSFER.Get() == 0.0) {
  
    //@JEB -- old method is default for consistency! 
    if (m_world->GetConfig().DEMES_SEED_METHOD.Get() == 0) {
      // Here's the idea: store up a list of the genotypes from the source that we
      // need to copy to the target. Then clear both the source and target demes, 
      // and finally inject organisms from the saved genotypes into both the source 
      // and target.
      //
      // This is a little ugly - Note that if you're trying to get something a little
      // more random, there's also the "fruiting body" option (DEMES_PROB_ORG_TRANSFER),
      // and the even less contrived MIGRATION_RATE.
      //
      // @todo In order to get lineage tracking to work again, we need to change this
      //       from tracking cGenomes to tracking cGenotypes.  But that's a pain,
      //       because the cGenotype* from cOrganism::GetGenotype may not live after
      //       a call to cDeme::KillAll.
      std::vector<std::pair<cGenome,int> > xfer; // List of genomes we're going to transfer.
    
      switch(m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get()) {
        case 0: { // Random w/ replacement (meaning, we don't prevent the same genotype from
          // being selected more than once).
          while((int)xfer.size() < m_world->GetConfig().DEMES_REPLICATE_SIZE.Get()) {
            int cellid = source_deme.GetCellID(random.GetUInt(source_deme.GetSize()));
            if(cell_array[cellid].IsOccupied()) {
              xfer.push_back(std::make_pair(cell_array[cellid].GetOrganism()->GetGenome(),
                                            cell_array[cellid].GetOrganism()->GetLineageLabel()));
            }
          }
          break;
        }
        case 1: { // Sequential selection, from the beginning.  Good with DEMES_ORGANISM_PLACEMENT=3.
          for(int i=0; i<m_world->GetConfig().DEMES_REPLICATE_SIZE.Get(); ++i) {
            int cellid = source_deme.GetCellID(i);
            if(cell_array[cellid].IsOccupied()) {
              xfer.push_back(std::make_pair(cell_array[cellid].GetOrganism()->GetGenome(),
                                            cell_array[cellid].GetOrganism()->GetLineageLabel()));
            }
          }
          break;
        }
        default: {
          cout << "Undefined value (" << m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get()
            << ") for DEMES_ORGANISM_SELECTION." << endl;
          exit(1);
        }
      }   
      // We'd better have at *least* one genome.
      assert(xfer.size()>0);
      
      // Clear the demes.
      source_deme.UpdateStats();
      source_deme.KillAll();

      target_deme.UpdateStats();
      target_deme.KillAll();

      // And now populate the source and target.
      int j=0;
      for(std::vector<std::pair<cGenome,int> >::iterator i=xfer.begin(); i!=xfer.end(); ++i, ++j) {
        int cellid = DemeSelectInjectionCell(source_deme, j);
        InjectGenome(cellid, i->first, i->second);
        DemePostInjection(source_deme, cell_array[cellid]);

        cellid = DemeSelectInjectionCell(target_deme, j);
        InjectGenome(cellid, i->first, i->second);
        DemePostInjection(target_deme, cell_array[cellid]);      
      }
    } else /* if (m_world->GetConfig().DEMES_SEED_METHOD.Get() != 0) */{
    
      // @JEB
      // Updated seed deme method that maintains genotype inheritance.
      
      // deconstruct founders into two lists...
      tArray<cOrganism*> source_founders; // List of organisms we're going to transfer.
      tArray<cOrganism*> target_founders; // List of organisms we're going to transfer.
      
                  /*
      // Debug Code
      cGenotype * original_source_founder_genotype = NULL;
      if (1) {
        tArray<int>& source_founders = source_deme.GetFounderGenotypeIDs();
        if (source_founders.GetSize() > 0) {
          original_source_founder_genotype = m_world->GetClassificationManager().FindGenotype(source_founders[0]);
          cout << "Source:" << endl << original_source_founder_genotype->GetGenome().AsString() << endl;
        }
        tArray<int>& target_founders = target_deme.GetFounderGenotypeIDs();
        if (target_founders.GetSize() > 0) {
          cGenotype * target_founder_genotype = m_world->GetClassificationManager().FindGenotype(target_founders[0]);
          cout << "Target:" << endl << target_founder_genotype->GetGenome().AsString() << endl;
        }
      }

      tArray<int>& source_founders = source_deme.GetFounders();
      cerr << "Original source genotype ids:" << endl;
      for(int i=0; i<source_founders.GetSize(); i++) {
        cerr << source_founders[i] << " ";
      }
      cerr << endl;
  
      tArray<int>& target_founders = target_deme.GetFounders();
      cerr << "Original target genotype ids:" << endl;
      for(int i=0; i<target_founders.GetSize(); i++) {
        cerr << target_founders[i] << " ";
      }
      cerr << endl;
      
      // Debug Code
      //// Count the number of orgs in each deme.
      int count = 0;
      for(int i=0; i<target_deme.GetSize(); ++i) {
        int cell_id = target_deme.GetCellID(i);
        if(cell_array[cell_id].IsOccupied()) count++;
      }
      cout << "Initial orgs in target deme: " << count << endl;

      count = 0;
      for(int i=0; i<source_deme.GetSize(); ++i) {
        int cell_id = source_deme.GetCellID(i);
        if(cell_array[cell_id].IsOccupied()) count++;
      }
      cout << "Initial orgs in source deme: " << count << endl;     
      */

      
      switch(m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get()) {
        case 0: { // Random w/ replacement (meaning, we don't prevent the same genotype from
          // being selected more than once).
          tArray<cOrganism*> founders; // List of organisms we're going to transfer.
          while(founders.GetSize() < m_world->GetConfig().DEMES_REPLICATE_SIZE.Get()) {
            int cellid = source_deme.GetCellID(random.GetUInt(source_deme.GetSize()));
            if(cell_array[cellid].IsOccupied()) {
              founders.Push(cell_array[cellid].GetOrganism());
            }
          }
          source_founders = founders;
          target_founders = founders;
          break;
        }
        case 1: { // Sequential selection, from the beginning.  Good with DEMES_ORGANISM_PLACEMENT=3.
          tArray<cOrganism*> founders; // List of organisms we're going to transfer.
          for(int i=0; i<m_world->GetConfig().DEMES_REPLICATE_SIZE.Get(); ++i) {
            int cellid = source_deme.GetCellID(i);
            if(cell_array[cellid].IsOccupied()) {
              founders.Push(cell_array[cellid].GetOrganism());
            }
          }
          source_founders = founders;
          target_founders = founders;
          break;
        }
        case 6: { // Random w/o replacement.  Take as many for each deme as DEME_REPLICATE_SIZE.
          tList<cOrganism> prospective_founders;
          
          //Die if not >= two organisms. 
          if (source_deme.GetOrgCount() >= 2) {
          
            //Collect prospective founder organisms into a list
            for (int i=0; i < source_deme.GetSize(); i++) {
              if ( GetCell(source_deme.GetCellID(i)).IsOccupied() ) {
                  prospective_founders.Push( GetCell(source_deme.GetCellID(i)).GetOrganism() );
              }
            }

            //add orgs alternately to source and target founders until
            //we run out or each reaches DEMES_REPLICATE_SIZE.
            int num_chosen_orgs = 0;
            while( ((target_founders.GetSize() < m_world->GetConfig().DEMES_REPLICATE_SIZE.Get())
                || (source_founders.GetSize() < m_world->GetConfig().DEMES_REPLICATE_SIZE.Get()))
                && (prospective_founders.GetSize() > 0) ) {

              int chosen_org = random.GetUInt(prospective_founders.GetSize());
              if (num_chosen_orgs % 2 == 0) {
                source_founders.Push(prospective_founders.PopPos(chosen_org));
              } else {
                target_founders.Push(prospective_founders.PopPos(chosen_org));
              } 
                          
              num_chosen_orgs++;
            }
          }
          break;
        }

        case 2: 
        case 3:
        case 4:
        case 5:
        { // Selection based on germline propensities.
          // 2: sum the germline propensities of all organisms
          // and pick TWO based on each organism getting a 
          // weighted probability of being germline
          // 3: treat germline propensities as zero or nonzero for picking
          // 4: same as 3: but replication to target fails if only one germ.
          // 5: same as 3: but replication fails and source dies if fewer than two germs.
          tArray<cOrganism*> founders; // List of organisms we're going to transfer.

          if (source_deme.GetOrgCount() >= 2) {
          
            if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() != 0) {
              m_world->GetDriver().RaiseFatalException(1, "Germline DEMES_ORGANISM_SELECTION methods 2 and 3 can only be used with DEMES_DIVIDE_METHOD 0.");
            }
            
            tArray<cOrganism*> prospective_founders;
            
            cDoubleSum gp_sum;
            double min = -1;
            for (int i=0; i < source_deme.GetSize(); i++) {
              if ( GetCell(source_deme.GetCellID(i)).IsOccupied() ) {
                double gp = GetCell(source_deme.GetCellID(i)).GetOrganism()->GetPhenotype().GetPermanentGermlinePropensity();
                if (gp > 0.0) {
                  gp_sum.Add( gp );
                  prospective_founders.Push( GetCell(source_deme.GetCellID(i)).GetOrganism() );
                }
                //cout << gp << " ";
                if ( (min == -1) || (gp < min) ) min = gp;
              }
            }
            
            if (m_world->GetVerbosity() >= VERBOSE_ON) cout << "Germline Propensity Sum: " << gp_sum.Sum() << endl;
            if (m_world->GetVerbosity() >= VERBOSE_ON) cout << "Num prospective founders: " << prospective_founders.GetSize() << endl;
            
            if (prospective_founders.GetSize() < 2) {
            
              // there were not enough orgs with nonzero germlines
              // pick additional orgs at random without replacement,
              // unless our method forbids this

              // leave the founder list empty for method 5
              if (m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() != 5) {
              
                founders = prospective_founders;
                
                //do not add additional founders randomly for method 4
                if (m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() != 4) {
                
                  while(founders.GetSize() < 2) {
                    int cellid = source_deme.GetCellID(random.GetUInt(source_deme.GetSize()));
                    if( cell_array[cellid].IsOccupied() ) {
                      cOrganism * org = cell_array[cellid].GetOrganism();
                      bool found = false;
                      for(int i=0; i< founders.GetSize(); i++) {
                        if (founders[i] == org) found = true;
                      }
                      if (!found) founders.Push(cell_array[cellid].GetOrganism());
                    }
                  }
                }  
              }
            } else {
            
            // pick two orgs based on germline propensities from prospective founders

              while(founders.GetSize() < 2) {
              
                double choice = (m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() == 2)
                  ? random.GetDouble( gp_sum.Sum() ) : random.GetDouble( gp_sum.Count() );


                //cout <<  "Count: " << gp_sum.Count() << endl;
                
                // find the next organism to choose
                cOrganism * org = NULL;
                int on = 0;
                while( (choice > 0) && (on < prospective_founders.GetSize()) ) {
                  org = prospective_founders[on];
                  
                  // did we already have this org?
                  bool found = false;
                  for(int i=0; i< founders.GetSize(); i++) {
                    if (founders[i] == org) found = true;
                  }
                  
                  // if it wasn't already chosen, then we count down...
                  if (!found) {
                    choice -= (m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() == 2) 
                      ? org->GetPhenotype().GetPermanentGermlinePropensity()
                      : (org->GetPhenotype().GetPermanentGermlinePropensity() > 0);
                  }
                  on++;
                }
                
                gp_sum.Subtract(org->GetPhenotype().GetPermanentGermlinePropensity());
                assert(org);
                founders.Push(org);
              }
            }      
            
            if (founders.GetSize() > 0) source_founders.Push(founders[0]);
            if (founders.GetSize() > 1) target_founders.Push(founders[1]);       
            
            /*
            // Debug Code
            cout << endl;
            cout << "sum " << gp_sum.Sum() << endl;
            cout << "min " << min << endl;
            cout << "choice " << choice << endl;
            */
              
            //cout << "Chose germline propensity " << chosen_org->GetPhenotype().GetLastGermlinePropensity() << endl;
          } else { 
            //Failure because we didn't have at least two organisms...
            //m_world->GetDriver().RaiseFatalException(1, "Germline DEMES_ORGANISM_SELECTION method didn't find at least two organisms in deme.");
          }
        
        break;
        }  
        default: {
          cout << "Undefined value (" << m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get()
            << ") for DEMES_ORGANISM_SELECTION." << endl;
          exit(1);
        }
      }
      

      // We'd better have at *least* one genome.
      // Methods that require a germline can sometimes come up short...
      //assert(source_founders.GetSize()>0);
      //assert(target_founders.GetSize()>0);

      // We clear the deme, but trick cPopulation::KillOrganism
      // to NOT delete the organisms, by pretending 
      // the orgs are running. This way we can still create 
      // clones of them that will track stats correctly.
      // We also need to defer adjusting the genotype
      // or it will be prematurely deleted before we are done!
      
     // cDoubleSum gen;
      tArray<cOrganism*> old_source_organisms;
      for(int i=0; i<source_deme.GetSize(); ++i) {
        int cell_id = source_deme.GetCellID(i);
          
        if(cell_array[cell_id].IsOccupied()) {
          cOrganism * org = cell_array[cell_id].GetOrganism();
          old_source_organisms.Push(org);
          org->SetRunning(true);
          org->GetGenotype()->IncDeferAdjust();
          
         // cout << org->GetPhenotype().GetGeneration()-source_deme.GetAvgFounderGeneration() << " ";
         // gen.Add(org->GetPhenotype().GetGeneration()-source_deme.GetAvgFounderGeneration());
        }
      }  
      //cout << endl;
      //cout << "Average: " << gen.Average() << endl;

      
      tArray<cOrganism*> old_target_organisms;
      for(int i=0; i<target_deme.GetSize(); ++i) {
        int cell_id = target_deme.GetCellID(i);
          
        if(cell_array[cell_id].IsOccupied()) {
          cOrganism * org = cell_array[cell_id].GetOrganism();
          old_target_organisms.Push(org);
          org->SetRunning(true);
          org->GetGenotype()->IncDeferAdjust();
        }
      }
      
      // Clear the target deme (if we have successfully replaced it).
      if (target_founders.GetSize() > 0) {
        target_deme.ClearFounders();
        target_deme.UpdateStats();
        target_deme.KillAll();
      } else {
        successfully_seeded = false;
      }
      
      //cout << founders.GetSize() << " founders." << endl;

      // Now populate the target (and optionally the source) using InjectGenotype.
      // In the future InjectClone could be used, but this would require the 
      // deme keeping complete copies of the founder organisms when
      // we wanted to re-seed from the original founders.
      for(int i=0; i<target_founders.GetSize(); i++) {
          int cellid = DemeSelectInjectionCell(target_deme, i);
          InjectDemeFounder(cellid, *target_founders[i]->GetGenotype(), &target_founders[i]->GetPhenotype());
          target_deme.AddFounder(*target_founders[i]->GetGenotype(), &target_founders[i]->GetPhenotype());
          DemePostInjection(target_deme, cell_array[cellid]);
      }
      
      // We either repeat this procedure in the source deme,
      // restart the source deme from its old founders, 
      // or do nothing to the source deme...
      
      // source deme is replaced in the same way as the target
      if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 0) {    

        source_deme.ClearFounders();
        source_deme.UpdateStats();
        source_deme.KillAll();
        
        for(int i=0; i<source_founders.GetSize(); i++) {
          int cellid = DemeSelectInjectionCell(source_deme, i); 
          InjectDemeFounder(cellid, *source_founders[i]->GetGenotype(), &source_founders[i]->GetPhenotype());
          source_deme.AddFounder(*source_founders[i]->GetGenotype(), &source_founders[i]->GetPhenotype());
          DemePostInjection(source_deme, cell_array[cellid]);
        }
      }
      // source deme is "reset" by re-injecting founder genotypes
      else if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 1) {
        // Do not update the last founder generation, since the founders have not changed.
        
        source_deme.UpdateStats();
        source_deme.KillAll();
        // do not clear or change founder list
        
        // use it to recreate ancestral state of genotypes
        tArray<int>& source_founders = source_deme.GetFounderGenotypeIDs();
        tArray<cPhenotype>& source_founder_phenotypes = source_deme.GetFounderPhenotypes();
        for(int i=0; i<source_founders.GetSize(); i++) {
        
          int cellid = DemeSelectInjectionCell(source_deme, i);
          //cout << "founder: " << source_founders[i] << endl;
          cGenotype * genotype = m_world->GetClassificationManager().FindGenotype(source_founders[i]);
          InjectDemeFounder(cellid, *genotype, &source_founder_phenotypes[i]);
          DemePostInjection(source_deme, cell_array[cellid]);
        }
        
        //cout << target_deme.GetOrgCount() << " target orgs." << endl;
        //cout << source_deme.GetOrgCount() << " source orgs." << endl;
      }
      // source deme is left untouched
      else if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 2) {
      }
      else {
        m_world->GetDriver().RaiseFatalException(1, "Unknown DEMES_DIVIDE_METHOD");
      }
       
      /*
      // Debug Code
      //// Count the number of orgs in each deme.
      count = 0;
      for(int i=0; i<target_deme.GetSize(); ++i) {
        int cell_id = target_deme.GetCellID(i);
        if(cell_array[cell_id].IsOccupied()) count++;
      }
      cout << "Final orgs in target deme: " << count << endl;

      count = 0;
      for(int i=0; i<source_deme.GetSize(); ++i) {
        int cell_id = source_deme.GetCellID(i);
        if(cell_array[cell_id].IsOccupied()) count++;
      }
      cout << "Final orgs in source deme: " << count << endl;

      if (1) {
        tArray<int>& source_founders = source_deme.GetFounderGenotypeIDs();
        cGenotype * source_founder_genotype = m_world->GetClassificationManager().FindGenotype(source_founders[0]);
        tArray<int>& target_founders = target_deme.GetFounderGenotypeIDs();
        cGenotype * target_founder_genotype = m_world->GetClassificationManager().FindGenotype(target_founders[0]);
        if (original_source_founder_genotype->GetGenome().AsString() != source_founder_genotype->GetGenome().AsString())
        {
          cout << "Original source founder does not equal final source founder!!!!" << endl;
        }
        
        cout << "Source:" << endl << source_founder_genotype->GetGenome().AsString() << endl;
        cout << "Target:" << endl << target_founder_genotype->GetGenome().AsString() << endl;
      }
      
      // Debug
      tArray<int>& new_source_founders = source_deme.GetFounders();
      cerr << "New source genotype ids:" << endl;
      for(int i=0; i<new_source_founders.GetSize(); i++) {
        cerr << new_source_founders[i] << " ";
      }
      cerr << endl;
  
      tArray<int>& new_target_founders = target_deme.GetFounders();
      cerr << "New target genotype ids:" << endl;
      for(int i=0; i<new_target_founders.GetSize(); i++) {
        cerr << new_target_founders[i] << " ";
      }
      cerr << endl;
      */
              
      // remember to delete the old target organisms and adjust their genotypes
        for(int i=0; i<old_target_organisms.GetSize(); ++i) {
          old_target_organisms[i]->SetRunning(false);
          cGenotype * genotype = old_target_organisms[i]->GetGenotype();
          genotype->DecDeferAdjust();
          m_world->GetClassificationManager().AdjustGenotype(*genotype);
          
          // ONLY delete target orgs if seeding was successful
          // otherwise they still exist in the population!!!
          if (successfully_seeded) delete old_target_organisms[i];
        }
      
      for(int i=0; i<old_source_organisms.GetSize(); ++i) {
        old_source_organisms[i]->SetRunning(false);
        cGenotype * genotype = old_source_organisms[i]->GetGenotype();
        genotype->DecDeferAdjust();
        m_world->GetClassificationManager().AdjustGenotype(*genotype);
        
        // delete old source organisms ONLY if source was replaced
        if ( (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 0)
          || (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 1) ) {
        delete old_source_organisms[i];
        }
      }
      
    } //End DEMES_PROB_ORG_TRANSFER > 0 methods
  } else {
    // Probabilistic organism replication -- aka "fruiting body."
    //
    // This is a little different than the other deme replication styles.  First,
    // in a fruiting body we don't reset the individuals in the source deme; we
    // leave them unchanged.  The idea is that we're pulling random organisms
    // out, and letting the source continue.
    
    // Kill all the organisms in the target deme.
    target_deme.KillAll();

    // Check all the organisms in the source deme to see if they get transferred
    // to the target.
    for(int i=0,j=0; i<source_deme.GetSize(); ++i) {
      int source_cellid = source_deme.GetCellID(i);
      
      // Does this organism stay with the source or move to the target?
      if(cell_array[source_cellid].IsOccupied() && random.P(m_world->GetConfig().DEMES_PROB_ORG_TRANSFER.Get())) {
        // Moves to the target; save the genome and lineage label of organism being transfered.
        cOrganism* seed = cell_array[source_cellid].GetOrganism();
        cGenome genome = seed->GetGenome();
        int lineage = seed->GetLineageLabel();
        seed = 0; // We're done with the seed organism.
        
        // Remove this organism from the source.
        KillOrganism(cell_array[source_cellid]);
        
        // And inject it into target deme.
        int target_cellid = DemeSelectInjectionCell(target_deme, j++);
        InjectGenome(target_cellid, genome, lineage);
        DemePostInjection(target_deme, cell_array[target_cellid]);
      } 
      //else {
      // Stays with the source.  Nothing to do here yet.
      //}
    }
  }
  
  return successfully_seeded;
}

void cPopulation::InjectDemeFounder(int _cell_id, cGenotype& _genotype, cPhenotype* _phenotype)
{
  // phenotype can be NULL
  
  InjectGenotype(_cell_id, &_genotype);
  
  // At this point, the cell had better be occupied...
  assert(GetCell(_cell_id).IsOccupied());
  cOrganism * organism = GetCell(_cell_id).GetOrganism();


  //Now we need to set up the phenotype of this organism...
  if (_phenotype) {
    //If we want founders to have their proper phenotypes
    // then we might do something like this... (untested)
    //organism->SetPhenotype(*_phenotype);
    // Re-initialize the time-slice for this new organism.
    //AdjustSchedule(_cell_id, organism->GetPhenotype().GetMerit());

    // For now, just copy the generation...
    organism->GetPhenotype().SetGeneration( _phenotype->GetGeneration() );
    
    // and germline propensity.
    organism->GetPhenotype().SetPermanentGermlinePropensity( _phenotype->GetPermanentGermlinePropensity()  );
    
    if (m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get() >= 0.0) {
      organism->GetPhenotype().SetPermanentGermlinePropensity( m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get() );
    }
  }
  
  /* It requires much more than this to correctly implement, do later if needed @JEB
  //Optionally, set the first organism's merit to a constant value
  //actually, we need to add so the first organism is seeded this way too...
  if (m_world->GetConfig().DEMES_DEFAULT_MERIT.Get()) {
    organism->GetPhenotype().SetMerit( cMerit(m_world->GetConfig().DEMES_DEFAULT_MERIT.Get()) );
    AdjustSchedule(GetCell(_cell_id), organism->GetPhenotype().GetMerit());
  }
  */
}


/*! Helper method that determines the cell into which an organism will be placed.
Respects all of the different placement options that are relevant for deme replication.

@param sequence The number of times DemeSelectInjectionCell has been called on this deme
                already.  Used for replicating multiple organisms from the source to the
                target deme in a sensible fashion.
*/
int cPopulation::DemeSelectInjectionCell(cDeme& deme, int sequence) {
  int cellid = -1;
  
  assert(sequence < deme.GetSize()); // cannot inject a sequence bigger then the deme
  
  switch(m_world->GetConfig().DEMES_ORGANISM_PLACEMENT.Get()) {
    case 0: { // Array-middle.
      cellid = deme.GetCellID((deme.GetSize()/2 + sequence) % deme.GetSize());
      break;
    }
    case 1: { // Sequential placement, start in the center of the deme.
      assert(sequence == 0);  // not sure how to handle a sequence in this case (Ben)
      cellid = deme.GetCellID(deme.GetWidth()/2, deme.GetHeight()/2);
      break;
    }
    case 2: { // Random placement.
      cellid = deme.GetCellID(m_world->GetRandom().GetInt(0, deme.GetSize()-1));
      while(cell_array[cellid].IsOccupied()) {
        cellid = deme.GetCellID(m_world->GetRandom().GetInt(0, deme.GetSize()-1));
      }
      break;
    }
    case 3: { // Sequential.
      cellid = deme.GetCellID(sequence);
      break;
    }
    default: {
      assert(false); // Shouldn't ever reach here.
    }
  }
  
  assert(cellid >= 0 && cellid < cell_array.GetSize());
  assert(cellid >= deme.GetCellID(0));
  assert(cellid <= deme.GetCellID(deme.GetSize()-1));
  return cellid;  
}


/*! Helper method to perform any post-injection fixups on the organism/cell that
was injected into a deme.  Handles all the rotation / facing options.
*/
void cPopulation::DemePostInjection(cDeme& deme, cPopulationCell& cell) {
  assert(cell.GetID() >= deme.GetCellID(0));
  assert(cell.GetID() <= deme.GetCellID(deme.GetSize()-1));
  switch(m_world->GetConfig().DEMES_ORGANISM_FACING.Get()) {
    case 0: { // Unchanged.
      break;
    }
    case 1: { // Spin cell to face NW.
      cell.Rotate(cell_array[GridNeighbor(cell.GetID()-deme.GetCellID(0),
                                          deme.GetWidth(),
                                          deme.GetHeight(), -1, -1)+deme.GetCellID(0)]);
      break; 
    }
    case 2: { // Spin cell to face randomly.
      const int rotate_count = m_world->GetRandom().GetInt(0, cell.ConnectionList().GetSize());
      for(int i=0; i<rotate_count; ++i) {
        cell.ConnectionList().CircNext();
      }
      break;
    }
    default: {
      assert(false);
    }
  }
}


// Loop through all demes to determine if any are ready to be divided.  All
// full demes have 1/2 of their organisms (the odd ones) moved into a new deme.

void cPopulation::DivideDemes()
{
  // Determine which demes should be replicated.
  const int num_demes = GetNumDemes();
  cRandom & random = m_world->GetRandom();
  
  // Loop through all candidate demes...
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cDeme & source_deme = deme_array[deme_id];
    
    // Only divide full demes.
    if (source_deme.IsFull() == false) continue;
    
    // Choose a random target deme to replicate to...
    int target_id = deme_id;
    while (target_id == deme_id) target_id = random.GetUInt(num_demes);
    cDeme & target_deme = deme_array[target_id];
    const int deme_size = target_deme.GetSize();
    
    // Clear out existing cells in target deme.
    for (int i = 0; i < deme_size; i++) {
      KillOrganism(cell_array[ target_deme.GetCellID(i) ]);
    }
    
    // Setup an array to collect the total number of tasks performed.
    const int num_tasks = cell_array[source_deme.GetCellID(0)].GetOrganism()->
      GetPhenotype().GetLastTaskCount().GetSize();
    tArray<int> tot_tasks(num_tasks);
    tot_tasks.SetAll(0);
    
    // Move over the odd numbered cells.
    for (int pos = 0; pos < deme_size; pos += 2) {
      const int cell1_id = source_deme.GetCellID( pos+1 );
      const int cell2_id = target_deme.GetCellID( pos );
      cOrganism * org1 = cell_array[cell1_id].GetOrganism();
      
      // Keep track of what tasks have been done.
      const tArray<int> & cur_tasks = org1->GetPhenotype().GetLastTaskCount();
      for (int i = 0; i < num_tasks; i++) {
        tot_tasks[i] += cur_tasks[i];
      }
      
      // Inject a copy of the odd organisms into the even cells.
      InjectClone( cell2_id, *org1 );    
      
      // Kill the organisms in the odd cells.
      KillOrganism( cell_array[cell1_id] );
    }
    
    // Figure out the merit each organism should have.
    int merit = 100;
    for (int i = 0; i < num_tasks; i++) {
      if (tot_tasks[i] > 0) merit *= 2;
    }
    
    // Setup the merit of both old and new individuals.
    for (int pos = 0; pos < deme_size; pos += 2) {
      cell_array[source_deme.GetCellID(pos)].GetOrganism()->UpdateMerit(merit);
      cell_array[target_deme.GetCellID(pos)].GetOrganism()->UpdateMerit(merit);
    }
    
  }
}


// Reset Demes goes through each deme and resets the individual organisms as
// if they were just injected into the population.

void cPopulation::ResetDemes()
{
  // re-inject all demes into themselves to reset them.
  for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
    for (int i = 0; i < deme_array[deme_id].GetSize(); i++) {
      int cur_cell_id = deme_array[deme_id].GetCellID(i);
      if (cell_array[cur_cell_id].IsOccupied() == false) continue;
      InjectClone( cur_cell_id, *(cell_array[cur_cell_id].GetOrganism()) );
    }
  }
}


// Copy the full contents of one deme into another.

void cPopulation::CopyDeme(int deme1_id, int deme2_id)
{
  cDeme & deme1 = deme_array[deme1_id];
  cDeme & deme2 = deme_array[deme2_id];
  
  for (int i = 0; i < deme1.GetSize(); i++) {
    int from_cell = deme1.GetCellID(i);
    int to_cell = deme2.GetCellID(i);
    if (cell_array[from_cell].IsOccupied() == false) {
      KillOrganism(cell_array[to_cell]);
      continue;
    }
    InjectClone( to_cell, *(cell_array[from_cell].GetOrganism()) );    
  }
}


// Print out statistics about donations
                  
void cPopulation::PrintDonationStats()
{
  cDoubleSum donation_makers;
  cDoubleSum donation_receivers;
  cDoubleSum donation_cheaters;

  cDoubleSum edit_donation_makers;
  cDoubleSum edit_donation_receivers;
  cDoubleSum edit_donation_cheaters;

  cDoubleSum kin_donation_makers;
  cDoubleSum kin_donation_receivers;
  cDoubleSum kin_donation_cheaters;

  cDoubleSum threshgb_donation_makers;
  cDoubleSum threshgb_donation_receivers;
  cDoubleSum threshgb_donation_cheaters;

  cDoubleSum quanta_threshgb_donation_makers;
  cDoubleSum quanta_threshgb_donation_receivers;
  cDoubleSum quanta_threshgb_donation_cheaters;

  cStats& stats = m_world->GetStats();

  cDataFile & dn_donors = m_world->GetDataFile("donations.dat");
  dn_donors.WriteComment("Info about organisms giving donations in the population");
  dn_donors.WriteTimeStamp();
  dn_donors.Write(stats.GetUpdate(), "update");


  for (int i = 0; i < cell_array.GetSize(); i++)
    {
      // Only look at cells with organisms in them.
      if (cell_array[i].IsOccupied() == false) continue;
      cOrganism * organism = cell_array[i].GetOrganism();
      const cPhenotype & phenotype = organism->GetPhenotype();
   
      // donors & receivers in general
      if (phenotype.IsDonorLast()) donation_makers.Add(1);       //found a donor
      if (phenotype.IsReceiverLast()){
        donation_receivers.Add(1);                              //found a receiver
        if (phenotype.IsDonorLast()==0){
          donation_cheaters.Add(1);                             //found a receiver whose parent did not give
        }
      }
      // edit donors & receivers
      if (phenotype.IsDonorEditLast()) edit_donation_makers.Add(1);       //found a edit donor
      if (phenotype.IsReceiverEditLast()){
        edit_donation_receivers.Add(1);                              //found a edit receiver
        if (phenotype.IsDonorEditLast()==0){
          edit_donation_cheaters.Add(1);                             //found a edit receiver whose parent did...
        }                                                              //...not make a edit donation
      }

      // kin donors & receivers
      if (phenotype.IsDonorKinLast()) kin_donation_makers.Add(1);       //found a kin donor
      if (phenotype.IsReceiverKinLast()){
        kin_donation_receivers.Add(1);                              //found a kin receiver
        if (phenotype.IsDonorKinLast()==0){
          kin_donation_cheaters.Add(1);                             //found a kin receiver whose parent did...
        }                                                              //...not make a kin donation
      }

      // threshgb donors & receivers
      if (phenotype.IsDonorThreshGbLast()) threshgb_donation_makers.Add(1); //found a threshgb donor
      if (phenotype.IsReceiverThreshGbLast()){
        threshgb_donation_receivers.Add(1);                              //found a threshgb receiver
        if (phenotype.IsDonorThreshGbLast()==0){
          threshgb_donation_cheaters.Add(1);                             //found a threshgb receiver whose parent did...
        }                                                              //...not make a threshgb donation
      }

      // quanta_threshgb donors & receivers
      if (phenotype.IsDonorQuantaThreshGbLast()) quanta_threshgb_donation_makers.Add(1); //found a quanta threshgb donor
      if (phenotype.IsReceiverQuantaThreshGbLast()){
        quanta_threshgb_donation_receivers.Add(1);                              //found a quanta threshgb receiver
        if (phenotype.IsDonorQuantaThreshGbLast()==0){
          quanta_threshgb_donation_cheaters.Add(1);                             //found a quanta threshgb receiver whose parent did...
        }                                                              //...not make a quanta threshgb donation
      }

    }

  dn_donors.Write(donation_makers.Sum(), "parent made at least one donation");
  dn_donors.Write(donation_receivers.Sum(), "parent received at least one donation");
  dn_donors.Write(donation_cheaters.Sum(),  "parent received at least one donation but did not make one");
  dn_donors.Write(edit_donation_makers.Sum(), "parent made at least one edit_donation");
  dn_donors.Write(edit_donation_receivers.Sum(), "parent received at least one edit_donation");
  dn_donors.Write(edit_donation_cheaters.Sum(),  "parent received at least one edit_donation but did not make one");
  dn_donors.Write(kin_donation_makers.Sum(), "parent made at least one kin_donation");
  dn_donors.Write(kin_donation_receivers.Sum(), "parent received at least one kin_donation");
  dn_donors.Write(kin_donation_cheaters.Sum(),  "parent received at least one kin_donation but did not make one");
  dn_donors.Write(threshgb_donation_makers.Sum(), "parent made at least one threshgb_donation");
  dn_donors.Write(threshgb_donation_receivers.Sum(), "parent received at least one threshgb_donation");
  dn_donors.Write(threshgb_donation_cheaters.Sum(),  "parent received at least one threshgb_donation but did not make one");
  dn_donors.Write(quanta_threshgb_donation_makers.Sum(), "parent made at least one quanta_threshgb_donation");
  dn_donors.Write(quanta_threshgb_donation_receivers.Sum(), "parent received at least one quanta_threshgb_donation");
  dn_donors.Write(quanta_threshgb_donation_cheaters.Sum(),  "parent received at least one quanta_threshgb_donation but did not make one");

  dn_donors.Endl();
}
// Copy a single indvidual out of a deme into a new one (which is first purged
// of existing organisms.)

void cPopulation::SpawnDeme(int deme1_id, int deme2_id)
{
  // Must spawn into a different deme.
  assert(deme1_id != deme2_id);
  
  const int num_demes = deme_array.GetSize();
  
  // If the second argument is a -1, choose a deme at random.
  cRandom & random = m_world->GetRandom();
  while (deme2_id == -1 || deme2_id == deme1_id) {
    deme2_id = random.GetUInt(num_demes);
  }
  
  // Make sure we have all legal values...
  assert(deme1_id >= 0 && deme1_id < num_demes);
  assert(deme2_id >= 0 && deme2_id < num_demes);
  
  // Find the demes that we're working with.
  cDeme & deme1 = deme_array[deme1_id];
  cDeme & deme2 = deme_array[deme2_id];
  
  // Make sure that the deme we're copying from has at least 1 organism.
  assert(deme1.GetOrgCount() > 0);
  
  // Determine the cell to copy from.
  int cell1_id = deme1.GetCellID( random.GetUInt(deme1.GetSize()) );
  while (cell_array[cell1_id].IsOccupied() == false) {
    cell1_id = deme1.GetCellID( random.GetUInt(deme1.GetSize()) );
  }
  
  // Clear out existing cells in target deme.
  for (int i = 0; i < deme2.GetSize(); i++) {
    KillOrganism(cell_array[ deme2.GetCellID(i) ]);
  }
  
  // And do the spawning.
  int cell2_id = deme2.GetCellID( random.GetUInt(deme2.GetSize()) );
  InjectClone( cell2_id, *(cell_array[cell1_id].GetOrganism()) );    
}

void cPopulation::AddDemePred(cString type, int times) {
  if(type == "EventReceivedCenter") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventReceivedCenterPred(times);
    }
  } else if(type == "EventReceivedLeftSide") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventReceivedLeftSidePred(times);
    }
  } else if(type == "EventMovedIntoCenter") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventMoveCenterPred(times);
    }  
  } else if(type == "EventMovedBetweenTargets") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventMoveBetweenTargetsPred(times);
    }  
  } else if(type == "EventNUniqueIndividualsMovedIntoTarget") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventEventNUniqueIndividualsMovedIntoTargetPred(times);
    }  
  } else {
    cout << "Unknown Predicate\n";
    exit(1);
  }
}

void cPopulation::CheckImplicitDemeRepro(cDeme& deme) {

  if (GetNumDemes() <= 1) return;
  
  if (m_world->GetConfig().DEMES_REPLICATE_CPU_CYCLES.Get() 
    && (deme.GetTimeUsed() >= m_world->GetConfig().DEMES_REPLICATE_CPU_CYCLES.Get())) ReplicateDeme(deme);
  else if (m_world->GetConfig().DEMES_REPLICATE_TIME.Get() 
    && (deme.GetNormalizedTimeUsed() >= m_world->GetConfig().DEMES_REPLICATE_TIME.Get())) ReplicateDeme(deme); 
  else if (m_world->GetConfig().DEMES_REPLICATE_BIRTHS.Get() 
    && (deme.GetBirthCount() >= m_world->GetConfig().DEMES_REPLICATE_BIRTHS.Get())) ReplicateDeme(deme); 
  else if (m_world->GetConfig().DEMES_REPLICATE_ORGS.Get() 
    && (deme.GetOrgCount() >= m_world->GetConfig().DEMES_REPLICATE_ORGS.Get())) ReplicateDeme(deme);      
}

// Print out all statistics about individual demes
void cPopulation::PrintDemeAllStats() {
  PrintDemeFitness();
  PrintDemeLifeFitness();
  PrintDemeMerit();
  PrintDemeGestationTime();
  PrintDemeTasks();
  PrintDemeDonor();
  PrintDemeReceiver();
  PrintDemeMutationRate();
  PrintDemeResource();
  PrintDemeInstructions();
    
  if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
    PrintDemeSpatialEnergyData();
    PrintDemeSpatialSleepData();
  }
}

void cPopulation::PrintDemeDonor() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  cDataFile & df_donor = m_world->GetDataFile("deme_donor.dat");
  df_donor.WriteComment("Num orgs doing doing a donate for each deme in population");
  df_donor.WriteTimeStamp();
  df_donor.Write(stats.GetUpdate(), "update");

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_donor;

    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_donor.Add(phenotype.IsDonorLast()); 	
    }
    comment.Set("Deme %d", deme_id);
    df_donor.Write(single_deme_donor.Sum(), comment);
  }
  df_donor.Endl();
}

void cPopulation::PrintDemeFitness() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  cDataFile & df_fit = m_world->GetDataFile("deme_fitness.dat");
  df_fit.WriteComment("Average fitnesses for each deme in the population");
  df_fit.WriteTimeStamp();
  df_fit.Write(stats.GetUpdate(), "update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_fitness;

    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_fitness.Add(phenotype.GetFitness()); 	
    }
    comment.Set("Deme %d", deme_id);
    df_fit.Write(single_deme_fitness.Ave(), comment);
  }
  df_fit.Endl();
}

void cPopulation::PrintDemeGestationTime() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  cDataFile & df_gest = m_world->GetDataFile("deme_gest_time.dat");
  df_gest.WriteComment("Average gestation time for each deme in population");
  df_gest.WriteTimeStamp();
  df_gest.Write(stats.GetUpdate(), "update");

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_gest_time;

    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_gest_time.Add(phenotype.GetGestationTime()); 	
    }
    comment.Set("Deme %d", deme_id);
    df_gest.Write(single_deme_gest_time.Ave(), comment);
  }
  df_gest.Endl();
}

void cPopulation::PrintDemeInstructions() {  
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  const int num_inst = m_world->GetNumInstructions();
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString filename;
    filename.Set("deme_instruction-%d.dat", deme_id);
    cDataFile & df_inst = m_world->GetDataFile(filename); 
    cString comment;
    comment.Set("Number of times each instruction is exectued in deme %d",
                deme_id);
    df_inst.WriteComment(comment);
    df_inst.WriteTimeStamp();
    df_inst.Write(stats.GetUpdate(), "update");
        
    tArray<cIntSum> single_deme_inst(num_inst);
    
    const cDeme & cur_deme = deme_array[deme_id];
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      
      for (int j = 0; j < num_inst; j++) {
        single_deme_inst[j].Add(phenotype.GetLastInstCount()[j]);
      } 
    }
    
    for (int j = 0; j < num_inst; j++) {
      comment.Set("Inst %d", j);
      df_inst.Write((int) single_deme_inst[j].Sum(), comment);
    }
    df_inst.Endl();    
  }
}

void cPopulation::PrintDemeLifeFitness() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  cDataFile & df_life_fit = m_world->GetDataFile("deme_lifetime_fitness.dat");
  df_life_fit.WriteComment("Average life fitnesses for each deme in the population");
  df_life_fit.WriteTimeStamp();
  df_life_fit.Write(stats.GetUpdate(), "update");

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_life_fitness;

    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_life_fitness.Add(phenotype.GetLifeFitness()); 	
    }
    comment.Set("Deme %d", deme_id);
    df_life_fit.Write(single_deme_life_fitness.Ave(), comment);
  }
  df_life_fit.Endl();
}

void cPopulation::PrintDemeMerit() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  cDataFile & df_merit = m_world->GetDataFile("deme_merit.dat");
  df_merit.WriteComment("Average merits for each deme in population");
  df_merit.WriteTimeStamp();
  df_merit.Write(stats.GetUpdate(), "update");

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_merit;

    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_merit.Add(phenotype.GetMerit().GetDouble()); 	
    }
    comment.Set("Deme %d", deme_id);
    df_merit.Write(single_deme_merit.Ave(), comment);
  }
  df_merit.Endl();
}

void cPopulation::PrintDemeMutationRate() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  cDataFile & df_mut_rates = m_world->GetDataFile("deme_mut_rates.dat");
  df_mut_rates.WriteComment("Average mutation rates for organisms in each deme");
  df_mut_rates.WriteTimeStamp();
  df_mut_rates.Write(stats.GetUpdate(), "update");
  cDoubleSum total_mut_rate;

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_mut_rate;

    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      single_deme_mut_rate.Add(GetCell(cur_cell).GetOrganism()->MutationRates().GetCopyMutProb());
    }
    comment.Set("Deme %d", deme_id);
    df_mut_rates.Write(single_deme_mut_rate.Ave(), comment);
    total_mut_rate.Add(single_deme_mut_rate.Ave());
  }
  df_mut_rates.Write(total_mut_rate.Ave(), "Average deme mutation rate averaged across Demes.");
  df_mut_rates.Endl();
}

void cPopulation::PrintDemeReceiver() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  cDataFile & df_receiver = m_world->GetDataFile("deme_receiver.dat");
  df_receiver.WriteComment("Num orgs doing receiving a donate for each deme in population");
  df_receiver.WriteTimeStamp();
  df_receiver.Write(stats.GetUpdate(), "update");

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_receiver;

    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_receiver.Add(phenotype.IsReceiver()); 	
    }
    comment.Set("Deme %d", deme_id);
    df_receiver.Write(single_deme_receiver.Sum(), comment);
  }
    df_receiver.Endl();
}

void cPopulation::PrintDemeResource() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  cDataFile & df_resources = m_world->GetDataFile("deme_resources.dat");
  df_resources.WriteComment("Avida deme resource data");
  df_resources.WriteTimeStamp();
  df_resources.Write(stats.GetUpdate(), "update");

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cDeme & cur_deme = deme_array[deme_id];

    cur_deme.UpdateDemeRes();
    cResourceCount res = GetDeme(deme_id).GetDemeResourceCount();
    for(int j = 0; j < res.GetSize(); j++) {
      const char * tmp = res.GetResName(j);
      df_resources.Write(res.Get(j), cStringUtil::Stringf("Deme %d Resource %s", deme_id, tmp)); //comment);
      if((res.GetResourcesGeometry())[j] != nGeometry::GLOBAL) {
        PrintDemeSpatialResData(res, j, deme_id);
      }
    }
  }
  df_resources.Endl();
}

// Write spatial energy data to a file that can easily be read into Matlab
void cPopulation::PrintDemeSpatialEnergyData() const {
  int cellID = 0;
  int update = m_world->GetStats().GetUpdate();
  
  for(int i = 0; i < m_world->GetPopulation().GetNumDemes(); i++) {
    cString tmpfilename = cStringUtil::Stringf( "deme_%07i_spacial_energy.m", i);  // moved here for easy movie making
    cDataFile& df = m_world->GetDataFile(tmpfilename);
    cString UpdateStr = cStringUtil::Stringf( "deme_%07i_energy_%07i = [ ...", i, update );
    df.WriteRaw(UpdateStr);

    int gridsize = m_world->GetPopulation().GetDeme(i).GetSize();
    int xsize = m_world->GetConfig().WORLD_X.Get();

    // write grid to file
    for (int j = 0; j < gridsize; j++) {
      cPopulationCell& cell = m_world->GetPopulation().GetCell(cellID);
      if(cell.IsOccupied()) {
        df.WriteBlockElement(cell.GetOrganism()->GetPhenotype().GetStoredEnergy(), j, xsize);
      } else {
        df.WriteBlockElement(0.0, j, xsize);
      }
      cellID++;
    }
    df.WriteRaw("];");
    df.Endl();
  }
}

// Write spatial data to a file that can easily be read into Matlab
void cPopulation::PrintDemeSpatialResData( cResourceCount res, const int i, const int deme_id) const {
  const char* tmpResName = res.GetResName(i);
  cString tmpfilename = cStringUtil::Stringf( "deme_spacial_resource_%s.m", tmpResName );
  cDataFile& df = m_world->GetDataFile(tmpfilename);
  cString UpdateStr = cStringUtil::Stringf( "deme_%07i_%s_%07i = [ ...", deme_id, static_cast<const char*>(res.GetResName(i)), m_world->GetStats().GetUpdate() );

  df.WriteRaw(UpdateStr);

  cSpatialResCount sp_res = res.GetSpatialResource(i);
  int gridsize = sp_res.GetSize();
  int xsize = m_world->GetConfig().WORLD_X.Get();

  for (int j = 0; j < gridsize; j++) {
    df.WriteBlockElement(sp_res.GetAmount(j), j, xsize);
  }
  df.WriteRaw("];");
  df.Endl();
}

// Write spatial energy data to a file that can easily be read into Matlab
void cPopulation::PrintDemeSpatialSleepData() const {
  int cellID = 0;
  cString tmpfilename = "deme_spacial_sleep.m";
  cDataFile& df = m_world->GetDataFile(tmpfilename);
  int update = m_world->GetStats().GetUpdate();
  
  for(int i = 0; i < m_world->GetPopulation().GetNumDemes(); i++) {
    cString UpdateStr = cStringUtil::Stringf( "deme_%07i_sleep_%07i = [ ...", i, update);
    df.WriteRaw(UpdateStr);

    int gridsize = m_world->GetPopulation().GetDeme(i).GetSize();
    int xsize = m_world->GetConfig().WORLD_X.Get();

    // write grid to file
    for (int j = 0; j < gridsize; j++) {
      cPopulationCell cell = m_world->GetPopulation().GetCell(cellID);
      if(cell.IsOccupied()) {
        df.WriteBlockElement(cell.GetOrganism()->IsSleeping(), j, xsize);
      } else {
        df.WriteBlockElement(0.0, j, xsize);
      }
      cellID++;
    }
    df.WriteRaw("];");
    df.Endl();
  }
}

void cPopulation::PrintDemeTasks() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  const int num_task = environment.GetNumTasks();
  cDataFile & df_task = m_world->GetDataFile("deme_task.dat");
  df_task.WriteComment("Num orgs doing each task for each deme in population");
  df_task.WriteTimeStamp();
  df_task.Write(stats.GetUpdate(), "update");

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    tArray<cIntSum> single_deme_task(num_task);

    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      for (int j = 0; j < num_task; j++) {
        // only interested if task is done once! 
        if (phenotype.GetLastTaskCount()[j] > 0) {
          single_deme_task[j].Add(1);
        }
      }
    }
    for (int j = 0; j < num_task; j++) {
      comment.Set("Deme %d, Task %d", deme_id, j);
      df_task.Write((int) single_deme_task[j].Sum(), comment);
    }
  }
  df_task.Endl();
}

void cPopulation::DumpDemeFounders(ofstream& fp) {
  fp << "#filetype deme_founders" << endl
  << "#format deme_id num_founders genotype_ids" << endl
  << endl
  << "#  1: Deme ID" << endl
  << "#  2: Number of founders" << endl
  << "#  3+: founder genotype ids" << endl << endl;
  
  for(int i=0; i<deme_array.GetSize(); i++) {
  
    if(deme_array[i].IsEmpty()) continue;
    
    tArray<int>& deme_founders = deme_array[i].GetFounderGenotypeIDs();   
      
    fp << i << " " << deme_founders.GetSize();    
    for(int j=0; j<deme_founders.GetSize(); j++) {  
      fp << " " << deme_founders[j];
    }
  
    fp << endl;
  }
}


/**
* This function is responsible for adding an organism to a given lineage,
 * and setting the organism's lineage label and the lineage pointer.
 **/
void cPopulation::LineageSetupOrganism(cOrganism* organism, cLineage* lin, int lin_label, cGenotype* parent_genotype)
{
  // If we have some kind of lineage control, adjust the default values passed in.
  if (m_world->GetConfig().LOG_LINEAGES.Get()){
    lin = m_world->GetClassificationManager().GetLineage(m_world->GetDefaultContext(), organism->GetGenotype(), parent_genotype, lin, lin_label);
    lin_label = lin->GetID();
  }
  
  organism->SetLineageLabel( lin_label );
  organism->SetLineage( lin );
}


/**
This function will set up coalescence clade information.  If this feature is activated in the configuration,
 a list of coalescence genotype ids must be read in initially.  These are furnished by doing an initial run 
 with the same seed and setup and retrieving information from the final dominant lineage and coalescence points.
 
 The value is either (by default) inherited from the parent or the organism's genotypeID if it is known
 to be a coalescence id.
 
 Defaulting is established in Inject or ActivateOffspring methods of this class.
 
 @MRR May 2007
 **/
void cPopulation::CCladeSetupOrganism(cOrganism* organism)
{
		int gen_id = organism->GetGenotype()->GetID();	
		if (m_world->GetConfig().TRACK_CCLADES.Get() > 0)
		{
			if (m_world->GetClassificationManager().IsCCladeFounder(gen_id))
			{	
				organism->SetCCladeLabel(gen_id);
			}
		}
}


/**
* This function directs which position function should be used.  It
 * could have also been done with a function pointer, but the dividing
 * of an organism takes enough time that this will be a negligible addition,
 * and it gives a centralized function to work with.  The parent_ok flag asks
 * if it is okay to replace the parent.
 **/
//@AWC -- This could REALLY stand some functional abstraction...
cPopulationCell& cPopulation::PositionChild(cPopulationCell& parent_cell, bool parent_ok)
{
  assert(parent_cell.IsOccupied());
  
  const int birth_method = m_world->GetConfig().BIRTH_METHOD.Get();

  // @WRE carrying capacity handling
  /* Pick and kill an organism here if needed
   * and then enter choices for birth method handling.
   */
  if ((0 < m_world->GetConfig().BIOMIMETIC_K.Get()) &&
      (num_organisms >= m_world->GetConfig().BIOMIMETIC_K.Get())) {
    // Measure temporary variables
    double max_msr = 0.0;
    double msr = 0.0;
    int max_msrndx = 0;
    for  (int i=0; i < cell_array.GetSize(); i++) {
      if (cell_array[i].IsOccupied()) {
        if (cell_array[i].GetOrganism()->GetPhenotype().OK()) {
          // Get measurement, exclude parent
          if (parent_cell.GetID() != cell_array[i].GetID()) {
            msr = m_world->GetRandom().GetDouble();
          } else {
            msr = 0.0;
          }
          if (max_msr < msr) {
            max_msr = msr;
            max_msrndx = i;
          }
        }
      }
    }
    KillOrganism(cell_array[max_msrndx]);
  }

  //@AWC -- decide wether the child will migrate to another deme -- if migrating we ignore the birth method.  
  if ((m_world->GetConfig().MIGRATION_RATE.Get() > 0.0) //@AWC -- Pedantic test to maintain consistancy.
      && m_world->GetRandom().P(m_world->GetConfig().MIGRATION_RATE.Get())) {

    //cerr << "Attempting to migrate with rate " << m_world->GetConfig().MIGRATION_RATE.Get() << "!" << endl;
    int deme_id = parent_cell.GetDemeID();
         
    //get another -unadjusted- deme id
    int rnd_deme_id = m_world->GetRandom().GetInt(deme_array.GetSize()-1);
    
    //if the -unadjusted- id is above the excluded id, bump it up one
    //insures uniform prob of landing in any deme but the parent's
    if(rnd_deme_id >= deme_id) rnd_deme_id++;
    
    //set the new deme_id
    deme_id = rnd_deme_id;
    
    //The rest of this is essentially POSITION_CHILD_DEME_RANDOM
    //@JEB: But note that this will not honor PREFER_EMPTY in the new deme.
    const int deme_size = deme_array[deme_id].GetSize();
    
    int out_pos = m_world->GetRandom().GetUInt(deme_size);
    int out_cell_id = deme_array[deme_id].GetCellID(out_pos);
    while (parent_ok == false && out_cell_id == parent_cell.GetID()) {
      out_pos = m_world->GetRandom().GetUInt(deme_size);
      out_cell_id = deme_array[deme_id].GetCellID(out_pos);
    }
    
    deme_array[deme_id].IncBirthCount();
    GetCell(out_cell_id).SetMigrant();
    return GetCell(out_cell_id);    
  }
  // Fix so range can be arbitrary and we won't accidentally include ourself as a target
  // @AWC If not migrating try out global/full-deme birth methods first...
  
  // Similar to MIGRATION_RATE code above but allows for a bit more flexibility
  // in how migration between demes works. Also, respects PREFER_EMPTY in new deme.
  // Temporary until Deme 
  if ((m_world->GetConfig().DEMES_MIGRATION_RATE.Get() > 0.0)
      && m_world->GetRandom().P(m_world->GetConfig().DEMES_MIGRATION_RATE.Get())) 
  {
    deme_array[parent_cell.GetDemeID()].IncBirthCount();
    return PositionDemeMigration(parent_cell, parent_ok);
  }
  
  // This block should be changed to a switch statment with functions handling 
  // the cases. For now, a bunch of if's that return if they handle.
  
  if (birth_method == POSITION_CHILD_FULL_SOUP_RANDOM) {
   
    // Look randomly within empty cells first, if requested
    if (m_world->GetConfig().PREFER_EMPTY.Get()) {
      int num_empty_cells = UpdateEmptyCellIDArray();
      if (num_empty_cells > 0) {
        int out_pos = m_world->GetRandom().GetUInt(num_empty_cells);
        return GetCell(empty_cell_id_array[out_pos]);
      } 
    }

    int out_pos = m_world->GetRandom().GetUInt(cell_array.GetSize());
    while (parent_ok == false && out_pos == parent_cell.GetID()) {
      out_pos = m_world->GetRandom().GetUInt(cell_array.GetSize());
    }
    return GetCell(out_pos);
  }
  
  if (birth_method == POSITION_CHILD_FULL_SOUP_ELDEST) {
    cPopulationCell * out_cell = reaper_queue.PopRear();
    if (parent_ok == false && out_cell->GetID() == parent_cell.GetID()) {
      out_cell = reaper_queue.PopRear();
      reaper_queue.PushRear(&parent_cell);
    }
    return *out_cell;
  }
  
  if (birth_method == POSITION_CHILD_DEME_RANDOM) {
    deme_array[parent_cell.GetDemeID()].IncBirthCount();
    return PositionDemeRandom(parent_cell.GetDemeID(), parent_cell, parent_ok);
  }
  else if (birth_method == POSITION_CHILD_PARENT_FACING) {
    return parent_cell.GetCellFaced();
  }
  else if (birth_method == POSITION_CHILD_NEXT_CELL) {
    int out_cell_id = parent_cell.GetID() + 1;
    if (out_cell_id == cell_array.GetSize()) out_cell_id = 0;
    return GetCell(out_cell_id);
  }
  else if (birth_method == POSITION_CHILD_FULL_SOUP_ENERGY_USED) {
    tList<cPopulationCell> found_list;
    int max_time_used = 0;
    for  (int i=0; i < cell_array.GetSize(); i++)
    {
      int time_used = cell_array[i].IsOccupied() ? cell_array[i].GetOrganism()->GetPhenotype().GetTimeUsed() : INT_MAX;
      if (time_used == max_time_used)
      {
        found_list.Push(&cell_array[i]);
      }
      else if (time_used > max_time_used)
      {
        max_time_used = time_used;
        found_list.Clear();
        found_list.Push(&cell_array[i]);
      }
    }
    int choice = m_world->GetRandom().GetUInt(found_list.GetSize());
    return *( found_list.GetPos(choice) );
  }
  
  // All remaining methods require us to choose among mulitple local positions.
  
  // Construct a list of equally viable locations to place the child...
  tList<cPopulationCell> found_list;
  
  // First, check if there is an empty organism to work with (always preferred)
  tList<cPopulationCell> & conn_list = parent_cell.ConnectionList();
  
  if (m_world->GetConfig().PREFER_EMPTY.Get() == false &&
      birth_method == POSITION_CHILD_RANDOM) {
    found_list.Append(conn_list);
    if (parent_ok == true) found_list.Push(&parent_cell);
  } else {
    FindEmptyCell(conn_list, found_list);
  }
  
  // If we have not found an empty organism, we must use the specified function
  // to determine how to choose among the filled organisms.
  if (found_list.GetSize() == 0) {
    switch(birth_method) {
      case POSITION_CHILD_AGE:
        PositionAge(parent_cell, found_list, parent_ok);
        break;
      case POSITION_CHILD_MERIT:
        PositionMerit(parent_cell, found_list, parent_ok);
        break;
      case POSITION_CHILD_RANDOM:
        found_list.Append(conn_list);
        if (parent_ok == true) found_list.Push(&parent_cell);
          break;
      case POSITION_CHILD_NEIGHBORHOOD_ENERGY_USED:
        PositionEnergyUsed(parent_cell, found_list, parent_ok);
      case POSITION_CHILD_EMPTY:
        // Nothing is in list if no empty cells are found...
        break;
    }
  }
  
  if (deme_array.GetSize() > 0) {
    const int deme_id = parent_cell.GetDemeID();
    deme_array[deme_id].IncBirthCount();
  }
  
  // If there are no possibilities, return parent.
  if (found_list.GetSize() == 0) return parent_cell;
  
  // Choose the organism randomly from those in the list, and return it.
  int choice = m_world->GetRandom().GetUInt(found_list.GetSize());
  return *( found_list.GetPos(choice) );
}

void cPopulation::PositionAge(cPopulationCell & parent_cell,
                              tList<cPopulationCell> & found_list,
                              bool parent_ok)
{
  // Start with the parent organism as the replacement, and see if we can find
  // anything equivilent or better.
  
  found_list.Push(&parent_cell);
  int max_age = parent_cell.GetOrganism()->GetPhenotype().GetAge();
  if (parent_ok == false) max_age = -1;
  
  // Now look at all of the neighbors.
  tListIterator<cPopulationCell> conn_it( parent_cell.ConnectionList() );
  
  cPopulationCell * test_cell;
  while ( (test_cell = conn_it.Next()) != NULL) {
    const int cur_age = test_cell->GetOrganism()->GetPhenotype().GetAge();
    if (cur_age > max_age) {
      max_age = cur_age;
      found_list.Clear();
      found_list.Push(test_cell);
    }
    else if (cur_age == max_age) {
      found_list.Push(test_cell);
    }
  }
}

void cPopulation::PositionMerit(cPopulationCell & parent_cell,
                                tList<cPopulationCell> & found_list,
                                bool parent_ok)
{
  // Start with the parent organism as the replacement, and see if we can find
  // anything equivilent or better.
  
  found_list.Push(&parent_cell);
  double max_ratio = parent_cell.GetOrganism()->CalcMeritRatio();
  if (parent_ok == false) max_ratio = -1;
  
  // Now look at all of the neighbors.
  tListIterator<cPopulationCell> conn_it( parent_cell.ConnectionList() );
  
  cPopulationCell * test_cell;
  while ( (test_cell = conn_it.Next()) != NULL) {
    const double cur_ratio = test_cell->GetOrganism()->CalcMeritRatio();
    if (cur_ratio > max_ratio) {
      max_ratio = cur_ratio;
      found_list.Clear();
      found_list.Push(test_cell);
    }
    else if (cur_ratio == max_ratio) {
      found_list.Push(test_cell);
    }
  }
}

void cPopulation::PositionEnergyUsed(cPopulationCell & parent_cell,
                                tList<cPopulationCell> & found_list,
                                bool parent_ok)
{
  // Start with the parent organism as the replacement, and see if we can find
  // anything equivilent or better.
  
  found_list.Push(&parent_cell);
  int max_energy_used = parent_cell.GetOrganism()->GetPhenotype().GetTimeUsed();
  if (parent_ok == false) max_energy_used = -1;
  
  // Now look at all of the neighbors.
  tListIterator<cPopulationCell> conn_it( parent_cell.ConnectionList() );
  
  cPopulationCell * test_cell;
  while ( (test_cell = conn_it.Next()) != NULL) {
    const int cur_energy_used = test_cell->GetOrganism()->GetPhenotype().GetTimeUsed();
    if (cur_energy_used > max_energy_used) {
      max_energy_used = cur_energy_used;
      found_list.Clear();
      found_list.Push(test_cell);
    }
    else if (cur_energy_used == max_energy_used) {
      found_list.Push(test_cell);
    }
  }
}

// This function handles PositionChild() when there is migration between demes
cPopulationCell& cPopulation::PositionDemeMigration(cPopulationCell& parent_cell, bool parent_ok)
{
    //cerr << "Attempting to migrate with rate " << m_world->GetConfig().MIGRATION_RATE.Get() << "!" << endl;
    int deme_id = parent_cell.GetDemeID();
    
    // Position randomly in any other deme
    if (m_world->GetConfig().DEMES_MIGRATION_METHOD.Get() == 0) {  
                    
      //get another -unadjusted- deme id
      int rnd_deme_id = m_world->GetRandom().GetInt(deme_array.GetSize()-1);
      
      //if the -unadjusted- id is above the excluded id, bump it up one
      //insures uniform prob of landing in any deme but the parent's
      if(rnd_deme_id >= deme_id) rnd_deme_id++;
      
      //set the new deme_id
      deme_id = rnd_deme_id;
    }
    
    //Position randomly in an adjacent deme in neighborhood (assuming torus)
    //Extremely hacked DEMES_NUM_X config setting simulates grid
    else if (m_world->GetConfig().DEMES_MIGRATION_METHOD.Get() == 1) {
    
      //get a random eight-neighbor
      int dir = m_world->GetRandom().GetInt(8);
      
      // 0 = NW, 1=N, continuing clockwise....
    
      // Up one row
      if (m_world->GetConfig().DEMES_NUM_X.Get() == 0) {
          m_world->GetDriver().RaiseFatalException(1, "DEMES_NUM_X must be non-zero if DEMES_MIGRATION_METHOD 1 used.");
      }
      int x_size = m_world->GetConfig().DEMES_NUM_X.Get();
      int y_size = (int) (m_world->GetConfig().NUM_DEMES.Get() / x_size);

      assert(y_size * x_size == m_world->GetConfig().NUM_DEMES.Get());

      int x = deme_id % x_size;
      int y = (int) (deme_id / x_size);

      if ( (dir == 0) || (dir == 1) || (dir == 2) ) y--;
      if ( (dir == 5) || (dir == 6) || (dir == 7) ) y++;
      if ( (dir == 0) || (dir == 3) || (dir == 5) ) x--;
      if ( (dir == 2) || (dir == 4) || (dir == 7) ) x++;
      
      //handle boundary conditions...
      
      x = (x + x_size) % x_size;
      y = (y + y_size) % y_size;

      //set the new deme_id
      deme_id = x + x_size * y;

      assert(deme_id > 0);
      assert(deme_id > 0);
    }
    
    //Random deme adjacent in list
    else if (m_world->GetConfig().DEMES_MIGRATION_METHOD.Get() == 2) {

      //get a random direction to move in deme list
      int rnd_deme_id = m_world->GetRandom().GetInt(1);
      if (rnd_deme_id == 0) rnd_deme_id = -1;
      
      //set the new deme_id
      deme_id = (deme_id + rnd_deme_id + GetNumDemes()) % GetNumDemes();
    }
    
    // TODO the above choice of deme does not respect PREFER_EMPTY
    // i.e., it does not preferentially pick a deme with empty cells if they are 
    // it might make sense for that to happen...
    
    // Now return an empty cell from the chosen deme
    return PositionDemeRandom(deme_id, parent_cell, parent_ok); 
}

// This function handles PositionChild() by returning a random cell from the entire deme.
cPopulationCell& cPopulation::PositionDemeRandom(int deme_id, cPopulationCell& parent_cell, bool parent_ok)
{
  assert((deme_id >=0) && (deme_id < deme_array.GetSize()));
  
  const int deme_size = deme_array[deme_id].GetSize();
  cDeme& deme = deme_array[deme_id];

  // Look randomly within empty cells first, if requested
  if (m_world->GetConfig().PREFER_EMPTY.Get()) {
    
    int num_empty_cells = UpdateEmptyCellIDArray(deme_id);
    if (num_empty_cells > 0) {
      int out_pos = m_world->GetRandom().GetUInt(num_empty_cells);
      return GetCell(empty_cell_id_array[out_pos]);
    } 
  }

  int out_pos = m_world->GetRandom().GetUInt(deme_size);
  int out_cell_id = deme.GetCellID(out_pos);
  
  while (parent_ok == false && out_cell_id == parent_cell.GetID()) {
    out_pos = m_world->GetRandom().GetUInt(deme_size);
    out_cell_id = deme.GetCellID(out_pos);
  }
  
  return GetCell(out_cell_id); 
}


// This function updates the list of empty cell ids in the population
// and returns the number of empty cells found. Used by global PREFER_EMPTY
// PositionChild() methods. If deme_id is -1 (the default), then operates
// on the entire population. 
int cPopulation::UpdateEmptyCellIDArray(int deme_id)
{
  int num_empty_cells = 0;  
  // Note: empty_cell_id_array was resized to be large enough to hold
  // all cells in the cPopulation when it was created. Using functions
  // that resize it (like Push) will slow this code down considerably.
  // Instead, we keep track of how much of this memory we are using.
      
  // Look at all cells
  if (deme_id == -1) { 
    for (int i=0; i<cell_array.GetSize(); i++) {
      if (GetCell(i).IsOccupied() == false) empty_cell_id_array[num_empty_cells++] = i;
    }
  }
  // Look at a specific deme
  else {
	cDeme& deme = deme_array[deme_id];
    for (int i=0; i<deme.GetSize(); i++) {
      if (GetCell(deme.GetCellID(i)).IsOccupied() == false) empty_cell_id_array[num_empty_cells++] = deme.GetCellID(i);
    }
  }
  return num_empty_cells;
}


int cPopulation::ScheduleOrganism()
{
  return schedule->GetNextID();
}

void cPopulation::ProcessStep(cAvidaContext& ctx, double step_size, int cell_id)
{
  assert(step_size > 0.0);
  assert(cell_id < cell_array.GetSize());
  
  // If cell_id is negative, no cell could be found -- stop here.
  if (cell_id < 0) return;
  
  cPopulationCell& cell = GetCell(cell_id);
  assert(cell.IsOccupied()); // Unoccupied cell getting processor time!
  cOrganism* cur_org = cell.GetOrganism();

  cell.GetHardware()->SingleProcess(ctx);    
  
  if (cur_org->GetPhenotype().GetToDelete() == true) delete cur_org;

  m_world->GetStats().IncExecuted();
  resource_count.Update(step_size);
  
  // Deme specific
  if (GetNumDemes() > 1)
  {
    for(int i = 0; i < GetNumDemes(); i++) GetDeme(i).Update(step_size);
  
    cDeme & deme = GetDeme(GetCell(cell_id).GetDemeID());
    deme.IncTimeUsed(cur_org->GetPhenotype().GetMerit().GetDouble());
    CheckImplicitDemeRepro(deme);
  }
}


void cPopulation::ProcessStepSpeculative(cAvidaContext& ctx, double step_size, int cell_id)
{
  assert(step_size > 0.0);
  assert(cell_id < cell_array.GetSize());
  assert(m_world->GetHardwareManager().SupportsSpeculative());

  // If cell_id is negative, no cell could be found -- stop here.
  if (cell_id < 0) return;
  
  cPopulationCell& cell = GetCell(cell_id);
  assert(cell.IsOccupied()); // Unoccupied cell getting processor time!
  
  cOrganism* cur_org = cell.GetOrganism();
  cHardwareBase* hw = cell.GetHardware();
  
  if (cell.GetSpeculativeState()) {
    // We have already executed this instruction, just decrement the counter
    cell.DecSpeculative();
  } else {
    // Execute the actual instruction
    if (hw->SingleProcess(ctx)) {
      // Speculatively execute additional instructions
      int spec_count = 0;
      while (spec_count < 32) {
        if (hw->SingleProcess(ctx, true)) spec_count++;
        else break;
      }
      cell.SetSpeculativeState(spec_count);
      m_world->GetStats().AddSpeculative(spec_count);
    }
  }
  
  if (cur_org->GetPhenotype().GetToDelete() == true) delete cur_org;

  m_world->GetStats().IncExecuted();
  resource_count.Update(step_size);
 
   // Deme specific
  if (GetNumDemes() > 1)
  {
    for(int i = 0; i < GetNumDemes(); i++) GetDeme(i).Update(step_size);
  
    cDeme & deme = GetDeme(GetCell(cell_id).GetDemeID());
    deme.IncTimeUsed(cur_org->GetPhenotype().GetMerit().GetDouble());
    CheckImplicitDemeRepro(deme);
  }
  
}

// Loop through all the demes getting stats and doing calculations
// which must be done on a deme by deme basis.
void cPopulation::UpdateDemeStats() {

  // bail early to save time if there are no demes
  if (GetNumDemes() == 1) return ;

  cStats& stats = m_world->GetStats();

  stats.SumDemeAge().Clear();
  stats.SumDemeBirthCount().Clear();
  stats.SumDemeOrgCount().Clear();
  stats.SumDemeGeneration().Clear();
  
  stats.SumDemeGestationTime().Clear();
  stats.SumDemeNormalizedTimeUsed().Clear();
  stats.SumDemeMerit().Clear();

  stats.SumDemeGenerationsPerLifetime().Clear();

  stats.ClearNumOccupiedDemes();

  for(int i = 0; i < GetNumDemes(); i++) {
    cDeme& deme = GetDeme(i);
    if(deme.IsEmpty())  // ignore empty demes
    { 
      continue;
    }
    stats.IncNumOccupiedDemes();
    
    stats.SumDemeAge().Add(deme.GetAge());
    stats.SumDemeBirthCount().Add(deme.GetBirthCount());
    stats.SumDemeOrgCount().Add(deme.GetOrgCount());
    stats.SumDemeGeneration().Add(deme.GetGeneration());

    stats.SumDemeLastBirthCount().Add(deme.GetLastBirthCount());
    stats.SumDemeLastOrgCount().Add(deme.GetLastOrgCount());

    stats.SumDemeGestationTime().Add(deme.GetGestationTime());
    stats.SumDemeNormalizedTimeUsed().Add(deme.GetLastNormalizedTimeUsed());
    stats.SumDemeMerit().Add(deme.GetDemeMerit().GetDouble());
    
    stats.SumDemeGenerationsPerLifetime().Add(deme.GetGenerationsPerLifetime());
  }
}


void cPopulation::UpdateOrganismStats()
{
  // Loop through all the cells getting stats and doing calculations
  // which must be done on a creature by creature basis.
  
  cStats& stats = m_world->GetStats();
  
  // Clear out organism sums...
  stats.SumFitness().Clear();
  stats.SumGestation().Clear();
  stats.SumMerit().Clear();
  stats.SumCreatureAge().Clear();
  stats.SumGeneration().Clear();
  stats.SumNeutralMetric().Clear();
  stats.SumLineageLabel().Clear();
  stats.SumCopyMutRate().Clear();
  stats.SumDivMutRate().Clear();
  stats.SumCopySize().Clear();
  stats.SumExeSize().Clear();
  stats.SumMemSize().Clear();
  
  stats.ZeroTasks();
  stats.ZeroRewards();
  
#if INSTRUCTION_COUNT
  stats.ZeroInst();
#endif
  
  // Counts...
  int num_breed_true = 0;
  int num_parasites = 0;
  int num_no_birth = 0;
  int num_multi_thread = 0;
  int num_single_thread = 0;
  int num_threads = 0;
  int num_modified = 0;
  
  // Maximums...
  cMerit max_merit(0);
  double max_fitness = 0;
  int max_gestation_time = 0;
  int max_genome_length = 0;
  
  // Minimums...
  cMerit min_merit(FLT_MAX);
  double min_fitness = FLT_MAX;
  int min_gestation_time = INT_MAX;
  int min_genome_length = INT_MAX;
  
  for (int i = 0; i < cell_array.GetSize(); i++) {
    // Only look at cells with organisms in them.
    if (cell_array[i].IsOccupied() == false) {
      
      // Genotype map needs zero for all non-occupied cells
      
      stats.SetGenoMapElement(i, 0);
      continue;
    }
    
    cOrganism * organism = cell_array[i].GetOrganism();
    const cPhenotype & phenotype = organism->GetPhenotype();
    const cMerit cur_merit = phenotype.GetMerit();
    const double cur_fitness = phenotype.GetFitness();
    const int cur_gestation_time = phenotype.GetGestationTime();
    const int cur_genome_length = phenotype.GetGenomeLength();
    
    stats.SumFitness().Add(cur_fitness);
    stats.SumMerit().Add(cur_merit.GetDouble());
    stats.SumGestation().Add(phenotype.GetGestationTime());
    stats.SumCreatureAge().Add(phenotype.GetAge());
    stats.SumGeneration().Add(phenotype.GetGeneration());
    stats.SumNeutralMetric().Add(phenotype.GetNeutralMetric());
    stats.SumLineageLabel().Add(organism->GetLineageLabel());
    stats.SumCopyMutRate().Add(organism->MutationRates().GetCopyMutProb());
    stats.SumLogCopyMutRate().Add(log(organism->MutationRates().GetCopyMutProb()));
    stats.SumDivMutRate().Add(organism->MutationRates().GetDivMutProb() / organism->GetPhenotype().GetDivType());
    stats.SumLogDivMutRate().Add(log(organism->MutationRates().GetDivMutProb() /organism->GetPhenotype().GetDivType()));
    stats.SumCopySize().Add(phenotype.GetCopiedSize());
    stats.SumExeSize().Add(phenotype.GetExecutedSize());
    stats.SetGenoMapElement(i, organism->GetGenotype()->GetID());
    
#if INSTRUCTION_COUNT
    for (int j = 0; j < m_world->GetNumInstructions(); j++) {
      stats.SumExeInst()[j].Add(organism->GetPhenotype().GetLastInstCount()[j]);
    }
#endif
    
    if (cur_merit > max_merit) max_merit = cur_merit;
    if (cur_fitness > max_fitness) max_fitness = cur_fitness;
    if (cur_gestation_time > max_gestation_time) max_gestation_time = cur_gestation_time;
    if (cur_genome_length > max_genome_length) max_genome_length = cur_genome_length;
    
    if (cur_merit < min_merit) min_merit = cur_merit;
    if (cur_fitness < min_fitness) min_fitness = cur_fitness;
    if (cur_gestation_time < min_gestation_time) min_gestation_time = cur_gestation_time;
    if (cur_genome_length < min_genome_length) min_genome_length = cur_genome_length;
    
    // Test what tasks this creatures has completed.
    for (int j = 0; j < m_world->GetEnvironment().GetNumTasks(); j++) {
      if (phenotype.GetCurTaskCount()[j] > 0) {
        stats.AddCurTask(j);
        stats.AddCurTaskQuality(j, phenotype.GetCurTaskQuality()[j]);
      }
      
      if (phenotype.GetLastTaskCount()[j] > 0) {
        stats.AddLastTask(j);
        stats.AddLastTaskQuality(j, phenotype.GetLastTaskQuality()[j]);
        stats.IncTaskExeCount(j, phenotype.GetLastTaskCount()[j]);
      } 
    }
    
    // Record what add bonuses this organism garnered for different reactions    
    for (int j = 0; j < m_world->GetNumReactions(); j++) {
      stats.AddLastReactionAddReward(j, phenotype.GetLastReactionAddReward()[j]);    
    }
    
    // Test what resource combinations this creature has sensed
    for (int j = 0; j < stats.GetSenseSize(); j++) {
      if (phenotype.GetLastSenseCount()[j] > 0) {
        stats.AddLastSense(j);
        stats.IncLastSenseExeCount(j, phenotype.GetLastSenseCount()[j]);
      }
    }
    
    // Increment the counts for all qualities the organism has...
    num_parasites += organism->GetNumParasites();
    if (phenotype.ParentTrue()) num_breed_true++;
    if (phenotype.GetNumDivides() == 0) num_no_birth++;
    if (phenotype.IsMultiThread()) num_multi_thread++;
    else num_single_thread++;
    
    if(phenotype.IsModified()) num_modified++;    
    
    cHardwareBase& hardware = organism->GetHardware();
    stats.SumMemSize().Add(hardware.GetMemory().GetSize());
    num_threads += hardware.GetNumThreads();
    
    // Increment the age of this organism.
    organism->GetPhenotype().IncAge();
  }
  
  stats.SetBreedTrueCreatures(num_breed_true);
  stats.SetNumNoBirthCreatures(num_no_birth);
  stats.SetNumParasites(num_parasites);
  stats.SetNumSingleThreadCreatures(num_single_thread);
  stats.SetNumMultiThreadCreatures(num_multi_thread);
  stats.SetNumThreads(num_threads);
  stats.SetNumModified(num_modified);
  
  stats.SetMaxMerit(max_merit.GetDouble());
  stats.SetMaxFitness(max_fitness);
  stats.SetMaxGestationTime(max_gestation_time);
  stats.SetMaxGenomeLength(max_genome_length);
  
  stats.SetMinMerit(min_merit.GetDouble());
  stats.SetMinFitness(min_fitness);
  stats.SetMinGestationTime(min_gestation_time);
  stats.SetMinGenomeLength(min_genome_length);
  
  stats.SetResources(resource_count.GetResources());
  stats.SetSpatialRes(resource_count.GetSpatialRes());
  stats.SetResourcesGeometry(resource_count.GetResourcesGeometry());
  
  //TODO: update deme resource  (what calls DoOutput)
  
  for(int i = 0; i < GetNumDemes(); i++) {
    GetDeme(i).UpdateDemeRes();
  }
}


void cPopulation::UpdateGenotypeStats()
{
  // Loop through all genotypes, finding stats and doing calcuations.
  
  cStats& stats = m_world->GetStats();
  
  // Clear out genotype sums...
  stats.SumGenotypeAge().Clear();
  stats.SumAbundance().Clear();
  stats.SumGenotypeDepth().Clear();
  stats.SumSize().Clear();
  stats.SumThresholdAge().Clear();
  
  double entropy = 0.0;
  
  cGenotype * cur_genotype = m_world->GetClassificationManager().GetBestGenotype();
  for (int i = 0; i < m_world->GetClassificationManager().GetGenotypeCount(); i++) {
    const int abundance = cur_genotype->GetNumOrganisms();
    
    // If we're at a dead genotype, we've hit the end of the list!
    if (abundance == 0) break;
    
    // Update stats...
    const int age = stats.GetUpdate() - cur_genotype->GetUpdateBorn();
    stats.SumGenotypeAge().Add(age, abundance);
    stats.SumAbundance().Add(abundance);
    stats.SumGenotypeDepth().Add(cur_genotype->GetDepth(), abundance);
    stats.SumSize().Add(cur_genotype->GetLength(), abundance);
    
    // Calculate this genotype's contribution to entropy
    // - when p = 1.0, partial_ent calculation would return -0.0. This may propagate
    //   to the output stage, but behavior is dependent on compiler used and optimization
    //   level.  For consistent output, ensures that 0.0 is returned.
    const double p = ((double) abundance) / (double) num_organisms;
    const double partial_ent = (abundance == num_organisms) ? 0.0 : -(p * Log(p)); 
    entropy += partial_ent;
    
    // Do any special calculations for threshold genotypes.
    if (cur_genotype->GetThreshold()) {
      stats.SumThresholdAge().Add(age, abundance);
    }
    
    // ...and advance to the next genotype...
    cur_genotype = cur_genotype->GetNext();
  }
  
  stats.SetEntropy(entropy);
}


void cPopulation::UpdateSpeciesStats()
{
  cStats& stats = m_world->GetStats();
  double species_entropy = 0.0;
  
  stats.SumSpeciesAge().Clear();
  
  // Loop through all species that need to be reset prior to calculations.
  cSpecies * cur_species = m_world->GetClassificationManager().GetFirstSpecies();
  for (int i = 0; i < m_world->GetClassificationManager().GetNumSpecies(); i++) {
    cur_species->ResetStats();
    cur_species = cur_species->GetNext();
  }
  
  // Collect info from genotypes and send it to their species.
  cGenotype * genotype = m_world->GetClassificationManager().GetBestGenotype();
  for (int i = 0; i < m_world->GetClassificationManager().GetGenotypeCount(); i++) {
    if (genotype->GetSpecies() != NULL) {
      genotype->GetSpecies()->AddOrganisms(genotype->GetNumOrganisms());
    }
    genotype = genotype->GetNext();
  }
  
  // Loop through all of the species in the soup, taking info on them.
  cur_species = m_world->GetClassificationManager().GetFirstSpecies();
  for (int i = 0; i < m_world->GetClassificationManager().GetNumSpecies(); i++) {
    const int abundance = cur_species->GetNumOrganisms();
    // const int num_genotypes = cur_species->GetNumGenotypes();
    
    // Basic statistical collection...
    const int species_age = stats.GetUpdate() - cur_species->GetUpdateBorn();
    stats.SumSpeciesAge().Add(species_age, abundance);
    
    // Caculate entropy on the species level...
    // - when p = 1.0, partial_ent calculation would return -0.0. This may propagate
    //   to the output stage, but behavior is dependent on compiler used and optimization
    //   level.  For consistent output, ensures that 0.0 is returned.
    if (abundance > 0) {
      double p = ((double) abundance) / (double) num_organisms;
      double partial_ent = (abundance == num_organisms) ? 0.0 : -(p * Log(p));
      species_entropy += partial_ent;
    }
    
    // ...and advance to the next species...
    cur_species = cur_species->GetNext();
  }
  
  stats.SetSpeciesEntropy(species_entropy);
}

void cPopulation::UpdateDominantStats()
{
  cStats& stats = m_world->GetStats();
  cGenotype * dom_genotype = m_world->GetClassificationManager().GetBestGenotype();
  if (dom_genotype == NULL) return;
  
  stats.SetDomGenotype(dom_genotype);
  stats.SetDomMerit(dom_genotype->GetMerit());
  stats.SetDomGestation(dom_genotype->GetGestationTime());
  stats.SetDomReproRate(dom_genotype->GetReproRate());
  stats.SetDomFitness(dom_genotype->GetFitness());
  stats.SetDomCopiedSize(dom_genotype->GetCopiedSize());
  stats.SetDomExeSize(dom_genotype->GetExecutedSize());
  
  stats.SetDomSize(dom_genotype->GetLength());
  stats.SetDomID(dom_genotype->GetID());
  stats.SetDomName(dom_genotype->GetName());
  stats.SetDomBirths(dom_genotype->GetThisBirths());
  stats.SetDomBreedTrue(dom_genotype->GetThisBreedTrue());
  stats.SetDomBreedIn(dom_genotype->GetThisBreedIn());
  stats.SetDomBreedOut(dom_genotype->GetThisBreedOut());
  stats.SetDomAbundance(dom_genotype->GetNumOrganisms());
  stats.SetDomGeneDepth(dom_genotype->GetDepth());
  stats.SetDomSequence(dom_genotype->GetGenome().AsString());
}

void cPopulation::UpdateDominantParaStats()
{
  cStats& stats = m_world->GetStats();
  cInjectGenotype * dom_inj_genotype = m_world->GetClassificationManager().GetBestInjectGenotype();
  if (dom_inj_genotype == NULL) return;
  
  stats.SetDomInjGenotype(dom_inj_genotype);
  
  stats.SetDomInjSize(dom_inj_genotype->GetLength());
  stats.SetDomInjID(dom_inj_genotype->GetID());
  stats.SetDomInjName(dom_inj_genotype->GetName());
  stats.SetDomInjAbundance(dom_inj_genotype->GetNumInjected());
  stats.SetDomInjSequence(dom_inj_genotype->GetGenome().AsString());
}

void cPopulation::CalcUpdateStats()
{
  cStats& stats = m_world->GetStats();
  // Reset the Genebank to prepare it for stat collection.
  m_world->GetClassificationManager().UpdateReset();
  
  UpdateDemeStats();
  UpdateOrganismStats();
  UpdateGenotypeStats();
  UpdateSpeciesStats();
  UpdateDominantStats();
  UpdateDominantParaStats();
  
  // Do any final calculations...
  stats.SetNumCreatures(GetNumOrganisms());
  stats.SetNumGenotypes(m_world->GetClassificationManager().GetGenotypeCount());
  stats.SetNumThreshSpecies(m_world->GetClassificationManager().GetNumSpecies());
  
  // Have stats calculate anything it now can...
  stats.CalcEnergy();
  stats.CalcFidelity();
}


bool cPopulation::SaveClone(ofstream& fp)
{
  if (fp.good() == false) return false;
  
  // Save the current update
  fp << m_world->GetStats().GetUpdate() << " ";
  
  // Save the archive info.
  m_world->GetClassificationManager().SaveClone(fp);
  
  // Save the genotypes manually.
  fp << m_world->GetClassificationManager().GetGenotypeCount() << " ";
  
  cGenotype * cur_genotype = m_world->GetClassificationManager().GetBestGenotype();
  for (int i = 0; i < m_world->GetClassificationManager().GetGenotypeCount(); i++) {
    cur_genotype->SaveClone(fp);
    
    // Advance...
    cur_genotype = cur_genotype->GetNext();
  }
  
  // Save the organim layout...
  fp << cell_array.GetSize() << " ";
  for (int i = 0; i < cell_array.GetSize(); i++) {
    if (cell_array[i].IsOccupied() == true) {
      fp <<  cell_array[i].GetOrganism()->GetGenotype()->GetID() << " ";
    }
    else fp << "-1 ";
  }
  
  return true;
}


bool cPopulation::LoadClone(ifstream & fp)
{
  if (fp.good() == false) return false;
  
  // Pick up the update where it was left off.
  int cur_update;
  fp >> cur_update;
  
  m_world->GetStats().SetCurrentUpdate(cur_update);
  
  // Clear out the population
  for (int i = 0; i < cell_array.GetSize(); i++) KillOrganism(cell_array[i]);
  
  // Load the archive info.
  m_world->GetClassificationManager().LoadClone(fp);
  
  // Load up the genotypes.
  int num_genotypes = 0;
  fp >> num_genotypes;
  
  cGenotype** genotype_array = new cGenotype*[num_genotypes];
  for (int i = 0; i < num_genotypes; i++) {
    genotype_array[i] = cGenotype::LoadClone(m_world, fp);
  }
  
  // Now load them into the organims.  @CAO make sure cell_array.GetSize() is right!
  int in_num_cells;
  int genotype_id;
  fp >> in_num_cells;
  if (cell_array.GetSize() != in_num_cells) return false;
  
  for (int i = 0; i < cell_array.GetSize(); i++) {
    fp >> genotype_id;
    if (genotype_id == -1) continue;
    int genotype_index = -1;
    for (int j = 0; j < num_genotypes; j++) {
      if (genotype_array[j]->GetID() == genotype_id) {
        genotype_index = j;
        break;
      }
    }
    
    assert(genotype_index != -1);
    InjectGenome(i, genotype_array[genotype_index]->GetGenome(), 0);
  }
  
  sync_events = true;
  
  return true;
}

// This class is needed for the next function
class cTmpGenotype {
public:
  int id_num;
  int parent_id;
  int num_cpus;
  int total_cpus;
  double merit;
  int update_born;
  int update_dead;
  
  cGenotype *genotype;
  
  bool operator<( const cTmpGenotype rhs ) const {
    return id_num < rhs.id_num; }
};	


bool cPopulation::LoadDumpFile(cString filename, int update)
{
  // set the update if requested
  if ( update >= 0 )
    m_world->GetStats().SetCurrentUpdate(update);
  
  // Clear out the population
  for (int i = 0; i < cell_array.GetSize(); i++) KillOrganism(cell_array[i]);
  
  cout << "Loading: " << filename << endl;
  
  cInitFile input_file(filename);
  if (!input_file.WasOpened()) {
    tConstListIterator<cString> err_it(input_file.GetErrors());
    const cString* errstr = NULL;
    while ((errstr = err_it.Next())) cerr << "Error: " << *errstr << endl;
    cerr << "Error: Cannot load file: \"" << filename << "\"." << endl;
    exit(1);
  }
  
  // First, we read in all the genotypes and store them in a list
  
  vector<cTmpGenotype> genotype_vect;
  
  for (int line_id = 0; line_id < input_file.GetNumLines(); line_id++) {
    cString cur_line = input_file.GetLine(line_id);
    
    // Setup the genotype for this line...
    cTmpGenotype tmp;
    tmp.id_num      = cur_line.PopWord().AsInt();
    tmp.parent_id   = cur_line.PopWord().AsInt();
    /*parent_dist =*/          cur_line.PopWord().AsInt();
    tmp.num_cpus    = cur_line.PopWord().AsInt();
    tmp.total_cpus  = cur_line.PopWord().AsInt();
    /*length      =*/          cur_line.PopWord().AsInt();
    tmp.merit 	    = cur_line.PopWord().AsDouble();
    /*gest_time   =*/ cur_line.PopWord().AsInt();
    /*fitness     =*/ cur_line.PopWord().AsDouble();
    tmp.update_born = cur_line.PopWord().AsInt();
    tmp.update_dead = cur_line.PopWord().AsInt();
    /*depth       =*/ cur_line.PopWord().AsInt();
    cString name = cStringUtil::Stringf("org-%d", tmp.id_num);
    cGenome genome( cur_line.PopWord() );
    
    // we don't allow birth or death times larger than the current update
    if ( m_world->GetStats().GetUpdate() > tmp.update_born )
      tmp.update_born = m_world->GetStats().GetUpdate();
    if ( m_world->GetStats().GetUpdate() > tmp.update_dead )
      tmp.update_dead = m_world->GetStats().GetUpdate();
    
    tmp.genotype = m_world->GetClassificationManager().GetGenotypeLoaded(genome, tmp.update_born, tmp.id_num);
    tmp.genotype->SetName( name );
    
    genotype_vect.push_back( tmp );
  }
  
  // now, we sort them in ascending order according to their id_num
  sort( genotype_vect.begin(), genotype_vect.end() );
  // set the parents correctly
  
  vector<cTmpGenotype>::const_iterator it = genotype_vect.begin();
  for ( ; it != genotype_vect.end(); it++ ){
    vector<cTmpGenotype>::const_iterator it2 = it;
    cGenotype *parent = 0;
    // search backwards till we find the parent
    if ( it2 != genotype_vect.begin() )
      do{
        it2--;
        if ( (*it).parent_id == (*it2).id_num ){
          parent = (*it2).genotype;
          break;
        }	
      }
        while ( it2 != genotype_vect.begin() );
    (*it).genotype->SetParent( parent, NULL );
  }
  
  int cur_update = m_world->GetStats().GetUpdate(); 
  int current_cell = 0;
  bool soup_full = false;
  it = genotype_vect.begin();
  for ( ; it != genotype_vect.end(); it++ ){
    if ( (*it).num_cpus == 0 ){ // historic organism
                                // remove immediately, so that it gets transferred into the
                                // historic database. We change the update temporarily to the
                                // true death time of this organism, so that all stats are correct.
      m_world->GetStats().SetCurrentUpdate( (*it).update_dead );
      m_world->GetClassificationManager().RemoveGenotype( *(*it).genotype );
      m_world->GetStats().SetCurrentUpdate( cur_update );
    }
    else{ // otherwise, we insert as many organisms as we need
      for ( int i=0; i<(*it).num_cpus; i++ ){
        if ( current_cell >= cell_array.GetSize() ){
          soup_full = true;
          break;
        }	  
        InjectGenotype( current_cell, (*it).genotype );
        cPhenotype & phenotype = GetCell(current_cell).GetOrganism()->GetPhenotype();
        if ( (*it).merit > 0) phenotype.SetMerit( cMerit((*it).merit) );
        AdjustSchedule(GetCell(current_cell), phenotype.GetMerit());
        
        int lineage_label = 0;
        LineageSetupOrganism(GetCell(current_cell).GetOrganism(),
                             0, lineage_label,
                             (*it).genotype->GetParentGenotype());
        current_cell += 1;
      }
    }

// @DMB - This seems to be debugging output...
//    cout << (*it).id_num << " " << (*it).parent_id << " " << (*it).genotype->GetParentID() << " "
//         << (*it).genotype->GetNumOffspringGenotypes() << " " << (*it).num_cpus << " " << (*it).genotype->GetNumOrganisms() << endl;
    
    if (soup_full) {
      cout << "Warning: Too many organisms in population file, remainder ignored" << endl;
      break;
    }
  }
  sync_events = true;
  
  return true;
}


bool cPopulation::DumpMemorySummary(ofstream& fp)
{
  if (fp.good() == false) return false;
  
  // Dump the memory...
  
  for (int i = 0; i < cell_array.GetSize(); i++) {
    fp << i << " ";
    if (cell_array[i].IsOccupied() == false) {
      fp << "EMPTY" << endl;
    }
    else {
      cGenome & mem = cell_array[i].GetOrganism()->GetHardware().GetMemory();
      fp << mem.GetSize() << " "
        << mem.AsString() << endl;
    }
  }
  return true;
}

bool cPopulation::OK()
{
  // First check all sub-objects...
  if (!schedule->OK()) return false;
  
  // Next check organisms...
  for (int i = 0; i < cell_array.GetSize(); i++) {
    assert(cell_array[i].GetID() == i);
  }
  
  // And stats...
  assert(world_x * world_y == cell_array.GetSize());
  
  return true;
}


/**
* This function loads a genome from a given file, and initializes
 * a cpu with it.
 *
 * @param filename The name of the file to load.
 * @param in_cpu The grid-position into which the genome should be loaded.
 * @param merit An initial merit value.
 * @param lineage_label A value that allows to track the daughters of
 * this organism.
 **/

void cPopulation::Inject(const cGenome & genome, int cell_id, double merit, int lineage_label, double neutral)
{
  // If an invalid cell was given, choose a new ID for it.
  if (cell_id < 0) {
    switch (m_world->GetConfig().BIRTH_METHOD.Get()) {
      case POSITION_CHILD_FULL_SOUP_ELDEST:
        cell_id = reaper_queue.PopRear()->GetID();
      default:
        cell_id = 0;
    }
  }
  
  InjectGenome(cell_id, genome, lineage_label);
  cPhenotype& phenotype = GetCell(cell_id).GetOrganism()->GetPhenotype();
  phenotype.SetNeutralMetric(neutral);
    
  if (merit > 0) phenotype.SetMerit(cMerit(merit));
  AdjustSchedule(GetCell(cell_id), phenotype.GetMerit());
  
  LineageSetupOrganism(GetCell(cell_id).GetOrganism(), 0, lineage_label);
  
  if (GetNumDemes() > 1) {
    cDeme& deme = deme_array[GetCell(cell_id).GetDemeID()];

    // If we're using germlines, then we have to be a little careful here.
    // This should probably not be within Inject() since we mainly want it to
    // apply to the START_CREATURE? -- @JEB
    
    //@JEB This section is very messy to maintain consistency with other deme ways.
    
    if(m_world->GetConfig().DEMES_SEED_METHOD.Get() == 0) {
      if(m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 1) {
        if(deme.GetGermline().Size()==0) {  
          deme.GetGermline().Add(GetCell(cell_id).GetOrganism()->GetGenome());
        }
      }
    }
    else if(m_world->GetConfig().DEMES_SEED_METHOD.Get() == 1) {    
      if(m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 2) {
        //find the genotype we just created from the genome, and save it
        cGenotype * genotype = GetCell(cell_id).GetOrganism()->GetGenotype();
        deme.ReplaceGermline(*genotype);        
      } 
      else { // not germlines, save org as founder
        cGenotype * genotype = GetCell(cell_id).GetOrganism()->GetGenotype();
        deme.AddFounder(*genotype, &phenotype);
      }
      
      GetCell(cell_id).GetOrganism()->GetPhenotype().SetPermanentGermlinePropensity
        (m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get());
        
        
      if (m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get() >= 0.0) {
         GetCell(cell_id).GetOrganism()->GetPhenotype().SetPermanentGermlinePropensity
          ( m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get() );
      }

    }
  }
}

void cPopulation::InjectParasite(const cCodeLabel& label, const cGenome& injected_code, int cell_id)
{
  cOrganism* target_organism = cell_array[cell_id].GetOrganism();
  
  if (target_organism == NULL) return;
  
  cHardwareBase& child_cpu = target_organism->GetHardware();
  if (child_cpu.GetNumThreads() == m_world->GetConfig().MAX_CPU_THREADS.Get()) return;
  
  if (target_organism->InjectHost(label, injected_code)) {
    cInjectGenotype* child_genotype = m_world->GetClassificationManager().GetInjectGenotype(injected_code, NULL);
    
    target_organism->AddParasite(child_genotype);
    child_genotype->AddParasite();
    child_cpu.ThreadSetOwner(child_genotype);
    m_world->GetClassificationManager().AdjustInjectGenotype(*child_genotype);
  }
}


cPopulationCell& cPopulation::GetCell(int in_num)
{
  return cell_array[in_num];
}

void cPopulation::UpdateResources(const tArray<double> & res_change)
{
  resource_count.Modify(res_change);
}

void cPopulation::UpdateResource(int id, double change)
{
  resource_count.Modify(id, change);
}

void cPopulation::UpdateCellResources(const tArray<double> & res_change, 
                                      const int cell_id)
{
  resource_count.ModifyCell(res_change, cell_id);
}

void cPopulation::UpdateDemeCellResources(const tArray<double> & res_change, 
                                          const int cell_id)
{
  GetDeme(GetCell(cell_id).GetDemeID()).ModifyDemeResCount(res_change, cell_id);
}

void cPopulation::SetResource(int id, double new_level)
{
  resource_count.Set(id, new_level);
}

void cPopulation::ResetInputs(cAvidaContext& ctx)
{
  for (int i=0; i<GetSize(); i++)
  {
    cPopulationCell& cell = GetCell(i);
    cell.ResetInputs(ctx);
    if (cell.IsOccupied())
    {
      cell.GetOrganism()->ResetInput();
    }
  }
}

void cPopulation::BuildTimeSlicer(cChangeList * change_list)
{
  switch (m_world->GetConfig().SLICING_METHOD.Get()) {
    case SLICE_CONSTANT:
      schedule = new cConstSchedule(cell_array.GetSize());
      break;
    case SLICE_PROB_MERIT:
      schedule = new cProbSchedule(cell_array.GetSize(), m_world->GetRandom().GetInt(0x7FFFFFFF));
      break;
    case SLICE_DEME_PROB_MERIT:
      schedule = new cDemeProbSchedule(cell_array.GetSize(), m_world->GetRandom().GetInt(0x7FFFFFFF), deme_array.GetSize());
      break;
    case SLICE_PROB_DEMESIZE_PROB_MERIT:
      schedule = new cProbDemeProbSchedule(cell_array.GetSize(), m_world->GetRandom().GetInt(0x7FFFFFFF), deme_array.GetSize());
      break;
    case SLICE_INTEGRATED_MERIT:
      schedule = new cIntegratedSchedule(cell_array.GetSize());
      break;
    case SLICE_CONSTANT_BURST:
      schedule = new cConstBurstSchedule(cell_array.GetSize(), m_world->GetConfig().SLICING_BURST_SIZE.Get());
    default:
      cout << "Warning: Requested Time Slicer not found, defaulting to Integrated." << endl;
      schedule = new cIntegratedSchedule(cell_array.GetSize());
      break;
  }
  schedule->SetChangeList(change_list);
}


void cPopulation::FindEmptyCell(tList<cPopulationCell> & cell_list,
                                tList<cPopulationCell> & found_list)
{
  tListIterator<cPopulationCell> cell_it(cell_list);
  cPopulationCell * test_cell;
  
  while ( (test_cell = cell_it.Next()) != NULL) {
    // If this cell is empty, add it to the list...
    if (test_cell->IsOccupied() == false) found_list.Push(test_cell);
  }
}

// This function injects a new organism into the population at cell_id based
// on the genotype passed in.
void cPopulation::InjectGenotype(int cell_id, cGenotype *new_genotype)
{
  assert(cell_id >= 0 && cell_id < cell_array.GetSize());
  if (cell_id < 0 || cell_id >= cell_array.GetSize()) {
    m_world->GetDriver().RaiseFatalException(1, "InjectGenotype into nonexistent cell");
  }
  
  cAvidaContext& ctx = m_world->GetDefaultContext();
  
  cOrganism* new_organism = new cOrganism(m_world, ctx, new_genotype->GetGenome());
	
  //Coalescense Clade Setup
  new_organism->SetCCladeLabel(-1);  
  
  // Set the genotype...
  new_organism->SetGenotype(new_genotype);
  
  // Setup the phenotype...
  cPhenotype & phenotype = new_organism->GetPhenotype();
  phenotype.SetupInject(new_genotype->GetGenome());  //TODO  sets merit to lenght of genotype
  
  if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
    phenotype.SetMerit(cMerit(cMerit::EnergyToMerit(phenotype.GetStoredEnergy(), m_world)));
  } else {
    phenotype.SetMerit( cMerit(new_genotype->GetTestMerit(ctx)) );
  }
  
  // @CAO are these really needed?
  phenotype.SetLinesCopied( new_genotype->GetTestCopiedSize(ctx) );
  phenotype.SetLinesExecuted( new_genotype->GetTestExecutedSize(ctx) );
  phenotype.SetGestationTime( new_genotype->GetTestGestationTime(ctx) );
  
  // Prep the cell..
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_CHILD_FULL_SOUP_ELDEST &&
      cell_array[cell_id].IsOccupied() == true) {
    // Have to manually take this cell out of the reaper Queue.
    reaper_queue.Remove( &(cell_array[cell_id]) );
  }
  
  // Setup the child's mutation rates.  Since this organism is being injected
  // and has no parent, we should always take the rate from the environment.
  new_organism->MutationRates().Copy(cell_array[cell_id].MutationRates());
  
  
  // Activate the organism in the population...
  ActivateOrganism(ctx, new_organism, cell_array[cell_id]);

  // Log the injection of this organism if LOG_INJECT is set to 1 and
  // the current update number is >= INJECT_LOG_START
  if ( (m_world->GetConfig().LOG_INJECT.Get() == 1) &&
       (m_world->GetStats().GetUpdate() >= m_world->GetConfig().INJECT_LOG_START.Get()) ){

      cString tmpfilename = cStringUtil::Stringf("injectlog.dat");
      cDataFile& df = m_world->GetDataFile(tmpfilename);

      int update = m_world->GetStats().GetUpdate();
      int orgid = new_organism->GetID();
      int deme_id = m_world->GetPopulation().GetCell(cell_id).GetDemeID();
      int facing = new_organism->GetFacing();
      const char *orgname = (const char *)new_genotype->GetName();

      cString UpdateStr = cStringUtil::Stringf("%d %d %d %d %d %s", update, orgid, cell_id, deme_id, facing, orgname);
      df.WriteRaw(UpdateStr);
  }

}


// This function injects a new organism into the population at cell_id that
// is an exact clone of the organism passed in.

void cPopulation::InjectClone(int cell_id, cOrganism& orig_org)
{
  assert(cell_id >= 0 && cell_id < cell_array.GetSize());
  
  cAvidaContext& ctx = m_world->GetDefaultContext();
  
  cOrganism* new_organism = new cOrganism(m_world, ctx, orig_org.GetGenome());
  
  // Set the genotype...
  new_organism->SetGenotype(orig_org.GetGenotype());
  
  // Setup the phenotype...
  new_organism->GetPhenotype().SetupClone(orig_org.GetPhenotype());
  
  // Prep the cell..
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_CHILD_FULL_SOUP_ELDEST &&
      cell_array[cell_id].IsOccupied() == true) {
    // Have to manually take this cell out of the reaper Queue.
    reaper_queue.Remove( &(cell_array[cell_id]) );
  }
  
  // Setup the mutation rate based on the population cell...
  const int mut_source = m_world->GetConfig().MUT_RATE_SOURCE.Get();
  if (mut_source == 1) {
    // Update the mutation rates of each child from the environment....
    new_organism->MutationRates().Copy(cell_array[cell_id].MutationRates());
  } else {
    // Update the mutation rates of each child from its parent.
    new_organism->MutationRates().Copy(orig_org.MutationRates());
  }
  
  // Activate the organism in the population...
  ActivateOrganism(ctx, new_organism, cell_array[cell_id]);
}

// This function injects the child genome of an organism into the population at cell_id.
// Takes care of divide mutations.
void cPopulation::InjectChild(int cell_id, cOrganism& parent)
{
  assert(cell_id >= 0 && cell_id < cell_array.GetSize());
  
  cAvidaContext& ctx = m_world->GetDefaultContext();
      
  // Do mutations on the child genome, but restore it to its current state afterward.
  cGenome save_child = parent.ChildGenome();
  parent.GetHardware().Divide_DoMutations(ctx);
  cGenome child_genome = parent.ChildGenome();
  parent.GetHardware().Divide_TestFitnessMeasures(ctx);
  parent.ChildGenome() = save_child;
  cOrganism* new_organism = new cOrganism(m_world, ctx, child_genome);

  // Set the genotype...
  assert(parent.GetGenotype());  
  cGenotype* new_genotype = m_world->GetClassificationManager().GetGenotype(child_genome, parent.GetGenotype(), NULL);
  new_organism->SetGenotype(new_genotype);
    
  // Setup the phenotype...
  new_organism->GetPhenotype().SetupOffspring(parent.GetPhenotype(),child_genome);
  
  // Prep the cell..
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_CHILD_FULL_SOUP_ELDEST &&
      cell_array[cell_id].IsOccupied() == true) {
    // Have to manually take this cell out of the reaper Queue.
    reaper_queue.Remove( &(cell_array[cell_id]) );
  }
  
  // Setup the mutation rate based on the population cell...
  const int mut_source = m_world->GetConfig().MUT_RATE_SOURCE.Get();
  if (mut_source == 1) {
    // Update the mutation rates of each child from the environment....
    new_organism->MutationRates().Copy(cell_array[cell_id].MutationRates());
  } else {
    // Update the mutation rates of each child from its parent.
    new_organism->MutationRates().Copy(parent.MutationRates());
  }
  
  // Activate the organism in the population...
  ActivateOrganism(ctx, new_organism, cell_array[cell_id]);  

}


void cPopulation::InjectGenome(int cell_id, const cGenome& genome, int lineage_label)
{
  // Setup the genotype...
  cGenotype* new_genotype = m_world->GetClassificationManager().GetGenotypeInjected(genome, lineage_label);
  
  // The rest is done by InjectGenotype();
  InjectGenotype( cell_id, new_genotype );
}


void cPopulation::SerialTransfer(int transfer_size, bool ignore_deads)
{
  assert(transfer_size > 0);
  
  // If we are ignoring all dead organisms, remove them from the population.
  if (ignore_deads == true) {
    for (int i = 0; i < GetSize(); i++) {
      cPopulationCell & cell = cell_array[i];
      if (cell.IsOccupied() && cell.GetOrganism()->GetTestFitness(m_world->GetDefaultContext()) == 0.0) {
        KillOrganism(cell);
      }
    }
  }
  
  // If removing the dead was enough, stop here.
  if (num_organisms <= transfer_size) return;
  
  // Collect a vector of the occupied cells...
  vector<int> transfer_pool;
  transfer_pool.reserve(num_organisms);
  for (int i = 0; i < GetSize(); i++) {
    if (cell_array[i].IsOccupied()) transfer_pool.push_back(i);
  }
  
  // Remove the proper number of cells.
  const int removal_size = num_organisms - transfer_size;
  for (int i = 0; i < removal_size; i++) {
    int j = (int) m_world->GetRandom().GetUInt(transfer_pool.size());
    KillOrganism(cell_array[transfer_pool[j]]);
    transfer_pool[j] = transfer_pool.back();
    transfer_pool.pop_back();
  }
}


void cPopulation::PrintPhenotypeData(const cString& filename)
{
  set<int> ids;
  set<cString> complete;
  double average_shannon_diversity = 0.0;
  int num_orgs = 0; //could get from elsewhere, but more self-contained this way
  double average_num_tasks = 0.0;
  
  //implementing a very poor man's hash...
  tArray<int> phenotypes;
  tArray<int> phenotype_counts;
    
  for (int i = 0; i < cell_array.GetSize(); i++) {
    // Only look at cells with organisms in them.
    if (cell_array[i].IsOccupied() == false) continue;
    
    num_orgs++;
    const cPhenotype& phenotype = cell_array[i].GetOrganism()->GetPhenotype();
    
    int total_tasks = 0;
    int id = 0;
    cString key;
    for (int j = 0; j < phenotype.GetLastTaskCount().GetSize(); j++) {
      if (phenotype.GetLastTaskCount()[j] > 0) id += (1 << j);
      if (phenotype.GetLastTaskCount()[j] > 0) average_num_tasks += 1.0;
      key += cStringUtil::Stringf("%i-", phenotype.GetLastTaskCount()[j]);
      total_tasks += phenotype.GetLastTaskCount()[j];
    }
    ids.insert(id);
    complete.insert(key);

    // add one to our count for this key
    int k;
    for(k=0; k<phenotypes.GetSize(); k++)
    {
      if (phenotypes[k] == id) {
        phenotype_counts[k] = phenotype_counts[k] + 1;
        break;
      }
    }
    // this is a new key
    if (k == phenotypes.GetSize()) {
      phenotypes.Push(id);
      phenotype_counts.Push(1);
    }

    // go through again to calculate Shannon Diversity of task counts
    // now that we know the total number of tasks done
    double shannon_diversity = 0;
    for (int j = 0; j < phenotype.GetLastTaskCount().GetSize(); j++) {
      if (phenotype.GetLastTaskCount()[j] == 0) continue;
      double fraction = static_cast<double>(phenotype.GetLastTaskCount()[j]) / static_cast<double>(total_tasks);
      shannon_diversity -= fraction * log(fraction) / log(2.0);
    }
    
    average_shannon_diversity += static_cast<double>(shannon_diversity);
  }
  
  double shannon_diversity_of_phenotypes = 0.0;
  for (int j = 0; j < phenotype_counts.GetSize(); j++) {
    double fraction = static_cast<double>(phenotype_counts[j]) / static_cast<double>(num_orgs);
    shannon_diversity_of_phenotypes -= fraction * log(fraction) / log(2.0);
  }
  
  average_shannon_diversity /= static_cast<double>(num_orgs);
  average_num_tasks /= num_orgs;
  
  cDataFile& df = m_world->GetDataFile(filename);
  df.WriteTimeStamp();
  df.Write(m_world->GetStats().GetUpdate(), "Update");
  df.Write(static_cast<int>(ids.size()), "Unique Phenotypes (by task done)");
  df.Write(shannon_diversity_of_phenotypes, "Shannon Diversity of Phenotypes (by task done)");
  df.Write(static_cast<int>(complete.size()), "Unique Phenotypes (by task count)");
  df.Write(average_shannon_diversity, "Average Phenotype Shannon Diversity (by task count)");
  df.Write(average_num_tasks, "Average Task Diversity (number of different tasks)");
  df.Endl();
}

void cPopulation::PrintPhenotypeStatus(const cString& filename)
{
  cDataFile& df_phen = m_world->GetDataFile(filename);
  
  df_phen.WriteComment("Num orgs doing each task for each deme in population");
  df_phen.WriteTimeStamp();
  df_phen.Write(m_world->GetStats().GetUpdate(), "Update");
  
  cString comment;
  
  for (int i = 0; i < cell_array.GetSize(); i++) 
  {
    // Only look at cells with organisms in them.
    if (cell_array[i].IsOccupied() == false) continue;
    
    const cPhenotype& phenotype = cell_array[i].GetOrganism()->GetPhenotype();
    
    comment.Set("cur_merit %d;", i); 
    df_phen.Write(phenotype.GetMerit().GetDouble(), comment); 
    
    comment.Set("cur_merit_base %d;", i); 
    df_phen.Write(phenotype.GetCurMeritBase(), comment); 
    
    comment.Set("cur_merit_bonus %d;", i); 
    df_phen.Write(phenotype.GetCurBonus(), comment); 
    
    //    comment.Set("last_merit %d", i); 
    //    df_phen.Write(phenotype.GetLastMerit(), comment); 
    
    comment.Set("last_merit_base %d", i); 
    df_phen.Write(phenotype.GetLastMeritBase(), comment); 
    
    comment.Set("last_merit_bonus %d", i); 
    df_phen.Write(phenotype.GetLastBonus(), comment); 
    
    comment.Set("life_fitness %d", i); 
    df_phen.Write(phenotype.GetLifeFitness(), comment); 
    
    comment.Set("*"); 
    df_phen.Write("*", comment); 
    
  } 
  df_phen.Endl();
  
}     


bool cPopulation::UpdateMerit(int cell_id, double new_merit)
{
  assert( GetCell(cell_id).IsOccupied() == true);
  assert( new_merit >= 0.0 );
  
  cPhenotype & phenotype = GetCell(cell_id).GetOrganism()->GetPhenotype();
  double old_merit = phenotype.GetMerit().GetDouble(); 
  
  phenotype.SetMerit( cMerit(new_merit) );
  phenotype.SetLifeFitness(new_merit/phenotype.GetGestationTime()); 
  if (new_merit <= old_merit) {
	  phenotype.SetIsDonorCur(); }  
  else  { phenotype.SetIsReceiver(); } 
  AdjustSchedule(GetCell(cell_id), phenotype.GetMerit());
  
  return true;
}

void cPopulation::SetChangeList(cChangeList *change_list){
  schedule->SetChangeList(change_list);
}
cChangeList *cPopulation::GetChangeList(){
  return schedule->GetChangeList();
}

void cPopulation::AddBeginSleep(int cellID, int start_time) {
  sleep_log[cellID].Add(make_pair(start_time,-1));
}
  
void cPopulation::AddEndSleep(int cellID, int end_time) {
  pair<int,int> p = sleep_log[cellID][sleep_log[cellID].Size()-1];
  sleep_log[cellID].RemoveAt(sleep_log[cellID].Size()-1);
  sleep_log[cellID].Add(make_pair(p.first, end_time));
}

// Starts a new trial for each organism in the population
void cPopulation::NewTrial()
{
  for (int i=0; i< GetSize(); i++)
  {
    cPopulationCell& cell = GetCell(i);
    if (cell.IsOccupied())
    {
      cPhenotype & p =  cell.GetOrganism()->GetPhenotype();

      // Don't continue if the time used was zero
      if (p.GetTrialTimeUsed() != 0)
      {
        // Correct gestation time for speculative execution
        p.SetTrialTimeUsed(p.GetTrialTimeUsed() - cell.GetSpeculativeState());
        p.SetTimeUsed(p.GetTimeUsed() - cell.GetSpeculativeState());

        cell.GetOrganism()->NewTrial();
        cell.GetOrganism()->GetHardware().Reset();
        
        cell.SetSpeculativeState(0);
      }
    }
  }
  
  //Recalculate the stats immediately, so that if they are printed before a new update 
  //is processed, they accurately reflect this trial only...
  cStats& stats = m_world->GetStats();
  stats.ProcessUpdate();
  CalcUpdateStats();
}

/*
  CompeteOrganisms
  
   parents_survive => for any organism represented by >=1 child, the first created is the parent (has no mutations)
   dynamic_scaling => rescale the time interval such that the geometric mean of the highest fitness versus lower fitnesses
                      equals a time of 1 unit
*/

void cPopulation::CompeteOrganisms(int competition_type, int parents_survive)
{
  NewTrial();

  double total_fitness = 0;
  int num_cells = GetSize();
  tArray<double> org_fitness(num_cells); 

  double lowest_fitness = -1.0;
  double average_fitness = 0;
  double highest_fitness = -1.0;
  double lowest_fitness_copied = -1.0;
  double average_fitness_copied = 0;
  double highest_fitness_copied = -1.0;
  int different_orgs_copied = 0;
  int num_competed_orgs = 0;

  int num_trials = -1;
  
  int dynamic_scaling = 0; 

  if (competition_type==3) dynamic_scaling = 1;
  else if  (competition_type==4) dynamic_scaling = 2;
    
  // How many trials were there? -- same for every organism
  // we just need to find one...
  for (int i = 0; i < num_cells; i++) 
  {
    if (GetCell(i).IsOccupied())
    { 
      cPhenotype& p = GetCell(i).GetOrganism()->GetPhenotype();
      // We trigger a lot of asserts if the copied size is zero... 
      p.SetLinesCopied(p.GetGenomeLength());
      
      if ( (num_trials != -1) && (num_trials != p.GetTrialFitnesses().GetSize()) )
      {
        cout << "The number of trials is not the same for every organism in the population.\n";
        cout << "You need to remove all normal ways of replicating for CompeteOrganisms to work correctly.\n";
        exit(1);
      }
      
      num_trials = p.GetTrialFitnesses().GetSize();
    }
  }
  
  //If there weren't any trials then end here (but call new trial so things are set up for the next iteration)
  if (num_trials == 0) return;
  
  if (m_world->GetVerbosity() > VERBOSE_SILENT) cout << "==Compete Organisms==" << endl;

  tArray<double> min_trial_fitnesses(num_trials);
  tArray<double> max_trial_fitnesses(num_trials);
  tArray<double> avg_trial_fitnesses(num_trials);
  avg_trial_fitnesses.SetAll(0);

  bool init = false;
  // What is the min and max fitness in each trial
  for (int i = 0; i < num_cells; i++) 
  {
    if (GetCell(i).IsOccupied())
    {
      num_competed_orgs++;
      cPhenotype& p = GetCell(i).GetOrganism()->GetPhenotype();
      tArray<double> trial_fitnesses = p.GetTrialFitnesses();
      for (int t=0; t < num_trials; t++) 
      { 
        if ((!init) || (min_trial_fitnesses[t] > trial_fitnesses[t])) min_trial_fitnesses[t] = trial_fitnesses[t];
        if ((!init) || (max_trial_fitnesses[t] < trial_fitnesses[t])) max_trial_fitnesses[t] = trial_fitnesses[t];
        avg_trial_fitnesses[t] += trial_fitnesses[t];
      }
      init = true;
    }
  } 
  
  //divide averages for each trial
  for (int t=0; t < num_trials; t++) 
  {
    avg_trial_fitnesses[t] /= num_competed_orgs;
  }
  
  if (m_world->GetVerbosity() > VERBOSE_SILENT)
  {
    if (min_trial_fitnesses.GetSize() > 1)
    {
      for (int t=0; t < min_trial_fitnesses.GetSize(); t++) 
      {
        cout << "Trial #" << t << " Min Fitness = " << min_trial_fitnesses[t] << ", Avg fitness = " << avg_trial_fitnesses[t] << " Max Fitness = " << max_trial_fitnesses[t] << endl;
      }
    }
  }
  
  bool using_trials = true;
  for (int i = 0; i < num_cells; i++) 
  {
    if (GetCell(i).IsOccupied())
    {
      double fitness = 0.0;
      cPhenotype& p = GetCell(i).GetOrganism()->GetPhenotype();
      //Don't need to reset trial_fitnesses because we will call cPhenotype::OffspringReset on the entire pop
      tArray<double> trial_fitnesses = p.GetTrialFitnesses();

      //If there are no trial fitnesses...use the actual fitness.
      if (trial_fitnesses.GetSize() == 0)
      {
        using_trials = false;
        trial_fitnesses.Push(p.GetFitness());
      }
      switch (competition_type)
      {
        //Geometric Mean        
        case 0:
        case 3:
        case 4:    
          //Treat as logs to avoid overflow when multiplying very large fitnesses
          fitness = 0;
          for (int t=0; t < trial_fitnesses.GetSize(); t++) 
          { 
            fitness += log(trial_fitnesses[t]); 
          }
          fitness /= (double)trial_fitnesses.GetSize();
          fitness = exp( fitness );
        break;
  
        //Product
        case 5:
          //Treat as logs to avoid overflow when multiplying very large fitnesses
          fitness = 0;
          for (int t=0; t < trial_fitnesses.GetSize(); t++) 
          { 
            fitness += log(trial_fitnesses[t]); 
          }       
          fitness = exp( fitness );
        break;
         
        //Geometric Mean of normalized values
        case 1:
          fitness = 1.0;        
          for (int t=0; t < trial_fitnesses.GetSize(); t++) 
          { 
             fitness*=trial_fitnesses[t] / max_trial_fitnesses[t]; 
          }
          fitness = exp( (1.0/((double)trial_fitnesses.GetSize())) * log(fitness) );
        break;
        
        //Arithmetic Mean
        case 2:
          fitness = 0;
          for (int t=0; t < trial_fitnesses.GetSize(); t++) 
          { 
            fitness+=trial_fitnesses[t]; 
          }
          fitness /= (double)trial_fitnesses.GetSize();
        break;
      
        default:
          m_world->GetDriver().RaiseFatalException(1, "Unknown CompeteOrganisms method");
      }
      if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Trial fitness in cell " << i << " = " << fitness << endl;
      org_fitness[i] = fitness;
      total_fitness += fitness;
      
      if ((highest_fitness == -1.0) || (fitness > highest_fitness)) highest_fitness = fitness;
      if ((lowest_fitness == -1.0) || (fitness < lowest_fitness)) lowest_fitness = fitness;
    } // end if occupied
  }
  average_fitness = total_fitness / num_competed_orgs;
 
  //Rescale by the geometric mean of the difference from the top score and the average
  if ( dynamic_scaling == 1 ) {
    double dynamic_factor = 1.0;
    if (highest_fitness > 0) {
      dynamic_factor = 2 / highest_fitness;
    }
    
    total_fitness = 0;
    for (int i = 0; i < num_cells; i++) {
      if ( GetCell(i).IsOccupied() ) {
        org_fitness[i] *= dynamic_factor;
        total_fitness += org_fitness[i];
      }
    }
  }
  
  // Rescale geometric mean to 1
  else if ( dynamic_scaling == 2 ) {
    int num_org = 0;
    double dynamic_factor = 1.0;
    for (int i = 0; i < num_cells; i++) {
      if ( GetCell(i).IsOccupied() && (org_fitness[i] > 0.0)) {
        num_org++;
        dynamic_factor += log(org_fitness[i]);
      }
    }
        
    cout << "Scaling factor = " << dynamic_factor << endl;
    if (num_org > 0) dynamic_factor = exp(dynamic_factor / (double)num_org);
    cout << "Scaling factor = " << dynamic_factor << endl;
    
    total_fitness = 0;

    for (int i = 0; i < num_cells; i++) {
      if ( GetCell(i).IsOccupied() ) {
        org_fitness[i] /= dynamic_factor;
        total_fitness += org_fitness[i];
      }
    }
  }
  
  // Pick which orgs should be in the next generation. (Filling all cells)
  tArray<int> new_orgs(num_cells);
  for (int i = 0; i < num_cells; i++) {
    double birth_choice = (double) m_world->GetRandom().GetDouble(total_fitness);
    double test_total = 0;
    for (int test_org = 0; test_org < num_cells; test_org++) {
      test_total += org_fitness[test_org];
      if (birth_choice < test_total) {
        new_orgs[i] = test_org;
        if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Propagating from cell " << test_org << " to " << i << endl;
        if ((highest_fitness_copied == -1.0) || (org_fitness[test_org] > highest_fitness_copied)) highest_fitness_copied = org_fitness[test_org];
        if ((lowest_fitness_copied == -1.0) || (org_fitness[test_org] < lowest_fitness_copied)) lowest_fitness_copied = org_fitness[test_org];
        average_fitness_copied += org_fitness[test_org];
        break;
      }
    }
  }
  // average assumes we fill all cells.
  average_fitness_copied /= num_cells;
  
  // Track how many of each org we should have.
  tArray<int> org_count(num_cells);
  org_count.SetAll(0);
  for (int i = 0; i < num_cells; i++) {
    org_count[new_orgs[i]]++;
  }
  
  // Reset organism phenotypes that have successfully divided! Must do before injecting children.
  // -- but not the full reset if we are using trials, the trial reset should already cover things like task counts, etc.
  // calling that twice would erase this information before it could potentially be output between NewTrial and CompeteOrganisms events.
  for (int i = 0; i < num_cells; i++) {
    if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Cell " << i << " has " << org_count[i] << " copies in the next generation" << endl;  

    if (org_count[i] > 0) {
      different_orgs_copied++;
      cPhenotype& p = GetCell(i).GetOrganism()->GetPhenotype();
      if (using_trials)
      {
        p.TrialDivideReset( GetCell(i).GetOrganism()->GetGenome() );
      }
      else //trials not used
      {
        //TrialReset has never been called so we need the entire routine to make "last" of "cur" stats.
        p.DivideReset( GetCell(i).GetOrganism()->GetGenome() );
      }
    }
  }
  
  tArray<bool> is_init(num_cells); 
  is_init.SetAll(false);
  
  // Copy orgs until all org counts are 1.
  int last_from_cell_id = 0;
  int last_to_cell_id = 0;
  while (true) {
    // Find the next org to copy...
    int from_cell_id, to_cell_id;
    for (from_cell_id = last_from_cell_id; from_cell_id < num_cells; from_cell_id++) {
      if (org_count[from_cell_id] > 1) break;
    }
    last_from_cell_id = from_cell_id;
    
    // Stop if we didn't find another org to copy
    if (from_cell_id == num_cells) break;
    
    for (to_cell_id = last_to_cell_id; to_cell_id < num_cells; to_cell_id++) {
      if (org_count[to_cell_id] == 0) break;
    }
    last_to_cell_id = to_cell_id;
    
    // We now have both a "from" and a "to" org....
    org_count[from_cell_id]--;
    org_count[to_cell_id]++;
    
    cOrganism * organism = GetCell(from_cell_id).GetOrganism();
    organism->ChildGenome() = organism->GetGenome();
    if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Injecting Child " << from_cell_id << " to " << to_cell_id << endl;  
    InjectChild( to_cell_id, *organism );  
    
    is_init[to_cell_id] = true;
  }

  if (!parents_survive)
  {
    // Now create children from remaining cells into themselves
    for (int cell_id = 0; cell_id < num_cells; cell_id++) {
      if (!is_init[cell_id])
      {
        cOrganism * organism = GetCell(cell_id).GetOrganism();
        organism->ChildGenome() = organism->GetGenome();
        if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Re-injecting Self " << cell_id << " to " << cell_id << endl;  
        InjectChild( cell_id, *organism ); 
      }
    }
  }
  
  
  if (m_world->GetVerbosity() > VERBOSE_SILENT)
  {
    cout << "Competed: Min fitness = " << lowest_fitness << ", Avg fitness = " << average_fitness << " Max fitness = " << highest_fitness << endl;
    cout << "Copied  : Min fitness = " << lowest_fitness_copied << ", Avg fitness = " << average_fitness_copied << ", Max fitness = " << highest_fitness_copied << endl;
    cout << "Copied  : Different organisms = " << different_orgs_copied << endl;
  }
  if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Genotype Count: " << m_world->GetClassificationManager().GetGenotypeCount() << endl;
  
  // copy stats to cStats, so that these can be remembered and printed
  m_world->GetStats().SetCompetitionTrialFitnesses(avg_trial_fitnesses);
  m_world->GetStats().SetCompetitionFitnesses(average_fitness, lowest_fitness, highest_fitness, average_fitness_copied, lowest_fitness_copied, highest_fitness_copied);
  m_world->GetStats().SetCompetitionOrgsReplicated(different_orgs_copied);
  
  NewTrial();
}

/* This routine is designed to change values in the resource count in the 
   middle of a run.  This is designed to work with cActionChangeEnvironment
   routine BDB 22-Feb-2008 */

void cPopulation::UpdateResourceCount(const int Verbosity) {
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  int deme_res_index = -1;
  int num_deme_res = 0;

  //setting size of global and deme-level resources
  for(int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if(res->GetDemeResource())
      num_deme_res++;
  }
  
  for(int i = 0; i < GetNumDemes(); i++) {
    cResourceCount tmp_deme_res_count(num_deme_res);
    GetDeme(i).SetDemeResourceCount(tmp_deme_res_count);
  }

  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (!res->GetDemeResource()) {
      global_res_index++;
      const double decay = 1.0 - res->GetOutflow();
      resource_count.Setup(global_res_index, res->GetName(), res->GetInitial(), 
                           res->GetInflow(), decay,
                           res->GetGeometry(), res->GetXDiffuse(),
                           res->GetXGravity(), res->GetYDiffuse(), 
                           res->GetYGravity(), res->GetInflowX1(), 
                           res->GetInflowX2(), res->GetInflowY1(), 
                           res->GetInflowY2(), res->GetOutflowX1(), 
                           res->GetOutflowX2(), res->GetOutflowY1(), 
                           res->GetOutflowY2(), res->GetCellListPtr(),
                           Verbosity);
      m_world->GetStats().SetResourceName(global_res_index, res->GetName());
    } else if(res->GetDemeResource()) {
      deme_res_index++;
      for(int j = 0; j < GetNumDemes(); j++) {
        GetDeme(j).SetupDemeRes(deme_res_index, res, Verbosity);
        // could add deme resources to global resource stats here
      }
    } else {
      cerr<< "ERROR: Resource \"" << res->GetName() <<"\"is not a global or deme resource.  Exit";
      exit(1);
    }
  }

}

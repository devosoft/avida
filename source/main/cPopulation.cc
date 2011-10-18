/*
 *  cPopulation.cc
 *  Avida
 *
 *  Called "population.cc" prior to 12/5/05.
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

#include "cPopulation.h"

#include "avida/core/Sequence.h"

#include "AvidaTools.h"

#include "cAvidaContext.h"
#include "cBioGroup.h"
#include "cBioGroupManager.h"
#include "cClassificationManager.h"
#include "cCPUTestInfo.h"
#include "cCodeLabel.h"
#include "cConstBurstSchedule.h"
#include "cConstSchedule.h"
#include "cDataFile.h"
#include "cDemePlaceholderUnit.h"
#include "cDemeProbSchedule.h"
#include "cEnvironment.h"
#include "cGenomeTestMetrics.h"
#include "cBGGenotype.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInitFile.h"
#include "cInstSet.h"
#include "cIntegratedSchedule.h"
#include "cOrganism.h"
#include "cParasite.h"
#include "cPhenotype.h"
#include "cPopulationCell.h"
#include "cProbSchedule.h"
#include "cProbDemeProbSchedule.h"
#include "cRandom.h"
#include "cResource.h"
#include "cResourceCount.h"
#include "cSaleItem.h"
#include "cStats.h"
#include "cTestCPU.h"
#include "cTopology.h"
#include "cWorld.h"
#include "tArrayUtils.h"
#include "tKVPair.h"
#include "tHashMap.h"
#include "tManagedPointerArray.h"

#include "cHardwareCPU.h"

#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <set>
#include <cfloat>
#include <cmath>
#include <climits>
#include <limits>

using namespace std;
using namespace AvidaTools;


cPopulation::cPopulation(cWorld* world)  
: m_world(world)
, schedule(NULL)
//, resource_count(world->GetEnvironment().GetResourceLib().GetSize())
, birth_chamber(world)
, environment(world->GetEnvironment())
, num_organisms(0)
, sync_events(false)
, m_hgt_resid(-1)
{
  // Avida specific information.
  world_x = world->GetConfig().WORLD_X.Get();
  world_y = world->GetConfig().WORLD_Y.Get();
  world_z = world->GetConfig().WORLD_Z.Get();

  int num_demes = m_world->GetConfig().NUM_DEMES.Get();
  const int num_cells = world_x * world_y * world_z;
  const int geometry = world->GetConfig().WORLD_GEOMETRY.Get();

  if (m_world->GetConfig().ENERGY_CAP.Get() == -1) {
    m_world->GetConfig().ENERGY_CAP.Set(std::numeric_limits<double>::max());
  }

  if (m_world->GetConfig().LOG_SLEEP_TIMES.Get() == 1)  {
    sleep_log = new tVector<pair<int,int> >[world_x*world_y];
  }
  // Print out world details
  if (world->GetVerbosity() > VERBOSE_NORMAL) {
    cout << "Building world " << world_x << "x" << world_y << "x" << world_z << " = " << num_cells << " organisms." << endl;
    switch(geometry) {
      case nGeometry::GRID: { cout << "Geometry: Bounded grid" << endl; break; }
      case nGeometry::TORUS: { cout << "Geometry: Torus" << endl; break; }
      case nGeometry::CLIQUE: { cout << "Geometry: Clique" << endl; break; }
      case nGeometry::HEX: { cout << "Geometry: Hex" << endl; break; }
      case nGeometry::LATTICE: { cout << "Geometry: Lattice" << endl; break; }
      case nGeometry::PARTIAL: { cout << "Geometry: Partial" << endl; break; }
			case nGeometry::RANDOM_CONNECTED: { cout << "Geometry: Random connected" << endl; break; }
      case nGeometry::SCALE_FREE: { cout << "Geometry: Scale-free" << endl; break; }

      default:
        cout << "Unknown geometry!" << endl;
        assert(false);
    }
  }

  // Invalid settings should be changed to one deme
  if (num_demes <= 0) {
    num_demes = 1; // One population == one deme.
  }

  // Not ready for prime time yet; need to look at how resources work in this now
  // more complex world.
  assert(world_z == 1);

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

  if (num_demes > 1) {
    assert(birth_method != POSITION_OFFSPRING_FULL_SOUP_ELDEST);
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
    if (world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST) {
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
        build_grid(&cell_array.begin()[i],
                   &cell_array.begin()[i+deme_size],
                   deme_size_x, deme_size_y);
        break;
      case nGeometry::TORUS:
        build_torus(&cell_array.begin()[i],
                    &cell_array.begin()[i+deme_size],
                    deme_size_x, deme_size_y);
        break;
      case nGeometry::CLIQUE:
        build_clique(&cell_array.begin()[i],
                     &cell_array.begin()[i+deme_size],
                     deme_size_x, deme_size_y);
        break;
      case nGeometry::HEX:
        build_hex(&cell_array.begin()[i],
                  &cell_array.begin()[i+deme_size],
                  deme_size_x, deme_size_y);
        break;
      case nGeometry::LATTICE:
        build_lattice(&cell_array.begin()[i],
                      &cell_array.begin()[i+deme_size],
                      deme_size_x, deme_size_y, world_z);
        break;
      case nGeometry::RANDOM_CONNECTED:
        build_random_connected_network(&cell_array.begin()[i],
                                       &cell_array.begin()[i+deme_size],
                                       deme_size_x, deme_size_y, m_world->GetRandom());
        break;
      case nGeometry::SCALE_FREE:
        build_scale_free(&cell_array.begin()[i], &cell_array.begin()[i+deme_size],
                         world->GetConfig().SCALE_FREE_M.Get(),
                         world->GetConfig().SCALE_FREE_ALPHA.Get(),
                         world->GetConfig().SCALE_FREE_ZERO_APPEAL.Get(),
                         m_world->GetRandom());
        break;
      default:
        assert(false);
    }
  }

  BuildTimeSlicer();

  // Setup the resources...
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  int deme_res_index = -1;
  int num_deme_res = 0;

  //setting size of global and deme-level resources
  for(int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (res->GetDemeResource())
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

    // check to see if this is the hgt resource:
    if (res->GetHGTMetabolize()) {
      if (m_hgt_resid != -1) {
	m_world->GetDriver().RaiseFatalException(-1, "Only one HGT resource is currently supported.");
      }
      m_hgt_resid = i;
    }

    if (!res->GetDemeResource()) {
      global_res_index++;
      const double decay = 1.0 - res->GetOutflow();
      resource_count.Setup(world, global_res_index, res->GetName(), res->GetInitial(),
                           res->GetInflow(), decay,
                           res->GetGeometry(), res->GetXDiffuse(),
                           res->GetXGravity(), res->GetYDiffuse(),
                           res->GetYGravity(), res->GetInflowX1(),
                           res->GetInflowX2(), res->GetInflowY1(),
                           res->GetInflowY2(), res->GetOutflowX1(),
                           res->GetOutflowX2(), res->GetOutflowY1(),
                           res->GetOutflowY2(), res->GetCellListPtr(),
                           res->GetCellIdListPtr(), world->GetVerbosity(),
                           res->GetDynamicResource(), res->GetPeaks(), 
                           res->GetMinHeight(), res->GetMinRadius(), res->GetRadiusRange(),
                           res->GetAh(), res->GetAr(),
                           res->GetAcx(), res->GetAcy(),
                           res->GetHStepscale(), res->GetRStepscale(),
                           res->GetCStepscaleX(), res->GetCStepscaleY(),
                           res->GetHStep(), res->GetRStep(),
                           res->GetCStepX(), res->GetCStepY(),
                           res->GetUpdateDynamic(), res->GetPeakX(), res->GetPeakY(),
                           res->GetHeight(), res->GetSpread(), res->GetPlateau(), res->GetDecay(),
                           res->GetMaxX(), res->GetMinX(), res->GetMaxY(), res->GetMinY(), res->GetAscaler(),res->GetUpdateStep(),
                           res->GetHalo(), res->GetHaloInnerRadius(), res->GetHaloWidth(),
                           res->GetHaloAnchorX(), res->GetHaloAnchorY(), res->GetMoveSpeed(),
                           res->GetPlateauInflow(), res->GetPlateauOutflow(), 
                           res->GetIsPlateauCommon(), res->GetFloor(), res->GetHabitat(), 
                           res->GetMinSize(), res->GetMaxSize(), res->GetConfig(), res->GetCount(), res->GetResistance(), res->GetGradient()
                           ); 
      m_world->GetStats().SetResourceName(global_res_index, res->GetName());
    } else if (res->GetDemeResource()) {
      deme_res_index++;
      for(int j = 0; j < GetNumDemes(); j++) {
        GetDeme(j).SetupDemeRes(deme_res_index, res, world->GetVerbosity(), world);                                  
        // could add deme resources to global resource stats here
      }
    } else {
      cerr<< "ERROR: Resource \"" << res->GetName() <<"\"is not a global or deme resource.  Exit";
      exit(1);
    }
  }
  
  // if HGT is on, make sure there's a resource for it:
  if (m_world->GetConfig().ENABLE_HGT.Get() && (m_hgt_resid == -1)) {
    m_world->GetDriver().NotifyWarning("HGT is enabled, but no HGT resource is defined; add hgt=1 to a single resource in the environment file.");
  }

}

bool cPopulation::InitiatePop(cUserFeedback* feedback)
{
  Genome start_org;
  const cString& filename = m_world->GetConfig().START_ORGANISM.Get();

  if (filename != "-" && filename != "") {
    if (!start_org.LoadFromDetailFile(filename, m_world->GetWorkingDir(), m_world->GetHardwareManager(), *feedback)) return false;
    if (start_org.GetSize() != 0) {
      Inject(start_org, SRC_ORGANISM_FILE_LOAD, m_world->GetDefaultContext());
    } else {
      if (feedback) feedback->Warning("zero length start organism, not injecting into initial population");
    }
  } else {
    if (feedback) feedback->Warning("no start organism specified");
  }

  return true;
}


cPopulation::~cPopulation()
{
  for (int i = 0; i < cell_array.GetSize(); i++) KillOrganism(cell_array[i], m_world->GetDefaultContext()); 
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

bool cPopulation::ActivateOffspring(cAvidaContext& ctx, const Genome& offspring_genome, cOrganism* parent_organism)
{
  if (m_world->GetConfig().FASTFORWARD_NUM_ORGS.Get() > 0 && GetNumOrganisms() >= m_world->GetConfig().FASTFORWARD_NUM_ORGS.Get())
  {
    return true;
  }
  assert(parent_organism != NULL);
  bool is_doomed = false;
  int doomed_cell = (m_world->GetConfig().WORLD_X.Get() * m_world->GetConfig().WORLD_Y.Get()) - 1; //Also at the end of cPopulation::ActivateOrganism
  tArray<cOrganism*> offspring_array;
  tArray<cMerit> merit_array;

  // Update the parent's phenotype.
  // This needs to be done before the parent goes into the birth chamber
  // or the merit doesn't get passed onto the offspring correctly
  cPhenotype& parent_phenotype = parent_organism->GetPhenotype();
  parent_phenotype.DivideReset(parent_organism->GetGenome().GetSequence());

  birth_chamber.SubmitOffspring(ctx, offspring_genome, parent_organism, offspring_array, merit_array);

  // First, setup the genotype of all of the offspring.
  const int parent_id = parent_organism->GetOrgInterface().GetCellID();
  assert(parent_id >= 0 && parent_id < cell_array.GetSize());
  cPopulationCell& parent_cell = cell_array[parent_id];

  // If this is multi-process Avida, test to see if we should send the offspring
  // to a different world.  We check this here so that 1) we avoid all the extra
  // work below in the case of a migration event and 2) so that we don't mess up
  // and mistakenly kill the parent.
  if (m_world->GetConfig().ENABLE_MP.Get()) {
    tArray<cOrganism*> non_migrants;
    tArray<cMerit> non_migrant_merits;
    for (int i=0; i<offspring_array.GetSize(); ++i) {
      if (m_world->TestForMigration()) {
        // this offspring is outta here!
        m_world->MigrateOrganism(offspring_array[i], parent_cell, merit_array[i], parent_organism->GetLineageLabel());
        delete offspring_array[i]; // this offspring isn't hanging around.
      } else {
        // boring; stay here.
        non_migrants.Push(offspring_array[i]);
        non_migrant_merits.Push(merit_array[i]);
      }
    }
    offspring_array = non_migrants;
    merit_array = non_migrant_merits;
  }

  tArray<int> target_cells(offspring_array.GetSize());

  // Loop through choosing the later placement of each offspring in the population.
  bool parent_alive = true;  // Will the parent live through this process?
  for (int i = 0; i < offspring_array.GetSize(); i++) {
    /*
    THIS code will remove zero merit orgnaisms, thus never putting them into the scheduler.
    WARNING: uncommenting this code will break consistancy, but will generalize the solution.
    Currently, only asexual organisms that use the energy model are removed when they have zero merit.
    If this code gets added then remove the "if (merit_array[0].GetDouble() <= 0.0)" block from cBirthChamber::DoAsexBirth,
    does not break consistancy for test energy_deme_level_res

    if (merit_array[i].GetDouble() <= 0.0) {
    // no weaklings!
    if (offspring_array.GetSize() > 1) {
    offspring_array.Swap(i, offspring_array.GetSize()-1);
    offspring_array = offspring_array.Subset(0, offspring_array.GetSize()-2);
    } else {
    offspring_array.ResizeClear(0);
    break;
    }
    --i;
    continue;
    }
    */
    target_cells[i] = PositionOffspring(parent_cell, ctx, m_world->GetConfig().ALLOW_PARENT.Get()).GetID(); 
    // If we replaced the parent, make a note of this.
    if (target_cells[i] == parent_cell.GetID()) parent_alive = false;

    const int mut_source = m_world->GetConfig().MUT_RATE_SOURCE.Get();
    if (mut_source == 1) {
      // Update the mutation rates of each offspring from the environment....
      offspring_array[i]->MutationRates().Copy(GetCell(target_cells[i]).MutationRates());
    } 
    else {
      // Update the mutation rates of each offspring from its parent.
      offspring_array[i]->MutationRates().Copy(parent_organism->MutationRates());
      // If there is a meta-mutation rate, do tests for it.
      if (offspring_array[i]->MutationRates().GetMetaCopyMutProb() > 0.0) {
        offspring_array[i]->MutationRates().DoMetaCopyMut(ctx);
      }
    }

    // Update the phenotypes of each offspring....
    const Sequence& genome = offspring_array[i]->GetGenome().GetSequence();
    offspring_array[i]->GetPhenotype().SetupOffspring(parent_phenotype, genome);
    offspring_array[i]->GetPhenotype().SetMerit(merit_array[i]);
    offspring_array[i]->SetLineageLabel(parent_organism->GetLineageLabel());

    //By default, store the parent cclade, this may get modified in ActivateOrgansim (@MRR)
    offspring_array[i]->SetCCladeLabel(parent_organism->GetCCladeLabel());

    // If inherited reputation is turned on, set the offspring's reputation
    // to that of its parent.
    if (m_world->GetConfig().INHERIT_REPUTATION.Get() == 1) {
      offspring_array[i]->SetReputation(parent_organism->GetReputation());
    } 
    else if (m_world->GetConfig().INHERIT_REPUTATION.Get() == 2) {
      offspring_array[i]->SetTag(parent_organism->GetTag());
    }
    else if (m_world->GetConfig().INHERIT_REPUTATION.Get() == 3) {
      offspring_array[i]->SetTag(parent_organism->GetTag());
      offspring_array[i]->SetReputation(parent_organism->GetReputation());
    }

    // If spatial groups are used, put the offspring in the
    // parents' group, if tolerances are used check if the offspring
    // is successfully born into the parent's group or successfully immigrates
    // into another group.
    if (m_world->GetConfig().USE_FORM_GROUPS.Get()) {
      // If tolerances are on ... @JJB
      if ((m_world->GetConfig().TOLERANCE_WINDOW.Get() != 0) && (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 0)) {
        bool joins_group = AttemptOffspringParentGroup(ctx, parent_organism, offspring_array[i]);
        if (!joins_group) {
          target_cells[i] = doomed_cell;
          is_doomed = true;
        }
      }
      else {
        // If not using tolerances, put the offspring in the parent's group.
        assert(parent_organism->HasOpinion());
        if (m_world->GetConfig().INHERIT_OPINION.Get()) {
          int group = parent_organism->GetOpinion().first;
          offspring_array[i]->SetOpinion(group);
          JoinGroup(offspring_array[i], group);
        }
      }
    }
    // if parent org has executed teach_offspring intruction, teach the offspring the parent's learned foraging/targeting behavior
    if (parent_organism->IsTeacher()) offspring_array[i]->SetForageTarget(parent_organism->GetForageTarget());
  }
  
  // If we're not about to kill the parent, do some extra work on it.
  if (parent_alive == true) {
    if (parent_phenotype.GetMerit().GetDouble() <= 0.0) {
      // no weakling parents either!
      parent_organism->GetPhenotype().SetToDie();
      parent_alive = false;
    }
    else {
      // Reset inputs and re-calculate merit if required
      if (m_world->GetConfig().RESET_INPUTS_ON_DIVIDE.Get() > 0){
        environment.SetupInputs(ctx, parent_cell.m_inputs);

        int pc_phenotype = m_world->GetConfig().PRECALC_PHENOTYPE.Get();
        if (pc_phenotype) {
          cCPUTestInfo test_info;
          cTestCPU* test_cpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
          test_info.UseManualInputs(parent_cell.GetInputs()); // Test using what the environment will be
          Genome mg(parent_organism->GetGenome());
          mg.SetSequence(parent_organism->GetHardware().GetMemory());
          test_cpu->TestGenome(ctx, test_info, mg); // Use the true genome
          if (pc_phenotype & 1) {  // If we must update the merit
            parent_phenotype.SetMerit(test_info.GetTestPhenotype().GetMerit());
          }
          if (pc_phenotype & 2) {  // If we must update the gestation time
            parent_phenotype.SetGestationTime(test_info.GetTestPhenotype().GetGestationTime());
          }
          if (pc_phenotype & 4) {  // If we must update the last instruction counts
            parent_phenotype.SetTestCPUInstCount(test_info.GetTestPhenotype().GetLastInstCount());
          }
          parent_phenotype.SetFitness(parent_phenotype.GetMerit().CalcFitness(parent_phenotype.GetGestationTime())); // Update fitness
          delete test_cpu;
        }
      }
      AdjustSchedule(parent_cell, parent_phenotype.GetMerit());

      if (!is_doomed) {
        // In a local run, face the offspring toward the parent.
        const int birth_method = m_world->GetConfig().BIRTH_METHOD.Get();
        if (birth_method < NUM_LOCAL_POSITION_OFFSPRING || birth_method == POSITION_OFFSPRING_PARENT_FACING) {
          for (int i = 0; i < offspring_array.GetSize(); i++) {
            GetCell(target_cells[i]).Rotate(parent_cell);
          }
        }
      }
      // Purge the mutations since last division
      parent_organism->OffspringGenome().GetSequence().GetMutationSteps().Clear();
    }
  }

  // Do any statistics on the parent that just gave birth...
  parent_organism->HandleGestation();

  // Place all of the offspring...
  for (int i = 0; i < offspring_array.GetSize(); i++) {
    //@JEB - we may want to pass along some state information from parent to offspring
    if ( (m_world->GetConfig().EPIGENETIC_METHOD.Get() == EPIGENETIC_METHOD_OFFSPRING)
      || (m_world->GetConfig().EPIGENETIC_METHOD.Get() == EPIGENETIC_METHOD_BOTH) ) {
        offspring_array[i]->GetHardware().InheritState(parent_organism->GetHardware());
    }    
    ActivateOrganism(ctx, offspring_array[i], GetCell(target_cells[i]));
  }
  return parent_alive;
}

bool cPopulation::TestForParasiteInteraction(cOrganism* infected_host, cOrganism* target_host)
{
  //default to failing the interaction
  bool interaction_fails = true;
  int infection_mechanism = m_world->GetConfig().INFECTION_MECHANISM.Get();
  
  cPhenotype& parent_phenotype = infected_host->GetPhenotype();
  
  tArray<int> host_task_counts = target_host->GetPhenotype().GetLastHostTaskCount();
  tArray<int> parasite_task_counts = parent_phenotype.GetLastParasiteTaskCount();

  
  // 1: Parasite must match at least 1 task the host does (Inverse GFG)
  if(infection_mechanism == 1)
  {
    //handle skipping of first task
    int start = 0;
    if(m_world->GetConfig().INJECT_SKIP_FIRST_TASK.Get())
      start += 1;
    
    //find if there is a matching task
    for (int i=start;i<host_task_counts.GetSize();i++)
    {
      if(host_task_counts[i] > 0 && parasite_task_counts[i] > 0)
      {
        //inject should succeed if there is a matching task
        interaction_fails = false;
      }
    }
  }

  // 2: Parasite must perform at least one task the host does not (GFG)
  if(infection_mechanism == 2)
  {
    //handle skipping of first task
    int start = 0;
    if(m_world->GetConfig().INJECT_SKIP_FIRST_TASK.Get())
      start += 1;
    
    //find if there is a parasite task that the host isn't doing
    for (int i=start;i<host_task_counts.GetSize();i++)
    {
      if(host_task_counts[i] == 0 && parasite_task_counts[i] > 0)
      {
        //inject should succeed if there is a matching task
        interaction_fails = false;
      }
    }

  }
  
  // 3: Parasite tasks must match host tasks exactly. (Matching Alleles) 
  if(infection_mechanism == 3)
  {
    //handle skipping of first task
    int start = 0;
    if(m_world->GetConfig().INJECT_SKIP_FIRST_TASK.Get())
      start += 1;
    
    //This time if we trigger the if statments we DO fail. 
    interaction_fails = false;
    for (int i=start;i<host_task_counts.GetSize();i++)
    {
      if( (host_task_counts[i] == 0 && parasite_task_counts[i] > 0) || (host_task_counts[i] > 0 && parasite_task_counts[i] == 0) )
      {
        //inject should fail if either the host or parasite is doing a task the other isn't.
        interaction_fails = true;
      }
    }
  }
  
  // TODO: Add other infection mechanisms -LZ
  if(interaction_fails)
  {
    double prob_success = m_world->GetConfig().INJECT_DEFAULT_SUCCESS.Get();
    double rand = m_world->GetRandom().GetDouble();
    
    if (rand > prob_success)
      return false;
  }

  //infection_mechanism == 0
  return true;
}

bool cPopulation::ActivateParasite(cOrganism* host, cBioUnit* parent, const cString& label, const Sequence& injected_code)
{
  assert(parent != NULL);

  // Quick check for empty parasites
  if (injected_code.GetSize() == 0) return false;


  // Pull the host cell
  const int host_id = host->GetOrgInterface().GetCellID();
  assert(host_id >= 0 && host_id < cell_array.GetSize());
  cPopulationCell& host_cell = cell_array[host_id];


  // Select a target organism
  // @TODO - activate parasite target selection should account for hardware type
  cOrganism* target_organism = NULL;
  if (m_world->GetConfig().BIRTH_METHOD.Get() ==  POSITION_OFFSPRING_FULL_SOUP_RANDOM) {
    target_organism = GetCell(m_world->GetRandom().GetUInt(cell_array.GetSize())).GetOrganism();
  } else {
    target_organism =
    host_cell.ConnectionList().GetPos(m_world->GetRandom().GetUInt(host->GetNeighborhoodSize()))->GetOrganism();
  }
  if (target_organism == NULL) return false;


  // Pre-check target hardware
  const cHardwareBase& hw = target_organism->GetHardware();
  if (hw.GetType() != parent->GetGenome().GetHardwareType() ||
      hw.GetInstSet().GetInstSetName() != parent->GetGenome().GetInstSet() ||
      hw.GetNumThreads() == m_world->GetConfig().MAX_CPU_THREADS.Get()) return false;

  //Handle host specific injection
  if(TestForParasiteInteraction(host, target_organism) == false)
    return false;
  

  // Attempt actual parasite injection

  Genome mg(parent->GetGenome().GetHardwareType(), parent->GetGenome().GetInstSet(), injected_code);
  cParasite* parasite = new cParasite(m_world, mg, parent->GetPhenotype().GetGeneration(), SRC_PARASITE_INJECT, label);

  //Handle potential virulence evolution if this parasite is comming from a parasite 
  //and virulence is inhereted from the parent (source == 1)
  if (parent->IsParasite() && m_world->GetConfig().VIRULENCE_SOURCE.Get() == 1)
  {
    //mutate virulence
    // m_world->GetConfig().PARASITE_VIRULENCE.Get()
    double oldVir = dynamic_cast<cParasite*>(parent)->GetVirulence();
    
    //default to not mutating
    double newVir = oldVir;
    
    //but if we do mutate...
    if (m_world->GetRandom().GetDouble() < m_world->GetConfig().VIRULENCE_MUT_RATE.Get())
    {
      //get this in a temp variable so we don't have to make the next line huge
      double vir_sd = m_world->GetConfig().VIRULENCE_SD.Get();
      
      //sd^2 = varience
      newVir = m_world->GetRandom().GetRandNormal(oldVir, vir_sd * vir_sd);

    }
    parasite->SetVirulence(Max(Min(newVir, 1.0), 0.0));
  }
  else
  {
    //get default virulence
    parasite->SetVirulence(m_world->GetConfig().PARASITE_VIRULENCE.Get());
  }
  if (!target_organism->ParasiteInfectHost(parasite)) {
    delete parasite;
    return false;
  }

  //If parasite was successfully injected, update the phenotype for the parasite in new organism
  target_organism->GetPhenotype().SetLastParasiteTaskCount(host->GetPhenotype().GetLastParasiteTaskCount());

  // Classify the parasite
  tArray<const tArray<cBioGroup*>*> pgrps(1);
  pgrps[0] = &parent->GetBioGroups();
  parasite->SelfClassify(pgrps);

  // Handle post injection actions
  if (m_world->GetConfig().INJECT_STERILIZES_HOST.Get()) target_organism->GetPhenotype().Sterilize();

  return true;
}

void cPopulation::ActivateOrganism(cAvidaContext& ctx, cOrganism* in_organism, cPopulationCell& target_cell)
{
  assert(in_organism != NULL);
  assert(in_organism->GetGenome().GetSize() >= 1);
  
  in_organism->SetOrgInterface(ctx, new cPopulationInterface(m_world));
  
  // Update the contents of the target cell.
  KillOrganism(target_cell, ctx); 
  target_cell.InsertOrganism(in_organism, ctx); 
  AddLiveOrg(in_organism); 
  
  // Setup the inputs in the target cell.
  environment.SetupInputs(ctx, target_cell.m_inputs);
  
  // Precalculate the phenotype if requested
  int pc_phenotype = m_world->GetConfig().PRECALC_PHENOTYPE.Get();
  if (pc_phenotype){
    cCPUTestInfo test_info;
    cTestCPU* test_cpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
    test_info.UseManualInputs(target_cell.GetInputs()); // Test using what the environment will be
    Genome mg(in_organism->GetGenome());
    mg.SetSequence(in_organism->GetHardware().GetMemory());
    test_cpu->TestGenome(ctx, test_info, mg);  // Use the true genome
    
    if (pc_phenotype & 1)
      in_organism->GetPhenotype().SetMerit(test_info.GetTestPhenotype().GetMerit());
    if (pc_phenotype & 2)
      in_organism->GetPhenotype().SetGestationTime(test_info.GetTestPhenotype().GetGestationTime());
    in_organism->GetPhenotype().SetFitness(in_organism->GetPhenotype().GetMerit().CalcFitness(in_organism->GetPhenotype().GetGestationTime()));
    delete test_cpu;
  }
  // Update the archive...
  
  
  // Initialize the time-slice for this new organism.
  AdjustSchedule(target_cell, in_organism->GetPhenotype().GetMerit());
  
  // Special handling for certain birth methods.
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST) {
    reaper_queue.Push(&target_cell);
  }
  
  // Keep track of statistics for organism counts...
  num_organisms++;
  
  if (deme_array.GetSize() > 0) {
    deme_array[target_cell.GetDemeID()].IncOrgCount();
  }
  
  // Statistics...
  m_world->GetStats().RecordBirth(in_organism->GetPhenotype().ParentTrue());
  
  // @MRR Do coalescence clade setup for new organisms.
  CCladeSetupOrganism(in_organism );
  
  //count how many times MERIT_BONUS_INST (rewarded instruction) is in the genome
  //only relevant if merit is proportional to # times MERIT_BONUS_INST is in the genome
  int rewarded_instruction = m_world->GetConfig().MERIT_BONUS_INST.Get();
  int num_rewarded_instructions = 0;
  int genome_length = in_organism->GetGenome().GetSize();
  
  if (rewarded_instruction == -1){
    //no key instruction, so no bonus
    in_organism->GetPhenotype().SetCurBonusInstCount(0);
  }
  else{
    for(int i = 1; i <= genome_length; i++){
      if (in_organism->GetGenome().GetSequence()[i-1].GetOp() == rewarded_instruction){
        num_rewarded_instructions++;
      }
    }
    in_organism->GetPhenotype().SetCurBonusInstCount(num_rewarded_instructions);
  }
  // ok, after we've gone through all that, there's a catch.  It is possible that the
  // cell into which this organism has been injected is in fact a "gateway" to another
  // world.  if so, we then migrate this organism out of this world and empty the cell.
  if(m_world->IsWorldBoundary(target_cell)) {
    m_world->MigrateOrganism(in_organism, target_cell, in_organism->GetPhenotype().GetMerit(), in_organism->GetLineageLabel());
    KillOrganism(target_cell, ctx);
  }

  if (m_world->GetConfig().USE_FORM_GROUPS.Get() != 0) {
    if (!in_organism->HasOpinion()) {
      if (m_world->GetConfig().DEFAULT_GROUP.Get() != -1) {
        in_organism->SetOpinion(m_world->GetConfig().DEFAULT_GROUP.Get());
        JoinGroup(in_organism, m_world->GetConfig().DEFAULT_GROUP.Get());
      }
      else {
        if (m_world->GetConfig().USE_FORM_GROUPS.Get() == 1) {
          const int op = (int) abs(ctx.GetRandom().GetDouble());
          in_organism->SetOpinion(op);
          JoinGroup(in_organism, op);                    
        }
        else if (m_world->GetConfig().USE_FORM_GROUPS.Get() == 2) {
          const int op = ctx.GetRandom().GetInt(0, m_world->GetEnvironment().GetResourceLib().GetSize() + 1);
          in_organism->SetOpinion(op);
          JoinGroup(in_organism, op);          
        }
      }
    }
  }

  // For tolerance_window, we cheated by dumping doomed offspring into cell (X * Y) - 1 ...now that we updated the stats, we need to 
  // kill that org. @JJB
  int doomed_cell = (m_world->GetConfig().WORLD_X.Get() * m_world->GetConfig().WORLD_Y.Get()) - 1;
  if ((m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) && (in_organism->GetCellID() == doomed_cell) && (m_world->GetStats().GetUpdate() != 0)) {
    KillOrganism(target_cell, ctx);
  }
}

// @WRE 2007/07/05 Helper function to take care of side effects of Avidian
// movement that cannot be directly handled in cHardwareCPU.cc
bool cPopulation::MoveOrganisms(cAvidaContext& ctx, int src_cell_id, int dest_cell_id)
{
  cPopulationCell& src_cell = GetCell(src_cell_id);
  cPopulationCell& dest_cell = GetCell(dest_cell_id);
  
  // check for habitat effects on movement
  // get the resource library
  const cResourceLib & resource_lib = environment.GetResourceLib();
  // get the destination cell resource levels
  tArray<double> dest_cell_resources = m_world->GetPopulation().GetCellResources(dest_cell_id, ctx);
  // get the current cell resource levels
  tArray<double> src_cell_resources = m_world->GetPopulation().GetCellResources(src_cell_id, ctx);
  // movement fails if there are any barrier resources in the faced cell (unless the org is already on a barrier,
  // which would happen if we built a new barrier under an org and we need to let it get off)
  bool curr_is_barrier = false;
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 2 && src_cell_resources[i] > 0) {
      curr_is_barrier = true;      
      break;
    }
  }
  if (!curr_is_barrier) {
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->GetHabitat() == 2 && dest_cell_resources[i] > 0) return false;      
    }    
  }
  // if any of the resources in current cells are hills, find the id of the most resistant resource
  int steepest_hill = 0;
  double curr_resistance = 1.0;
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 1 && src_cell_resources[i] > 0) {
      if (resource_lib.GetResource(i)->GetResistance() > curr_resistance) {
        curr_resistance = resource_lib.GetResource(i)->GetResistance();
        steepest_hill = i;
      }
    }
  } 
  // apply the chance of move failing for the steepest hill in this cell, if there is a hill at all
  if (resource_lib.GetResource(steepest_hill)->GetHabitat() == 1 && src_cell_resources[steepest_hill] > 0) {
    // we use resistance to determine chance of movement succeeding: 'resistance == # move instructions executed, on average, to move one step/cell'
    int chance_move_success = int(((1/curr_resistance) * 100) + 0.5);
    if (ctx.GetRandom().GetInt(0,101) > chance_move_success) return false;      
  }      
  
  if (m_world->GetConfig().DEADLY_BOUNDARIES.Get() == 1 && m_world->GetConfig().WORLD_GEOMETRY.Get() == 1) {
    int absolute_cell_ID = src_cell.GetOrganism()->GetCellID();
    int deme_id = src_cell.GetOrganism()->GetDemeID();
    // Fail if we're running in the test CPU.
    if((deme_id < 0) || (absolute_cell_ID < 0)) return false;
    
    std::pair<int, int> pos = m_world->GetPopulation().GetDeme(deme_id).GetCellPosition(absolute_cell_ID);  
    if (pos.first == 0 || pos.second == 0 || pos.first == m_world->GetConfig().WORLD_X.Get() - 1 || pos.second == m_world->GetConfig().WORLD_Y.Get() - 1) {
//      KillOrganism(src_cell, ctx);  //APW
      src_cell.GetOrganism()->Die(ctx);  //APW
    return false; 
    }
  }    
  if (m_world->GetConfig().MOVEMENT_COLLISIONS_LETHAL.Get() && dest_cell.IsOccupied()) {
    if (m_world->GetConfig().MOVEMENT_COLLISIONS_LETHAL.Get() == 2) return false;
    bool kill_source = true;
    switch (m_world->GetConfig().MOVEMENT_COLLISIONS_SELECTION_TYPE.Get()) {
      case 0: // 50% chance, no modifiers
      default:
        kill_source = ctx.GetRandom().P(0.5);
        break;

      case 1: // binned vitality based on age
        double src_vitality = src_cell.GetOrganism()->GetVitality();
        double dest_vitality = dest_cell.GetOrganism()->GetVitality();
        kill_source = (src_vitality < ctx.GetRandom().GetDouble(src_vitality + dest_vitality));
        break;
    }

    if (kill_source) {
      KillOrganism(src_cell, ctx); 

      // Killing the moving organism means that we shouldn't actually do the swap, so return
      return false;
    }

    KillOrganism(dest_cell, ctx); 
  }

  SwapCells(src_cell_id, dest_cell_id, ctx); 


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
  
  return true;
}

// Kill Random Organism in Group (But Not Self)!! 
void cPopulation::KillGroupMember(cAvidaContext& ctx, int group_id, cOrganism *org)
{
  //Check to make sure we are not killing self!
  if (group_list[group_id].GetSize() == 1 && group_list[group_id][0] == org) return;
  if (group_list[group_id].GetSize() == 0) return;
  int index;
  while(true) {
    index = ctx.GetRandom().GetUInt(0, group_list[group_id].GetSize());
    if (group_list[group_id][index] == org) continue;
    else break;
  }
  
  int cell_id = group_list[group_id][index]->GetCellID();
  KillOrganism(cell_array[cell_id], ctx); 
}

// Attack organism faced by this one, if there is an organism in front.
void cPopulation::AttackFacedOrg(cAvidaContext& ctx, int loser)
{
  cPopulationCell& loser_cell = GetCell(loser);
  KillOrganism(loser_cell, ctx); 
}

void cPopulation::KillOrganism(cPopulationCell& in_cell, cAvidaContext& ctx) 
{
  // do we actually have something to kill?
  if (in_cell.IsOccupied() == false) return;

  // Statistics...
  cOrganism* organism = in_cell.GetOrganism();
  m_world->GetStats().RecordDeath();
  
  RemoveLiveOrg(organism); 
  
  int cellID = in_cell.GetID();

  organism->NotifyDeath(ctx);

  // @TODO @DMB - this should really move to cOrganism::NotifyDeath
  if (m_world->GetConfig().LOG_SLEEP_TIMES.Get() == 1) {
    if (sleep_log[cellID].Size() > 0) {
      pair<int,int> p = sleep_log[cellID][sleep_log[cellID].Size()-1];
      if (p.second == -1) {
        AddEndSleep(cellID,m_world->GetStats().GetUpdate());
      }
    }
  }

  // @TODO @DMB - this should really move to cOrganism::NotifyDeath
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


  // Update count statistics...
  num_organisms--;

  // Handle deme updates.
  if (deme_array.GetSize() > 0) {
    deme_array[in_cell.GetDemeID()].DecOrgCount();
    deme_array[in_cell.GetDemeID()].OrganismDeath(in_cell);
  }

  // If HGT is turned on and there's a possibility of natural competence,
	// this organism's genome needs to be split up into fragments
  // and deposited in its cell.  We then also have to add the size of this genome to
  // the HGT resource.
  if(m_world->GetConfig().ENABLE_HGT.Get()
		 && (m_world->GetConfig().HGT_COMPETENCE_P.Get() > 0.0)) {
    in_cell.AddGenomeFragments(ctx, organism->GetGenome().GetSequence());
  }

  // And clear it!
  in_cell.RemoveOrganism(ctx); 
  if (!organism->IsRunning()) delete organism;
  else organism->GetPhenotype().SetToDelete();

  // Alert the scheduler that this cell has a 0 merit.
  AdjustSchedule(in_cell, cMerit(0));
}

void cPopulation::Kaboom(cPopulationCell& in_cell, cAvidaContext& ctx, int distance) 
{
  cOrganism* organism = in_cell.GetOrganism();
  cString ref_genome = organism->GetGenome().GetSequence().AsString();
  int bgid = organism->GetBioGroup("genotype")->GetID();

  int radius = 2;

  for (int i = -1 * radius; i <= radius; i++) {
    for (int j = -1 * radius; j <= radius; j++) {
      cPopulationCell& death_cell = cell_array[GridNeighbor(in_cell.GetID(), world_x, world_y, i, j)];

      //do we actually have something to kill?
      if (death_cell.IsOccupied() == false) continue;

      cOrganism* org_temp = death_cell.GetOrganism();

      if (distance == 0) {
        int temp_id = org_temp->GetBioGroup("genotype")->GetID();
        if (temp_id != bgid) KillOrganism(death_cell, ctx); 
      } else {
        cString genome_temp = org_temp->GetGenome().GetSequence().AsString();
        int diff = 0;
        for (int i = 0; i < genome_temp.GetSize(); i++) if (genome_temp[i] != ref_genome[i]) diff++;
        if (diff > distance) KillOrganism(death_cell, ctx); 
      }
    }
  }
  KillOrganism(in_cell, ctx); 
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

void cPopulation::SwapCells(int cell_id1, int cell_id2, cAvidaContext& ctx)
{
  // Sanity checks: Don't process if the cells are the same
  if (cell_id1 == cell_id2) return;

  cPopulationCell& cell1 = GetCell(cell_id1);
  cPopulationCell& cell2 = GetCell(cell_id2);

  // Clear current contents of cells
  cOrganism* org1 = cell1.RemoveOrganism(ctx); 
  cOrganism* org2 = cell2.RemoveOrganism(ctx); 

  if (org2 != NULL) {
    cell1.InsertOrganism(org2, ctx); 
    AdjustSchedule(cell1, org2->GetPhenotype().GetMerit());
  } else {
    AdjustSchedule(cell1, cMerit(0));
  }

  if (org1 != NULL) {
    cell2.InsertOrganism(org1, ctx); 
    cell2.IncVisits();  // Increment visit count
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

void cPopulation::CompeteDemes(cAvidaContext& ctx, int competition_type)
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

    cDeme& from_deme = deme_array[from_deme_id];
    cDeme& to_deme   = deme_array[to_deme_id];

    // Ideally, the below bit of code would be replaced with a call to ReplaceDeme:
    // ReplaceDeme(from_deme, to_deme);
    //
    // But, use of InjectClone messes that up, breaking consistency.  So the next
    // time that someone comes in here looking to refactor, consider fixing this.

    // Do the actual copy!
    for (int i = 0; i < from_deme.GetSize(); i++) {
      int from_cell_id = from_deme.GetCellID(i);
      int to_cell_id = to_deme.GetCellID(i);
      if (cell_array[from_cell_id].IsOccupied() == true) {
        InjectClone(to_cell_id, *(cell_array[from_cell_id].GetOrganism()), SRC_DEME_COMPETE);
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
      InjectClone(cur_cell_id, *(cell_array[cur_cell_id].GetOrganism()), cell_array[cur_cell_id].GetOrganism()->GetUnitSource());
    }
  }

  // Reset all deme stats to zero.
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    deme_array[deme_id].Reset(ctx, deme_array[deme_id].GetGeneration()); // increase deme generation by 1
  }
}


/*! Compete all demes with each other based on the given vector of fitness values.

 This form of compete demes supports both fitness-proportional selection and a
 variant of tournament selection.  It integrates with the various deme replication options
 used in ReplicateDemes.

 Note: New deme competition fitness functions are added in PopulationActions.cc by subclassing
 cActionAbstractCompeteDemes and overriding cActionAbstractCompeteDemes::Fitness(cDeme&).  (Don't forget
 to register the action and add it to the events file).

 Another note: To mimic the behavior of the other version of CompeteDemes (which is kept around
 for backwards compatibility), change the config option DEMES_REPLICATE_SIZE to be the size of
 each deme.
 */
void cPopulation::CompeteDemes(const std::vector<double>& calculated_fitness, cAvidaContext& ctx) {
  // it's possible that we'll be changing the fitness values of some demes, so make a copy:
  std::vector<double> fitness(calculated_fitness);

  // Each deme must have a fitness:
  assert((int)fitness.size() == deme_array.GetSize());

  // To prevent sterile demes from replicating, we're going to replace the fitness
  // of all sterile demes with 0; this effectively makes it impossible for a sterile
  // deme to be selected via fitness proportional selection.
  if (m_world->GetConfig().DEMES_PREVENT_STERILE.Get()) {
    for(int i=0; i<deme_array.GetSize(); ++i) {
      if (deme_array[i].GetBirthCount() == 0) {
        fitness[i] = 0.0;
      }
    }
  }

  // Stat-tracking:
  m_world->GetStats().CompeteDemes(fitness);

  // This is to facilitate testing.  Obviously we can't do competition if there's
  // only one deme, but we do want the stat-tracking.
  if (fitness.size() == 1) {
    return;
  }

  // to facilitate control runs, sometimes we want to know what the fitness values
  // are, but we don't want competition to depend on them.
  if (m_world->GetConfig().DEMES_OVERRIDE_FITNESS.Get()) {
    for(int i=0; i<static_cast<int>(fitness.size()); ++i) {
      fitness[i] = 1.0;
    }
  }

  // Number of demes (at index) which should wind up in the next generation.
  std::vector<unsigned int> deme_counts(deme_array.GetSize(), 0);
  // Now, compete all demes based on the competition style.
  switch(m_world->GetConfig().DEMES_COMPETITION_STYLE.Get()) {
    case SELECTION_TYPE_PROPORTIONAL: {
      // Fitness-proportional selection.
      //
      // Each deme has a probability equal to its fitness / sum(deme fitnesses)
      // of proceeding to the next generation.

      const double total_fitness = std::accumulate(fitness.begin(), fitness.end(), 0.0);
      assert(total_fitness > 0.0); // Must have *some* positive fitnesses...

      // Sum up the fitnesses until we reach or exceed the target fitness.
      // Then we're marking that deme as being part of the next generation.
      for (int i=0; i<deme_array.GetSize(); ++i) {
        double running_sum = 0.0;
        double target_sum = m_world->GetRandom().GetDouble(total_fitness);
        for (int j=0; j<deme_array.GetSize(); ++j) {
          running_sum += fitness[j];
          if (running_sum >= target_sum) {
            // j'th deme will be replicated.
            ++deme_counts[j];
            break;
          }
        }
      }
      break;
    }
    case SELECTION_TYPE_TOURNAMENT: {
      // Tournament selection.
      //
      // We run NUM_DEMES tournaments of size DEME_TOURNAMENT_SIZE, and select the
      // **single** winner of the tournament to proceed to the next generation.

      // construct a list of all possible deme ids that could participate in a tournament,
      // pruning out sterile demes:
      std::vector<int> deme_ids;
      for (int i=0; i<deme_array.GetSize(); ++i) {
        if (!m_world->GetConfig().DEMES_PREVENT_STERILE.Get() ||
            (deme_array[i].GetBirthCount() > 0)) {
          deme_ids.push_back(i);
        }
      }

      // better have more than deme tournament size, otherwise something is *really* screwed up:
      if (m_world->GetConfig().DEMES_TOURNAMENT_SIZE.Get() > static_cast<int>(deme_ids.size())) {
        m_world->GetDriver().RaiseFatalException(-1,
                                                 "Number of demes available to participate in a tournament < the deme tournament size.");
      }

      // Run the tournaments.
      for (int i=0; i<m_world->GetConfig().NUM_DEMES.Get(); ++i) {
        // Which demes are in this tournament?
        std::vector<int> tournament(m_world->GetConfig().DEMES_TOURNAMENT_SIZE.Get());
        sample_without_replacement(deme_ids.begin(), deme_ids.end(),
                                   tournament.begin(), tournament.end(),
                                   cRandomStdAdaptor(m_world->GetRandom()));

        // Now, iterate through the fitnesses of each of the tournament players,
        // capturing the winner's index and fitness.
        //
        // If no deme actually won, meaning no one had fitness greater than 0.0,
        // then the winner is selected at random from the tournament.
        std::pair<int, double> winner(tournament[m_world->GetRandom().GetInt(tournament.size())], 0.0);
        for(std::vector<int>::iterator j=tournament.begin(); j!=tournament.end(); ++j) {
          if (fitness[*j] > winner.second) {
            winner = std::make_pair(*j, fitness[*j]);
          }
        }

        // We have a winner!  Increment his replication count.
        ++deme_counts[winner.first];
      }
      break;
    }
    default: {
      // should never get here.
      assert(false);
    }
  }

  // Housekeeping: re-inject demes with count of 1 back into self (energy-related).
  for (int i = 0; i < (int)deme_counts.size(); i++) {
    if (deme_counts[i] == 1)
      ReplaceDeme(deme_array[i], deme_array[i], ctx); 
  }

  // Ok, the below algorithm relies upon the fact that we have a strict weak ordering
  // of fitness values for all demes.  We're going to loop through, find demes with a
  // count greater than one, and insert them into demes with a count of zero.
  while (true) {
    int source_id=0;
    for(; source_id<(int)deme_counts.size(); ++source_id) {
      if (deme_counts[source_id] > 1) {
        --deme_counts[source_id];
        break;
      }
    }

    if (source_id == (int)deme_counts.size()) {
      break; // All done; we looped through the whole list of counts, and didn't find any > 1.
    }

    int target_id=0;
    for(; target_id<(int)deme_counts.size(); ++target_id) {
      if (deme_counts[target_id] == 0) {
        ++deme_counts[target_id];
        break;
      }
    }

    assert(source_id < deme_array.GetSize());
    assert(target_id < deme_array.GetSize());
    assert(source_id != target_id);

    // Replace the target with a copy of the source:
    ReplaceDeme(deme_array[source_id], deme_array[target_id], ctx); 
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
 6: 'events-killed' ...demes that have killed a certian number of events
 7: 'sat-msg-pred'...demes whose movement predicate was previously satisfied
 8: 'sat-deme-predicate'...demes whose predicate has been satisfied; does not include movement or message predicates as those are organisms-level
 9: 'perf-reactions' ...demes that have performed X number of each task are replicated
 10:'consume-res' ...demes that have consumed a sufficienct amount of resources


 */

void cPopulation::ReplicateDemes(int rep_trigger, cAvidaContext& ctx) 
{
  assert(GetNumDemes()>1); // Sanity check.

  // Loop through all candidate demes...
  const int num_demes = GetNumDemes();
  for (int deme_id=0; deme_id<num_demes; ++deme_id) {
    cDeme& source_deme = deme_array[deme_id];

    // Test this deme to determine if it should be replicated.  If not,
    // continue on to the next deme.
    switch (rep_trigger) {
      case DEME_TRIGGER_ALL: {
        // Replicate all non-empty demes.
        if (source_deme.IsEmpty()) continue;
        break;
      }
      case DEME_TRIGGER_FULL: {
        // Replicate all full demes.
        if (!source_deme.IsFull()) continue;
        break;
      }
      case DEME_TRIGGER_CORNERS: {
        // Replicate all demes with the corners filled in.
        // The first and last IDs represent the two corners.
        const int id1 = source_deme.GetCellID(0);
        const int id2 = source_deme.GetCellID(source_deme.GetSize() - 1);
        if (cell_array[id1].IsOccupied() == false ||
            cell_array[id2].IsOccupied() == false) continue;
        break;
      }
      case DEME_TRIGGER_AGE: {
        // Replicate old demes.
        if (source_deme.GetAge() < m_world->GetConfig().DEMES_MAX_AGE.Get()) continue;
        break;
      }
      case DEME_TRIGGER_BIRTHS: {
        // Replicate demes that have had a certain number of births.
        if (source_deme.GetBirthCount() < m_world->GetConfig().DEMES_MAX_BIRTHS.Get()) continue;
        break;
      }
      case DEME_TRIGGER_MOVE_PREDATORS: {
        if (!(source_deme.MovPredSatisfiedPreviously())) continue;
        break;
      }
      case DEME_TRIGGER_GROUP_KILL: {
        int currentSlotSuccessful = 0;
        double kill_ratio = 0.0;

        if (source_deme.GetSlotFlowRate() == 0) {
          kill_ratio = 1.0;
        } else {
          kill_ratio = static_cast<double>(source_deme.GetEventsKilledThisSlot()) /
          static_cast<double>(source_deme.GetSlotFlowRate());
        }

        if (kill_ratio >= m_world->GetConfig().DEMES_MIM_EVENTS_KILLED_RATIO.Get()) {
          currentSlotSuccessful = 1;
        }

        // Replicate demes that have killed a certain number of event.
        if (source_deme.GetConsecutiveSuccessfulEventPeriods() + currentSlotSuccessful
            < m_world->GetConfig().DEMES_MIM_SUCCESSFUL_EVENT_PERIODS.Get()) {
          continue;
        }
        break;
      }
      case DEME_TRIGGER_MESSAGE_PREDATORS: {
        if (!(source_deme.MsgPredSatisfiedPreviously())) continue;
        break;
      }
      case DEME_TRIGGER_PREDICATE: {
        if (!(source_deme.DemePredSatisfiedPreviously())) continue;
        break;
      }
      case DEME_TRIGGER_PERFECT_REACTIONS: {
        // loop through each reaction. Make sure each has been performed X times.
        if (source_deme.MinNumTimesReactionPerformed() < m_world->GetConfig().REACTION_THRESH.Get()) {
          continue;
        }
        break;
      }
      case DEME_TRIGGER_CONSUME_RESOURCES: {
        // check how many resources have been consumed by the deme
        if (source_deme.GetTotalResourceAmountConsumed() <
            m_world->GetConfig().RES_FOR_DEME_REP.Get()) {
          continue;
        }
        break;
      }
      default: {
        cerr << "ERROR: Invalid replication trigger " << rep_trigger
        << " in cPopulation::ReplicateDemes()" << endl;
        assert(false);
      }
    }

    ReplicateDeme(source_deme, ctx); 
  }
}


/*! ReplicateDeme is a helper method for replicating a source deme.
 */
void cPopulation::ReplicateDeme(cDeme& source_deme, cAvidaContext& ctx) 
{
  // Doesn't make sense to try and replicate a deme that *has no organisms*.
  if (source_deme.IsEmpty()) return;

	source_deme.UpdateShannonAll();

  // Prevent sterile demes from replicating.
  if (m_world->GetConfig().DEMES_PREVENT_STERILE.Get() && (source_deme.GetBirthCount() == 0)) {
    // assumes that all group level tasks cannot be solved by a single organism
    source_deme.KillAll(ctx); 
    return;
  }

  // Update stats
  // calculate how many different reactions the deme performed.
  double deme_performed_rx=0;
  tArray<int> deme_reactions = source_deme.GetCurReactionCount();
  for(int i=0; i< deme_reactions.GetSize(); ++i) {
    //HJG
    if (deme_reactions[i] > 0){
      deme_performed_rx++;
    }
  }

  // calculate how many penalties were accrued by the orgs on average
  double switch_penalties = source_deme.GetNumSwitchingPenalties();
  double num_orgs_perf_reaction = source_deme.GetNumOrgsPerformedReaction();
  double shannon_div = source_deme.GetShannonMutualInformation();
  double per_reproductives = source_deme.GetPercentReproductives();

  if (switch_penalties > 0) {
    switch_penalties = (switch_penalties)/(source_deme.GetInjectedCount() + source_deme.GetBirthCount());
  }


  m_world->GetStats().IncDemeReactionDiversityReplicationData(deme_performed_rx, switch_penalties,
                                                              shannon_div, num_orgs_perf_reaction, per_reproductives);

  //Option to bridge between kin and group selection.
  if (m_world->GetConfig().DEMES_REPLICATION_ONLY_RESETS.Get()) {
    //Reset deme (resources and births, among other things)
    bool source_deme_resource_reset = m_world->GetConfig().DEMES_RESET_RESOURCES.Get() == 0;
    source_deme.DivideReset(ctx, source_deme, source_deme_resource_reset);

    //Reset all organisms in deme, by re-injecting them?
    if (m_world->GetConfig().DEMES_REPLICATION_ONLY_RESETS.Get() == 2) {
      for (int i=0; i<source_deme.GetSize(); i++) {
        int cellid = source_deme.GetCellID(i);
        if (GetCell(cellid).IsOccupied()) {
          int lineage = GetCell(cellid).GetOrganism()->GetLineageLabel();
          const Genome& genome = GetCell(cellid).GetOrganism()->GetGenome();
          InjectGenome(cellid, SRC_DEME_REPLICATE, genome, ctx, lineage); 
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

  // Write some logging information if LOG_DEMES_REPLICATE is set.
  if ( (m_world->GetConfig().LOG_DEMES_REPLICATE.Get() == 1) &&
      (m_world->GetStats().GetUpdate() >= m_world->GetConfig().DEMES_REPLICATE_LOG_START.Get()) ) {
    cString tmpfilename = cStringUtil::Stringf("deme_replication.dat");
    cDataFile& df = m_world->GetDataFile(tmpfilename);

    cString UpdateStr = cStringUtil::Stringf("%d,%d,%d",
                                             m_world->GetStats().GetUpdate(),
                                             source_deme.GetDemeID(), target_id);
    df.WriteRaw(UpdateStr);
  }

  ReplaceDeme(source_deme, deme_array[target_id], ctx); 
}

/*! ReplaceDeme is a helper method that handles all the different configuration
 options related to the replacement of a target deme by a source.  It works with
 both CompeteDemes and ReplicateDemes (and can be called directly via an event if
 so desired).

 @refactor Replace manual mutation with strategy pattern.
 */
void cPopulation::ReplaceDeme(cDeme& source_deme, cDeme& target_deme, cAvidaContext& ctx2) 
{
  // Stats tracking; pre-replication hook.
  m_world->GetStats().DemePreReplication(source_deme, target_deme);

  // used to pass energy to offspring demes (set to zero if energy model is not enabled)
  double source_deme_energy(0.0), deme_energy_decay(0.0), parent_deme_energy(0.0), offspring_deme_energy(0.0);
  if (m_world->GetConfig().ENERGY_ENABLED.Get()) {
    double energyRemainingInSourceDeme = source_deme.CalculateTotalEnergy(ctx2); 
    source_deme.SetEnergyRemainingInDemeAtReplication(energyRemainingInSourceDeme); 
    source_deme_energy = energyRemainingInSourceDeme + source_deme.GetTotalEnergyTestament(); 

    m_world->GetStats().SumEnergyTestamentAcceptedByDeme().Add(source_deme.GetTotalEnergyTestament());
    deme_energy_decay = 1.0 - m_world->GetConfig().FRAC_ENERGY_DECAY_AT_DEME_BIRTH.Get();
    parent_deme_energy = source_deme_energy * deme_energy_decay * (1.0 - m_world->GetConfig().FRAC_PARENT_ENERGY_GIVEN_TO_DEME_AT_BIRTH.Get());
    offspring_deme_energy = source_deme_energy * deme_energy_decay * m_world->GetConfig().FRAC_PARENT_ENERGY_GIVEN_TO_DEME_AT_BIRTH.Get();
  }

  bool target_successfully_seeded = true;


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
  if (m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
    // Transfer energy from source to target if we're using the energy model.
    if (target_successfully_seeded) target_deme.DivideReset(ctx2, source_deme, target_deme_resource_reset, offspring_deme_energy);
    source_deme.DivideReset(ctx2, source_deme, source_deme_resource_reset, parent_deme_energy);
  } else {
    // Default; reset both source and target.
    if (target_successfully_seeded) target_deme.DivideReset(ctx2, source_deme, target_deme_resource_reset);
    source_deme.DivideReset(ctx2, source_deme, source_deme_resource_reset);
  }


  // Are we using germlines?  If so, we need to mutate the germline to get the
  // genome that we're going to seed the target with.
  if (m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 1) {
    // @JEB Original germlines
    Genome next_germ(source_deme.GetGermline().GetLatest());
    const cInstSet& instset = m_world->GetHardwareManager().GetInstSet(next_germ.GetInstSet());
    cAvidaContext ctx(m_world, m_world->GetRandom());

    if (m_world->GetConfig().GERMLINE_COPY_MUT.Get() > 0.0) {
      for(int i=0; i<next_germ.GetSize(); ++i) {
        if (m_world->GetRandom().P(m_world->GetConfig().GERMLINE_COPY_MUT.Get())) {
          next_germ.GetSequence()[i] = instset.GetRandomInst(ctx);
        }
      }
    }

    if ((m_world->GetConfig().GERMLINE_INS_MUT.Get() > 0.0)
        && m_world->GetRandom().P(m_world->GetConfig().GERMLINE_INS_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(next_germ.GetSize() + 1);
      next_germ.GetSequence().Insert(mut_line, instset.GetRandomInst(ctx));
    }

    if ((m_world->GetConfig().GERMLINE_DEL_MUT.Get() > 0.0)
        && m_world->GetRandom().P(m_world->GetConfig().GERMLINE_DEL_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(next_germ.GetSize());
      next_germ.GetSequence().Remove(mut_line);
    }

    // Replace the target deme's germline with the source deme's, and add the newly-
    // mutated germ to ONLY the target's germline.  The source remains unchanged.
    target_deme.ReplaceGermline(source_deme.GetGermline());
    target_deme.GetGermline().Add(next_germ);

    // Germline stats tracking.
    m_world->GetStats().GermlineReplication(source_deme.GetGermline(), target_deme.GetGermline());

    // All done with the germline manipulation; seed each deme.
    SeedDeme(source_deme, source_deme.GetGermline().GetLatest(), SRC_DEME_GERMLINE, ctx2); 

    /* MJM - source and target deme could be the same!
     * Seeding the same deme twice probably shouldn't happen.
     */
    if (source_deme.GetDemeID() != target_deme.GetDemeID()) {
      SeedDeme(target_deme, target_deme.GetGermline().GetLatest(), SRC_DEME_GERMLINE, ctx2); 
    }

  } else if (m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 2) {
    // @JEB -- New germlines using cGenotype

    // get germline genotype
    int germline_genotype_id = source_deme.GetGermlineGenotypeID();
    cBioGroup* germline_genotype = m_world->GetClassificationManager().GetBioGroupManager("genotype")->GetBioGroup(germline_genotype_id);
    assert(germline_genotype);

    // create a new genome by mutation
    Genome mg(germline_genotype->GetProperty("genome").AsString());
    cCPUMemory new_genome(mg.GetSequence());
    const cInstSet& instset = m_world->GetHardwareManager().GetInstSet(mg.GetInstSet());
    cAvidaContext ctx(m_world, m_world->GetRandom());

    if (m_world->GetConfig().GERMLINE_COPY_MUT.Get() > 0.0) {
      for(int i=0; i<new_genome.GetSize(); ++i) {
        if (m_world->GetRandom().P(m_world->GetConfig().GERMLINE_COPY_MUT.Get())) {
          new_genome[i] = instset.GetRandomInst(ctx);
        }
      }
    }

    if ((m_world->GetConfig().GERMLINE_INS_MUT.Get() > 0.0)
        && m_world->GetRandom().P(m_world->GetConfig().GERMLINE_INS_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(new_genome.GetSize() + 1);
      new_genome.Insert(mut_line, instset.GetRandomInst(ctx));
    }

    if ((m_world->GetConfig().GERMLINE_DEL_MUT.Get() > 0.0)
        && m_world->GetRandom().P(m_world->GetConfig().GERMLINE_DEL_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(new_genome.GetSize());
      new_genome.Remove(mut_line);
    }

    mg.SetSequence(new_genome);

    //Create a new genotype which is daughter to the old one.
    cDemePlaceholderUnit unit(SRC_DEME_GERMLINE, mg);
    tArray<cBioGroup*> parents;
    parents.Push(germline_genotype);
    cBioGroup* new_germline_genotype = germline_genotype->ClassifyNewBioUnit(&unit, &parents);
    source_deme.ReplaceGermline(new_germline_genotype);
    target_deme.ReplaceGermline(new_germline_genotype);
    SeedDeme(source_deme, new_germline_genotype, SRC_DEME_GERMLINE, ctx2); 
    SeedDeme(target_deme, new_germline_genotype, SRC_DEME_GERMLINE, ctx2); 
    new_germline_genotype->RemoveBioUnit(&unit);
  } else {
    // Not using germlines; things are much simpler.  Seed the target from the source.
    target_successfully_seeded = SeedDeme(source_deme, target_deme, ctx2); 
  }


	// split energy from parent deme evenly among orgs in child deme
	if (m_world->GetConfig().ENERGY_ENABLED.Get() == 1 && m_world->GetConfig().ENERGY_PASSED_ON_DEME_REPLICATION_METHOD.Get() == 0) {
		assert(source_deme.GetOrgCount() > 0 && target_deme.GetOrgCount() > 0);
    if (offspring_deme_energy > 0.0) {
      // split deme energy evenly between organisms in target deme
      double totalEnergyInjectedIntoOrganisms(0.0);
      for (int i=0; i < target_deme.GetSize(); i++) {
        int cellid = target_deme.GetCellID(i);
        cPopulationCell& cell = m_world->GetPopulation().GetCell(cellid);
        if (cell.IsOccupied()) {
          cOrganism* organism = cell.GetOrganism();
          cPhenotype& phenotype = organism->GetPhenotype();
          phenotype.SetEnergy(phenotype.GetStoredEnergy() + offspring_deme_energy/static_cast<double>(target_deme.GetOrgCount()));
          phenotype.SetMerit(cMerit(phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy() * phenotype.GetEnergyUsageRatio())));
          totalEnergyInjectedIntoOrganisms += phenotype.GetStoredEnergy();
        }
      }
      target_deme.SetEnergyInjectedIntoOrganisms(totalEnergyInjectedIntoOrganisms);
    }
    if (parent_deme_energy > 0.0) {
      // split deme energy evenly between organisms in source deme
      double totalEnergyInjectedIntoOrganisms(0.0);
      for (int i=0; i < source_deme.GetSize(); i++) {
        int cellid = source_deme.GetCellID(i);
        cPopulationCell& cell = m_world->GetPopulation().GetCell(cellid);
        if (cell.IsOccupied()) {
          cOrganism* organism = cell.GetOrganism();
          cPhenotype& phenotype = organism->GetPhenotype();
          phenotype.SetEnergy(phenotype.GetStoredEnergy() + parent_deme_energy/static_cast<double>(source_deme.GetOrgCount()));
          phenotype.SetMerit(cMerit(phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy() * phenotype.GetEnergyUsageRatio())));
          totalEnergyInjectedIntoOrganisms += phenotype.GetStoredEnergy();
        }
      }
      source_deme.SetEnergyInjectedIntoOrganisms(totalEnergyInjectedIntoOrganisms);
    }
  }



  // The source's merit must be transferred to the target, and then the source has
  // to rotate its heritable merit to its current merit.
  if (target_successfully_seeded) target_deme.UpdateDemeMerit(source_deme);
  source_deme.UpdateDemeMerit();
  source_deme.ClearShannonInformationStats();
  target_deme.ClearShannonInformationStats();

  // do our post-replication stats tracking.
  m_world->GetStats().DemePostReplication(source_deme, target_deme);
}


/*! Helper method to seed a deme from the given genome.
 If the passed-in deme is populated, all resident organisms are terminated.  The
 deme's germline is left unchanged.

 @todo Fix lineage label on injected genomes.
 @todo Different strategies for non-random placement.
 */
void cPopulation::SeedDeme(cDeme& deme, Genome& genome, eBioUnitSource src, cAvidaContext& ctx) { 
  // Kill all the organisms in the deme.
  deme.KillAll(ctx); 

  // Create the specified number of organisms in the deme.
  for(int i=0; i< m_world->GetConfig().DEMES_REPLICATE_SIZE.Get(); ++i) {
    int cellid = DemeSelectInjectionCell(deme, i);
    InjectGenome(cellid, src, genome, ctx, 0); 
    DemePostInjection(deme, cell_array[cellid]);
  }
}

void cPopulation::SeedDeme(cDeme& _deme, cBioGroup* bg, eBioUnitSource src, cAvidaContext& ctx) { 
  // Kill all the organisms in the deme.
  _deme.KillAll(ctx); 
  _deme.ClearFounders();

  // Create the specified number of organisms in the deme.
  for(int i=0; i< m_world->GetConfig().DEMES_REPLICATE_SIZE.Get(); ++i) {
    int cellid = DemeSelectInjectionCell(_deme, i);
    InjectGenome(cellid, src, Genome(bg->GetProperty("genome").AsString()), ctx); 
    DemePostInjection(_deme, cell_array[cellid]);
    _deme.AddFounder(bg);
  }

}

/*! Helper method to seed a target deme from the organisms in the source deme.
 All organisms in the target deme are terminated, and a subset of the organisms in
 the source will be cloned to the target. Returns whether target deme was successfully seeded.
 */
bool cPopulation::SeedDeme(cDeme& source_deme, cDeme& target_deme, cAvidaContext& ctx) { 
  cRandom& random = m_world->GetRandom();

  bool successfully_seeded = true;

  // Check to see if we're doing probabilistic organism replication from source
  // to target deme.
  if (m_world->GetConfig().DEMES_PROB_ORG_TRANSFER.Get() == 0.0) {

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
      //       from tracking Genomes to tracking cGenotypes.  But that's a pain,
      //       because the cGenotype* from cOrganism::GetGenotype may not live after
      //       a call to cDeme::KillAll.
      std::vector<std::pair<Genome,int> > xfer; // List of genomes we're going to transfer.

      switch(m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get()) {
        case 0: { // Random w/ replacement (meaning, we don't prevent the same genotype from
          // being selected more than once).
          while((int)xfer.size() < m_world->GetConfig().DEMES_REPLICATE_SIZE.Get()) {
            int cellid = source_deme.GetCellID(random.GetUInt(source_deme.GetSize()));
            if (cell_array[cellid].IsOccupied()) {
              xfer.push_back(std::make_pair(cell_array[cellid].GetOrganism()->GetGenome(),
                                            cell_array[cellid].GetOrganism()->GetLineageLabel()));
            }
          }
          break;
        }
        case 1: { // Sequential selection, from the beginning.  Good with DEMES_ORGANISM_PLACEMENT=3.
          for(int i=0; i<m_world->GetConfig().DEMES_REPLICATE_SIZE.Get(); ++i) {
            int cellid = source_deme.GetCellID(i);
            if (cell_array[cellid].IsOccupied()) {
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
      source_deme.KillAll(ctx); 

      target_deme.UpdateStats();
      target_deme.KillAll(ctx); 

      // And now populate the source and target.
      int j=0;
      for(std::vector<std::pair<Genome,int> >::iterator i=xfer.begin(); i!=xfer.end(); ++i, ++j) {
        int cellid = DemeSelectInjectionCell(source_deme, j);
        InjectGenome(cellid, SRC_DEME_REPLICATE, i->first, ctx, i->second); 
        DemePostInjection(source_deme, cell_array[cellid]);

        if (source_deme.GetDemeID() != target_deme.GetDemeID()) {
          cellid = DemeSelectInjectionCell(target_deme, j);
          InjectGenome(cellid, SRC_DEME_REPLICATE, i->first, ctx, i->second); 
          DemePostInjection(target_deme, cell_array[cellid]);
        }

      }
    } else /* if (m_world->GetConfig().DEMES_SEED_METHOD.Get() != 0) */{

      // @JEB
      // Updated seed deme method that maintains genotype inheritance.

      // deconstruct founders into two lists...
      tArray<cOrganism*> source_founders; // List of organisms we're going to transfer.
      tArray<cOrganism*> target_founders; // List of organisms we're going to transfer.


      switch(m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get()) {
        case 0: { // Random w/ replacement (meaning, we don't prevent the same genotype from
          // being selected more than once).
          tArray<cOrganism*> founders; // List of organisms we're going to transfer.
          while(founders.GetSize() < m_world->GetConfig().DEMES_REPLICATE_SIZE.Get()) {
            int cellid = source_deme.GetCellID(random.GetUInt(source_deme.GetSize()));
            if (cell_array[cellid].IsOccupied()) {
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
            if (cell_array[cellid].IsOccupied()) {
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
                    if ( cell_array[cellid].IsOccupied() ) {
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

        if (cell_array[cell_id].IsOccupied()) {
          cOrganism * org = cell_array[cell_id].GetOrganism();
          old_source_organisms.Push(org);
          org->SetRunning(true);
        }
      }


      tArray<cOrganism*> old_target_organisms;
      for(int i=0; i<target_deme.GetSize(); ++i) {
        int cell_id = target_deme.GetCellID(i);

        if (cell_array[cell_id].IsOccupied()) {
          cOrganism * org = cell_array[cell_id].GetOrganism();
          old_target_organisms.Push(org);
          org->SetRunning(true);
        }
      }

      // Clear the target deme (if we have successfully replaced it).
      if (target_founders.GetSize() > 0) {
        target_deme.ClearFounders();
        target_deme.UpdateStats();
        target_deme.KillAll(ctx);
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
        SeedDeme_InjectDemeFounder(cellid, target_founders[i]->GetBioGroup("genotype"), ctx, &target_founders[i]->GetPhenotype()); 
        target_deme.AddFounder(target_founders[i]->GetBioGroup("genotype"), &target_founders[i]->GetPhenotype());
        DemePostInjection(target_deme, cell_array[cellid]);
      }

      // We either repeat this procedure in the source deme,
      // restart the source deme from its old founders,
      // or do nothing to the source deme...

      // source deme is replaced in the same way as the target
      if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 0) {

        source_deme.ClearFounders();
        source_deme.UpdateStats();
        source_deme.KillAll(ctx); 

        for(int i=0; i<source_founders.GetSize(); i++) {
          int cellid = DemeSelectInjectionCell(source_deme, i);
          SeedDeme_InjectDemeFounder(cellid, source_founders[i]->GetBioGroup("genotype"), ctx, &source_founders[i]->GetPhenotype()); 
          source_deme.AddFounder(source_founders[i]->GetBioGroup("genotype"), &source_founders[i]->GetPhenotype());
          DemePostInjection(source_deme, cell_array[cellid]);
        }
      }
      // source deme is "reset" by re-injecting founder genotypes
      else if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 1) {
        // Do not update the last founder generation, since the founders have not changed.

        source_deme.UpdateStats();
        source_deme.KillAll(ctx); 
        // do not clear or change founder list

        // use it to recreate ancestral state of genotypes
        tArray<int>& source_founders = source_deme.GetFounderGenotypeIDs();
        tArray<cPhenotype>& source_founder_phenotypes = source_deme.GetFounderPhenotypes();
        for(int i=0; i<source_founders.GetSize(); i++) {

          int cellid = DemeSelectInjectionCell(source_deme, i);
          //cout << "founder: " << source_founders[i] << endl;
          cBioGroup* bg = m_world->GetClassificationManager().GetBioGroupManager("genotype")->GetBioGroup(source_founders[i]);
          SeedDeme_InjectDemeFounder(cellid, bg, ctx, &source_founder_phenotypes[i]); 
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


      // remember to delete the old target organisms and adjust their genotypes
      for(int i=0; i<old_target_organisms.GetSize(); ++i) {
        old_target_organisms[i]->SetRunning(false);
        // ONLY delete target orgs if seeding was successful
        // otherwise they still exist in the population!!!
        if (successfully_seeded) delete old_target_organisms[i];
      }

      for(int i=0; i<old_source_organisms.GetSize(); ++i) {
        old_source_organisms[i]->SetRunning(false);

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
    target_deme.KillAll(ctx); 

    // Check all the organisms in the source deme to see if they get transferred
    // to the target.
    for(int i=0,j=0; i<source_deme.GetSize(); ++i) {
      int source_cellid = source_deme.GetCellID(i);

      // Does this organism stay with the source or move to the target?
      if (cell_array[source_cellid].IsOccupied() && random.P(m_world->GetConfig().DEMES_PROB_ORG_TRANSFER.Get())) {
        // Moves to the target; save the genome and lineage label of organism being transfered.
        cOrganism* seed = cell_array[source_cellid].GetOrganism();
        const Genome& genome = seed->GetGenome();
        int lineage = seed->GetLineageLabel();
        seed = 0; // We're done with the seed organism.

        // Remove this organism from the source.
        KillOrganism(cell_array[source_cellid], ctx); 

        // And inject it into target deme.
        int target_cellid = DemeSelectInjectionCell(target_deme, j++);
        InjectGenome(target_cellid, SRC_DEME_REPLICATE, genome, ctx, lineage); 
        DemePostInjection(target_deme, cell_array[target_cellid]);
      }
      //else {
      // Stays with the source.  Nothing to do here yet.
      //}
    }
  }

	return successfully_seeded;
}

void cPopulation::SeedDeme_InjectDemeFounder(int _cell_id, cBioGroup* bg, cAvidaContext& ctx, cPhenotype* _phenotype) 
{
  // phenotype can be NULL

  InjectGenome(_cell_id, SRC_DEME_REPLICATE, Genome(bg->GetProperty("genome").AsString()), ctx); 

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
      cellid = deme.GetCellID(m_world->GetRandom().GetInt(0, deme.GetSize()));
      while(cell_array[cellid].IsOccupied()) {
        cellid = deme.GetCellID(m_world->GetRandom().GetInt(0, deme.GetSize()));
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
 was injected into a deme.  Handles all the rotation / facing options. Also increments
 the number of organisms injected into the deme.
 */
void cPopulation::DemePostInjection(cDeme& deme, cPopulationCell& cell) {
  assert(cell.GetID() >= deme.GetCellID(0));
  assert(cell.GetID() <= deme.GetCellID(deme.GetSize()-1));
  deme.IncInjectedCount();
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

void cPopulation::DivideDemes(cAvidaContext& ctx) 
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
      KillOrganism(cell_array[ target_deme.GetCellID(i) ], ctx); 
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
      InjectClone(cell2_id, *org1, SRC_DEME_REPLICATE);

      // Kill the organisms in the odd cells.
      KillOrganism( cell_array[cell1_id], ctx); 
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
      InjectClone(cur_cell_id, *(cell_array[cur_cell_id].GetOrganism()), cell_array[cur_cell_id].GetOrganism()->GetUnitSource());
    }
  }
}


// Copy the full contents of one deme into another.

void cPopulation::CopyDeme(int deme1_id, int deme2_id, cAvidaContext& ctx) 
{
  cDeme & deme1 = deme_array[deme1_id];
  cDeme & deme2 = deme_array[deme2_id];

  for (int i = 0; i < deme1.GetSize(); i++) {
    int from_cell = deme1.GetCellID(i);
    int to_cell = deme2.GetCellID(i);
    if (cell_array[from_cell].IsOccupied() == false) {
      KillOrganism(cell_array[to_cell], ctx); 
      continue;
    }
    InjectClone(to_cell, *(cell_array[from_cell].GetOrganism()), SRC_DEME_COPY);
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
    if (phenotype.IsDonorLast()) donation_makers.Add(1);  // Found donor
    if (phenotype.IsReceiverLast()) {
      donation_receivers.Add(1);                          // Found receiver
      if (phenotype.IsDonorLast()==0) {
        donation_cheaters.Add(1);                         // Found receiver with non-donor parent
      }
    }
    // edit donors & receivers
    if (phenotype.IsDonorEditLast()) edit_donation_makers.Add(1);  // Found edit donor
    if (phenotype.IsReceiverEditLast()) {
      edit_donation_receivers.Add(1);                              // Found edit receiver
      if (phenotype.IsDonorEditLast()==0) {
        edit_donation_cheaters.Add(1);                             // Found edit receiver whose parent did not make a edit donation
      }
    }

    // kin donors & receivers
    if (phenotype.IsDonorKinLast()) kin_donation_makers.Add(1); //found a kin donor
    if (phenotype.IsReceiverKinLast()){
      kin_donation_receivers.Add(1);                            //found a kin receiver
      if (phenotype.IsDonorKinLast()==0){
        kin_donation_cheaters.Add(1);                           //found a kin receiver whose parent did not make a kin donation
      }
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

void cPopulation::SpawnDeme(int deme1_id, cAvidaContext& ctx, int deme2_id) 
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
    KillOrganism(cell_array[ deme2.GetCellID(i) ], ctx); 
  }

  // And do the spawning.
  int cell2_id = deme2.GetCellID( random.GetUInt(deme2.GetSize()) );
  InjectClone( cell2_id, *(cell_array[cell1_id].GetOrganism()), SRC_DEME_SPAWN);
}

void cPopulation::AddDemePred(cString type, int times) {
  if (type == "EventReceivedCenter") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventReceivedCenterPred(times);
    }
  } else if (type == "EventReceivedLeftSide") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventReceivedLeftSidePred(times);
    }
  } else if (type == "EventMovedIntoCenter") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventMoveCenterPred(times);
    }
  } else if (type == "EventMovedBetweenTargets") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventMoveBetweenTargetsPred(times);
    }
  } else if (type == "EventNUniqueIndividualsMovedIntoTarget") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventEventNUniqueIndividualsMovedIntoTargetPred(times);
    }
  } else {
    cout << "Unknown Predicate\n";
    exit(1);
  }
}

void cPopulation::CheckImplicitDemeRepro(cDeme& deme, cAvidaContext& ctx) {

  if (GetNumDemes() <= 1) return;

  if (m_world->GetConfig().DEMES_REPLICATE_CPU_CYCLES.Get()
      && (deme.GetTimeUsed() >= m_world->GetConfig().DEMES_REPLICATE_CPU_CYCLES.Get())) ReplicateDeme(deme, ctx); 
  else if (m_world->GetConfig().DEMES_REPLICATE_TIME.Get() 
           && (deme.GetNormalizedTimeUsed() >= m_world->GetConfig().DEMES_REPLICATE_TIME.Get())) ReplicateDeme(deme, ctx); 
  else if (m_world->GetConfig().DEMES_REPLICATE_BIRTHS.Get() 
           && (deme.GetBirthCount() >= m_world->GetConfig().DEMES_REPLICATE_BIRTHS.Get())) ReplicateDeme(deme, ctx); 
  else if (m_world->GetConfig().DEMES_REPLICATE_ORGS.Get() 
           && (deme.GetOrgCount() >= m_world->GetConfig().DEMES_REPLICATE_ORGS.Get())) ReplicateDeme(deme, ctx);      
}

// Print out all statistics about individual demes
void cPopulation::PrintDemeAllStats(cAvidaContext& ctx) { 
  PrintDemeFitness();
  PrintDemeLifeFitness();
  PrintDemeMerit();
  PrintDemeGestationTime();
  PrintDemeTasks();
  PrintDemeDonor();
  PrintDemeReceiver();
  PrintDemeMutationRate();
  PrintDemeResource(ctx); 
  PrintDemeInstructions();

  if (m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
    PrintDemeSpatialEnergyData();
    PrintDemeSpatialSleepData();
  }
}

void cPopulation::PrintDemeTestamentStats(const cString& filename) {
  cStats& stats = m_world->GetStats();

  cDataFile& df = m_world->GetDataFile(filename);
  df.WriteTimeStamp();
  df.Write(m_world->GetStats().GetUpdate(), "Update");
  df.Write(stats.SumEnergyTestamentToDemeOrganisms().Average(),     "Energy Testament to Deme Organisms");
  df.Write(stats.SumEnergyTestamentToFutureDeme().Average(),        "Energy Testament to Future Deme");
  df.Write(stats.SumEnergyTestamentToNeighborOrganisms().Average(), "Energy Testament to Neighbor Organisms");
  df.Write(stats.SumEnergyTestamentAcceptedByDeme().Average(),      "Energy Testament Accepted by Future Deme");
  df.Write(stats.SumEnergyTestamentAcceptedByOrganisms().Average(), "Energy Testament Accepted by Organisms");
  df.Endl();


  stats.SumEnergyTestamentToDemeOrganisms().Clear();
  stats.SumEnergyTestamentToFutureDeme().Clear();
  stats.SumEnergyTestamentToNeighborOrganisms().Clear();
}

void cPopulation::PrintCurrentMeanDemeDensity(const cString& filename) {
	cDoubleSum demeDensity;
	demeDensity.Clear();
	const int num_demes = deme_array.GetSize();
	for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
    const cDeme & cur_deme = deme_array[deme_id];
		demeDensity.Add(cur_deme.GetDensity());
	}

	cDataFile& df = m_world->GetDataFile(filename);
  df.WriteTimeStamp();
  df.Write(m_world->GetStats().GetUpdate(), "Update");
	df.Write(demeDensity.Average(), "Current mean deme density");
	df.Endl();
}

// Print some stats about the energy sharing behavior of each deme
void cPopulation::PrintDemeEnergySharingStats() {
  const int num_demes = deme_array.GetSize();
  cStats& stats = m_world->GetStats();
  cDataFile & df_donor = m_world->GetDataFile("deme_energy_sharing.dat");
  df_donor.WriteComment("Average energy donation statistics for each deme in population");
  df_donor.WriteTimeStamp();
  df_donor.Write(stats.GetUpdate(), "update");

  double num_requestors = 0.0;
  double num_donors = 0.0;
  double num_receivers = 0.0;
  double num_donations = 0.0;
  double num_receptions = 0.0;
  double num_applications = 0.0;
  double amount_donated = 0.0;
  double amount_received = 0.0;
  double amount_applied = 0.0;

  for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
    const cDeme & cur_deme = deme_array[deme_id];

    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      if (phenotype.IsEnergyRequestor()) num_requestors++;
      if (phenotype.IsEnergyDonor()) num_donors++;
      if (phenotype.IsEnergyReceiver()) num_receivers++;
      num_donations += phenotype.GetNumEnergyDonations();
      num_receptions += phenotype.GetNumEnergyReceptions();
      num_applications += phenotype.GetNumEnergyApplications();
      amount_donated += phenotype.GetAmountEnergyDonated();
      amount_received += phenotype.GetAmountEnergyReceived();
      amount_applied += phenotype.GetAmountEnergyApplied();
    }
  }

  df_donor.Write(num_requestors/num_demes, "Average number of organisms that have requested energy");
  df_donor.Write(num_donors/num_demes, "Average number of organisms that have donated energy");
  df_donor.Write(num_receivers/num_demes, "Average number of organisms that have received energy");
  df_donor.Write(num_donations/num_demes, "Average number of donations per deme");
  df_donor.Write(num_receptions/num_demes, "Average number of receipts per deme");
  df_donor.Write(num_applications/num_demes, "Average number of applications per deme");
  df_donor.Write(amount_donated/num_demes, "Average total amount of energy donated per deme");
  df_donor.Write(amount_received/num_demes, "Average total amount of energy received per deme");
  df_donor.Write(amount_applied/num_demes, "Average total amount of donated energy applied per deme");
  df_donor.Endl();

} //End PrintDemeEnergySharingStats()


// Print some stats about the distribution of energy among the cells in a deme
// If a cell is occupied, the amount returned is that of the organism in that cell
// If a cell is not occupied, the amount returned is the amount of energy resource in the cell
void cPopulation::PrintDemeEnergyDistributionStats(cAvidaContext& ctx) {
  const int num_demes = deme_array.GetSize();
  cStats& stats = m_world->GetStats();
  cString comment;

  cDoubleSum deme_energy_distribution;

  cDoubleSum overall_average;
  cDoubleSum overall_variance;
  cDoubleSum overall_stddev;

  cDataFile & df_dist = m_world->GetDataFile("deme_energy_distribution.dat");
  comment.Set("Average distribution of energy among cells in each of %d %d x %d demes", num_demes, m_world->GetConfig().WORLD_X.Get(), m_world->GetConfig().WORLD_Y.Get()/num_demes);
  df_dist.WriteComment(comment);
  df_dist.WriteTimeStamp();
  df_dist.Write(stats.GetUpdate(), "Update");

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    const cDeme & cur_deme = deme_array[deme_id];

    for (int i = 0; i < cur_deme.GetSize(); i++) {

      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) {
        deme_energy_distribution.Add(cur_deme.GetCellEnergy(cur_cell, ctx));
        continue;
      }

      deme_energy_distribution.Add(GetCell(cur_cell).GetOrganism()->GetPhenotype().GetStoredEnergy());
    }

    overall_average.Add(deme_energy_distribution.Average());
    overall_variance.Add(deme_energy_distribution.Variance());
    overall_stddev.Add(deme_energy_distribution.StdDeviation());
    deme_energy_distribution.Clear();

  }

  df_dist.Write(overall_average.Average(), "Average of Average Energy Level");
  df_dist.Write(overall_variance.Average(), "Average of Energy Level Variance");
  df_dist.Write(overall_stddev.Average(), "Average of Energy Level Standard Deviations");

  df_dist.Endl();

} //End PrintDemeEnergyDistributionStats()


// Print some stats about the distribution of energy among the organism in a deme
// If a cell is occupied, the amount returned is that of the organism in that cell
// If a cell is not occupied, 0 energy is returned for that cell.
void cPopulation::PrintDemeOrganismEnergyDistributionStats() {
  const int num_demes = deme_array.GetSize();
  cStats& stats = m_world->GetStats();
  cString comment;

  cDoubleSum deme_energy_distribution;

  cDoubleSum overall_average;
  cDoubleSum overall_variance;
  cDoubleSum overall_stddev;

  cDataFile & df_dist = m_world->GetDataFile("deme_org_energy_distribution.dat");
  comment.Set("Average distribution of energy among organisms in each of %d %d x %d demes", num_demes, m_world->GetConfig().WORLD_X.Get(), m_world->GetConfig().WORLD_Y.Get()/num_demes);
  df_dist.WriteComment(comment);
  df_dist.WriteTimeStamp();
  df_dist.Write(stats.GetUpdate(), "Update");

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    const cDeme & cur_deme = deme_array[deme_id];

    for (int i = 0; i < cur_deme.GetSize(); i++) {

      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) {
        deme_energy_distribution.Add(0);
        continue;
      }

      deme_energy_distribution.Add(GetCell(cur_cell).GetOrganism()->GetPhenotype().GetStoredEnergy());
    }

    overall_average.Add(deme_energy_distribution.Average());
    overall_variance.Add(deme_energy_distribution.Variance());
    overall_stddev.Add(deme_energy_distribution.StdDeviation());
    deme_energy_distribution.Clear();

  }

  df_dist.Write(overall_average.Average(), "Average of Average Energy Level");
  df_dist.Write(overall_variance.Average(), "Average of Energy Level Variance");
  df_dist.Write(overall_stddev.Average(), "Average of Energy Level Standard Deviations");

  df_dist.Endl();

} //End PrintDemeOrganismEnergyDistributionStats()


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

void cPopulation::PrintDemeTotalAvgEnergy(cAvidaContext& ctx) { 
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  cDataFile & df_fit = m_world->GetDataFile("deme_totalAvgEnergy.dat");
  df_fit.WriteComment("Average energy for demes in the population");
  df_fit.WriteTimeStamp();
  df_fit.Write(stats.GetUpdate(), "update");
	cDoubleSum avg_energy;

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    const cDeme & cur_deme = deme_array[deme_id];
		avg_energy.Add(cur_deme.CalculateTotalEnergy(ctx)); 
	}
	df_fit.Write(avg_energy.Ave(), "Total Average Energy");
	df_fit.Endl();
}

void cPopulation::PrintDemeGestationTime()
{
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

void cPopulation::PrintDemeInstructions()
{
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    for (int is_id = 0; is_id < m_world->GetHardwareManager().GetNumInstSets(); is_id++) {
      const cString& inst_set = m_world->GetHardwareManager().GetInstSet(is_id).GetInstSetName();
      int num_inst = m_world->GetHardwareManager().GetInstSet(is_id).GetSize();

      cDataFile& df_inst = m_world->GetDataFile(cStringUtil::Stringf("deme_instruction-%d-%s.dat", deme_id, (const char*)inst_set));
      df_inst.WriteComment(cStringUtil::Stringf("Number of times each instruction is exectued in deme %d", deme_id));
      df_inst.WriteTimeStamp();
      df_inst.Write(stats.GetUpdate(), "update");

      tArray<cIntSum> single_deme_inst(num_inst);

      const cDeme& cur_deme = deme_array[deme_id];
      for (int i = 0; i < cur_deme.GetSize(); i++) {
        int cur_cell = cur_deme.GetCellID(i);
        if (!cell_array[cur_cell].IsOccupied()) continue;
        if (cell_array[cur_cell].GetOrganism()->GetGenome().GetInstSet() != inst_set) continue;
        cPhenotype& phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();

        for (int j = 0; j < num_inst; j++) single_deme_inst[j].Add(phenotype.GetLastInstCount()[j]);
      }

      for (int j = 0; j < num_inst; j++) df_inst.Write((int)single_deme_inst[j].Sum(), cStringUtil::Stringf("Inst %d", j));
      df_inst.Endl();
    }
  }
}

void cPopulation::PrintDemeLifeFitness()
{
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

void cPopulation::PrintDemeResource(cAvidaContext& ctx) { 
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  cDataFile & df_resources = m_world->GetDataFile("deme_resources.dat");
  df_resources.WriteComment("Avida deme resource data");
  df_resources.WriteTimeStamp();
  df_resources.Write(stats.GetUpdate(), "update");

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cDeme & cur_deme = deme_array[deme_id];

    cur_deme.UpdateDemeRes(ctx); 
    const cResourceCount& res = GetDeme(deme_id).GetDemeResourceCount();
    for(int j = 0; j < res.GetSize(); j++) {
      const char * tmp = res.GetResName(j);
      df_resources.Write(res.Get(ctx, j), cStringUtil::Stringf("Deme %d Resource %s", deme_id, tmp)); //comment);
      if ((res.GetResourcesGeometry())[j] != nGeometry::GLOBAL && (res.GetResourcesGeometry())[j] != nGeometry::PARTIAL) {
        PrintDemeSpatialResData(res, j, deme_id, ctx); 
      }
    }
  }
  df_resources.Endl();
}

//Write deme global resource levels to a file that can be easily read into Matlab.
//Each time this runs, a Matlab array is created that contains an array.  Each row in the array contains <deme id> <res level 0> ... <res level n>
void cPopulation::PrintDemeGlobalResources(cAvidaContext& ctx) { 
  const int num_demes = deme_array.GetSize();
  cDataFile & df = m_world->GetDataFile("deme_global_resources.dat");
  df.WriteComment("Avida deme resource data");
  df.WriteTimeStamp();

  cString UpdateStr = cStringUtil::Stringf( "deme_global_resources_%07i = [ ...", m_world->GetStats().GetUpdate());
  df.WriteRaw(UpdateStr);

  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cDeme & cur_deme = deme_array[deme_id];
    cur_deme.UpdateDemeRes(ctx);

    const cResourceCount & res = GetDeme(deme_id).GetDemeResourceCount();
    const int num_res = res.GetSize();

    df.WriteBlockElement(deme_id, 0, num_res + 1);

    for(int r = 0; r < num_res; r++) {
      if (!res.IsSpatial(r)) {
        df.WriteBlockElement(res.Get(ctx, r), r + 1, num_res + 1);
      }

    } //End iterating through resources

  } //End iterating through demes

  df.WriteRaw("];");
  df.Endl();
}


// Write spatial energy data to a file that can easily be read into Matlab
void cPopulation::PrintDemeSpatialEnergyData() const {
  int cellID = 0;
  int update = m_world->GetStats().GetUpdate();

  for(int i = 0; i < m_world->GetPopulation().GetNumDemes(); i++) {
    cString tmpfilename = cStringUtil::Stringf( "deme_%07i_spatial_energy.m", i);  // moved here for easy movie making
    cDataFile& df = m_world->GetDataFile(tmpfilename);
    cString UpdateStr = cStringUtil::Stringf( "deme_%07i_energy_%07i = [ ...", i, update );
    df.WriteRaw(UpdateStr);

    int gridsize = m_world->GetPopulation().GetDeme(i).GetSize();
    int xsize = m_world->GetConfig().WORLD_X.Get();

    // write grid to file
    for (int j = 0; j < gridsize; j++) {
      cPopulationCell& cell = m_world->GetPopulation().GetCell(cellID);
      if (cell.IsOccupied()) {
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
void cPopulation::PrintDemeSpatialResData(const cResourceCount& res, const int i, const int deme_id, cAvidaContext& ctx) const { 
  const char* tmpResName = res.GetResName(i);
  cString tmpfilename = cStringUtil::Stringf( "deme_spatial_resource_%s.m", tmpResName );
  cDataFile& df = m_world->GetDataFile(tmpfilename);
  cString UpdateStr = cStringUtil::Stringf( "deme_%07i_%s_%07i = [ ...", deme_id, static_cast<const char*>(res.GetResName(i)), m_world->GetStats().GetUpdate() );

  df.WriteRaw(UpdateStr);

  const cSpatialResCount& sp_res = res.GetSpatialResource(i); 
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
  cString tmpfilename = "deme_spatial_sleep.m";
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
      if (cell.IsOccupied()) {
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

    if (deme_array[i].IsEmpty()) continue;

    tArray<int>& deme_founders = deme_array[i].GetFounderGenotypeIDs();

    fp << i << " " << deme_founders.GetSize();
    for(int j=0; j<deme_founders.GetSize(); j++) {
      fp << " " << deme_founders[j];
    }

    fp << endl;
  }
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
  //  int gen_id = organism->GetBioGroup("genotype")->GetID();
  if (m_world->GetConfig().TRACK_CCLADES.Get() > 0) {
    // @TODO - support for IsCCladeFounder?
    //    if (m_world->GetClassificationManager().IsCCladeFounder(gen_id)) organism->SetCCladeLabel(gen_id);
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
cPopulationCell& cPopulation::PositionOffspring(cPopulationCell& parent_cell, cAvidaContext& ctx, bool parent_ok) 
{
  assert(parent_cell.IsOccupied());

  const int birth_method = m_world->GetConfig().BIRTH_METHOD.Get();

  // Handle Population Cap (if enabled)
  int pop_cap = m_world->GetConfig().POPULATION_CAP.Get();
  if (pop_cap > 0 && num_organisms >= pop_cap) {
    double max_msr = 0.0;
    int cell_id = 0;
    for (int i = 0; i < cell_array.GetSize(); i++) {
      if (cell_array[i].IsOccupied() && cell_array[i].GetID() != parent_cell.GetID()) {
        double msr = m_world->GetRandom().GetDouble();
        if (msr > max_msr) {
          max_msr = msr;
          cell_id = i;
        }
      }
    }
    KillOrganism(cell_array[cell_id], ctx); 
  }
  
  // Handle Pop Cap Eldest (if enabled)  
  // Pop Cap Eldest Added sets a population limit whereby oldest in entire population is one killed (as in POPULATION CAP), 
  // but incoming offspring is born in place as determined by BIRTH METHOD (e.g. facing cell), rather than wherever the 
  // (killed) oldest was.
  int pop_eldest = m_world->GetConfig().POP_CAP_ELDEST.Get();
  if (pop_eldest > 0 && num_organisms >= pop_eldest) {
    double max_age = 0.0;
    int cell_id = 0;
    for (int i = 0; i < cell_array.GetSize(); i++) {
      if (cell_array[i].IsOccupied() && cell_array[i].GetID() != parent_cell.GetID()) {       
        double age = cell_array[i].GetOrganism()->GetPhenotype().GetAge();
        if (age > max_age) {
          max_age = age;
          cell_id = i;
        }
      }
    }
    KillOrganism(cell_array[cell_id], ctx);
  }
	
	// increment the number of births in the **parent deme**.  in the case of a
	// migration, only the origin has its birth count incremented.
  if (deme_array.GetSize() > 0) {
    const int deme_id = parent_cell.GetDemeID();
    deme_array[deme_id].IncBirthCount();
  }

  // Decide if offspring will migrate to another deme -- if migrating we ignore the birth method.
  if (m_world->GetConfig().MIGRATION_RATE.Get() > 0.0 &&
      m_world->GetRandom().P(m_world->GetConfig().MIGRATION_RATE.Get())) {

    //cerr << "Attempting to migrate with rate " << m_world->GetConfig().MIGRATION_RATE.Get() << "!" << endl;
    int deme_id = parent_cell.GetDemeID();

    //get another -unadjusted- deme id
    int rnd_deme_id = m_world->GetRandom().GetInt(deme_array.GetSize()-1);

    //if the -unadjusted- id is above the excluded id, bump it up one
    //insures uniform prob of landing in any deme but the parent's
    if (rnd_deme_id >= deme_id) rnd_deme_id++;

    //set the new deme_id
    deme_id = rnd_deme_id;

    //The rest of this is essentially POSITION_OFFSPRING_DEME_RANDOM
    //@JEB: But note that this will not honor PREFER_EMPTY in the new deme.
    const int deme_size = deme_array[deme_id].GetSize();

    int out_pos = m_world->GetRandom().GetUInt(deme_size);
    int out_cell_id = deme_array[deme_id].GetCellID(out_pos);
    while (parent_ok == false && out_cell_id == parent_cell.GetID()) {
      out_pos = m_world->GetRandom().GetUInt(deme_size);
      out_cell_id = deme_array[deme_id].GetCellID(out_pos);
    }

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
    return PositionDemeMigration(parent_cell, parent_ok);
  }

  // This block should be changed to a switch statment with functions handling
  // the cases. For now, a bunch of if's that return if they handle.

  if (birth_method == POSITION_OFFSPRING_FULL_SOUP_RANDOM) {

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

  if (birth_method == POSITION_OFFSPRING_FULL_SOUP_ELDEST) {
    cPopulationCell * out_cell = reaper_queue.PopRear();
    if (parent_ok == false && out_cell->GetID() == parent_cell.GetID()) {
      out_cell = reaper_queue.PopRear();
      reaper_queue.PushRear(&parent_cell);
    }
    return *out_cell;
  }

  if (birth_method == POSITION_OFFSPRING_DEME_RANDOM) {
    return PositionDemeRandom(parent_cell.GetDemeID(), parent_cell, parent_ok);
  }
  else if (birth_method == POSITION_OFFSPRING_PARENT_FACING) {
    return parent_cell.GetCellFaced();
  }
  else if (birth_method == POSITION_OFFSPRING_NEXT_CELL) {
    int out_cell_id = parent_cell.GetID() + 1;
    if (out_cell_id == cell_array.GetSize()) out_cell_id = 0;
    return GetCell(out_cell_id);
  }
  else if (birth_method == POSITION_OFFSPRING_FULL_SOUP_ENERGY_USED) {
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
  tList<cPopulationCell>& conn_list = parent_cell.ConnectionList();

  const bool prefer_empty = m_world->GetConfig().PREFER_EMPTY.Get();

  if (birth_method == POSITION_OFFSPRING_DISPERSAL && conn_list.GetSize() > 0) {
    tList<cPopulationCell>* disp_list = &conn_list;

    // hop through connection lists based on the dispersal rate
    int hops = m_world->GetRandom().GetRandPoisson(m_world->GetConfig().DISPERSAL_RATE.Get());
    for (int i = 0; i < hops; i++) {
      disp_list = &(disp_list->GetPos(m_world->GetRandom().GetUInt(disp_list->GetSize()))->ConnectionList());
      if (disp_list->GetSize() == 0) break;
    }

    // if prefer empty, select an empty cell from the final connection list
    if (prefer_empty) FindEmptyCell(*disp_list, found_list);

    // if prefer empty is off, or there are no empty cells, use the whole connection list as possiblities
    if (found_list.GetSize() == 0) {
      found_list = *disp_list;
      // if no hops were taken and ALLOW_PARENT is set, throw the parent cell into the hat for possible selection
      if (hops == 0 && parent_ok) found_list.Push(&parent_cell);
    }
  } else if (prefer_empty) {
    FindEmptyCell(conn_list, found_list);
  }

  // If we have not found an empty organism, we must use the specified function
  // to determine how to choose among the filled organisms.
  if (found_list.GetSize() == 0) {
    switch(birth_method) {
      case POSITION_OFFSPRING_AGE:
        PositionAge(parent_cell, found_list, parent_ok);
        break;
      case POSITION_OFFSPRING_MERIT:
        PositionMerit(parent_cell, found_list, parent_ok);
        break;
      case POSITION_OFFSPRING_RANDOM:
        found_list.Append(conn_list);
        if (parent_ok == true) found_list.Push(&parent_cell);
        break;
      case POSITION_OFFSPRING_NEIGHBORHOOD_ENERGY_USED:
        PositionEnergyUsed(parent_cell, found_list, parent_ok);
      case POSITION_OFFSPRING_EMPTY:
        // Nothing is in list if no empty cells are found...
        break;
    }
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

// This function handles PositionOffspring() when there is migration between demes
cPopulationCell& cPopulation::PositionDemeMigration(cPopulationCell& parent_cell, bool parent_ok)
{
  int deme_id = parent_cell.GetDemeID();
	int parent_id = parent_cell.GetDemeID();
	GetDeme(deme_id).AddMigrationOut();

  // Position randomly in any other deme
  if (m_world->GetConfig().DEMES_MIGRATION_METHOD.Get() == 0) {

    //get another -unadjusted- deme id
    int rnd_deme_id = m_world->GetRandom().GetInt(deme_array.GetSize()-1);

    //if the -unadjusted- id is above the excluded id, bump it up one
    //insures uniform prob of landing in any deme but the parent's
    if (rnd_deme_id >= deme_id) rnd_deme_id++;

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

  //Proportional-based on a points system (hjg)
  // The odds of a deme being selected are inversely proportional to the
  // number of points it has.
  else if (m_world->GetConfig().DEMES_MIGRATION_METHOD.Get() == 3) {

    double total_points = 0;
    int num_demes = GetNumDemes();

    // Identify how many points are in the population as a whole.
    for (int did = 0; did < num_demes; did++) {
      if (did != parent_id) {
        total_points +=  (1/(1+GetDeme(did).GetNumberOfPoints()));
      }
    }
    // Select a random number from 0 to 1:
    double rand_point = m_world->GetRandom().GetDouble(0, total_points);

    // Iterate through the demes until you find the appropriate
    // deme to insert the organism into.
    double lower_point = 0;
    double upper_point = 0;

    for (int curr_deme = 0; curr_deme < num_demes; curr_deme++) {
      if (curr_deme != parent_id){
        upper_point = lower_point + (1+GetDeme(curr_deme).GetNumberOfPoints());
        if ((lower_point <= rand_point) && (rand_point < upper_point)) {
          deme_id = curr_deme;
        }
        lower_point = upper_point;
      }
    }
  }

  GetDeme(deme_id).AddMigrationIn();

  // TODO the above choice of deme does not respect PREFER_EMPTY
  // i.e., it does not preferentially pick a deme with empty cells if they are
  // it might make sense for that to happen...

  // Now return an empty cell from the chosen deme

  cPopulationCell& mig_cell = PositionDemeRandom(deme_id, parent_cell, parent_ok);
  mig_cell.SetMigrant();
  return mig_cell;
}

// This function handles PositionOffspring() by returning a random cell from the entire deme.
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
// PositionOffspring() methods. If deme_id is -1 (the default), then operates
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

  double merit = cur_org->GetPhenotype().GetMerit().GetDouble();
  if (cur_org->GetPhenotype().GetToDelete() == true) delete cur_org;

  m_world->GetStats().IncExecuted();
  resource_count.Update(step_size);

  // These must be done even if there is only one deme.
  for(int i = 0; i < GetNumDemes(); i++) {
    GetDeme(i).Update(step_size);
  }

  cDeme & deme = GetDeme(GetCell(cell_id).GetDemeID());
  deme.IncTimeUsed(merit);

  if (GetNumDemes() >= 1) {
    CheckImplicitDemeRepro(deme, ctx); 
  }
}


void cPopulation::ProcessStepSpeculative(cAvidaContext& ctx, double step_size, int cell_id)
{
  assert(step_size > 0.0);
  assert(cell_id < cell_array.GetSize());

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

  // Deme specific
  if (GetNumDemes() > 1) {
    for(int i = 0; i < GetNumDemes(); i++) GetDeme(i).Update(step_size);

    cDeme& deme = GetDeme(GetCell(cell_id).GetDemeID());
    deme.IncTimeUsed(cur_org->GetPhenotype().GetMerit().GetDouble());
    CheckImplicitDemeRepro(deme, ctx); 
  }

  if (cur_org->GetPhenotype().GetToDelete() == true) {
    delete cur_org;
    cur_org = NULL;
  }

  m_world->GetStats().IncExecuted();
  resource_count.Update(step_size);
}

// Loop through all the demes getting stats and doing calculations
// which must be done on a deme by deme basis.
void cPopulation::UpdateDemeStats(cAvidaContext& ctx) { 

  // These must be updated, even if there is only one deme
  for(int i = 0; i < GetNumDemes(); i++) {
    GetDeme(i).UpdateDemeRes(ctx); 
  }

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

  stats.SumDemeEventsKilled().Clear();
  stats.SumDemeAttemptsToKillEvents() .Clear();

  for(int i = 0; i < GetNumDemes(); i++) {
    cDeme& deme = GetDeme(i);
    if (deme.IsEmpty()) {  // ignore empty demes
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

    stats.SumDemeEventsKilled().Add(deme.GetEventsKilled());
    stats.SumDemeAttemptsToKillEvents().Add(deme.GetEventKillAttempts());
  }
}


void cPopulation::UpdateOrganismStats(cAvidaContext& ctx) 
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
  stats.ZeroReactions();

  stats.ZeroInst();

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

  for (int i = 0; i < live_org_list.GetSize(); i++) {  
    cOrganism* organism = live_org_list[i];
    const cPhenotype& phenotype = organism->GetPhenotype();
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
    stats.SumCopyMutRate().Push(organism->MutationRates().GetCopyMutProb());
    stats.SumLogCopyMutRate().Push(log(organism->MutationRates().GetCopyMutProb()));
    stats.SumDivMutRate().Push(organism->MutationRates().GetDivMutProb() / organism->GetPhenotype().GetDivType());
    stats.SumLogDivMutRate().Push(log(organism->MutationRates().GetDivMutProb() /organism->GetPhenotype().GetDivType()));
    stats.SumCopySize().Add(phenotype.GetCopiedSize());
    stats.SumExeSize().Add(phenotype.GetExecutedSize());

    tArray<cIntSum>& inst_exe_counts = stats.InstExeCountsForInstSet(organism->GetGenome().GetInstSet());
    for (int j = 0; j < phenotype.GetLastInstCount().GetSize(); j++) {
      inst_exe_counts[j].Add(organism->GetPhenotype().GetLastInstCount()[j]);
    }

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

      if (phenotype.GetCurHostTaskCount()[j] > 0) {
        stats.AddCurHostTask(j);
      }

      if (phenotype.GetLastHostTaskCount()[j] > 0) {
        stats.AddLastHostTask(j);
      }

      if (phenotype.GetCurParasiteTaskCount()[j] > 0) {
        stats.AddCurParasiteTask(j);
      }

      if (phenotype.GetLastParasiteTaskCount()[j] > 0) {
        stats.AddLastParasiteTask(j);
      }

      if (phenotype.GetCurInternalTaskCount()[j] > 0) {
        stats.AddCurInternalTask(j);
        stats.AddCurInternalTaskQuality(j, phenotype.GetCurInternalTaskQuality()[j]);
      }

      if (phenotype.GetLastInternalTaskCount()[j] > 0) {
        stats.AddLastInternalTask(j);
        stats.AddLastInternalTaskQuality(j, phenotype.GetLastInternalTaskQuality()[j]);
      }
    }

    // Record what add bonuses this organism garnered for different reactions
    for (int j = 0; j < m_world->GetEnvironment().GetNumReactions(); j++) {
      if (phenotype.GetCurReactionCount()[j] > 0) {
        stats.AddCurReaction(j);
        stats.AddCurReactionAddReward(j, phenotype.GetCurReactionAddReward()[j]);
      }

      if (phenotype.GetLastReactionCount()[j] > 0) {
        stats.AddLastReaction(j);
        stats.IncReactionExeCount(j, phenotype.GetLastReactionCount()[j]);
        stats.AddLastReactionAddReward(j, phenotype.GetLastReactionAddReward()[j]);
      }
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

    if (phenotype.IsModified()) num_modified++;

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
  
  resource_count.UpdateGlobalResources(ctx);   
}

void cPopulation::UpdateResStats(cAvidaContext& ctx) 
{
  cStats& stats = m_world->GetStats();
  stats.SetResources(resource_count.GetResources(ctx)); 
  stats.SetSpatialRes(resource_count.GetSpatialRes(ctx)); 
  stats.SetResourcesGeometry(resource_count.GetResourcesGeometry()); 
}

void cPopulation::ProcessPostUpdate(cAvidaContext& ctx)
{
  ProcessUpdateCellActions(ctx);

  cStats& stats = m_world->GetStats();

  // Reset the Genebank to prepare it for stat collection.
  m_world->GetClassificationManager().UpdateReset();

  stats.SetNumCreatures(GetNumOrganisms());

  UpdateDemeStats(ctx); 
  UpdateOrganismStats(ctx);
  
  m_world->GetClassificationManager().UpdateStats(stats);

  // Have stats calculate anything it now can...
  stats.CalcEnergy();
  stats.CalcFidelity();

  for (int i = 0; i < deme_array.GetSize(); i++) deme_array[i].ProcessUpdate(ctx);   
}

void cPopulation::ProcessUpdateCellActions(cAvidaContext& ctx)
{
  for (int i = 0; i < cell_array.GetSize(); i++) {
    if (cell_array[i].MutationRates().TestDeath(ctx)) KillOrganism(cell_array[i], ctx); 
  }
}


struct sOrgInfo {
  int cell_id;
  int offset;
  int lineage_label;

  sOrgInfo() { ; }
  sOrgInfo(int c, int o, int l) : cell_id(c), offset(o), lineage_label(l) { ; }
};

struct sGroupInfo {
  cBioGroup* bg;
  tArray<sOrgInfo> orgs;
  bool parasite;

  sGroupInfo(cBioGroup* in_bg, bool is_para = false) : bg(in_bg), parasite(is_para) { ; }
};

bool cPopulation::SavePopulation(const cString& filename, bool save_historic)
{
  cDataFile& df = m_world->GetDataFile(filename);
  df.SetFileType("genotype_data");
  df.WriteComment("Structured Population Save");
  df.WriteTimeStamp();

  // Build up hash table of all current genotypes and the cells in which the organisms reside
  tHashMap<int, sGroupInfo*> genotype_map;

  for (int cell = 0; cell < cell_array.GetSize(); cell++) {
    if (cell_array[cell].IsOccupied()) {
      cOrganism* org = cell_array[cell].GetOrganism();

      // Handle any parasites
      const tArray<cBioUnit*>& parasites = org->GetParasites();
      for (int p = 0; p < parasites.GetSize(); p++) {
        cBioGroup* pg = parasites[p]->GetBioGroup("genotype");
        if (pg == NULL) continue;

        sGroupInfo* map_entry = NULL;
        if (genotype_map.Find(pg->GetID(), map_entry)) {
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1));
        } else {
          map_entry = new sGroupInfo(pg, true);
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1));
          genotype_map.Set(pg->GetID(), map_entry);
        }
      }


      // Handle the organism itself
      cBioGroup* genotype = org->GetBioGroup("genotype");
      if (genotype == NULL) continue;

      int offset = org->GetPhenotype().GetCPUCyclesUsed();

      sGroupInfo* map_entry = NULL;
      if (genotype_map.Find(genotype->GetID(), map_entry)) {
        map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel()));
      } else {
        map_entry = new sGroupInfo(genotype);
        map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel()));
        genotype_map.Set(genotype->GetID(), map_entry);
      }
    }
  }

  // Output all current genotypes
  tArray<sGroupInfo*> genotype_entries;
  genotype_map.GetValues(genotype_entries);
  for (int i = 0; i < genotype_entries.GetSize(); i++) {
    cBioGroup* genotype = genotype_entries[i]->bg;

    genotype->Save(df);

    tArray<sOrgInfo>& cells = genotype_entries[i]->orgs;
    cString cellstr;
    cString offsetstr;
    cString lineagestr;
    cellstr.Set("%d", cells[0].cell_id);
    offsetstr.Set("%d", cells[0].offset);
    lineagestr.Set("%d", cells[0].lineage_label);
    for (int cell_i = 1; cell_i < cells.GetSize(); cell_i++) {
      cellstr += cStringUtil::Stringf(",%d", cells[cell_i].cell_id);
      offsetstr += cStringUtil::Stringf(",%d", cells[cell_i].offset);
      lineagestr += cStringUtil::Stringf(",%d", cells[cell_i].lineage_label);
    }
    df.Write(cellstr, "Occupied Cell IDs", "cells");
    if (genotype_entries[i]->parasite) df.Write("", "Gestation (CPU) Cycle Offsets", "gest_offset");
    else df.Write(offsetstr, "Gestation (CPU) Cycle Offsets", "gest_offset");
    df.Write(lineagestr, "Lineage Label", "lineage");
    df.Endl();

    delete genotype_entries[i];
  }

  // Output historic genotypes
  if (save_historic) m_world->GetClassificationManager().SaveBioGroups("genotype", df);

  m_world->GetDataFileManager().Remove(filename);
  return true;
}


struct sTmpGenotype
{
public:
  int id_num;
  tDictionary<cString>* props;

  int num_cpus;
  tArray<int> cells;
  tArray<int> offsets;
  tArray<int> lineage_labels;

  cBioGroup* bg;


  inline sTmpGenotype() : id_num(-1), props(NULL) { ; }
  inline bool operator<(const sTmpGenotype& rhs) const { return id_num > rhs.id_num; }
  inline bool operator>(const sTmpGenotype& rhs) const { return id_num < rhs.id_num; }
  inline bool operator<=(const sTmpGenotype& rhs) const { return id_num >= rhs.id_num; }
  inline bool operator>=(const sTmpGenotype& rhs) const { return id_num <= rhs.id_num; }
};


bool cPopulation::LoadPopulation(const cString& filename, cAvidaContext& ctx, int cellid_offset, int lineage_offset) 
{
  // @TODO - build in support for verifying population dimensions

  cInitFile input_file(filename, m_world->GetWorkingDir());
  if (!input_file.WasOpened()) {
    const cUserFeedback& feedback = input_file.GetFeedback();
    for (int i = 0; i < feedback.GetNumMessages(); i++) {
      switch (feedback.GetMessageType(i)) {
        case cUserFeedback::UF_ERROR:    m_world->GetDriver().RaiseException(feedback.GetMessage(i)); break;
        case cUserFeedback::UF_WARNING:  m_world->GetDriver().NotifyWarning(feedback.GetMessage(i)); break;
        default:                      m_world->GetDriver().NotifyComment(feedback.GetMessage(i)); break;
      };
    }
    return false;
  }

  // Clear out the population, unless an offset is being used
  if (cellid_offset == 0) {
    for (int i = 0; i < cell_array.GetSize(); i++) KillOrganism(cell_array[i], ctx); 
  }



  // First, we read in all the genotypes and store them in an array
  tManagedPointerArray<sTmpGenotype> genotypes(input_file.GetNumLines());

  bool structured = false;
  for (int line_id = 0; line_id < input_file.GetNumLines(); line_id++) {
    cString cur_line = input_file.GetLine(line_id);

    // Setup the genotype for this line...
    sTmpGenotype& tmp = genotypes[line_id];
    tmp.props = input_file.GetLineAsDict(line_id);
    tmp.id_num = tmp.props->Get("id").AsInt();

    // Loads "num_units" preferrentially, but will fall back to "num_cpus" if present
    assert(tmp.props->HasEntry("num_cpus") || tmp.props->HasEntry("num_units"));
    tmp.num_cpus = (tmp.props->HasEntry("num_units")) ? tmp.props->Get("num_units").AsInt() : tmp.props->Get("num_cpus").AsInt();

    // Process resident cell ids
    cString cellstr(tmp.props->Get("cells"));
    if (structured || cellstr.GetSize()) {
      structured = true;
      while (cellstr.GetSize()) tmp.cells.Push(cellstr.Pop(',').AsInt());
      assert(tmp.cells.GetSize() == tmp.num_cpus);
    }

    // Process gestation time offsets
    cString offsetstr(tmp.props->Get("gest_offset"));
    if (offsetstr.GetSize()) {
      while (offsetstr.GetSize()) tmp.offsets.Push(offsetstr.Pop(',').AsInt());
      assert(tmp.offsets.GetSize() == tmp.num_cpus);
    }

    // Lineage label (only set if given in file)
    cString lineagestr(tmp.props->Get("lineage"));
    while (lineagestr.GetSize()) tmp.lineage_labels.Push(lineagestr.Pop(',').AsInt());
    // @blw preserve compatability with older .spop files that don't have lineage labels
    assert(tmp.lineage_labels.GetSize() == 0 || tmp.lineage_labels.GetSize() == tmp.num_cpus);
  }

  // Sort genotypes in ascending order according to their id_num
  tArrayUtils::QSort(genotypes);

  cBioGroupManager* bgm = m_world->GetClassificationManager().GetBioGroupManager("genotype");
  for (int i = 0; i < genotypes.GetSize(); i++) {
    // Fix Parent IDs
    cString nparentstr;
    int pcount = 0;
    cString lparentstr = genotypes[i].props->Get("parents");
    if (lparentstr == "(none)") lparentstr = "";
    cStringList opidlist(lparentstr, ',');
    while (opidlist.GetSize()) {
      int opid = opidlist.Pop().AsInt();
      int npid = -1;
      for (int j = i; j >= 0; j--) {
        if (genotypes[j].id_num == opid) {
          npid = genotypes[j].bg->GetID();
          break;
        }
      }
      assert(npid != -1);
      if (pcount) nparentstr += ",";
      nparentstr += cStringUtil::Convert(npid);
      pcount++;
    }
    genotypes[i].props->Set("parents", nparentstr);

    genotypes[i].bg = bgm->LoadBioGroup(*genotypes[i].props);
  }


  // Process genotypes, inject into organisms as necessary
  int u_cell_id = 0;
  for (int gen_i = genotypes.GetSize() - 1; gen_i >= 0; gen_i--) {
    sTmpGenotype& tmp = genotypes[gen_i];
    // otherwise, we insert as many organisms as we need
    for (int cell_i = 0; cell_i < tmp.num_cpus; cell_i++) {
      int cell_id = (structured) ? (tmp.cells[cell_i] + cellid_offset) : (u_cell_id++ + cellid_offset);

      // Set up lineage, including lineage label (0 if not loaded)
      int lineage_label = 0;
      if (tmp.lineage_labels.GetSize() != 0) {
        lineage_label = tmp.lineage_labels[cell_i] + lineage_offset;
      }

      cAvidaContext& ctx = m_world->GetDefaultContext();

      assert(tmp.bg->HasProperty("genome"));
      Genome mg(tmp.bg->GetProperty("genome").AsString());
      cOrganism* new_organism = new cOrganism(m_world, ctx, mg, -1, SRC_ORGANISM_FILE_LOAD);

      // Setup the phenotype...
      cPhenotype& phenotype = new_organism->GetPhenotype();

      phenotype.SetupInject(mg.GetSequence());

      // Classify this new organism
      tArrayMap<cString, tArrayMap<cString, cString> > hints;
      hints["genotype"]["id"] = cStringUtil::Stringf("%d", tmp.bg->GetID());
      m_world->GetClassificationManager().ClassifyNewBioUnit(new_organism, &hints);

      // Coalescense Clade Setup
      new_organism->SetCCladeLabel(-1);

      if (m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
        phenotype.SetMerit(cMerit(phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy())));
      } else {
        // Set the phenotype merit from the save file
        assert(tmp.props->HasEntry("merit"));
        double merit = tmp.props->Get("merit").AsDouble();

        if (merit > 0) {
          phenotype.SetMerit(cMerit(merit));
        } else {
          phenotype.SetMerit(cMerit(new_organism->GetTestMerit(ctx)));
        }

        if (tmp.offsets.GetSize() > cell_i) {
          // Adjust initial merit to account for organism execution at the time the population was saved
          // - this factors the merit by the fraction of the gestation time remaining
          // - this will be approximate, since gestation time may vary for each organism, but it should work for many cases
          double gest_time = tmp.props->Get("gest_time").AsDouble();
          double gest_remain = gest_time - (double)tmp.offsets[cell_i];
          if (gest_remain > 0.0 && gest_time > 0.0) {
            double new_merit = phenotype.GetMerit().GetDouble() * (gest_time / gest_remain);
            phenotype.SetMerit(cMerit(new_merit));
          }
        }
      }

      new_organism->SetLineageLabel(lineage_label);

      // Prep the cell..
      if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST &&
          cell_array[cell_id].IsOccupied() == true) {
        // Have to manually take this cell out of the reaper Queue.
        reaper_queue.Remove( &(cell_array[cell_id]) );
      }

      // Setup the child's mutation rates.  Since this organism is being injected
      // and has no parent, we should always take the rate from the environment.
      new_organism->MutationRates().Copy(cell_array[cell_id].MutationRates());


      // Activate the organism in the population...
      ActivateOrganism(ctx, new_organism, cell_array[cell_id]);

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
      Sequence & mem = cell_array[i].GetOrganism()->GetHardware().GetMemory();
      fp << mem.GetSize() << " "
      << mem.AsString() << endl;
    }
  }
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

void cPopulation::Inject(const Genome& genome, eBioUnitSource src, cAvidaContext& ctx, int cell_id, double merit, int lineage_label, double neutral) 
{
  // If an invalid cell was given, choose a new ID for it.
  if (cell_id < 0) {
    switch (m_world->GetConfig().BIRTH_METHOD.Get()) {
      case POSITION_OFFSPRING_FULL_SOUP_ELDEST:
        cell_id = reaper_queue.PopRear()->GetID();
      default:
        cell_id = 0;
    }
  }

  // We can't inject into the boundary of the world:
  if(m_world->IsWorldBoundary(GetCell(cell_id))) {
    cell_id += m_world->GetConfig().WORLD_X.Get()+1;
  }

  InjectGenome(cell_id, src, genome, ctx, lineage_label); 
  cPhenotype& phenotype = GetCell(cell_id).GetOrganism()->GetPhenotype();
  phenotype.SetNeutralMetric(neutral);

  if (merit > 0) phenotype.SetMerit(cMerit(merit));
  AdjustSchedule(GetCell(cell_id), phenotype.GetMerit());

  cell_array[cell_id].GetOrganism()->SetLineageLabel(lineage_label);

	
	// the following bit of code is required for proper germline support.
	// even if there's only one deme!!
	if(m_world->GetConfig().DEMES_USE_GERMLINE.Get()) {
    cDeme& deme = deme_array[GetCell(cell_id).GetDemeID()];

    // If we're using germlines, then we have to be a little careful here.
    // This should probably not be within Inject() since we mainly want it to
    // apply to the START_ORGANISM? -- @JEB

    //@JEB This section is very messy to maintain consistency with other deme ways.

    if (m_world->GetConfig().DEMES_SEED_METHOD.Get() == 0) {
      if (m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 1) {
        if (deme.GetGermline().Size()==0) {
          deme.GetGermline().Add(GetCell(cell_id).GetOrganism()->GetGenome());
        }
      }
    }
    else if (m_world->GetConfig().DEMES_SEED_METHOD.Get() == 1) {
      if (m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 2) {
        //find the genotype we just created from the genome, and save it
        deme.ReplaceGermline(GetCell(cell_id).GetOrganism()->GetBioGroup("genotype"));
      }
      else { // not germlines, save org as founder
        deme.AddFounder(GetCell(cell_id).GetOrganism()->GetBioGroup("genotype"), &phenotype);
      }

      GetCell(cell_id).GetOrganism()->GetPhenotype().SetPermanentGermlinePropensity
      (m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get());


      if (m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get() >= 0.0) {
        GetCell(cell_id).GetOrganism()->GetPhenotype().SetPermanentGermlinePropensity
        ( m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get() );
      }

    }
  }
  else if (m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 2) {
    //find the genotype we just created from the genome, and save it
    cDeme& deme = deme_array[GetCell(cell_id).GetDemeID()];
    cDemePlaceholderUnit unit(src, genome);
    cBioGroup* genotype = m_world->GetClassificationManager().GetBioGroupManager("genotype")->ClassifyNewBioUnit(&unit);
    deme.ReplaceGermline(genotype);
    genotype->RemoveBioUnit(&unit);
  }
}

void cPopulation::InjectParasite(const cString& label, const Sequence& injected_code, int cell_id)
{
  cOrganism* target_organism = cell_array[cell_id].GetOrganism();
  // target_organism-> target_organism->GetHardware().GetCurThread()
  if (target_organism == NULL) return;

  Genome mg(target_organism->GetHardware().GetType(), target_organism->GetHardware().GetInstSet().GetInstSetName(), injected_code);
  cParasite* parasite = new cParasite(m_world, mg, 0, SRC_PARASITE_FILE_LOAD, label);
  
  //default to configured parasite virulence
  parasite->SetVirulence(m_world->GetConfig().PARASITE_VIRULENCE.Get());
  
  if (target_organism->ParasiteInfectHost(parasite)) {
    m_world->GetClassificationManager().ClassifyNewBioUnit(parasite);
  } else {
    delete parasite;
  }
}


void cPopulation::UpdateResources(cAvidaContext& ctx, const tArray<double> & res_change)
{
  resource_count.Modify(ctx, res_change);
}

void cPopulation::UpdateResource(cAvidaContext& ctx, int res_index, double change)
{
  resource_count.Modify(ctx, res_index, change);
}

void cPopulation::UpdateCellResources(cAvidaContext& ctx, const tArray<double>& res_change, const int cell_id)
{
  resource_count.ModifyCell(ctx, res_change, cell_id);
}

void cPopulation::UpdateDemeCellResources(cAvidaContext& ctx, const tArray<double>& res_change, const int cell_id)
{
  GetDeme(GetCell(cell_id).GetDemeID()).ModifyDemeResCount(ctx, res_change, cell_id);
}

void cPopulation::SetResource(cAvidaContext& ctx, int res_index, double new_level)
{
  resource_count.Set(ctx, res_index, new_level);
}

/* This version of SetResource takes the name of the resource.
 * If a resource by this name does not exist, it does nothing.
 * Otherwise, it sets the resource to the new level, 
 * calling the index version of SetResource().
 */
void cPopulation::SetResource(cAvidaContext& ctx, const cString res_name, double new_level)
{
  cResource* res = environment.GetResourceLib().GetResource(res_name);
  if (res != NULL) SetResource(ctx, res->GetIndex(), new_level);
}

/* This method sets the inflow of the named resource.
 * It changes this value in the environment, then updates it in the
 * actual population's resource count.
 */
void cPopulation::SetResourceInflow(const cString res_name, double new_level)
{
  environment.SetResourceInflow(res_name, new_level);
  resource_count.SetInflow(res_name, new_level);
}

/* This method sets the outflow of the named resource.
 * It changes this value in the enviroment, then updates the
 * decay rate in the resource count (to 1 - the given outflow, as 
 * outflow is different than decay).
 */
void cPopulation::SetResourceOutflow(const cString res_name, double new_level)
{
  environment.SetResourceOutflow(res_name, new_level);
  resource_count.SetDecay(res_name, 1 - new_level);
}

/* This method sets a deme resource to the same level across
 * all demes.  If a resource by the given name does not exist,
 * it does nothing.
 */
void cPopulation::SetDemeResource(cAvidaContext& ctx, const cString res_name, double new_level)
{
  cResource* res = environment.GetResourceLib().GetResource(res_name);
  if (res != NULL) {
    int num_demes = GetNumDemes();
    for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
      cDeme& deme = GetDeme(deme_id);
      deme.SetResource(ctx, res->GetIndex(), new_level);
    }
  }
}

/* This method sets the inflow for the named deme resource in a specific deme. 
 * It changes the value in the environment, then updates it in the specified deme's
 * resource count.
 * 
 * ATTENTION: This leads to the rather bizzare consequence that the inflow rate
 * in the environment may not match the inflow rate in each deme's resource count.
 * This is not my own decision, simply a reflection of how the SetDemeResourceInflow
 * action (for which I am writing this as a helper) works.  Unless you have a specific
 * reason NOT to change the inflow for all demes, it is probably best to use
 * cPopulation::SetDemeResourceInflow() -- blw
 */
void cPopulation::SetSingleDemeResourceInflow(int deme_id, const cString res_name, double new_level)
{
  environment.SetResourceInflow(res_name, new_level);
  GetDeme(deme_id).GetDemeResources().SetInflow(res_name, new_level);
}

/* This method sets the inflow for the named deme resource across ALL demes. 
 * It changes the value in the environment, then updates it in the deme resource
 * counts.
 *
 * This maintains the connection between the enviroment value and the resource
 * count values, unlike cPopulation::SetSingleDemeResourceInflow()
 */
void cPopulation::SetDemeResourceInflow(const cString res_name, double new_level)
{
  environment.SetResourceInflow(res_name, new_level);
  int num_demes = GetNumDemes();
  for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
    GetDeme(deme_id).GetDemeResources().SetInflow(res_name, new_level);
  }
}

/* This method sets the outflow for the named deme resource in a specific deme. 
 * It changes the value in the environment, then updates the decay rate in the 
 * specified deme's resource count.
 * 
 * ATTENTION: This leads to the rather bizzare consequence that the outflow rate
 * in the environment may not match the decay (1-outflow) rate in each deme's resource count.
 * This is not my own decision, simply a reflection of how the SetDemeResourceOutflow
 * action (for which I am writing this as a helper) works.  Unless you have a specific
 * reason NOT to change the outflow for all demes, it is probably best to use
 * cPopulation::SetDemeResourceOutflow() -- blw
 */
void cPopulation::SetSingleDemeResourceOutflow(int deme_id, const cString res_name, double new_level)
{
  environment.SetResourceOutflow(res_name, new_level);
  GetDeme(deme_id).GetDemeResources().SetDecay(res_name, 1 - new_level);
}

/* This method sets the outflow for the named deme resource across ALL demes. 
 * It changes the value in the environment, then updates the decay rate in the 
 * deme resource counts.
 *
 * This maintains the connection between the enviroment value and the resource
 * count values, unlike cPopulation::SetSingleDemeResourceOutflow()
 */
void cPopulation::SetDemeResourceOutflow(const cString res_name, double new_level)
{
  environment.SetResourceOutflow(res_name, new_level);
  int num_demes = GetNumDemes();
  for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
    GetDeme(deme_id).GetDemeResources().SetDecay(res_name, 1 - new_level);
  }
}

void cPopulation::ResetInputs(cAvidaContext& ctx)
{
  for (int i=0; i<GetSize(); i++) {
    cPopulationCell& cell = GetCell(i);
    cell.ResetInputs(ctx);
    if (cell.IsOccupied()) {
      cell.GetOrganism()->ResetInput();
    }
  }
}

void cPopulation::BuildTimeSlicer()
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


// This function injects a new organism into the population at cell_id that
// is an exact clone of the organism passed in.

void cPopulation::InjectClone(int cell_id, cOrganism& orig_org, eBioUnitSource src)
{
  assert(cell_id >= 0 && cell_id < cell_array.GetSize());

  cAvidaContext& ctx = m_world->GetDefaultContext();

  cOrganism* new_organism = new cOrganism(m_world, ctx, orig_org.GetGenome(), orig_org.GetPhenotype().GetGeneration(), src);

  // Classify the new organism
  m_world->GetClassificationManager().ClassifyNewBioUnit(new_organism);

  // Setup the phenotype...
  new_organism->GetPhenotype().SetupClone(orig_org.GetPhenotype());

  // Prep the cell..
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST &&
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

// This function injects the offspring genome of an organism into the population at cell_id.
// Takes care of divide mutations.
void cPopulation::CompeteOrganisms_ConstructOffspring(int cell_id, cOrganism& parent)
{
  assert(cell_id >= 0 && cell_id < cell_array.GetSize());

  cAvidaContext& ctx = m_world->GetDefaultContext();

  // Do mutations on the child genome, but restore it to its current state afterward.
  Genome save_child = parent.OffspringGenome();
  parent.GetHardware().Divide_DoMutations(ctx);
  Genome child_genome = parent.OffspringGenome();
  parent.GetHardware().Divide_TestFitnessMeasures(ctx);
  parent.OffspringGenome() = save_child;
  cOrganism* new_organism = new cOrganism(m_world, ctx, child_genome, parent.GetPhenotype().GetGeneration(), SRC_ORGANISM_COMPETE);

  // Classify the offspring
  tArray<const tArray<cBioGroup*>*> pgrps(1);
  pgrps[0] = &parent.GetBioGroups();
  new_organism->SelfClassify(pgrps);

  // Setup the phenotype...
  new_organism->GetPhenotype().SetupOffspring(parent.GetPhenotype(),child_genome.GetSequence());

  // Prep the cell..
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST &&
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


void cPopulation::InjectGenome(int cell_id, eBioUnitSource src, const Genome& genome, cAvidaContext& ctx2, int lineage_label) 
{
  assert(cell_id >= 0 && cell_id < cell_array.GetSize());
  if (cell_id < 0 || cell_id >= cell_array.GetSize()) {
    m_world->GetDriver().RaiseFatalException(1, "InjectGenotype into nonexistent cell");
  }

  cAvidaContext& ctx = m_world->GetDefaultContext();

  cOrganism* new_organism = new cOrganism(m_world, ctx, genome, -1, src);

  // Setup the phenotype...
  cPhenotype& phenotype = new_organism->GetPhenotype();

  phenotype.SetupInject(genome.GetSequence());

  // Classify this new organism
  m_world->GetClassificationManager().ClassifyNewBioUnit(new_organism);

  //Coalescense Clade Setup
  new_organism->SetCCladeLabel(-1);

  cGenomeTestMetrics* metrics = cGenomeTestMetrics::GetMetrics(ctx, new_organism->GetBioGroup("genotype"));

  if (m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
    phenotype.SetMerit(cMerit(phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy())));
  } else {
    phenotype.SetMerit(cMerit(metrics->GetMerit()));
  }

  phenotype.SetLinesCopied(metrics->GetLinesCopied());
  phenotype.SetLinesExecuted(metrics->GetLinesExecuted());
  phenotype.SetGestationTime(metrics->GetGestationTime());


  // Prep the cell..
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST &&
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

    df.Write(m_world->GetStats().GetUpdate(), "Update");
    df.Write(new_organism->GetID(), "Organism ID");
    df.Write(m_world->GetPopulation().GetCell(cell_id).GetDemeID(), "Deme ID");
    df.Write(new_organism->GetFacing(), "Facing");
    df.Endl();
  }
}

// Note: cPopulation::SerialTransfer does not respect deme boundaries and only acts on a single population.
void cPopulation::SerialTransfer(int transfer_size, bool ignore_deads, cAvidaContext& ctx) 
{
  assert(transfer_size > 0);

  // If we are ignoring all dead organisms, remove them from the population.
  if (ignore_deads == true) {
    for (int i = 0; i < GetSize(); i++) {
      cPopulationCell & cell = cell_array[i];
      if (cell.IsOccupied() && cell.GetOrganism()->GetTestFitness(m_world->GetDefaultContext()) == 0.0) {
        KillOrganism(cell, ctx); 
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
    KillOrganism(cell_array[transfer_pool[j]], ctx); 
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

void cPopulation::PrintHostPhenotypeData(const cString& filename)
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
    for (int j = 0; j < phenotype.GetLastHostTaskCount().GetSize(); j++) {
      if (phenotype.GetLastHostTaskCount()[j] > 0) id += (1 << j);
      if (phenotype.GetLastHostTaskCount()[j] > 0) average_num_tasks += 1.0;
      key += cStringUtil::Stringf("%i-", phenotype.GetLastHostTaskCount()[j]);
      total_tasks += phenotype.GetLastHostTaskCount()[j];
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
    for (int j = 0; j < phenotype.GetLastHostTaskCount().GetSize(); j++) {
      if (phenotype.GetLastHostTaskCount()[j] == 0) continue;
      double fraction = static_cast<double>(phenotype.GetLastHostTaskCount()[j]) / static_cast<double>(total_tasks);
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

void cPopulation::PrintParasitePhenotypeData(const cString& filename)
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
    for (int j = 0; j < phenotype.GetLastParasiteTaskCount().GetSize(); j++) {
      if (phenotype.GetLastParasiteTaskCount()[j] > 0) id += (1 << j);
      if (phenotype.GetLastParasiteTaskCount()[j] > 0) average_num_tasks += 1.0;
      key += cStringUtil::Stringf("%i-", phenotype.GetLastParasiteTaskCount()[j]);
      total_tasks += phenotype.GetLastParasiteTaskCount()[j];
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
    for (int j = 0; j < phenotype.GetLastParasiteTaskCount().GetSize(); j++) {
      if (phenotype.GetLastParasiteTaskCount()[j] == 0) continue;
      double fraction = static_cast<double>(phenotype.GetLastParasiteTaskCount()[j]) / static_cast<double>(total_tasks);
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


void cPopulation::AddBeginSleep(int cellID, int start_time) {
  sleep_log[cellID].Add(make_pair(start_time,-1));
}

void cPopulation::AddEndSleep(int cellID, int end_time) {
  pair<int,int> p = sleep_log[cellID][sleep_log[cellID].Size()-1];
  sleep_log[cellID].RemoveAt(sleep_log[cellID].Size()-1);
  sleep_log[cellID].Add(make_pair(p.first, end_time));
}

// Starts a new trial for each organism in the population
void cPopulation::NewTrial(cAvidaContext& ctx)
{
  for (int i=0; i< GetSize(); i++) {
    cPopulationCell& cell = GetCell(i);
    if (cell.IsOccupied()) {
      cPhenotype & p =  cell.GetOrganism()->GetPhenotype();

      // Don't continue if the time used was zero
      if (p.GetTrialTimeUsed() != 0) {
        // Correct gestation time for speculative execution
        p.SetTrialTimeUsed(p.GetTrialTimeUsed() - cell.GetSpeculativeState());
        p.SetTimeUsed(p.GetTimeUsed() - cell.GetSpeculativeState());

        cell.GetOrganism()->NewTrial();
        cell.GetOrganism()->GetHardware().Reset(ctx);

        cell.SetSpeculativeState(0);
      }
    }
  }

  //Recalculate the stats immediately, so that if they are printed before a new update
  //is processed, they accurately reflect this trial only...
  cStats& stats = m_world->GetStats();
  stats.ProcessUpdate();
  ProcessPostUpdate(ctx);
}

/*
 CompeteOrganisms

 parents_survive => for any organism represented by >=1 child, the first created is the parent (has no mutations)
 dynamic_scaling => rescale the time interval such that the geometric mean of the highest fitness versus lower fitnesses
 equals a time of 1 unit
 */

void cPopulation::CompeteOrganisms(cAvidaContext& ctx, int competition_type, int parents_survive)
{
  NewTrial(ctx);

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
  for (int i = 0; i < num_cells; i++) {
    if (GetCell(i).IsOccupied()) {
      cPhenotype& p = GetCell(i).GetOrganism()->GetPhenotype();
      // We trigger a lot of asserts if the copied size is zero...
      p.SetLinesCopied(p.GetGenomeLength());

      if ( (num_trials != -1) && (num_trials != p.GetTrialFitnesses().GetSize()) ) {
        cout << "The number of trials is not the same for every organism in the population.\n";
        cout << "You need to remove all normal ways of replicating for CompeteOrganisms to work correctly.\n";
        exit(1);
      }

      num_trials = p.GetTrialFitnesses().GetSize();
    }
  }

  // If there weren't any trials then end here (but call new trial so things are set up for the next iteration)
  if (num_trials == 0) return;

  if (m_world->GetVerbosity() > VERBOSE_SILENT) cout << "==Compete Organisms==" << endl;

  tArray<double> min_trial_fitnesses(num_trials);
  tArray<double> max_trial_fitnesses(num_trials);
  tArray<double> avg_trial_fitnesses(num_trials);
  avg_trial_fitnesses.SetAll(0);

  bool init = false;
  // What is the min and max fitness in each trial
  for (int i = 0; i < num_cells; i++) {
    if (GetCell(i).IsOccupied()) {
      num_competed_orgs++;
      cPhenotype& p = GetCell(i).GetOrganism()->GetPhenotype();
      tArray<double> trial_fitnesses = p.GetTrialFitnesses();
      for (int t=0; t < num_trials; t++) {
        if ((!init) || (min_trial_fitnesses[t] > trial_fitnesses[t])) {
          min_trial_fitnesses[t] = trial_fitnesses[t];
        }
        if ((!init) || (max_trial_fitnesses[t] < trial_fitnesses[t])) {
          max_trial_fitnesses[t] = trial_fitnesses[t];
        }
        avg_trial_fitnesses[t] += trial_fitnesses[t];
      }
      init = true;
    }
  }

  //divide averages for each trial
  for (int t=0; t < num_trials; t++) {
    avg_trial_fitnesses[t] /= num_competed_orgs;
  }

  if (m_world->GetVerbosity() > VERBOSE_SILENT) {
    if (min_trial_fitnesses.GetSize() > 1) {
      for (int t=0; t < min_trial_fitnesses.GetSize(); t++) {
        cout << "Trial #" << t << " Min Fitness = " << min_trial_fitnesses[t] << ", Avg fitness = " << avg_trial_fitnesses[t] << " Max Fitness = " << max_trial_fitnesses[t] << endl;
      }
    }
  }

  bool using_trials = true;
  for (int i = 0; i < num_cells; i++) {
    if (GetCell(i).IsOccupied()) {
      double fitness = 0.0;
      cPhenotype& p = GetCell(i).GetOrganism()->GetPhenotype();
      //Don't need to reset trial_fitnesses because we will call cPhenotype::OffspringReset on the entire pop
      tArray<double> trial_fitnesses = p.GetTrialFitnesses();

      //If there are no trial fitnesses...use the actual fitness.
      if (trial_fitnesses.GetSize() == 0) {
        using_trials = false;
        trial_fitnesses.Push(p.GetFitness());
      }
      switch (competition_type) {
          //Geometric Mean
        case 0:
        case 3:
        case 4:
          //Treat as logs to avoid overflow when multiplying very large fitnesses
          fitness = 0;
          for (int t=0; t < trial_fitnesses.GetSize(); t++) {
            fitness += log(trial_fitnesses[t]);
          }
          fitness /= (double)trial_fitnesses.GetSize();
          fitness = exp( fitness );
          break;

          //Product
        case 5:
          //Treat as logs to avoid overflow when multiplying very large fitnesses
          fitness = 0;
          for (int t=0; t < trial_fitnesses.GetSize(); t++) {
            fitness += log(trial_fitnesses[t]);
          }
          fitness = exp( fitness );
          break;

          //Geometric Mean of normalized values
        case 1:
          fitness = 1.0;
          for (int t=0; t < trial_fitnesses.GetSize(); t++) {
            fitness*=trial_fitnesses[t] / max_trial_fitnesses[t];
          }
          fitness = exp( (1.0/((double)trial_fitnesses.GetSize())) * log(fitness) );
          break;

          //Arithmetic Mean
        case 2:
          fitness = 0;
          for (int t=0; t < trial_fitnesses.GetSize(); t++) {
            fitness+=trial_fitnesses[t];
          }
          fitness /= (double)trial_fitnesses.GetSize();
          break;

        default:
          m_world->GetDriver().RaiseFatalException(1, "Unknown CompeteOrganisms method");
      }
      if (m_world->GetVerbosity() >= VERBOSE_DETAILS) {
        cout << "Trial fitness in cell " << i << " = " << fitness << endl;
      }
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
        p.TrialDivideReset( GetCell(i).GetOrganism()->GetGenome().GetSequence() );
      }
      else //trials not used
      {
        //TrialReset has never been called so we need the entire routine to make "last" of "cur" stats.
        p.DivideReset( GetCell(i).GetOrganism()->GetGenome().GetSequence() );
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

    cOrganism* organism = GetCell(from_cell_id).GetOrganism();
    organism->OffspringGenome() = organism->GetGenome();
    if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Injecting Offspring " << from_cell_id << " to " << to_cell_id << endl;
    CompeteOrganisms_ConstructOffspring(to_cell_id, *organism);

    is_init[to_cell_id] = true;
  }

  if (!parents_survive)
  {
    // Now create children from remaining cells into themselves
    for (int cell_id = 0; cell_id < num_cells; cell_id++) {
      if (!is_init[cell_id])
      {
        cOrganism* organism = GetCell(cell_id).GetOrganism();
        organism->OffspringGenome() = organism->GetGenome();
        if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Re-injecting Self " << cell_id << " to " << cell_id << endl;
        CompeteOrganisms_ConstructOffspring(cell_id, *organism);
      }
    }
  }


  if (m_world->GetVerbosity() > VERBOSE_SILENT)
  {
    cout << "Competed: Min fitness = " << lowest_fitness << ", Avg fitness = " << average_fitness << " Max fitness = " << highest_fitness << endl;
    cout << "Copied  : Min fitness = " << lowest_fitness_copied << ", Avg fitness = " << average_fitness_copied << ", Max fitness = " << highest_fitness_copied << endl;
    cout << "Copied  : Different organisms = " << different_orgs_copied << endl;
  }

  // copy stats to cStats, so that these can be remembered and printed
  m_world->GetStats().SetCompetitionTrialFitnesses(avg_trial_fitnesses);
  m_world->GetStats().SetCompetitionFitnesses(average_fitness, lowest_fitness, highest_fitness, average_fitness_copied, lowest_fitness_copied, highest_fitness_copied);
  m_world->GetStats().SetCompetitionOrgsReplicated(different_orgs_copied);

  NewTrial(ctx);
}


/* This routine is designed to change values in the resource count in the
 middle of a run.  This is designed to work with cActionSetGradient Count */
//JW

void cPopulation::UpdateGradientCount(const int Verbosity, cWorld* world, const cString res_name)
{
    const cResourceLib & resource_lib = environment.GetResourceLib();
    int global_res_index = -1;
    
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      cResource * res = resource_lib.GetResource(i);
      
      if (!res->GetDemeResource()) global_res_index++;
      
      if (res->GetName() == res_name) {
      const double decay = 1.0 - res->GetOutflow();
      resource_count.Setup(world, global_res_index, res->GetName(), res->GetInitial(),
                           res->GetInflow(), decay,
                           res->GetGeometry(), res->GetXDiffuse(),
                           res->GetXGravity(), res->GetYDiffuse(),
                           res->GetYGravity(), res->GetInflowX1(),
                           res->GetInflowX2(), res->GetInflowY1(),
                           res->GetInflowY2(), res->GetOutflowX1(),
                           res->GetOutflowX2(), res->GetOutflowY1(),
                           res->GetOutflowY2(), res->GetCellListPtr(),
                           res->GetCellIdListPtr(), Verbosity,
                           res->GetDynamicResource(), res->GetPeaks(), 
                           res->GetMinHeight(), res->GetMinRadius(), res->GetRadiusRange(),
                           res->GetAh(), res->GetAr(),
                           res->GetAcx(), res->GetAcy(),
                           res->GetHStepscale(), res->GetRStepscale(),
                           res->GetCStepscaleX(), res->GetCStepscaleY(),
                           res->GetHStep(), res->GetRStep(),
                           res->GetCStepX(), res->GetCStepY(),
                           res->GetUpdateDynamic(), res->GetPeakX(), res->GetPeakY(),
                           res->GetHeight(), res->GetSpread(), res->GetPlateau(), res->GetDecay(), 
                           res->GetMaxX(), res->GetMinX(), res->GetMaxY(), res->GetMinY(), res->GetAscaler(), res->GetUpdateStep(),
                           res->GetHalo(), res->GetHaloInnerRadius(), res->GetHaloWidth(),
                           res->GetHaloAnchorX(), res->GetHaloAnchorY(), res->GetMoveSpeed(),
                           res->GetPlateauInflow(), res->GetPlateauOutflow(), 
                           res->GetIsPlateauCommon(), res->GetFloor(), res->GetHabitat(), 
                           res->GetMinSize(), res->GetMaxSize(), res->GetConfig(), res->GetCount(), res->GetResistance(), res->GetGradient()
                           ); 
      } 
   }
}


void cPopulation::UpdateResourceCount(const int Verbosity, cWorld* world) {                     
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  int deme_res_index = -1;
  int num_deme_res = 0;

  //setting size of global and deme-level resources
  for(int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (res->GetDemeResource())
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
      resource_count.Setup(world, global_res_index, res->GetName(), res->GetInitial(),
                           res->GetInflow(), decay,
                           res->GetGeometry(), res->GetXDiffuse(),
                           res->GetXGravity(), res->GetYDiffuse(),
                           res->GetYGravity(), res->GetInflowX1(),
                           res->GetInflowX2(), res->GetInflowY1(),
                           res->GetInflowY2(), res->GetOutflowX1(),
                           res->GetOutflowX2(), res->GetOutflowY1(),
                           res->GetOutflowY2(), res->GetCellListPtr(),
                           res->GetCellIdListPtr(), Verbosity,
                           res->GetDynamicResource(), res->GetPeaks(), 
                           res->GetMinHeight(), res->GetMinRadius(), res->GetRadiusRange(),
                           res->GetAh(), res->GetAr(),
                           res->GetAcx(), res->GetAcy(),
                           res->GetHStepscale(), res->GetRStepscale(),
                           res->GetCStepscaleX(), res->GetCStepscaleY(),
                           res->GetHStep(), res->GetRStep(),
                           res->GetCStepX(), res->GetCStepY(),
                           res->GetUpdateDynamic(), res->GetPeakX(), res->GetPeakY(),
                           res->GetHeight(), res->GetSpread(), res->GetPlateau(), res->GetDecay(), 
                           res->GetMaxX(), res->GetMinX(), res->GetMaxY(), res->GetMinY(), res->GetAscaler(), res->GetUpdateStep(),
                           res->GetHalo(), res->GetHaloInnerRadius(), res->GetHaloWidth(),
                           res->GetHaloAnchorX(), res->GetHaloAnchorY(), res->GetMoveSpeed(),
                           res->GetPlateauInflow(), res->GetPlateauOutflow(), 
                           res->GetIsPlateauCommon(), res->GetFloor(), res->GetHabitat(), 
                           res->GetMinSize(), res->GetMaxSize(), res->GetConfig(), res->GetCount(), res->GetResistance(), res->GetGradient()
                           ); 

    } else if (res->GetDemeResource()) {
      deme_res_index++;
      for(int j = 0; j < GetNumDemes(); j++) {
        GetDeme(j).SetupDemeRes(deme_res_index, res, Verbosity, world);                                                
        // could add deme resources to global resource stats here
      }
    } else {
      cerr<< "ERROR: Resource \"" << res->GetName() <<"\"is not a global or deme resource.  Exit";
      exit(1);
    }
  }

}


// Adds an organism to live org list  
void  cPopulation::AddLiveOrg(cOrganism* org)
{
  live_org_list.Push(org);
}

// Remove an organism from live org list  
void  cPopulation::RemoveLiveOrg(cOrganism* org)
{
  for (int i = 0; i < live_org_list.GetSize(); i++)
    if (live_org_list[i] == org) {
      unsigned int last = live_org_list.GetSize() - 1;
      live_org_list.Swap(i, last);
      live_org_list.Pop();
      break;
    }
}


// Adds an organism to a group
void  cPopulation::JoinGroup(cOrganism* org, int group_id)
{
  map<int,int>::iterator it;
  it=m_groups.find(group_id);
  if (it == m_groups.end()) {
    m_groups[group_id] = 0;
    tSmartArray<cOrganism*> temp;
    group_list.Set(group_id, temp);
  }
  m_groups[group_id]++;
  group_list[group_id].Push(org);
}

// Makes a new group (highest current group number +1). @JJB
void cPopulation::MakeGroup(cOrganism* org)
{
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() != 1) return;

  int highest_group;
  if (m_groups.size() > 0) {
    highest_group = m_groups.rbegin()->first;
  } else {
    highest_group = -1;
  }

  org->SetOpinion(highest_group + 1);
  JoinGroup(org, highest_group + 1);
}

// Removes an organism from a group
void  cPopulation::LeaveGroup(cOrganism* org, int group_id)
{
  map<int,int>::iterator it = m_groups.find(group_id);
  if (it != m_groups.end()) {
    m_groups[group_id]--;
    // If no restrictions on group ids,
    // removes empty groups so the number of total groups being tracked doesn't become excessive
    // (Removes the highest group even if empty, causes misstep in marching groups). @JJB
    if (m_world->GetConfig().USE_FORM_GROUPS.Get() == 1) {
      if (m_groups[group_id] <= 0) {
        m_groups.erase(group_id);
      }
    }
  }

  for (int i = 0; i < group_list[group_id].GetSize(); i++) {
    if (group_list[group_id][i] == org) {
      unsigned int last = group_list[group_id].GetSize() - 1;
      group_list[group_id].Swap(i,last);
      group_list[group_id].Pop();
      // If no restrictions, removes empty groups. @JJB
      if (m_world->GetConfig().USE_FORM_GROUPS.Get() == 1) {
        if (group_list[group_id].GetSize() <= 0) {
          group_list.Remove(group_id);
        }
      }
      break;
    }
  }
}

// Identifies the number of organisms in a group
int  cPopulation::NumberOfOrganismsInGroup(int group_id)
{
  map<int,int>::iterator it;
  it=m_groups.find(group_id);
  int num_orgs = 0;
  if (it != m_groups.end()) {
    num_orgs = m_groups[group_id];
  }
  return num_orgs;
}

// Calculates group tolerance towards immigrants @JJB
int cPopulation::CalcGroupToleranceImmigrants(int group_id)
{
  const int tolerance_max = m_world->GetConfig().MAX_TOLERANCE.Get();

  if (group_id < 0) return tolerance_max;

  int group_intolerance = 0;
  int single_member_intolerance = 0;
  for (int index = 0; index < group_list[group_id].GetSize(); index++) {
    single_member_intolerance = tolerance_max - group_list[group_id][index]->GetPhenotype().CalcToleranceImmigrants(false);
    group_intolerance += single_member_intolerance;
    if (group_intolerance >= tolerance_max) {
      group_intolerance = tolerance_max;
      break;
    }
  }
  int group_tolerance = tolerance_max - group_intolerance;
  return group_tolerance;
}

// Calculates group tolerance towards offspring (not including parent) @JJB
int cPopulation::CalcGroupToleranceOffspring(cOrganism* parent_organism)
{
  const int tolerance_max = m_world->GetConfig().MAX_TOLERANCE.Get();
  int group_id = parent_organism->GetOpinion().first;

  if ((group_id < 0) || (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 1)) return tolerance_max;

  int group_intolerance = 0;
  int single_member_intolerance = 0;
  for (int index = 0; index < group_list[group_id].GetSize(); index++) {
    // Skip the parent
    if (group_list[group_id][index] != parent_organism) {
      single_member_intolerance = tolerance_max - group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOthers(false);
      group_intolerance += single_member_intolerance;
    }
    if (group_intolerance >= tolerance_max) {
      group_intolerance = tolerance_max;
      break;
    }
  }
  int group_tolerance = tolerance_max - group_intolerance;
  return group_tolerance;
}

// Calculates the odds (out of 1) for successful immigration based on group's tolerance @JJB
double cPopulation::CalcGroupOddsImmigrants(int group_id)
{
  if (group_id < 0) return 1.0;

  const int tolerance_max = m_world->GetConfig().MAX_TOLERANCE.Get();
  int group_tolerance = CalcGroupToleranceImmigrants(group_id);
  double immigrant_odds = (double) group_tolerance / (double) tolerance_max;
  return immigrant_odds;
}

// Returns true if the org successfully passes immigration tolerance and joins the group @JJB
bool cPopulation::AttemptImmigrateGroup(int group_id, cOrganism* org)
{
  // If non-standard group, automatic success
  if (group_id < 0) {
    int opinion;
    if (org->HasOpinion()) {
      opinion = org->GetOpinion().first;
      org->LeaveGroup(opinion);
    }
    org->SetOpinion(group_id);
    opinion = org->GetOpinion().first;
    org->JoinGroup(opinion);
    return true;
  }

  // If there are no members of the target group, automatic successful immigration
  if (m_world->GetPopulation().NumberOfOrganismsInGroup(group_id) == 0) {
    int opinion;
    if (org->HasOpinion()) {
      opinion = org->GetOpinion().first;
      org->LeaveGroup(opinion);
    }
    org->SetOpinion(group_id);
    opinion = org->GetOpinion().first;
    org->JoinGroup(opinion);
    return true;
  }
  // Calculate chances based on target group tolerance of another org successfully immigrating
  else if (m_world->GetPopulation().NumberOfOrganismsInGroup(group_id) > 0) {
    double probability_immigration = CalcGroupOddsImmigrants(group_id);
    double rand = m_world->GetRandom().GetDouble();
    if (rand <= probability_immigration) {
      // Org successfully immigrates
      int opinion;
      if (org->HasOpinion()) {
        opinion = org->GetOpinion().first;
        org->LeaveGroup(opinion);
      }
      org->SetOpinion(group_id);
      opinion = org->GetOpinion().first;
      org->JoinGroup(opinion);
      return true;
    }
    // If the org fails to immigrate it stays in its current group
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

// Calculates the odds (out of 1) for the organism's offspring to be born into its parent's group @JJB
double cPopulation::CalcGroupOddsOffspring(cOrganism* parent)
{
  assert(parent->HasOpinion());

  // If non-standard group, automatic success
  if ((parent->GetOpinion().first < 0) || (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 1)) return 1.0;

  const double tolerance_max = (double) m_world->GetConfig().MAX_TOLERANCE.Get();

  double parent_tolerance = (double) parent->GetPhenotype().CalcToleranceOffspringOwn(false);
  double parent_group_tolerance = (double) CalcGroupToleranceOffspring(parent);

  const double prob_parent_allows =  parent_tolerance / tolerance_max;
  const double prob_group_allows = parent_group_tolerance / tolerance_max;

  double prob = prob_parent_allows * prob_group_allows;

  return prob;
}

// Calculates the odds (out of 1) for offspring to be born into the group @JJB
double cPopulation::CalcGroupOddsOffspring(int group_id)
{
  // If non-standard group, automatic success
  if ((group_id < 0) || (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 1)) return 1.0;

  const int tolerance_max = m_world->GetConfig().MAX_TOLERANCE.Get();

  int group_intolerance = 0;
  int single_member_intolerance = 0;
  for (int index = 0; index < group_list[group_id].GetSize(); index++) {
    single_member_intolerance = tolerance_max - group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOthers(false);
    group_intolerance += single_member_intolerance;
    if (group_intolerance >= tolerance_max) {
      group_intolerance = tolerance_max;
      break;
    }
  }

  int group_tolerance = tolerance_max - group_intolerance;
  double offspring_odds = (double) group_tolerance / (double) tolerance_max;
  return offspring_odds;
}

bool cPopulation::AttemptOffspringParentGroup(cAvidaContext& ctx, cOrganism* parent, cOrganism* offspring)
{
  // If joining a non-standard group, atomatic success
  if ((parent->GetOpinion().first < 0) || (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 1)) {
    int parent_group = parent->GetOpinion().first;
    offspring->SetOpinion(parent_group);
    JoinGroup(offspring, parent_group);
    return true;
  }

  // If using % chance of random migration
  if (m_world->GetConfig().TOLERANCE_WINDOW.Get() < 0) {
    const int parent_group = parent->GetOpinion().first;
    const double prob_immigrate = ((double) m_world->GetConfig().TOLERANCE_WINDOW.Get() * -1.0) / 100.0;
    double rand = m_world->GetRandom().GetDouble();
    if (rand <= prob_immigrate) {
      const int num_groups = m_world->GetPopulation().GetResources(ctx).GetSize();
      int target_group; 
      do {
        target_group = m_world->GetRandom().GetUInt(num_groups);
      } while (target_group == parent_group);
      offspring->SetOpinion(target_group);
      JoinGroup(offspring, target_group);
      return true;
    }
    else {
      // Put the offspring in the parent's group.
      assert(parent->HasOpinion());
      offspring->SetOpinion(parent_group);
      JoinGroup(offspring, parent_group);
      return true;
    }
  }
  
  // If using tolerance for migration
  else if (m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) {
    assert(parent->HasOpinion());
    const double tolerance_max = (double) m_world->GetConfig().MAX_TOLERANCE.Get();
    const int parent_group = parent->GetOpinion().first;
    
    // Retrieve the parent's tolerance for its offspring
    double parent_tolerance = (double) parent->GetPhenotype().CalcToleranceOffspringOwn(false);
    // Retrieve the parent group's tolerance for offspring
    double parent_group_tolerance = (double) CalcGroupToleranceOffspring(parent);
    
    // Offspring first passes parent vote, then must also pass group vote
    // offspring first attempt to join the parent group and if unsuccessful attempt to immigrate
    const double prob_parent_allows = parent_tolerance / tolerance_max;
    const double prob_group_allows = parent_group_tolerance / tolerance_max;
    double rand2 = m_world->GetRandom().GetDouble();
    double rand = m_world->GetRandom().GetDouble();
    
    bool join_parent_group = false;
    
    if (rand <= prob_parent_allows) {
      // If there is nobody else in the group, the offspring gets in
      join_parent_group = true;
      // If there are others in the group, it's their turn
      if (group_list[parent_group].GetSize() > 1) {
        if (rand2 <= prob_group_allows) {
          // Offspring successfully joins parent's group
          join_parent_group = true;                       
        }
        else join_parent_group = false;
      }
    }
    
    if (join_parent_group) {
      offspring->SetOpinion(parent_group);
      JoinGroup(offspring, parent_group);  
      // Let the parent know that its offspring was born into its group
      parent->GetPhenotype().SetBornParentGroup() = true;
      return true;
    }
    else {
      // Let the parent know its offspring was not born into its group
      parent->GetPhenotype().SetBornParentGroup() = false;
    }
    
    // If the offspring is rejected by the parent group, and there are no other groups, the offspring is doomed
    const int num_groups = m_world->GetPopulation().GetResources(ctx).GetSize();
    if (!join_parent_group && num_groups == 1) {
      return false;
    }
    
    // If the offspring is rejected by the parent group, and there are other groups, the offspring attempts to immigrate
    if (!join_parent_group && num_groups > 1) {
      // Find another group at random, which is not the parent's
      int target_group;
      do {
        target_group = m_world->GetRandom().GetUInt(num_groups);
      } while (target_group == parent_group);
      
      // If there are no members currently of the target group, offspring has 100% chance of immigrating
      if (group_list[target_group].GetSize() == 0) {
        offspring->SetOpinion(target_group);
        JoinGroup(offspring, target_group);
        return true;
      }
      else {
        // If there are group members, retrieve the target group's tolerance to immigrants
        double probability_born_target_group = CalcGroupOddsImmigrants(target_group);
        rand = m_world->GetRandom().GetDouble();
        // Calculate if the offspring successfully immigrates
        if (rand <= probability_born_target_group) {
          // Offspring joins target group
          offspring->SetOpinion(target_group);
          JoinGroup(offspring, target_group);
          return true;
        }
        else {
          // Offspring fails to immigrate and is doomed
          return false;
        }
      }
    }
  }
  return false;
}

// Calculates the average for intra-group tolerance to immigrants
double cPopulation::CalcGroupAveImmigrants(int group_id)
{
  cDoubleSum immigrant_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < group_list[group_id].GetSize(); index++) {
    single_member_tolerance = group_list[group_id][index]->GetPhenotype().CalcToleranceImmigrants(false);
    immigrant_tolerance.Add(single_member_tolerance);
  }
  double aveimmigrants = immigrant_tolerance.Average();
  return aveimmigrants;
}

// Calculates the standard deviation for group tolerance to immigrants
double cPopulation::CalcGroupSDevImmigrants(int group_id)
{
  cDoubleSum immigrant_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < group_list[group_id].GetSize(); index++) {
    single_member_tolerance = group_list[group_id][index]->GetPhenotype().CalcToleranceImmigrants(false);
    immigrant_tolerance.Add(single_member_tolerance);
  }
  double sdevimmigrants = immigrant_tolerance.StdDeviation();
  return sdevimmigrants;
}

// Calculates the average for intra-group tolerance to own offspring
double cPopulation::CalcGroupAveOwn(int group_id)
{
  cDoubleSum own_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < group_list[group_id].GetSize(); index++) {
    single_member_tolerance = group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOwn(false);
    own_tolerance.Add(single_member_tolerance);
  }
  double aveown = own_tolerance.Average();
  return aveown;
}

// Calculates the standard deviation for group tolerance to their own offspring
double cPopulation::CalcGroupSDevOwn(int group_id)
{
  cDoubleSum own_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < group_list[group_id].GetSize(); index++) {
    single_member_tolerance = group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOwn(false);
    own_tolerance.Add(single_member_tolerance);
  }
  double sdevown = own_tolerance.StdDeviation();
  return sdevown;
}

// Calculates the average for intra-group tolerance to other offspring
double cPopulation::CalcGroupAveOthers(int group_id)
{
  cDoubleSum others_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < group_list[group_id].GetSize(); index++) {
    single_member_tolerance = group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOthers(false);
    others_tolerance.Add(single_member_tolerance);
  }
  double aveothers = others_tolerance.Average();
  return aveothers;
}

// Calculates the standard deviation for group tolerance to other group offspring
double cPopulation::CalcGroupSDevOthers(int group_id)
{
  cDoubleSum others_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < group_list[group_id].GetSize(); index++) {
    single_member_tolerance = group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOthers(false);
    others_tolerance.Add(single_member_tolerance);
  }
  double sdevothers = others_tolerance.StdDeviation();
  return sdevothers;
}

/*!	Modify current level of the HGT resource.
 */
void cPopulation::AdjustHGTResource(cAvidaContext& ctx, double delta)
{
  if (m_hgt_resid != -1) {
    resource_count.Modify(ctx, m_hgt_resid, delta);
  }
}

/*! Mix all organisms in the population.

 This method rearranges the relationship between organisms and cells.  Specifically,
 we take all organisms in the population, and assign them to different randomly-selected
 cells.

 This isn't really useful in a single population run.  However, a mixing stage is a
 key component of biologically-inspired approaches to group selection (ie, Wilson's
 and Traulsen's models).

 \warning THIS METHOD CHANGES THE ORGANISM POINTERS OF CELLS.
 */
void cPopulation::MixPopulation(cAvidaContext& ctx)
{
  // Get the list of all organism pointers, including nulls:
  std::vector<cOrganism*> population(cell_array.GetSize());
  for(int i=0; i<cell_array.GetSize(); ++i) {
    population[i] = cell_array[i].GetOrganism();
  }

  // Shuffle them:
  cRandomStdAdaptor adapted_rng(m_world->GetRandom());
  std::random_shuffle(population.begin(), population.end(), adapted_rng);

  // Reset the organism pointers of all cells:
  for(int i=0; i<cell_array.GetSize(); ++i) {
    cell_array[i].RemoveOrganism(ctx);
    if (population[i] == 0) {
      AdjustSchedule(cell_array[i], cMerit(0));
    } else {
      cell_array[i].InsertOrganism(population[i], ctx); 
      AdjustSchedule(cell_array[i], cell_array[i].GetOrganism()->GetPhenotype().GetMerit());
    }
  }
}

/*
 *  cTestCPU.cc
 *  Avida
 *
 *  Called "test_cpu.cc" prior to 11/30/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#include "cTestCPU.h"

#include "avida/output/File.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cHardwareTracer.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cResource.h"
#include "cResourceCount.h"
#include "cResourceHistory.h"
#include "cResourceLib.h"
#include "cStringUtil.h"
#include "cTestCPUInterface.h"
#include "cWorld.h"
#include "tMatrix.h"

#include <iomanip>

using namespace std;
using namespace AvidaTools;


cTestCPU::cTestCPU(cAvidaContext& ctx, cWorld* world)
{
  m_world = world;
	m_use_manual_inputs = false;
  m_test_solo_res = -1;
  m_test_solo_res_lev = 0;
  InitResources(ctx);
}  

 
void cTestCPU::InitResources(cAvidaContext& ctx, int res_method, cResourceHistory* res, int update, int cpu_cycle_offset)
{  
  //FOR DEMES
  m_deme_resource_count.SetSize(0);

  m_res_method = (eTestCPUResourceMethod)res_method;
  // Make sure it's valid
  if (res_method < 0 || res_method >= RES_LAST) m_res_method = RES_INITIAL;
  
  // Setup the resources...
  m_res = res;
  m_res_cpu_cycle_offset = cpu_cycle_offset;
  m_res_update = update;

  // Adjust updates if time_spent_offset is greater than a time slice
  int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
  m_res_update += m_res_cpu_cycle_offset / ave_time_slice;
  m_res_cpu_cycle_offset %= ave_time_slice;
  assert(m_res_cpu_cycle_offset >= 0);
  
  // If they didn't send anything (usually during an avida run as opposed to analyze mode),
  // then we set up a static variable (or just point to it) that reflects the initial conditions of the run
  // from the environment file.  (JEB -- This code moved from cAnalyze::cAnalyze).
  if (m_res_method == RES_INITIAL)
  {
    // Initialize the time oriented resource list to be just the initial
    // concentrations of the resources in the environment.  This will only
    // be changed if LOAD_RESOURCES analyze command is called.  If there are
    // no resources in the environment or there is no environment, the list
    // is empty then the all resources will default to 0.0
    m_res = &m_world->GetEnvironment().GetResourceLib().GetInitialResourceLevels();
  }
  
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  assert(resource_lib.GetSize() >= 0);
  
  // Set the resource count to zero by default
  m_resource_count.SetSize(resource_lib.GetSize());
  m_faced_cell_resource_count.SetSize(resource_lib.GetSize());
  m_cell_resource_count.SetSize(resource_lib.GetSize());
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    m_resource_count.Set(ctx, i, 0.0);
    m_faced_cell_resource_count.Set(ctx, i, 0.0);
    m_cell_resource_count.Set(ctx, i, 0.0);
  }
    
  if (m_test_solo_res == -1) SetResourceUpdate(ctx, m_res_update, false);
  // Round down to the closest update to choose how to initialize resources
  else m_resource_count.Set(ctx, m_test_solo_res, m_test_solo_res_lev);
}

void cTestCPU::UpdateResources(cAvidaContext& ctx, int cpu_cycles_used)
{
  int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
  if ((m_res_method >= RES_UPDATED_DEPLETABLE) && (cpu_cycles_used % ave_time_slice == 0))
    SetResourceUpdate(ctx, m_res_update + 1, true);
}

void cTestCPU::UpdateRandomResources(cAvidaContext& ctx, int cpu_cycles_used)
{
  int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
  if ((m_res_method >= RES_UPDATED_DEPLETABLE) && (cpu_cycles_used % ave_time_slice == 0))
    SetResourceUpdate(ctx, m_res_update + 1, true);
}

inline void cTestCPU::SetResourceUpdate(cAvidaContext& ctx, int update, bool round_to_closest)
{
  // No resources defined? -- you can't do this!
  if (!m_res) return;

  m_res_update = update;
  
  m_res->GetResourceCountForUpdate(ctx, update, m_resource_count, !round_to_closest);
}

void cTestCPU::ModifyResources(cAvidaContext& ctx, const Apto::Array<double>& res_change)
{
  //We only let the testCPU modify the resources if we are using a DEPLETABLE option. @JEB
  if (m_res_method >= RES_UPDATED_DEPLETABLE) m_resource_count.Modify(ctx, res_change);
}


// NOTE: This method assumes that the organism is a fresh creation.
bool cTestCPU::ProcessGestation(cAvidaContext& ctx, cCPUTestInfo& test_info, int cur_depth)
{
  assert(test_info.org_array[cur_depth] != NULL);

  cOrganism & organism = *( test_info.org_array[cur_depth] );

  // Determine how long this organism should be tested for...
  ConstInstructionSequencePtr seq;
  seq.DynamicCastFrom(organism.UnitGenome().Representation());
  int time_allocated = m_world->GetConfig().TEST_CPU_TIME_MOD.Get() * seq->GetSize();
  time_allocated += m_res_cpu_cycle_offset; // If the resource offset has us starting at a different time, adjust @JEB

  // Prepare the inputs...
  cur_input = 0;
  cur_receive = 0;

  // Prepare the resources
  InitResources(ctx, test_info.m_res_method, test_info.m_res, test_info.m_res_update, test_info.m_res_cpu_cycle_offset);
	
	
  // This way of keeping track of time is only used to update resources...
  int time_used = m_res_cpu_cycle_offset; // Note: the offset is zero by default if no resources being used @JEB
  
  organism.GetHardware().SetTrace(test_info.GetTracer());
  while (time_used < time_allocated && organism.GetPhenotype().GetNumDivides() == 0 && !organism.IsDead())
  {
    time_used++;
    
    // @CAO Need to watch out for parasites.
    
    // Resources will be updated as if each update takes a number of cpu cycles equal to the average time slice
    UpdateResources(ctx, time_used);
    
    organism.GetHardware().SingleProcess(ctx);
  }
  
  organism.GetHardware().SetTrace(HardwareTracerPtr(NULL));

  // Print out some final info in trace...
  if (test_info.GetTracer()) test_info.GetTracer()->TraceTestCPU(time_used, time_allocated, organism);

  // For now, always return true.
  return true;
}


bool cTestCPU::TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const Genome& genome)
{
  ctx.SetTestMode();
  test_info.Clear();
  TestGenome_Body(ctx, test_info, genome, 0);
  ctx.ClearTestMode();
  
  return test_info.is_viable;
}

bool cTestCPU::TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const Genome& genome, ofstream& out_fp)
{
  ctx.SetTestMode();
  test_info.Clear();
  TestGenome_Body(ctx, test_info, genome, 0);

  ////////////////////////////////////////////////////////////////
  // IsViable() == false
  //   max_depth == 0  : (0) Parent doesn't divide
  //   max_depth == 1  : (2) Parent does divide, but child does not.
  //   max_depth >= 2  : (3) Parent and child do divide, but neither true.
  // ------------------------------------------------------------
  // IsViable() == true
  //   max_depth == 0  : (4) Parent Breed True
  //   max_depth == 1  : (5) Parent NOT Breed True, but Child Does
  //   max_depth >= 2  : (6) Multiple levels of non-breed true.
  ////////////////////////////////////////////////////////////////


  const int depth_comp = Apto::Min(test_info.max_depth, 2);
  int repro_type = ((int) test_info.is_viable) * 3 + 1 + depth_comp;
  if (test_info.is_viable == false && depth_comp == 0)  repro_type = 0;

  out_fp << test_info.is_viable << " "
	 << test_info.max_depth << " "
	 << test_info.depth_found << " "
	 << test_info.max_cycle << " "
	 << repro_type << endl;

  ctx.ClearTestMode();
  return test_info.is_viable;
}

bool cTestCPU::TestGenome_Body(cAvidaContext& ctx, cCPUTestInfo& test_info, const Genome& genome, int cur_depth)
{
  assert(cur_depth < test_info.generation_tests);

  // Input sizes can vary based on environment settings, must at least initialize
  m_use_random_inputs = test_info.GetUseRandomInputs(); // save this value in case ResetInputs is used.
  if (!test_info.GetUseManualInputs())
		m_world->GetEnvironment().SetupInputs(ctx, input_array, m_use_random_inputs);
  else
		input_array = test_info.manual_inputs;
	
  receive_array.Resize(3);
  if (test_info.GetUseRandomInputs()) {
    receive_array[0] = (15 << 24) + ctx.GetRandom().GetUInt(1 << 24);  // 00001111
    receive_array[1] = (51 << 24) + ctx.GetRandom().GetUInt(1 << 24);  // 00110011
    receive_array[2] = (85 << 24) + ctx.GetRandom().GetUInt(1 << 24);  // 01010101
  } else {
    receive_array[0] = 0x0f139f14;  // 00001111 00010011 10011111 00010100
    receive_array[1] = 0x33083ee5;  // 00110011 00001000 00111110 11100101
    receive_array[2] = 0x5562eb41;  // 01010101 01100010 11101011 01000001
  }
  
	if (cur_depth == 0) test_info.used_inputs = input_array;
	
  if (cur_depth > test_info.max_depth) test_info.max_depth = cur_depth;

  // Setup the organism we're working with now.
  if (test_info.org_array[cur_depth] != NULL) {
    delete test_info.org_array[cur_depth];
  }
  cOrganism* organism = new cOrganism(m_world, ctx, genome, -1, Systematics::Source(Systematics::DIVISION, "", true));
  
  // Copy the test mutation rates
  organism->MutationRates().Copy(test_info.MutationRates());
  
  test_info.org_array[cur_depth] = organism;
  organism->SetOrgInterface(ctx, new cTestCPUInterface(this, test_info, cur_depth));
  ConstInstructionSequencePtr seq;
  seq.DynamicCastFrom(genome.Representation());
  organism->GetPhenotype().SetupInject(*seq);

  // Run the current organism.
  ProcessGestation(ctx, test_info, cur_depth);

  
  // Notify the organism that it has died to allow for various cleanup methods to run
  organism->NotifyDeath(ctx);
  

  // Must be able to divide twice in order to form a successful colony,
  // assuming the CPU doesn't get reset on divides.
  //
  // The possibilities after this gestation cycle are:
  //  1: It did not copy at all => Exit this level.
  //  2: It copied true => Check next gestation cycle, or set is_viable.
  //  3: Its copy looks like an ancestor => copy true.
  //  4: It copied false => we must check the child.
  
  // Case 1:  ////////////////////////////////////
  if (organism->GetPhenotype().GetNumDivides() == 0)  return false;

  // Case 2:  ////////////////////////////////////
  if (organism->GetPhenotype().CopyTrue() == true) {
    test_info.depth_found = cur_depth;
    test_info.is_viable = true;
    return true;
  }

  // Case 3:  ////////////////////////////////////
  bool is_ancestor = false;
  for (int anc_depth = 0; anc_depth < cur_depth; anc_depth++) {
    if (organism->OffspringGenome() == test_info.org_array[anc_depth]->GetGenome()){
      is_ancestor = true;
      const int cur_cycle = cur_depth - anc_depth;
      if (test_info.max_cycle < cur_cycle) test_info.max_cycle = cur_cycle;
      test_info.cycle_to = anc_depth;
    }
  }
  if (is_ancestor) {
    test_info.depth_found = cur_depth;
    test_info.is_viable = true;
    return true;
  }

  // Case 4:  ////////////////////////////////////
  // If we haven't reached maximum depth yet, check out the child.
  if (cur_depth + 1 < test_info.generation_tests) {
    // Run the offspring's genome.
    return TestGenome_Body(ctx, test_info, organism->OffspringGenome(), cur_depth + 1);
  }

  // All options have failed; just return false.
  return false;
}


void cTestCPU::PrintGenome(cAvidaContext& ctx, const Genome& genome, cString filename, int update, bool for_groups, int last_birth_cell, int last_group_id, int last_forager_type)
{
  ConstInstructionSequencePtr seq;
  seq.DynamicCastFrom(genome.Representation());
  if (filename == "") filename.Set("archive/%03d-unnamed.org", seq->GetSize());
    
  cCPUTestInfo test_info;
  TestGenome(ctx, test_info, genome);
  
  // Open the file...
  Apto::String file_path((const char*)filename);
  Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), file_path);
  
  // Print the useful info at the top...
  df->WriteTimeStamp();  
  cString c("");
  
  df->WriteComment(c.Set("Filename........: %s", static_cast<const char*>(filename)));
  
  if (update >= 0) df->WriteComment(c.Set("Update Output...: %d", update));
  else df->WriteComment("Update Output...: N/A");
  
  df->WriteComment(c.Set("Is Viable.......: %d", test_info.IsViable()));
  df->WriteComment(c.Set("Repro Cycle Size: %d", test_info.GetMaxCycle()));
  df->WriteComment(c.Set("Depth to Viable.: %d", test_info.GetDepthFound()));
  
  df->WriteComment("");
  
  const int num_levels = test_info.GetMaxDepth() + 1;
  for (int j = 0; j < num_levels; j++) {
    df->WriteComment(c.Set("Generation: %d", j));

    cOrganism* organism = test_info.GetTestOrganism(j);
    assert(organism != NULL);
    cPhenotype& phenotype = organism->GetPhenotype();
    
    df->WriteComment(c.Set("Merit...........: %f", phenotype.GetMerit().GetDouble()));
    df->WriteComment(c.Set("Gestation Time..: %d", phenotype.GetGestationTime()));
    df->WriteComment(c.Set("Fitness.........: %f", phenotype.GetFitness()));
    df->WriteComment(c.Set("Errors..........: %d", phenotype.GetLastNumErrors()));
    df->WriteComment(c.Set("Genome Size.....: %d", seq->GetSize()));
    df->WriteComment(c.Set("Copied Size.....: %d", phenotype.GetCopiedSize()));
    df->WriteComment(c.Set("Executed Size...: %d", phenotype.GetExecutedSize()));
    
    if (for_groups) {
      df->WriteComment(c.Set("Last Birth Group ID........: %d", last_group_id));
      df->WriteComment(c.Set("Last Birth Forager Type....: %d", last_forager_type));   
      df->WriteComment(c.Set("Last Birth Cell............: %d", last_birth_cell));               
    }
    
    if (phenotype.GetNumDivides() == 0)
      df->WriteComment("Offspring.......: NONE");
    else if (phenotype.CopyTrue())
      df->WriteComment("Offspring.......: SELF");
    else if (test_info.GetCycleTo() != -1)
      df->WriteComment(c.Set("Offspring.......: %d", test_info.GetCycleTo()));
    else
      df->WriteComment(c.Set("Offspring.......: %d", j + 1));
    
    df->WriteComment("");
  }
  
  df->WriteComment("Tasks Performed:");
  
  const cEnvironment& env = m_world->GetEnvironment();
  const Apto::Array<int>& task_count = test_info.GetTestPhenotype().GetLastTaskCount();
  const Apto::Array<double>& task_qual = test_info.GetTestPhenotype().GetLastTaskQuality();
  for (int i = 0; i < task_count.GetSize(); i++) {
    df->WriteComment(c.Set("%s %d (%f)", static_cast<const char*>(env.GetTask(i).GetName()),
                          task_count[i], task_qual[i]));
  }
  
  // if resource bins are being used, print relevant information
  if(m_world->GetConfig().USE_RESOURCE_BINS.Get())  {
  	df->WriteComment("Tasks Performed Using Internal Resources:");
  	
  	const Apto::Array<int>& internal_task_count = test_info.GetTestPhenotype().GetLastInternalTaskCount();
  	const Apto::Array<double>& internal_task_qual = test_info.GetTestPhenotype().GetLastInternalTaskQuality();
  	
  	for (int i = 0; i < task_count.GetSize(); i++) {
  		df->WriteComment(c.Set("%s %d (%f)", static_cast<const char*>(env.GetTask(i).GetName()),
  		                      internal_task_count[i], internal_task_qual[i]));
  	}
  	
  	const Apto::Array<double>& rbins_total = test_info.GetTestPhenotype().GetLastRBinsTotal();
  	const Apto::Array<double>& rbins_avail = test_info.GetTestPhenotype().GetLastRBinsAvail();
  	
  	df->WriteComment(        "Resources Collected: Name\t\tTotal\t\tAvailable");
  	for (int i = 0; i < rbins_total.GetSize(); i++) {
  		df->WriteComment(c.Set("                %d : %s\t\t%f\t\t%f\t\t", i,
  		                      static_cast<const char*>(env.GetResourceLib().GetResource(i)->GetName()),
  		                      rbins_total[i], rbins_avail[i]));
  	}
  }

  df->Endl();
  
  // Display the genome
  test_info.GetTestOrganism()->GetHardware().GetInstSet().SaveInstructionSequence(df->OFStream(), *seq);  
}


void cTestCPU::ResetInputs(cAvidaContext& ctx) 
{ 
	if (!m_use_manual_inputs)
		m_world->GetEnvironment().SetupInputs(ctx, input_array, m_use_random_inputs);
}


/*
 *  cTestCPU.cc
 *  Avida
 *
 *  Called "test_cpu.cc" prior to 11/30/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#include "cTestCPU.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cEnvironment.h"
#include "functions.h"
#include "cGenomeUtil.h"
#include "cGenotype.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cHardwareStatusPrinter.h"
#include "cInjectGenotype.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cTestCPUInterface.h"
#include "cResourceCount.h"
#include "cResourceLib.h"
#include "cResource.h"
#include "cStringUtil.h"
#include "cWorld.h"
#include "tMatrix.h"

#include <iomanip>

using namespace std;

std::vector<std::pair<int, std::vector<double> > > * cTestCPU::s_resources = NULL;

cTestCPU::cTestCPU(cWorld* world)
{
  m_world = world;
	m_use_manual_inputs = false;
  InitResources();
}  

 
void cTestCPU::InitResources(int res_method, std::vector<std::pair<int, std::vector<double> > > * res, int update, int cpu_cycle_offset)
{  
  //FOR DEMES
  m_deme_resource_count.SetSize(0);

  m_res_method = (eTestCPUResourceMethod)res_method;
  // Make sure it's valid
  if(res_method < 0 ||  res_method >= RES_LAST) {
    m_res_method = RES_INITIAL;
  }
  
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
    if (!s_resources)
    {
      s_resources = new std::vector<std::pair<int, std::vector<double> > >;
      const cResourceLib &resource_lib = m_world->GetEnvironment().GetResourceLib();
      if(resource_lib.GetSize() > 0) 
      {
        vector<double> r;
        for(int i=0; i<resource_lib.GetSize(); i++) 
        {
          cResource *resource = resource_lib.GetResource(i);
          assert(resource);
          r.push_back(resource->GetInitial());
        }
        s_resources->push_back(make_pair(0, r));
      }
    }
    m_res = s_resources;
    assert(m_res != NULL);
  }
  
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  assert(resource_lib.GetSize() >= 0);
  
  // Set the resource count to zero by default
  m_resource_count.SetSize(resource_lib.GetSize());
  for(int i=0; i<resource_lib.GetSize(); i++) 
  {     
     m_resource_count.Set(i, 0.0);
  }
    
  SetResourceUpdate(m_res_update, true);
  // Round down to the closest update to choose how to initialize resources
}

void cTestCPU::UpdateResources(int cpu_cycles_used)
{
    int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
    if ((m_res_method >= RES_UPDATED_DEPLETABLE) && (cpu_cycles_used % ave_time_slice == 0))
    {
      SetResourceUpdate(m_res_update+1);
    }
}

void cTestCPU::SetResourceUpdate(int update, bool round_to_closest)
{
  // No resources defined? -- you can't do this!
  if (!m_res) return;

  m_res_update = update;

  int which = -1;
  if (round_to_closest)
  {
    // Assuming resource vector is sorted by update, front to back
    
    /*
    if(update <= (*m_res)[0].first) {
      which = 0;
    } else if(update >= (*m_res).back().first) {
      which = m_res->size() - 1;
    } else {
      // Find the update that is closest to the born update
      for(unsigned int i=0; i<(*m_res).size()-1; i++) {
        if(update >= (*m_res)[i+1].first) { continue; }
        if(update - (*m_res)[i].first <=
           (*m_res)[i+1].first - update) {
          which = i;
        } else {
          which = i + 1;
        }
        break;
      */
      // Find the update that is closest to the born update, round down instead @JEB
      which = 0;
      while ( which < (int)m_res->size() )
      {
        if ( (*m_res)[which].first > update ) break;
        which++;
      }
      if (which > 0) which--;
 // }
    assert(which >= 0);
  }
  else // Only find exact update matches
  {
    for(unsigned int i=0; i<m_res->size(); i++)
    {
      if (update == (*m_res)[i].first)
      {
        which = i;
        break;
      }
    }
    if (which < 0) return; // Not found (do nothing)
  }
  
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  for(int i=0; i<resource_lib.GetSize(); i++) 
  {
    if(i >= (int)(*m_res)[which].second.size()) {
      m_resource_count.Set(i, 0.0);
    } else {
      m_resource_count.Set(i, (*m_res)[which].second[i]);
    }
  }
}

void cTestCPU::ModifyResources(const tArray<double>& res_change)
{
  //We only let the testCPU modify the resources if we are using a DEPLETABLE option. @JEB
  if (m_res_method >= RES_UPDATED_DEPLETABLE) m_resource_count.Modify(res_change);
}


// NOTE: This method assumes that the organism is a fresh creation.
bool cTestCPU::ProcessGestation(cAvidaContext& ctx, cCPUTestInfo& test_info, int cur_depth)
{
  assert(test_info.org_array[cur_depth] != NULL);

  cOrganism & organism = *( test_info.org_array[cur_depth] );

  // Determine how long this organism should be tested for...
  int time_allocated = m_world->GetConfig().TEST_CPU_TIME_MOD.Get() * organism.GetGenome().GetSize();
  time_allocated += m_res_cpu_cycle_offset; // If the resource offset has us starting at a different time, adjust @JEB

  // Prepare the inputs...
  cur_input = 0;
  cur_receive = 0;

  // Prepare the resources
  InitResources(test_info.m_res_method, test_info.m_res, test_info.m_res_update, test_info.m_res_cpu_cycle_offset);

  // Determine if we're tracing and what we need to print.
  cHardwareTracer* tracer = test_info.GetTraceExecution() ? (test_info.GetTracer()) : NULL;
  std::ostream * tracerStream = NULL;
  if (tracer != NULL) tracerStream = tracer->GetStream();

  // This way of keeping track of time is only used to update resources...
  int time_used = m_res_cpu_cycle_offset; // Note: the offset is zero by default if no resources being used @JEB
  
  organism.GetHardware().SetTrace(tracer);
  while (time_used < time_allocated && organism.GetHardware().GetMemory().GetSize() &&
         organism.GetPhenotype().GetNumDivides() == 0 && !organism.IsDead())
  {
    time_used++;
    
    // @CAO Need to watch out for parasites.
    
    // Resources will be updated as if each update takes a number of cpu cycles equal to the average time slice
    UpdateResources(time_used);
    
    // Add extra info to trace files so that we can watch resource changes. 
    // This is a clumsy way to insert it in the trace file, but works for my purposes @JEB
    if ( (m_res_method >= RES_UPDATED_DEPLETABLE) && (tracerStream != NULL) ) 
    {
      const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
      assert(resource_lib.GetSize() >= 0);
      *tracerStream << "Resources:";
      // Print out resources
      for(int i=0; i<resource_lib.GetSize(); i++) 
      {     
         *tracerStream << " " << m_resource_count.Get(i);
      }
      *tracerStream << endl;
     }
     
     organism.GetHardware().SingleProcess(ctx);
  }
  
  
  // Output final resource information @JEB
  if ( (m_res_method >= RES_UPDATED_DEPLETABLE) && (tracerStream != NULL) ) 
  {
    const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
    assert(resource_lib.GetSize() >= 0);
    *tracerStream << "Resources:";
    // Print out resources
    for(int i=0; i<resource_lib.GetSize(); i++) 
    {     
       *tracerStream << " " << m_resource_count.Get(i);
    }
    *tracerStream << endl;
   }

  
  organism.GetHardware().SetTrace(NULL);

  // Print out some final info in trace...
  if (tracer != NULL) tracer->TraceTestCPU(time_used, time_allocated, organism);

  // For now, always return true.
  return true;
}


bool cTestCPU::TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const cGenome& genome)
{
  test_info.Clear();
  TestGenome_Body(ctx, test_info, genome, 0);

  return test_info.is_viable;
}

bool cTestCPU::TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const cGenome& genome,
                          ofstream& out_fp)
{
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


  const int depth_comp = Min(test_info.max_depth, 2);
  int repro_type = ((int) test_info.is_viable) * 3 + 1 + depth_comp;
  if (test_info.is_viable == false && depth_comp == 0)  repro_type = 0;

  out_fp << test_info.is_viable << " "
	 << test_info.max_depth << " "
	 << test_info.depth_found << " "
	 << test_info.max_cycle << " "
	 << repro_type << endl;

  return test_info.is_viable;
}

bool cTestCPU::TestGenome_Body(cAvidaContext& ctx, cCPUTestInfo& test_info,
                               const cGenome& genome, int cur_depth)
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
  
	if (cur_depth == 0)
		test_info.used_inputs = input_array;
	
  if (cur_depth > test_info.max_depth) test_info.max_depth = cur_depth;

  // Setup the organism we're working with now.
  if (test_info.org_array[cur_depth] != NULL) {
    delete test_info.org_array[cur_depth];
  }
  test_info.org_array[cur_depth] = new cOrganism(m_world, ctx, genome);
  cOrganism & organism = *( test_info.org_array[cur_depth] );
  organism.SetOrgInterface(new cTestCPUInterface(this));
  organism.GetPhenotype().SetupInject(genome);

  // Run the current organism.
  ProcessGestation(ctx, test_info, cur_depth);

  // Must be able to divide twice in order to form a successful colony,
  // assuming the CPU doesn't get reset on divides.
  //
  // The possibilities after this gestation cycle are:
  //  1: It did not copy at all => Exit this level.
  //  2: It copied true => Check next gestation cycle, or set is_viable.
  //  3: Its copy looks like an ancestor => copy true.
  //  4: It copied false => we must check the child.

  // Case 1:  ////////////////////////////////////
  if (organism.GetPhenotype().GetNumDivides() == 0)  return false;

  // Case 2:  ////////////////////////////////////
  if (organism.GetPhenotype().CopyTrue() == true) {
    test_info.depth_found = cur_depth;
    test_info.is_viable = true;
    return true;
  }

  // Case 3:  ////////////////////////////////////
  bool is_ancestor = false;
  for (int anc_depth = 0; anc_depth < cur_depth; anc_depth++) {
    if (organism.ChildGenome() == test_info.org_array[anc_depth]->GetGenome()){
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
  if (cur_depth+1 < test_info.generation_tests) {
    // Run the child's genome.
    return TestGenome_Body(ctx, test_info, organism.ChildGenome(), cur_depth+1);
  }

  // All options have failed; just return false.
  return false;
}


void cTestCPU::PrintGenome(cAvidaContext& ctx, const cGenome& genome, cString filename,
                           cGenotype* genotype, int update)
{
  if (filename == "") filename.Set("archive/%03d-unnamed.org", genome.GetSize());
    
  cCPUTestInfo test_info;
  TestGenome(ctx, test_info, genome);
  
  // Open the file...
  cDataFile& df = m_world->GetDataFile(filename);
  
  // Print the useful info at the top...
  df.WriteTimeStamp();  
  cString c("");
  
  df.WriteComment(c.Set("Filename........: %s", static_cast<const char*>(filename)));
  
  if (update >= 0) df.WriteComment(c.Set("Update Output...: %d", update));
  else df.WriteComment("Update Output...: N/A");
  
  df.WriteComment(c.Set("Is Viable.......: %d", test_info.IsViable()));
  df.WriteComment(c.Set("Repro Cycle Size: %d", test_info.GetMaxCycle()));
  df.WriteComment(c.Set("Depth to Viable.: %d", test_info.GetDepthFound()));
  
  if (genotype != NULL) {
    df.WriteComment(c.Set("Update Created..: %d", genotype->GetUpdateBorn()));
    df.WriteComment(c.Set("Genotype ID.....: %d", genotype->GetID()));
    df.WriteComment(c.Set("Parent Gen ID...: %d", genotype->GetParentID()));
    df.WriteComment(c.Set("Tree Depth......: %d", genotype->GetDepth()));
    df.WriteComment(c.Set("Parent Distance.: %d", genotype->GetParentDistance()));
  }

  df.WriteComment("");
  
  const int num_levels = test_info.GetMaxDepth() + 1;
  for (int j = 0; j < num_levels; j++) {
    df.WriteComment(c.Set("Generation: %d", j));

    cOrganism* organism = test_info.GetTestOrganism(j);
    assert(organism != NULL);
    cPhenotype& phenotype = organism->GetPhenotype();
    
    df.WriteComment(c.Set("Merit...........: %f", phenotype.GetMerit().GetDouble()));
    df.WriteComment(c.Set("Gestation Time..: %d", phenotype.GetGestationTime()));
    df.WriteComment(c.Set("Fitness.........: %f", phenotype.GetFitness()));
    df.WriteComment(c.Set("Errors..........: %d", phenotype.GetLastNumErrors()));
    df.WriteComment(c.Set("Genome Size.....: %d", organism->GetGenome().GetSize()));
    df.WriteComment(c.Set("Copied Size.....: %d", phenotype.GetCopiedSize()));
    df.WriteComment(c.Set("Executed Size...: %d", phenotype.GetExecutedSize()));
    
    if (phenotype.GetNumDivides() == 0)
      df.WriteComment("Offspring.......: NONE");
    else if (phenotype.CopyTrue())
      df.WriteComment("Offspring.......: SELF");
    else if (test_info.GetCycleTo() != -1)
      df.WriteComment(c.Set("Offspring.......: %d", test_info.GetCycleTo()));
    else
      df.WriteComment(c.Set("Offspring.......: %d", j + 1));
    
    df.WriteComment("");
  }
  
  df.WriteComment("Tasks Performed:");
  
  const cEnvironment& env = m_world->GetEnvironment();
  const tArray<int>& task_count = test_info.GetTestPhenotype().GetLastTaskCount();
  const tArray<double>& task_qual = test_info.GetTestPhenotype().GetLastTaskQuality();
  for (int i = 0; i < task_count.GetSize(); i++) {
    df.WriteComment(c.Set("%s %d (%f)", static_cast<const char*>(env.GetTask(i).GetName()),
                          task_count[i], task_qual[i]));
  }

  df.Endl();
  
  // Display the genome
  cGenomeUtil::SaveGenome(df.GetOFStream(), test_info.GetTestOrganism()->GetHardware().GetInstSet(), genome);
  
  m_world->GetDataFileManager().Remove(filename);
}


void cTestCPU::PrintInjectGenome(cAvidaContext& ctx, cInjectGenotype* inject_genotype,
                                 const cGenome& genome, cString filename, int update)
{
  if (filename == "") filename.Set("p%03d-unnamed", genome.GetSize());
  
  // Build the test info for printing.
  cCPUTestInfo test_info;
  TestGenome(ctx, test_info, genome);
  
  // Open the file...
  ofstream& fp = m_world->GetDataFileOFStream(filename);
  
  // @CAO Fix!!!!!!
  if( fp.good() == false ) {
    cerr << "Unable to open output file '" <<  filename << "'" << endl;
    return;
  }
  
  // Print the useful info at the top...
  
  fp << "# Filename........: " << filename << endl;
  
  if (update >= 0) fp << "# Update Output...: " << update << endl;
  else fp << "# Update Output...: N/A" << endl;
  
  
  if (inject_genotype != NULL) {
    fp << "# Update Created..: " << inject_genotype->GetUpdateBorn() << endl;
    fp << "# Genotype ID.....: " << inject_genotype->GetID() << endl;
    fp << "# Parent Gen ID...: " << inject_genotype->GetParentID() << endl;
    fp << "# Tree Depth......: " << inject_genotype->GetDepth() << endl;
  }
  fp << endl;
  
  // Display the genome
  cGenomeUtil::SaveGenome(fp, test_info.GetTestOrganism()->GetHardware().GetInstSet(), genome);
  
  m_world->GetDataFileManager().Remove(filename);
}


void cTestCPU::ResetInputs(cAvidaContext& ctx) 
{ 
	if (!m_use_manual_inputs)
		m_world->GetEnvironment().SetupInputs(ctx, input_array, m_use_random_inputs);
}


/*
 *  cTestCPU.cc
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#include "cTestCPU.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cEnvironment.h"
#include "functions.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cHardwareStatusPrinter.h"
#include "cInstSet.h"
#include "cInstUtil.h"
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

cTestResources::cTestResources(cWorld* world)
{
  // Setup the resources...
  const cResourceLib& resource_lib = world->GetEnvironment().GetResourceLib();
  assert(resource_lib.GetSize() >= 0);

  resource_count.SetSize(resource_lib.GetSize());
  d_emptyDoubleArray.ResizeClear(resource_lib.GetSize());
  d_resources.ResizeClear(resource_lib.GetSize());
  for(int i=0; i<resource_lib.GetSize(); i++) {
    d_emptyDoubleArray[i] = 0.0;
    d_resources[i] = 0.0;
  }

  resource_count.ResizeSpatialGrids(1, 1);

  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource* res = resource_lib.GetResource(i);
    const double decay = 1.0 - res->GetOutflow();
    resource_count.Setup(i, res->GetName(), res->GetInitial(), 
                           res->GetInflow(), decay,
                           res->GetGeometry(), res->GetXDiffuse(),
                           res->GetXGravity(), res->GetYDiffuse(), 
                           res->GetYGravity(), res->GetInflowX1(), 
                           res->GetInflowX2(), res->GetInflowY1(), 
                           res->GetInflowY2(), res->GetOutflowX1(), 
                           res->GetOutflowX2(), res->GetOutflowY1(), 
                           res->GetOutflowY2() );
  }
}


void cTestCPU::SetupResourceArray(const tArray<double> &resources)
{
  if (!m_localres) m_res = new cTestResources(*m_res);

  for(int i = 0; i < m_res->d_resources.GetSize(); i++) {
    if(i >= resources.GetSize()) {
      m_res->d_resources[i] = 0.0;
    } else {
      m_res->d_resources[i] = resources[i];
    }
  }
}

void cTestCPU::SetUseResources(bool use)
{
  if (m_res->d_useResources != use) {
    if (!m_localres) m_res = new cTestResources(*m_res);
    m_res->d_useResources = use;
  }
}


// NOTE: This method assumes that the organism is a fresh creation.
bool cTestCPU::ProcessGestation(cAvidaContext& ctx, cCPUTestInfo& test_info, int cur_depth)
{
  assert(test_info.org_array[cur_depth] != NULL);

  cOrganism & organism = *( test_info.org_array[cur_depth] );

  // Determine how long this organism should be tested for...
  int time_allocated = m_world->GetConfig().TEST_CPU_TIME_MOD.Get() * organism.GetGenome().GetSize();

  // Make sure this genome stands a chance...
  if (TestIntegrity(organism.GetGenome()) == false)  time_allocated = 0;

  // Prepare the inputs...
  cur_input = 0;
  cur_receive = 0;

  // Determine if we're tracing and what we need to print.
  cHardwareTracer* tracer = test_info.GetTraceExecution() ? (test_info.GetTracer()) : NULL;

  int time_used = 0;
  organism.GetHardware().SetTrace(tracer);
  while (time_used < time_allocated && organism.GetHardware().GetMemory().GetSize() &&
         organism.GetPhenotype().GetNumDivides() == 0)
  {
    time_used++;
    organism.GetHardware().SingleProcess(ctx);
    // @CAO Need to watch out for parasites.
  }
  organism.GetHardware().SetTrace(NULL);

  // Print out some final info in trace...
  if (tracer != NULL) {
    if (cHardwareTracer_TestCPU* tracer_test_cpu = dynamic_cast<cHardwareTracer_TestCPU*>(tracer)) {
      tracer_test_cpu->TraceHardware_TestCPU(
        time_used,
        time_allocated,
        organism.GetHardware().GetMemory().GetSize(),
        organism.GetHardware().GetMemory().AsString(),
        organism.ChildGenome().AsString()
      );
    }
  }

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

  if (test_info.GetUseRandomInputs() == false) {
    // We make sure that all combinations of inputs are present.  This is
    // done explicitly in the key columns... (0f, 33, and 55)
    input_array.Resize(3);
    //    input_array[0] = 0x130f149f;  // 00010011 00001111 00010100 10011111
    //    input_array[1] = 0x0833e53e;  // 00001000 00110011 11100101 00111110
    //    input_array[2] = 0x625541eb;  // 01100010 01010101 01000001 11101011

    input_array[0] = 0x0f13149f;  // 00001111 00010011 00010100 10011111
    input_array[1] = 0x3308e53e;  // 00110011 00001000 11100101 00111110
    input_array[2] = 0x556241eb;  // 01010101 01100010 01000001 11101011

    receive_array.Resize(3);
    receive_array[0] = 0x0f139f14;  // 00001111 00010011 10011111 00010100
    receive_array[1] = 0x33083ee5;  // 00110011 00001000 00111110 11100101
    receive_array[2] = 0x5562eb41;  // 01010101 01100010 11101011 01000001
  } else {
    m_world->GetEnvironment().SetupInputs(ctx, input_array);
    m_world->GetEnvironment().SetupInputs(ctx, receive_array);
  }

  if (cur_depth > test_info.max_depth) test_info.max_depth = cur_depth;

  // Setup the organism we're working with now.
  if (test_info.org_array[cur_depth] != NULL) {
    delete test_info.org_array[cur_depth];
  }
  test_info.org_array[cur_depth] = new cOrganism(m_world, ctx, genome);
  cOrganism & organism = *( test_info.org_array[cur_depth] );
  organism.SetOrgInterface(new cTestCPUInterface(this));
  organism.GetPhenotype().SetupInject(genome.GetSize());

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


void cTestCPU::TestThreads(cAvidaContext& ctx, const cGenome& genome)
{
  cCPUTestInfo test_info;
  test_info.TestThreads();
  cTestCPU::TestGenome(ctx, test_info, genome);
}


void cTestCPU::PrintThreads(cAvidaContext& ctx, const cGenome& genome)
{
  cCPUTestInfo test_info;
  test_info.TestThreads();
  test_info.PrintThreads();
  cTestCPU::TestGenome(ctx, test_info, genome);
}


bool cTestCPU::TestIntegrity(const cGenome & test_genome)
{
#ifdef QUICK_BASE_TEST_CPU
  // This checks to make sure a 'copy', 'divide', and 'allocate' are all in
  // the creatures, and if not doesn't even bother to test it.
  static unsigned char copy_id  = inst_set->GetInstID("copy");
  static unsigned char div_id   = inst_set->GetInstID("divide");
  static unsigned char alloc_id = inst_set->GetInstID("allocate");
#endif

#ifdef QUICK_HEAD_TEST_CPU
  // This checks to make sure a 'copy', 'divide', and 'allocate' are all in
  // the creatures, and if not doesn't even bother to test it.
  static unsigned char copy_id  = inst_set->GetInstID("h-copy");
  static unsigned char div_id   = inst_set->GetInstID("h-divide");
  static unsigned char alloc_id = inst_set->GetInstID("h-alloc");
#endif


#ifdef QUICK_TEST_CPU
  bool copy_found = false;
  bool div_found = false;
  bool alloc_found = false;

  for (int i = 0; i < test_genome.GetSize(); i++) {
    if (test_genome[i].GetOp() == copy_id)  copy_found  = true;
    if (test_genome[i].GetOp() == div_id)   div_found   = true;
    if (test_genome[i].GetOp() == alloc_id) alloc_found = true;
  }

  if (copy_found == false || div_found == false || alloc_found == false) {
    return false;
  }
#endif

  return true;
}


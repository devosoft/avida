//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cCPUTestInfo.h"

#include "hardware_status_printer.hh"
#include "organism.hh"
#include "phenotype.hh"

#include <assert.h>

/////////////////
// cCPUTestInfo
/////////////////

cCPUTestInfo::cCPUTestInfo(int max_tests)
  : generation_tests(max_tests)  // These vars not reset on Clear()
  , test_threads(false)
  , print_threads(false)
  , trace_execution(false)
  , trace_task_order(false)
  , use_random_inputs(false)
  , org_array(max_tests)
  , m_tracer(NULL)
{
  org_array.SetAll(NULL);
  Clear();
}


cCPUTestInfo::~cCPUTestInfo()
{
  for (int i = 0; i < generation_tests; i++) {
    if (org_array[i] != NULL) delete org_array[i];
  }
}


void cCPUTestInfo::Clear()
{
  is_viable = false;
  max_depth = -1;
  depth_found = -1;
  max_cycle = 0;
  cycle_to = -1;

  for (int i = 0; i < generation_tests; i++) {
    if (org_array[i] == NULL) break;
    delete org_array[i];
    org_array[i] = NULL;
  }
}
 

void cCPUTestInfo::SetTraceExecution(cHardwareTracer *tracer)
{
  trace_execution = (tracer)?(true):(false);
  m_tracer = tracer;
}


double cCPUTestInfo::GetGenotypeFitness()
{
  if (org_array[0] != NULL) return org_array[0]->GetPhenotype().GetFitness();
  return 0.0;
}


double cCPUTestInfo::GetColonyFitness()
{
  if (IsViable()) return GetColonyOrganism()->GetPhenotype().GetFitness();
  return 0.0;
}

/*
 *  cCPUTestInfo.h
 *  Avida
 *
 *  Called "cpu_test_info.hh" prior to 11/29/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#include "cCPUTestInfo.h"

#include "cInstSet.h"
#include "cHardwareStatusPrinter.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cResourceHistory.h"

#include <cassert>

cCPUTestInfo::cCPUTestInfo(int max_tests)
  : generation_tests(max_tests)  // These vars not reset on Clear()
  , trace_task_order(false)
  , use_random_inputs(false)
  , use_manual_inputs(false)
  , m_tracer(NULL)
  , m_cur_sg(0)
  , org_array(max_tests)
  , m_res_method(RES_INITIAL)
  , m_res(NULL)
  , m_res_update(0)
  , m_res_cpu_cycle_offset(0)
{
  org_array.SetAll(NULL);
  Clear();
}

cCPUTestInfo::cCPUTestInfo(const cCPUTestInfo& test_info)
{
  *this = test_info;
}


cCPUTestInfo& cCPUTestInfo::operator=(const cCPUTestInfo& test_info)
{
  generation_tests = test_info.generation_tests;
  trace_task_order = test_info.trace_task_order;
  use_random_inputs = test_info.use_random_inputs;
	use_manual_inputs = test_info.use_manual_inputs;
  manual_inputs = test_info.manual_inputs; 
  m_tracer = NULL;
  if (test_info.m_tracer) {
    *m_tracer = *test_info.m_tracer;
  }
  m_mut_rates = test_info.m_mut_rates;
  m_cur_sg = test_info.m_cur_sg;
  is_viable = test_info.is_viable;
  max_depth = test_info.max_depth;
  depth_found = test_info.depth_found;
  max_cycle = test_info.max_cycle;
  cycle_to = test_info.cycle_to;
  used_inputs = test_info.used_inputs; 
  org_array = test_info.org_array;
  m_res_method = test_info.m_res_method;
  m_res = NULL;  //Beware -- Resource history is NOT COPIED.
  m_res_update = test_info.m_res_update;
  m_res_cpu_cycle_offset = test_info.m_res_cpu_cycle_offset;
  return *this;
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

cPhenotype& cCPUTestInfo::GetTestPhenotype(int level)
{
  assert(org_array[level] != NULL);
  return org_array[level]->GetPhenotype();
}

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

#ifndef cCPUTestInfo_h
#define cCPUTestInfo_h

#ifndef nHardware_h
#include "nHardware.h"
#endif
#ifndef cMutationRates_h
#include "cMutationRates.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

class cHardwareTracer;
class cOrganism;
class cPhenotype;
class cResourceHistory;
class cString;


enum eTestCPUResourceMethod { RES_INITIAL = 0, RES_CONSTANT, RES_UPDATED_DEPLETABLE, RES_DYNAMIC, RES_LAST };  
// Modes for how the test CPU handles resources:
// OFF - all resources are at zero. (OLD: use_resources = 0)
// CONSTANT - resources stay constant at input values for the specified update. (OLD: use_resources = 1)
// UPDATED_DEPLETABLE - resources change every update according to resource data file (assuming an update
//    is an average time slice). The organism also depletes these resources when using them.
// DYNAMIC - UPDATED_DEPLETABLE + resources inflow/outflow (NOT IMPLEMENTED YET!)


class cCPUTestInfo
{
  friend class cTestCPU;
private:
  // Inputs...
  int generation_tests; // Maximum depth in generations to test
  bool trace_task_order;      // Should we keep track of ordering of tasks?
  bool use_random_inputs;     // Should we give the organism random inputs?
	bool use_manual_inputs;     // Do we have inputs that we must use?
	tArray<int> manual_inputs;  //   if so, use these.
  cHardwareTracer* m_tracer;
  cMutationRates m_mut_rates;
  
  int m_cur_sg;

  // Outputs...
  bool is_viable;         // Is this organism colony forming?
  int max_depth;          // Deepest tests went...
  int depth_found;        // Depth actually found (often same as max_depth)
  int max_cycle;          // Longest cycle found.
  int cycle_to;           // Cycle path of the last genotype.
	tArray<int> used_inputs; //Depth 0 inputs

  tArray<cOrganism*> org_array;
  
  // Information about how to handle resources
  eTestCPUResourceMethod m_res_method;
  cResourceHistory* m_res;
  int m_res_update;
  int m_res_cpu_cycle_offset;


public:
  cCPUTestInfo(int max_tests=nHardware::TEST_CPU_GENERATIONS);
  cCPUTestInfo(const cCPUTestInfo&);
  cCPUTestInfo& operator=(const cCPUTestInfo&);
  ~cCPUTestInfo();

  void Clear();
 
  // Input Setup
  void TraceTaskOrder(bool _trace=true) { trace_task_order = _trace; }
  void UseRandomInputs(bool _rand=true) { use_random_inputs = _rand; use_manual_inputs = false; }
  void UseManualInputs(tArray<int> inputs) {use_manual_inputs = true; use_random_inputs = false; manual_inputs = inputs;}
  void ResetInputMode() {use_manual_inputs = false; use_random_inputs = false;}
  void SetTraceExecution(cHardwareTracer* tracer = NULL) { m_tracer = tracer; }
  void SetResourceOptions(int res_method = RES_INITIAL, cResourceHistory* res = NULL, int update = 0, int cpu_cycle_offset = 0)
    { m_res_method = (eTestCPUResourceMethod)res_method; m_res = res; m_res_update = update; m_res_cpu_cycle_offset = cpu_cycle_offset; }
  
  void SetCurrentStateGridID(int sg) { m_cur_sg = sg; }
  cMutationRates& MutationRates() { return m_mut_rates; }

  // Input Accessors
  int GetGenerationTests() const { return generation_tests; }
  bool GetTraceTaskOrder() const { return trace_task_order; }
  bool GetUseRandomInputs() const { return use_random_inputs; }
  bool GetTraceExecution() const { return (m_tracer); }
	bool GetUseManualInputs() const { return use_manual_inputs; }
	tArray<int> GetTestCPUInputs() const { return used_inputs; }
  cHardwareTracer *GetTracer() { return m_tracer; }


  // Output Accessors
  bool IsViable() const { return is_viable; }
  int GetMaxDepth() const { return max_depth; }
  int GetDepthFound() const { return depth_found; }
  int GetMaxCycle() const { return max_cycle; }
  int GetCycleTo() const { return cycle_to; }

  // Genotype Stats...
  inline cOrganism* GetTestOrganism(int level = 0);
  cPhenotype& GetTestPhenotype(int level = 0);
  inline cOrganism* GetColonyOrganism();

  // And just because these are so commonly used...
  double GetGenotypeFitness();
  double GetColonyFitness();
  
  int GetStateGridID() const { return m_cur_sg; }
};


inline cOrganism* cCPUTestInfo::GetTestOrganism(int level)
{
  assert(org_array[level] != NULL);
  return org_array[level];
}

inline cOrganism* cCPUTestInfo::GetColonyOrganism()
{
  const int depth_used = (depth_found == -1) ? 0 : depth_found;
  assert(org_array[depth_used] != NULL);
  return org_array[depth_used];
}

#endif

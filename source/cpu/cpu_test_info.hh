//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CPU_TEST_INFO_HH
#define CPU_TEST_INFO_HH

#include <fstream>

#ifndef CPU_DEFS_HH
#include "cpu_defs.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef TARRAY_HH
#include "tArray.hh"
#endif

class cOrganism;
class cString;

class cCPUTestInfo {
  friend class cTestCPU;
private:
  // Inputs...
  const int generation_tests; // Maximum depth in generations to test
  bool test_threads;          // Should a report of threading be saved?
  bool print_threads;         // Should the report be printed?
  bool trace_execution;       // Should we trace this CPU?
  bool trace_task_order;      // Should we keep track of ordering of tasks?
  bool use_random_inputs;     // Should we give the organism random inputs?
  std::ofstream trace_fp;

  // Outputs...
  bool is_viable;         // Is this organism colony forming?
  int max_depth;          // Deepest tests went...
  int depth_found;        // Depth actually found (often same as max_depth)
  int max_cycle;          // Longest cycle found.
  int cycle_to;           // Cycle path of the last genotype.

  tArray<cOrganism *> org_array;

private:
  // disabled copy constructor.
  cCPUTestInfo(const cCPUTestInfo &);
public:
  cCPUTestInfo(int max_tests=TEST_CPU_GENERATIONS);
  ~cCPUTestInfo();

  void Clear();
 
  // Input Setup
  void TestThreads(bool _test=true) { test_threads = _test; }
  void PrintThreads(bool _print=true) { print_threads = _print; }
  void TraceTaskOrder(bool _trace=true) { trace_task_order = _trace; }
  void UseRandomInputs(bool _rand=true) { use_random_inputs = _rand; }
  void SetTraceExecution(const cString & filename="trace.dat");

  // Input Accessors
  int GetGenerationTests() const { return generation_tests; }
  bool GetTestThreads() const { return test_threads; }
  bool GetPrintThreads() const { return print_threads; }
  bool GetTraceTaskOrder() const { return trace_task_order; }
  bool GetUseRandomInputs() const { return use_random_inputs; }
  bool GetTraceExecution() const { return trace_execution; }
  std::ofstream & GetTraceFP() { return trace_fp; }


  // Output Accessors
  bool IsViable() const { return is_viable; }
  int GetMaxDepth() const { return max_depth; }
  int GetDepthFound() const { return depth_found; }
  int GetMaxCycle() const { return max_cycle; }
  int GetCycleTo() const { return cycle_to; }

  // Genotype Stats...
  cOrganism * GetTestOrganism(int level=0) {
    assert(org_array[level] != NULL);
    return org_array[level];
  }

  cOrganism * GetColonyOrganism() {
    const int depth_used = (depth_found == -1) ? 0 : depth_found;
    assert(org_array[depth_used] != NULL);
    return org_array[depth_used];
  }

  // And just because these are so commonly used...
  double GetGenotypeFitness();
  double GetColonyFitness();
};

#endif

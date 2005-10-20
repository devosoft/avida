//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef TEST_CPU_HH
#define TEST_CPU_HH

#include <fstream>

#ifndef TARRAY_HH
#include "tArray.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif

#ifndef RESOURCE_COUNT_HH
#include "cResourceCount.h"
#endif

class cInstSet;
class cEnvironment;
class cPopulationInterface;
template <class T> class tArray; // aggregate
class cResourceCount;
class cCPUTestInfo;
class cGenome;
class cString; // aggregate
class cWorld;

class cTestCPU {
private:
  static cWorld* m_world;
  static cInstSet * inst_set;
  static cEnvironment * environment;
  static cPopulationInterface* test_interface;
  static tArray<int> input_array;
  static tArray<int> receive_array;
  static int cur_input;
  static int cur_receive;
  static cResourceCount resource_count;
  static bool d_useResources;
  static tArray<double> d_emptyDoubleArray;
  static tArray<double> d_resources;
  
  static bool initialized;
  static int time_mod;

  static bool ProcessGestation(cCPUTestInfo & test_info, int cur_depth);

  static bool TestGenome_Body(cCPUTestInfo & test_info, const cGenome & genome,
			      int cur_depth);

public:
  static void Setup(cWorld* world, int resourceSize);
  static void SetInstSet(cInstSet * in_inst_set);

  static bool TestGenome(cCPUTestInfo & test_info, const cGenome & genome);
  static bool TestGenome(cCPUTestInfo & test_info, const cGenome & genome,
			 std::ofstream & out_fp);

//  static void TraceGenome(const cGenome &genome, cString filename="trace.dat");

  static void TestThreads(const cGenome & genome);
  static void PrintThreads(const cGenome & genome);

  // Test if a genome has any chance of being a replicator (i.e., in the
  // default set, has an allocate, a copy, and a divide).
  static bool TestIntegrity(const cGenome & test_genome);

  static cInstSet * GetInstSet() { return inst_set; }
  static cEnvironment * GetEnvironment() { return environment; }
  static void SetEnvironment(cEnvironment *);
  static int GetInput();
  static int GetInputAt(int & input_pointer);
  static int GetReceiveValue();
  static const tArray<double> & GetResources();
  static void UpdateResources(const tArray<double> & res_change);
  static void UpdateResource(int id, double change);
  static void UpdateCellResources(const tArray<double> & res_change,
				  const int cell_id);
  static void SetResource(int id, double new_level);
  static void SetupResources(void);
  static void SetupResourceArray(const tArray<double> &resources);
  static bool &UseResources(void) { return d_useResources; }
  static cResourceCount &GetResourceCount(void) {return resource_count;}
};

#endif

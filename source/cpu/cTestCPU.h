/*
 *  cTestCPU.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cTestCPU_h
#define cTestCPU_h

#include <fstream>

#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef cResourceCount_h
#include "cResourceCount.h"
#endif

class cInstSet;
class cResourceCount;
class cCPUTestInfo;
class cGenome;
class cWorld;

class cTestCPU
{
private:
  cWorld* m_world;
  tArray<int> input_array;
  tArray<int> receive_array;
  int cur_input;
  int cur_receive;

  cResourceCount resource_count;
  bool d_useResources;
  tArray<double> d_emptyDoubleArray;
  tArray<double> d_resources;
  
  bool ProcessGestation(cCPUTestInfo & test_info, int cur_depth);
  bool TestGenome_Body(cCPUTestInfo & test_info, const cGenome & genome, int cur_depth);
  void SetupResources(void);

  cTestCPU(); // @not_implemented
  cTestCPU(const cTestCPU&); // @not_implemented
  cTestCPU& operator=(const cTestCPU&); // @not_implemented
  
public:
  cTestCPU(cWorld* world) : m_world(world) { SetupResources(); }
  ~cTestCPU() { ; }
  
  bool TestGenome(cCPUTestInfo & test_info, const cGenome & genome);
  bool TestGenome(cCPUTestInfo & test_info, const cGenome & genome, std::ofstream& out_fp);

  void TestThreads(const cGenome & genome);
  void PrintThreads(const cGenome & genome);

  // Test if a genome has any chance of being a replicator (i.e., in the
  // default set, has an allocate, a copy, and a divide).
  bool TestIntegrity(const cGenome & test_genome);

  inline int GetInput();
  inline int GetInputAt(int & input_pointer);
  inline int GetReceiveValue();
  inline const tArray<double>& GetResources();
  inline void SetResource(int id, double new_level);
  void SetupResourceArray(const tArray<double> &resources);
  bool& UseResources(void) { return d_useResources; }
  cResourceCount& GetResourceCount(void) { return resource_count; }
};


// Inline Methods

inline int cTestCPU::GetInput()
{
  if (cur_input >= input_array.GetSize()) cur_input = 0;
  return input_array[cur_input++];
}

inline int cTestCPU::GetInputAt(int & input_pointer)
{
  if (input_pointer >= input_array.GetSize()) input_pointer = 0;
  return input_array[input_pointer++];
}

inline int cTestCPU::GetReceiveValue()
{
  if (cur_receive >= receive_array.GetSize()) cur_receive = 0;
  return receive_array[cur_receive++];
}

inline const tArray<double>& cTestCPU::GetResources()
{
  if(d_useResources) return d_resources;
  
  return d_emptyDoubleArray;
}

inline void cTestCPU::SetResource(int id, double new_level)
{
  resource_count.Set(id, new_level);
}

#endif

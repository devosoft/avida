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

class cAvidaContext;
class cInstSet;
class cResourceCount;
class cCPUTestInfo;
class cGenome;
class cWorld;

class cTestResources
{
  friend class cTestCPU;
private:
  cResourceCount resource_count;
  bool d_useResources;
  tArray<double> d_emptyDoubleArray;
  tArray<double> d_resources;
  
public:
  cTestResources(cWorld* world);
};

class cTestCPU
{
private:
  cWorld* m_world;
  tArray<int> input_array;
  tArray<int> receive_array;
  int cur_input;
  int cur_receive;
  
  cTestResources* m_res;
  bool m_localres;

  bool ProcessGestation(cAvidaContext& ctx, cCPUTestInfo& test_info, int cur_depth);
  bool TestGenome_Body(cAvidaContext& ctx, cCPUTestInfo& test_info, const cGenome& genome, int cur_depth);

  cTestCPU(); // @not_implemented
  cTestCPU(const cTestCPU&); // @not_implemented
  cTestCPU& operator=(const cTestCPU&); // @not_implemented
  
public:
  cTestCPU(cWorld* world) : m_world(world), m_res(new cTestResources(world)), m_localres(true) { ; }
  cTestCPU(cWorld* world, cTestResources* res) : m_world(world), m_res(res), m_localres(false) { ; }
  ~cTestCPU() { if (m_localres) delete m_res; }
  
  bool TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const cGenome& genome);
  bool TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const cGenome & genome, std::ofstream& out_fp);

  void TestThreads(cAvidaContext& ctx, const cGenome& genome);
  void PrintThreads(cAvidaContext& ctx, const cGenome& genome);

  // Test if a genome has any chance of being a replicator (i.e., in the
  // default set, has an allocate, a copy, and a divide).
  bool TestIntegrity(const cGenome & test_genome);

  inline int GetInput();
  inline int GetInputAt(int & input_pointer);
  inline int GetReceiveValue();
  inline const tArray<double>& GetResources();
  inline void SetResource(int id, double new_level);
  void SetupResourceArray(const tArray<double> &resources);
  void SetUseResources(bool use);
  bool GetUseResources() { return m_res->d_useResources; }
  cResourceCount& GetResourceCount(void) { return m_res->resource_count; }
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
  if(m_res->d_useResources) return m_res->d_resources;
  
  return m_res->d_emptyDoubleArray;
}

inline void cTestCPU::SetResource(int id, double new_level)
{
  if (!m_localres) m_res = new cTestResources(*m_res);  // copy resources locally
  m_res->resource_count.Set(id, new_level);
}

#endif

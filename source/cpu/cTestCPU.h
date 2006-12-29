/*
 *  cTestCPU.h
 *  Avida
 *
 *  Called "test_cpu.hh" prior to 11/30/05.
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
class cCPUTestInfo;
class cGenome;
class cGenotype;
class cInjectGenotype;
class cInstSet;
class cResourceCount;
class cWorld;


class cTestCPU
{
private:
  cWorld* m_world;
  tArray<int> input_array;
  tArray<int> receive_array;
  int cur_input;
  int cur_receive;  

  enum tTestCPUResourceMethod { RES_STATIC = 0, RES_DYNAMIC };  
  tTestCPUResourceMethod m_res_method;
  cResourceCount resource_count;
  tArray<double> d_resources;

  bool ProcessGestation(cAvidaContext& ctx, cCPUTestInfo& test_info, int cur_depth);
  bool TestGenome_Body(cAvidaContext& ctx, cCPUTestInfo& test_info, const cGenome& genome, int cur_depth);

  cTestCPU(); // @not_implemented
  cTestCPU(const cTestCPU&); // @not_implemented
  cTestCPU& operator=(const cTestCPU&); // @not_implemented
  
public:
  cTestCPU(cWorld* world);
  ~cTestCPU() { }
  
  bool TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const cGenome& genome);
  bool TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const cGenome& genome, std::ofstream& out_fp);
  
  void PrintGenome(cAvidaContext& ctx, const cGenome& genome, cString filename,
                   cGenotype* genotype = NULL, int update = -1);
  void PrintInjectGenome(cAvidaContext& ctx, cInjectGenotype* inject_genotype,
                         const cGenome& genome, cString filename = "", int update = -1);

  inline int GetInput();
  inline int GetInputAt(int & input_pointer);
  inline int GetReceiveValue();
  inline const tArray<double>& GetResources();  
  inline void SetResource(int id, double new_level);
  void InitResources();
  void SetResourcesFromArray(const tArray<double> &resources);
  void SetResourcesFromCell(int cell_x, int cell_y);
  void ModifyResources(const tArray<double>& res_change);
  cResourceCount& GetResourceCount() { return resource_count; }
};

#ifdef ENABLE_UNIT_TESTS
namespace nTestCPU {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif



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
  if(m_res_method == RES_STATIC) return d_resources;
  
  return resource_count.GetResources();
}

inline void cTestCPU::SetResource(int id, double new_level)
{
  resource_count.Set(id, new_level);
}

#endif

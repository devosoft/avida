/*
 *  cTestCPU.h
 *  Avida
 *
 *  Called "test_cpu.hh" prior to 11/30/05.
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
#ifndef cCPUTestInfo_h
#include "cCPUTestInfo.h"
#endif

class cAvidaContext;
class cBioGroup;
class cGenome;
class cInstSet;
class cGenome;
class cResourceCount;
class cResourceHistory;
class cWorld;


class cTestCPU
{
public:

private:
  cWorld* m_world;
  tArray<int> input_array;
  tArray<int> receive_array;
  int cur_input;
  int cur_receive;  
  bool m_use_random_inputs;
  bool m_use_manual_inputs;
  
  // Resource settings. Reinitialized from cCPUTestInfo on each test.
  eTestCPUResourceMethod m_res_method;
  const cResourceHistory* m_res;
  int m_res_update;
  int m_res_cpu_cycle_offset;

  // Actual CPU resources.
  cResourceCount m_resource_count;
  cResourceCount m_deme_resource_count;
  
  

  bool ProcessGestation(cAvidaContext& ctx, cCPUTestInfo& test_info, int cur_depth);
  bool TestGenome_Body(cAvidaContext& ctx, cCPUTestInfo& test_info, const cGenome& genome, int cur_depth);

  
  cTestCPU(); // @not_implemented
  cTestCPU(const cTestCPU&); // @not_implemented
  cTestCPU& operator=(const cTestCPU&); // @not_implemented
  
  // Internal methods for setting up and updating resources
  void InitResources(int res_method = RES_INITIAL, cResourceHistory* res = NULL, int update = 0, int cpu_cycle_offset = 0);
  void UpdateResources(int cpu_cycles_used);
  inline void SetResourceUpdate(int update, bool exact = true);
  inline void SetResource(int id, double new_level);
  
public:
  cTestCPU(cWorld* world);
  ~cTestCPU() { }
  
  bool TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const cGenome& genome);
  bool TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const cGenome& genome, std::ofstream& out_fp);
  
  void PrintGenome(cAvidaContext& ctx, const cGenome& genome, cString filename = "", int update = -1);
  void PrintBioGroup(cAvidaContext& ctx, cBioGroup* bg, cString filename = "", int update = -1);

  inline int GetInput();
  inline int GetInputAt(int & input_pointer);
  inline const tArray<int>& GetInputs() const { return input_array; }
  void ResetInputs(cAvidaContext& ctx);

  inline int GetReceiveValue();
  inline const tArray<double>& GetResources();
  inline const tArray<double>& GetDemeResources(int deme_id);
  inline const tArray< tArray<int> >& GetCellIdLists();
  
  // Used by cTestCPUInterface to get/update resources
  void ModifyResources(const tArray<double>& res_change);
  cResourceCount& GetResourceCount() { return m_resource_count; }
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
    return m_resource_count.GetResources();
}

inline const tArray<double>& cTestCPU::GetDemeResources(int deme_id)
{
    return m_deme_resource_count.GetResources();
}

inline const tArray< tArray<int> >& cTestCPU::GetCellIdLists()
{
	return m_resource_count.GetCellIdLists();
}

inline void cTestCPU::SetResource(int id, double new_level)
{
  m_resource_count.Set(id, new_level);
}

#endif

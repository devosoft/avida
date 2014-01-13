/*
 *  cTestCPU.h
 *  Avida
 *
 *  Called "test_cpu.hh" prior to 11/30/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cTestCPU_h
#define cTestCPU_h

#include <fstream>

#include "cString.h"
#include "cResourceCount.h"
#include "cCPUTestInfo.h"
#include "cWorld.h"


class cAvidaContext;
class cBioGroup;
class cInstSet;
class cResourceCount;
class cResourceHistory;

using namespace Avida;


class cTestCPU
{
public:

private:
  cWorld* m_world;
  Apto::Array<int> input_array;
  Apto::Array<int> receive_array;
  int cur_input;
  int cur_receive;  
  bool m_use_random_inputs;
  bool m_use_manual_inputs;
  int m_test_solo_res;
  double m_test_solo_res_lev;
  
  // Resource settings. Reinitialized from cCPUTestInfo on each test.
  eTestCPUResourceMethod m_res_method;
  const cResourceHistory* m_res;
  int m_res_update;
  int m_res_cpu_cycle_offset;

  // Actual CPU resources.
  cResourceCount m_resource_count;
  cResourceCount m_faced_cell_resource_count;
  cResourceCount m_deme_resource_count;
  cResourceCount m_cell_resource_count;
    

  bool ProcessGestation(cAvidaContext& ctx, cCPUTestInfo& test_info, int cur_depth);
  bool TestGenome_Body(cAvidaContext& ctx, cCPUTestInfo& test_info, const Genome& genome, int cur_depth);

  
  cTestCPU(); // @not_implemented
  cTestCPU(const cTestCPU&); // @not_implemented
  cTestCPU& operator=(const cTestCPU&); // @not_implemented
  
  // Internal methods for setting up and updating resources
  void InitResources(cAvidaContext& ctx, int res_method = RES_INITIAL, cResourceHistory* res = NULL, int update = 0, int cpu_cycle_offset = 0);
  void UpdateResources(cAvidaContext& ctx, int cpu_cycles_used);
  inline void SetResourceUpdate(cAvidaContext& ctx, int update, bool exact = true);
  inline void SetResource(cAvidaContext& ctx, int id, double new_level);
  
public:
  cTestCPU(cAvidaContext& ctx, cWorld* world);
  ~cTestCPU() { }
  
  bool TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const Genome& genome);
  bool TestGenome(cAvidaContext& ctx, cCPUTestInfo& test_info, const Genome& genome, std::ofstream& out_fp);
  
  void PrintGenome(cAvidaContext& ctx, const Genome& genome, cString filename = "", int update = -1, bool for_groups = false, int last_birth_cell = 0, int last_group_id = -1, int last_forager_type = -1);

  inline int GetInput();
  inline int GetInputAt(int & input_pointer);
  inline const Apto::Array<int>& GetInputs() const { return input_array; }
  void ResetInputs(cAvidaContext& ctx);

  inline int GetReceiveValue();
  inline const Apto::Array<double>& GetResources(cAvidaContext& ctx);
  inline double GetResourceVal(cAvidaContext& ctx, int res_id);
  inline const Apto::Array<double>& GetAVResources(cAvidaContext& ctx);
  inline double GetAVResourceVal(cAvidaContext& ctx, int res_id);
  inline const Apto::Array<double>& GetFacedCellResources(cAvidaContext& ctx); 
  inline double GetFacedResourceVal(cAvidaContext& ctx, int res_id);
  inline const Apto::Array<double>& GetAVFacedResources(cAvidaContext& ctx);
  double GetAVFacedResourceVal(cAvidaContext& ctx, int res_id);
  inline const Apto::Array<double>& GetDemeResources(int deme_id, cAvidaContext& ctx); 
  inline const Apto::Array<double>& GetCellResources(int cell_id, cAvidaContext& ctx); 
  inline const Apto::Array<double>& GetFrozenResources(cAvidaContext& ctx, int cell_id); 
  inline double GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id);
  inline double GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id);
  inline const Apto::Array< Apto::Array<int> >& GetCellIdLists();
  
  // Used by cTestCPUInterface to get/update resources
  void ModifyResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  cResourceCount& GetResourceCount() { return m_resource_count; }
  
  void SetSoloRes(int res_id, double res_amount) { m_test_solo_res = res_id; m_test_solo_res_lev = res_amount; }
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

inline const Apto::Array<double>& cTestCPU::GetResources(cAvidaContext& ctx)
{
  return m_resource_count.GetResources(ctx); 
}

inline double cTestCPU::GetResourceVal(cAvidaContext& ctx, int res_id)
{
  const Apto::Array<double>& cell_res = m_cell_resource_count.GetResources(ctx);
  return cell_res[res_id];
}

inline const Apto::Array<double>& cTestCPU::GetFacedCellResources(cAvidaContext& ctx)
{
  return m_faced_cell_resource_count.GetResources(ctx); 
}
 
inline double cTestCPU::GetFacedResourceVal(cAvidaContext& ctx, int res_id)
{
  const Apto::Array<double>& faced_res = m_faced_cell_resource_count.GetResources(ctx);
  return faced_res[res_id];
}

inline const Apto::Array<double>& cTestCPU::GetAVFacedResources(cAvidaContext& ctx)
{
  return m_faced_cell_resource_count.GetResources(ctx); 
}

inline double cTestCPU::GetAVFacedResourceVal(cAvidaContext& ctx, int res_id)
{
  const Apto::Array<double>& faced_res = m_faced_cell_resource_count.GetResources(ctx);
  return faced_res[res_id];
}

inline const Apto::Array<double>& cTestCPU::GetDemeResources(int deme_id, cAvidaContext& ctx)
{
    return m_deme_resource_count.GetResources(ctx); 
}

inline const Apto::Array<double>& cTestCPU::GetCellResources(int, cAvidaContext& ctx)
{
  return m_cell_resource_count.GetResources(ctx); 
}

inline const Apto::Array<double>& cTestCPU::GetAVResources(cAvidaContext& ctx)
{
  return m_cell_resource_count.GetResources(ctx); 
}

inline double cTestCPU::GetAVResourceVal(cAvidaContext& ctx, int res_id)
{
  const Apto::Array<double>& cell_res = m_cell_resource_count.GetResources(ctx);
  return cell_res[res_id];
}

inline const Apto::Array<double>& cTestCPU::GetFrozenResources(cAvidaContext& ctx, int cell_id)   
{
  return m_cell_resource_count.GetResources(ctx); 
}

inline double cTestCPU::GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id)
{
  const Apto::Array<double>& cell_res = m_cell_resource_count.GetResources(ctx);
  return cell_res[res_id];
}

inline double cTestCPU::GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id)
{
  const Apto::Array<double>& cell_res = m_cell_resource_count.GetResources(ctx);
  return cell_res[res_id];
}

inline const Apto::Array< Apto::Array<int> >& cTestCPU::GetCellIdLists()
{
	return m_resource_count.GetCellIdLists();
}

inline void cTestCPU::SetResource(cAvidaContext& ctx, int id, double new_level)
{
  m_resource_count.Set(ctx, id, new_level);
}

#endif

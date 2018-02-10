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
#include "cCPUTestInfo.h"
#include "resources/Types.h"
#include "cResourceHistory.h"


class cAvidaContext;
class cBioGroup;
class cInstSet;
class cWorld;

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
  cResourceHistory m_res_history;
  int m_res_update;
  int m_res_cpu_cycle_offset;

  // Actual CPU resources.
  CellResAmounts m_resource_abundances;
  CellResAmounts m_faced_cell_resource_abundances;
  CellResAmounts m_deme_resource_abundances;
  CellResAmounts m_cell_resource_abundances;
    

  bool ProcessGestation(cAvidaContext& ctx, cCPUTestInfo& test_info, int cur_depth);
  bool TestGenome_Body(cAvidaContext& ctx, cCPUTestInfo& test_info, const Genome& genome, int cur_depth);

  
  cTestCPU(); // @not_implemented
  cTestCPU(const cTestCPU&); // @not_implemented
  cTestCPU& operator=(const cTestCPU&); // @not_implemented
  
  // Internal methods for setting up and updating resources
  void InitResources(cAvidaContext& ctx, cResourceHistory res_history, int res_method = RES_INITIAL, int update = 0, int cpu_cycle_offset = 0);
  void UpdateRandomResources(cAvidaContext& ctx, int cpu_cycles_used);
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
  inline const CellResAmounts& GetResources(cAvidaContext& ctx);
  inline ResAmount GetResourceVal(cAvidaContext& ctx, int res_id);
  inline const CellResAmounts& GetAVResources(cAvidaContext& ctx);
  inline ResAmount GetAVResourceVal(cAvidaContext& ctx, int res_id);
  inline const CellResAmounts& GetFacedCellResources(cAvidaContext& ctx); 
  inline ResAmount GetFacedResourceVal(cAvidaContext& ctx, int res_id);
  inline const CellResAmounts& GetAVFacedResources(cAvidaContext& ctx);
  ResAmount GetAVFacedResourceVal(cAvidaContext& ctx, int res_id);
  inline const CellResAmounts& GetDemeResources(int deme_id, cAvidaContext& ctx); 
  inline const CellResAmounts& GetCellResources(int cell_id, cAvidaContext& ctx); 
  inline const CellResAmounts& GetFrozenResources(cAvidaContext& ctx, int cell_id); 
  inline ResAmount GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id);
  inline ResAmount GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id);
  inline const Apto::Array< Apto::Array<int> >& GetCellIdLists();
  
  // Used by cTestCPUInterface to get/update resources
  void ModifyResources(cAvidaContext& ctx, const CellResAmounts& res_change);
  CellResAmounts& GetResourceCount() { return m_resource_abundances; }
  
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

inline const CellResAmounts& cTestCPU::GetResources(cAvidaContext& ctx)
{
  return m_resource_abundances; 
}

inline ResAmount cTestCPU::GetResourceVal(cAvidaContext& ctx, ResID res_id)
{
  return m_resource_abundances[res_id];
}

inline const CellResAmounts& cTestCPU::GetFacedCellResources(cAvidaContext& ctx)
{
  return m_faced_cell_resource_abundances;
}
 
inline ResAmount cTestCPU::GetFacedResourceVal(cAvidaContext& ctx, ResID res_id)
{
  return m_faced_cell_resource_abundances[res_id];
}

inline const CellResAmounts& cTestCPU::GetAVFacedResources(cAvidaContext& ctx)
{
  return m_faced_cell_resource_abundances;
}

inline ResAmount cTestCPU::GetAVFacedResourceVal(cAvidaContext& ctx, ResID res_id)
{
  return m_faced_cell_resource_abundances[res_id];
}

inline const CellResAmounts& cTestCPU::GetDemeResources(int deme_id, cAvidaContext& ctx)
{
    return m_deme_resource_abundances; 
}

inline const CellResAmounts& cTestCPU::GetCellResources(int, cAvidaContext& ctx)
{
  return m_cell_resource_abundances; 
}

inline const CellResAmounts& cTestCPU::GetAVResources(cAvidaContext& ctx)
{
  return m_cell_resource_abundances; 
}

inline ResAmount cTestCPU::GetAVResourceVal(cAvidaContext& ctx, ResID res_id)
{
  return m_cell_resource_abundances[res_id];
}

inline const CellResAmounts& cTestCPU::GetFrozenResources(cAvidaContext& ctx, ResID cell_id)   
{
  return m_cell_resource_abundances;
}

inline ResAmount cTestCPU::GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, ResID res_id)
{
  return  m_cell_resource_abundances[res_id];
}

inline ResAmount cTestCPU::GetCellResVal(cAvidaContext& ctx, int cell_id, ResID res_id)
{
  return m_cell_resource_abundances[res_id];
}

/*inline const Apto::Array< Apto::Array<int> >& cTestCPU::GetCellIdLists()
{
	return m_resource_count.GetCellIdLists();
}
*/

inline void cTestCPU::SetResource(cAvidaContext& ctx, ResID id, ResAmount new_level)
{
  m_resource_abundances[id] = new_level;
}

#endif

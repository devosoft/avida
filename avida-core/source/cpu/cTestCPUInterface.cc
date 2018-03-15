/*
 *  cTestCPUOrgInterface.cc
 *  Avida
 *
 *  Created by David on 3/4/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#include "cTestCPUInterface.h"

#include "cOrganism.h"
#include "cTestCPU.h"


bool cTestCPUInterface::Divide(cAvidaContext&, cOrganism* parent, const Genome&)
{
  ConstInstructionSequencePtr seq;
  seq.DynamicCastFrom(parent->UnitGenome().Representation());
  parent->GetPhenotype().TestDivideReset(*seq);
  // @CAO in the future, we probably want to pass this offspring the test_cpu!
  return true;
}

const Apto::Array<cOrganism*, Apto::Smart>& cTestCPUInterface::GetLiveOrgList() const
{
  return m_empty_live_org_list;
}

int cTestCPUInterface::GetInputAt(int& input_pointer)
{
  return m_testcpu->GetInputAt(input_pointer);
}

void cTestCPUInterface::ResetInputs(cAvidaContext& ctx)
{ 
  m_testcpu->ResetInputs(ctx); 
}

const Apto::Array<int>& cTestCPUInterface::GetInputs() const
{
  return m_testcpu->GetInputs();
}

const Apto::Array<double>& cTestCPUInterface::GetResources(cAvidaContext& ctx)
{
  return m_testcpu->GetResources(ctx); 
}

double cTestCPUInterface::GetResourceVal(cAvidaContext& ctx, int res_id)
{
  return m_testcpu->GetResourceVal(ctx, res_id);
}

const Apto::Array<double>& cTestCPUInterface::GetFacedCellResources(cAvidaContext& ctx)
{
  return m_testcpu->GetFacedCellResources(ctx); 
}

double cTestCPUInterface::GetFacedResourceVal(cAvidaContext& ctx, int res_id)
{
  return m_testcpu->GetFacedResourceVal(ctx, res_id);
}

const Apto::Array<double>& cTestCPUInterface::GetDemeResources(int deme_id, cAvidaContext& ctx)
{
  return m_testcpu->GetDemeResources(deme_id, ctx); 
}

const Apto::Array<double>& cTestCPUInterface::GetCellResources(int cell_id, cAvidaContext& ctx)
{
  return m_testcpu->GetCellResources(cell_id, ctx); 
}

const Apto::Array<double>& cTestCPUInterface::GetFrozenResources(cAvidaContext& ctx, int cell_id)
{
  return m_testcpu->GetFrozenResources(ctx, cell_id); 
}

double cTestCPUInterface::GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id)
{
  return m_testcpu->GetFrozenCellResVal(ctx, cell_id, res_id);
}

double cTestCPUInterface::GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id)
{
  return m_testcpu->GetCellResVal(ctx, cell_id, res_id);
}

const Apto::Array< Apto::Array<int> >& cTestCPUInterface::GetCellIdLists()
{
	return m_testcpu->GetCellIdLists();
}

void cTestCPUInterface::UpdateResources(cAvidaContext& ctx, const Apto::Array<double>& res_change)
{
   m_testcpu->ModifyResources(ctx, res_change);
}

void cTestCPUInterface::UpdateRandomResources(cAvidaContext& ctx, const Apto::Array<double>& res_change)
{
   m_testcpu->ModifyResources(ctx, res_change);
}

void cTestCPUInterface::Kaboom(int distance, cAvidaContext& ctx)
{
  (void) distance;
  // @CAO We should keep a note that the organism tried to explode, and
  // record the probability it used.
}

void cTestCPUInterface::Kaboom(int distance, cAvidaContext& ctx, double effect)
{
  (void) distance;
  // @CAO We should keep a note that the organism tried to explode, and
  // record the probability it used.
}

int cTestCPUInterface::ReceiveValue()
{
  return m_testcpu->GetReceiveValue();
}

int cTestCPUInterface::BuyValue(const int label, const int buy_price)
{
	return m_testcpu->GetReceiveValue();
}

bool cTestCPUInterface::UpdateMerit(cAvidaContext& ctx, double new_merit)
{
  m_test_info.GetTestPhenotype(m_cur_depth).SetMerit(cMerit(new_merit));
  return true;
}

int cTestCPUInterface::GetStateGridID(cAvidaContext& ctx)
{
  return m_test_info.GetStateGridID();
}

Apto::Array<int> cTestCPUInterface::GetFormedGroupArray()
{
  Apto::Array<int> null_array;
  null_array.SetAll(NULL);
  return null_array;
}

Apto::Array<cOrganism*> cTestCPUInterface::GetFacedAVs(int av_num)
{
  Apto::Array<cOrganism*> null_array;
  null_array.SetAll(NULL);
  return null_array;
}

Apto::Array<cOrganism*> cTestCPUInterface::GetCellAVs(int cell_id, int av_num)
{
  Apto::Array<cOrganism*> null_array;
  null_array.SetAll(NULL);
  return null_array;
}

Apto::Array<cOrganism*> cTestCPUInterface::GetFacedPreyAVs(int av_num)
{
  Apto::Array<cOrganism*> null_array;
  null_array.SetAll(NULL);
  return null_array;
}

const Apto::Array<double>& cTestCPUInterface::GetAVResources(cAvidaContext& ctx, int av_num)
{
  return m_testcpu->GetAVResources(ctx);
}

double cTestCPUInterface::GetAVResourceVal(cAvidaContext& ctx, int res_id, int av_num)
{
  return m_testcpu->GetAVResourceVal(ctx, res_id);
}

const Apto::Array<double>& cTestCPUInterface::GetAVFacedResources(cAvidaContext& ctx, int av_num)
{
  return m_testcpu->GetAVFacedResources(ctx);
}

double cTestCPUInterface::GetAVFacedResourceVal(cAvidaContext& ctx, int res_id, int av_num)
{
  return m_testcpu->GetAVFacedResourceVal(ctx, res_id);
}

void cTestCPUInterface::UpdateAVResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, int av_num)
{
  m_testcpu->ModifyResources(ctx, res_change);
}

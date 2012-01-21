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


bool cTestCPUInterface::Divide(cAvidaContext& ctx, cOrganism* parent, const Genome& offspring_genome)
{
  parent->GetPhenotype().TestDivideReset(parent->GetGenome().GetSequence());
  // @CAO in the future, we probably want to pass this offspring the test_cpu!
  return true;
}

cOrganism* cTestCPUInterface::GetNeighbor()
{
  return NULL;
}

cOrganism* cTestCPUInterface::GetAVRandNeighbor()
{
  return NULL;
}

cOrganism* cTestCPUInterface::GetAVRandNeighborPrey()
{
  return NULL;
}

cOrganism* cTestCPUInterface::GetAVRandNeighborPred()
{
  return NULL;
}

tArray<cOrganism*> cTestCPUInterface::GetAVNeighbors()
{
  tArray<cOrganism*> null_array;
  null_array.SetAll(NULL);
  return null_array;
}

bool cTestCPUInterface::IsNeighborCellOccupied() {
  return false;
}

bool cTestCPUInterface::HasAVNeighbor() {
  return false;
}

bool cTestCPUInterface::HasAVNeighborPrey() {
  return false;
}

bool cTestCPUInterface::HasAVNeighborPred() {
  return false;
}

int cTestCPUInterface::GetNumNeighbors()
{
  return 0;
}

int cTestCPUInterface::GetAVNumNeighbors()
{
  return 0;
}

void cTestCPUInterface::GetNeighborhoodCellIDs(tArray<int>& list)
{
  
}

void cTestCPUInterface::Rotate(int direction)
{
}

int cTestCPUInterface::GetInputAt(int& input_pointer)
{
  return m_testcpu->GetInputAt(input_pointer);
}

void cTestCPUInterface::ResetInputs(cAvidaContext& ctx)
{ 
  m_testcpu->ResetInputs(ctx); 
}

const tArray<int>& cTestCPUInterface::GetInputs() const
{
  return m_testcpu->GetInputs();
}

const tArray<double>& cTestCPUInterface::GetResources(cAvidaContext& ctx) 
{
  return m_testcpu->GetResources(ctx); 
}

const tArray<double>& cTestCPUInterface::GetFacedCellResources(cAvidaContext& ctx) 
{
  return m_testcpu->GetFacedCellResources(ctx); 
}

const tArray<double>& cTestCPUInterface::GetFacedAVResources(cAvidaContext& ctx) 
{
  return m_testcpu->GetFacedAVResources(ctx); 
}

const tArray<double>& cTestCPUInterface::GetDemeResources(int deme_id, cAvidaContext& ctx) 
{ 
  return m_testcpu->GetDemeResources(deme_id, ctx); 
}

const tArray<double>& cTestCPUInterface::GetCellResources(int cell_id, cAvidaContext& ctx) 
{
  return m_testcpu->GetCellResources(cell_id, ctx); 
}

const tArray<double>& cTestCPUInterface::GetAVResources(cAvidaContext& ctx) 
{
  return m_testcpu->GetAVResources(ctx); 
}

const tArray<double>& cTestCPUInterface::GetFrozenResources(cAvidaContext& ctx, int cell_id) 
{
  return m_testcpu->GetFrozenResources(ctx, cell_id); 
}

const tArray< tArray<int> >& cTestCPUInterface::GetCellIdLists()
{
	return m_testcpu->GetCellIdLists();
}

void cTestCPUInterface::UpdateResources(cAvidaContext& ctx, const tArray<double>& res_change)
{
   m_testcpu->ModifyResources(ctx, res_change);
}

void cTestCPUInterface::UpdateAVResources(cAvidaContext& ctx, const tArray<double>& res_change)
{
  m_testcpu->ModifyResources(ctx, res_change);
}

void cTestCPUInterface::Die(cAvidaContext& ctx) 
{
}

void cTestCPUInterface::KillCellID(int target, cAvidaContext& ctx) 
{
}

void cTestCPUInterface::Kaboom(int distance, cAvidaContext& ctx)
{
  (void) distance;
  // @CAO We should keep a note that the organism tried to explode, and
  // record the probability it used.
}

void cTestCPUInterface::SpawnDeme(cAvidaContext& ctx)
{
}

int cTestCPUInterface::ReceiveValue()
{
  return m_testcpu->GetReceiveValue();
}

void cTestCPUInterface::SellValue(const int data, const int label, const int sell_price, const int org_id)
{

}

int cTestCPUInterface::BuyValue(const int label, const int buy_price)
{
	return m_testcpu->GetReceiveValue();
}

bool cTestCPUInterface::InjectParasite(cOrganism* host, cBioUnit* parent, const cString& label, const Sequence& injected_code)
{
  return false;
}

bool cTestCPUInterface::UpdateMerit(double new_merit)
{
  m_test_info.GetTestPhenotype(m_cur_depth).SetMerit(cMerit(new_merit));
  return true;
}

int cTestCPUInterface::GetStateGridID(cAvidaContext& ctx)
{
  return m_test_info.GetStateGridID();
}

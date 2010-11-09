/*
 *  cTestCPUOrgInterface.cc
 *  Avida
 *
 *  Created by David on 3/4/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#include "cTestCPUInterface.h"

#include "cOrganism.h"
#include "cTestCPU.h"


bool cTestCPUInterface::Divide(cAvidaContext& ctx, cOrganism* parent, const cGenome& offspring_genome)
{
  parent->GetPhenotype().TestDivideReset(parent->GetGenome().GetSequence());
  // @CAO in the future, we probably want to pass this offspring the test_cpu!
  return true;
}

cOrganism* cTestCPUInterface::GetNeighbor()
{
  return NULL;
}

bool cTestCPUInterface::IsNeighborCellOccupied() {
  return false;
}

int cTestCPUInterface::GetNumNeighbors()
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

const tArray<double>& cTestCPUInterface::GetResources()
{
  return m_testcpu->GetResources();  
}

const tArray<double>& cTestCPUInterface::GetDemeResources(int deme_id) {
  return m_testcpu->GetDemeResources(deme_id);
}

const tArray< tArray<int> >& cTestCPUInterface::GetCellIdLists()
{
	return m_testcpu->GetCellIdLists();
}

void cTestCPUInterface::UpdateResources(const tArray<double>& res_change)
{
   m_testcpu->ModifyResources(res_change);
}

void cTestCPUInterface::Die()
{
}

void cTestCPUInterface::Kaboom(int distance)
{
  (void) distance;
  // @CAO We should keep a note that the organism tried to explode, and
  // record the probability it used.
}

void cTestCPUInterface::SpawnDeme()
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

bool cTestCPUInterface::InjectParasite(cOrganism* host, cBioUnit* parent, const cString& label, const cSequence& injected_code)
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

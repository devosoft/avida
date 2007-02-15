/*
 *  cTestCPUOrgInterface.cc
 *  Avida
 *
 *  Created by David on 3/4/06.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#include "cGenotype.h"
#include "cOrganism.h"
#include "cTestCPU.h"


bool cTestCPUInterface::Divide(cAvidaContext& ctx, cOrganism* parent, cGenome& child_genome)
{
  parent->GetPhenotype().TestDivideReset(parent->GetGenome().GetSize());
  // @CAO in the future, we probably want to pass this child the test_cpu!
  return true;
}

cOrganism* cTestCPUInterface::GetNeighbor()
{
  return NULL;
}

int cTestCPUInterface::GetNumNeighbors()
{
  return 0;
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

int cTestCPUInterface::Debug()
{
  return -1;
}

const tArray<double>& cTestCPUInterface::GetResources()
{
  return m_testcpu->GetResources();  
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

bool cTestCPUInterface::InjectParasite(cOrganism* parent, const cCodeLabel& label, const cGenome& injected_code)
{
  return false;
}

bool cTestCPUInterface::UpdateMerit(double new_merit)
{
  return false;
}

/*
 *  cTestCPUOrgInterface.cc
 *  Avida
 *
 *  Created by David on 3/4/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "cTestCPUInterface.h"

#include "cGenotype.h"
#include "cOrganism.h"
#include "cOrgMessage.h"
#include "cTestCPU.h"


bool cTestCPUInterface::Divide(cOrganism * parent, cGenome& child_genome)
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

double cTestCPUInterface::TestFitness()
{
  return -1.0;
}

int cTestCPUInterface::GetInput()
{
  return m_testcpu->GetInput();
}

int cTestCPUInterface::GetInputAt(int& input_pointer)
{
  return m_testcpu->GetInputAt(input_pointer);
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
}

void cTestCPUInterface::Die()
{
}

void cTestCPUInterface::Kaboom()
{
}

bool cTestCPUInterface::SendMessage(cOrgMessage & mess)
{
  return false;
}

int cTestCPUInterface::ReceiveValue()
{
  return m_testcpu->GetReceiveValue();
}

bool cTestCPUInterface::InjectParasite(cOrganism * parent, const cGenome & injected_code)
{
  return false;
}

bool cTestCPUInterface::UpdateMerit(double new_merit)
{
  return false;
}

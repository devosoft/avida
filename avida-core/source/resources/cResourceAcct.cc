//
//  cAbstractAccountant.cpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#include "cResourceAcct.h"

using namespace Avida::Resource;

TotalResAmount cAbstractSpatialResourceAcct::GetTotalAmount() const
{
  double sum = 0.0;
  for (int k = 0; k < m_abundance.GetSize(); k++){
    sum += m_abundance[k];
  }
  return sum;
}

void cAbstractSpatialResourceAcct::SetTotalAmount(const TotalResAmount& val)
{
  ResAmount frac = val / m_abundance.GetSize();
  for (int k = 0; k < m_abundance.GetSize(); k++){
    m_abundance[k] = frac;
  }
}


ResAmount cAbstractSpatialResourceAcct::GetCellAmount(int cell_id) const
{
  return m_abundance[cell_id];
}


void cAbstractSpatialResourceAcct::AddResource(ResAmount amount)
{
  ResAmount delta = amount / GetSize();
  for (int k=0; k<GetSize(); k++)
  {
    m_abundance[k] = (m_abundance[k] + delta >= 0.0) ? m_abundance[k] + delta : 0.0;
  }
}

void cAbstractSpatialResourceAcct::ScaleResource(double scale)
{
  assert(scale > 0.0);
  for (int k=0; k<GetSize(); k++)
  {
    m_abundance[k] *= m_abundance[k]*scale;
  }
}

void cAbstractSpatialResourceAcct::SetCellAmount(int cell_id, ResAmount amount) 
{ 
  m_abundance(cell_id) = amount;  //Will the compiler chose the one with the reference?
}

ResAmount cAbstractSpatialResourceAcct::operator[](int cell_id) const
{
  return m_abundance[cell_id];
}


ResAmount cAbstractSpatialResourceAcct::operator()(int cell_id) const
{
  return m_abundance[cell_id];
}

ResAmount cAbstractSpatialResourceAcct::operator()(int x, int y) const
{
  return m_abundance(x,y);
}



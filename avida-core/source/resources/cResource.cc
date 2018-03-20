//
//  cResource.cpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#include "cResource.h"

using namespace Avida::Resource;

cResource::cResource(const cResource& _res)
: m_feedback(_res.m_feedback)
{
  *this = _res;
}

cResource& cResource::operator=(const cResource& _res)
{
  m_feedback = _res.m_feedback;
  m_id = _res.m_id;
  m_name = _res.m_name;
  m_cbox = _res.m_cbox;
  m_is_deme_resource = _res.m_is_deme_resource;
  m_is_org_resource = _res.m_is_org_resource;
  m_hgt_metabolize = _res.m_hgt_metabolize;
  m_is_collectable = _res.m_is_collectable;
  return *this;
}


void cResource::SetID(ResID id)
{
  m_id = id;
}

ResID cResource::GetID() const 
{ 
  return m_id; 
}

ResName cResource::GetName() const
{
  return m_name;
}

bool cResource::IsPresent(int cell_id) const
{
  return (m_cbox.IsDefined()) ? m_cbox.InCellBox(cell_id) : true;
}

bool cResource::IsPresent(int xx, int yy) const
{
  return (m_cbox.IsDefined()) ? m_cbox.InCellBox(xx,yy) : true;
}



cAnonymousRatedResource::cAnonymousRatedResource(const cAnonymousRatedResource& _res)
{
  *this = _res;
}

cAnonymousRatedResource& cAnonymousRatedResource::operator=(const cAnonymousRatedResource& _res)
{
  m_initial = _res.m_initial;
  m_inflow = _res.m_inflow;
  m_outflow = _res.m_outflow;
  m_decay = _res.m_decay;
  return *this;
}


void cAnonymousRatedResource::SetInitial(ResAmount initial)
{
  m_initial = initial;
}

void cAnonymousRatedResource::SetInflow(ResAmount inflow)
{
  m_inflow = inflow; 
}

void cAnonymousRatedResource::SetOutflow(ResRate outflow)
{
  m_outflow = outflow; m_decay = 1.0 - outflow;
}

void cAnonymousRatedResource::SetDecay(ResRate decay)
{
  m_outflow = 1.0 - decay; m_decay = decay;
}

ResAmount cAnonymousRatedResource::GetInitial() const
{
  return m_initial;
}

ResAmount cAnonymousRatedResource::GetInflow() const
{
  return m_inflow;
}

ResRate cAnonymousRatedResource::GetOutflow() const
{
  return m_outflow;
}

ResRate cAnonymousRatedResource::GetDecay() const
{
  return 1.0 - m_outflow;
}



cRatedResource::cRatedResource(const cRatedResource& _res)
: cResource(_res)
, cAnonymousRatedResource(_res)
{
}

cRatedResource& cRatedResource::operator=(const cRatedResource& _res)
{
  cResource::operator=(_res);
  cAnonymousRatedResource::operator=(_res);
  return *this;
}

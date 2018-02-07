//
//  cAbstractResource.cpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#include "cResource.h"


cAbstractResource::cAbstractResource(const cAbstractResource& _res)
: m_feedback(_res.m_feedback)
{
  *this = _res;
}

cAbstractResource& cAbstractResource::operator=(const cAbstractResource& _res)
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

cRatedResource::cRatedResource(const cRatedResource& _res)
: cAbstractResource(_res)
{
  *this = _res;
}

cRatedResource& cRatedResource::operator=(const cRatedResource& _res)
{
  this->cAbstractResource::operator=(_res);
  m_initial = _res.m_initial;
  m_inflow = _res.m_inflow;
  m_outflow = _res.m_outflow;
  m_decay = _res.m_decay;
  return *this;
}

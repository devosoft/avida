//
//  cCellResource.cpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/16/18.
//

#include "cCellResource.h"

cCellResource::cCellResource(const cCellResource& _res)
: cRatedResource(_res)
{
  *this = _res;
}

cCellResource& cCellResource::operator=(const cCellResource& _res)
{
  this->cRatedResource::operator=(_res);
  m_cell_id = _res.m_cell_id;
  return *this;
}

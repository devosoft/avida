//
//  cCellResource.cpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/16/18.
//

#include "cCellResource.h"
#include <string>



cCellResource::cCellResource(int cell_id)
: m_cell_id(cell_id)
{
}
    

cCellResource::cCellResource(const cCellResource& _res)
: cAnonymousRatedResource(_res)
{
  *this = _res;
}

cCellResource& cCellResource::operator=(const cCellResource& _res)
{
  cAnonymousRatedResource::operator=(_res);
  m_cell_id = _res.m_cell_id;
  return *this;
}



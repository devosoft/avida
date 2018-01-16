//
//  cCellResource.hpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/16/18.
//

#ifndef cCellResource_h
#define cCellResource_h

#include "cAbstractResource.h"

class cCellResource : public cRatedResource
{
  protected:
    int m_cell_id;
    
  public:
    cCellResource(int cell_id, double initial, double inflow, double outflow)
    : cRatedResource(initial, inflow, outflow)
    , m_cell_id(cell_id)
    {}
    
    int GetCellID() const { return m_cell_id; }
}

#endif /* cCellResource_h */

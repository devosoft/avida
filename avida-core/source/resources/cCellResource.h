//
//  cCellResource.hpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/16/18.
//

#ifndef cCellResource_h
#define cCellResource_h

#include "cResource.h"
#include "cResourceAcct.h"

class cCellResource : public cRatedResource
{
  protected:
    int m_cell_id;
    
  public:
    explicit cCellResource(int cell_id, Avida::Feedback& fb)
    : cRatedResource(-1, "(cell)", fb)
    , m_cell_id(cell_id)
    {}
    
    cCellResource(const cCellResource& _res);
    cCellResource& operator=(const cCellResource& _res);
    
    virtual ~cCellResource() {}
    
    ResID GetCellID() const { return m_cell_id; }
    
    virtual ResDescr ToString() const { return "Not implemented."; }
};



#endif /* cCellResource_h */

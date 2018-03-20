//
//  cCellResource.hpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/16/18.
//

#ifndef cCellResource_h
#define cCellResource_h

#include "cResource.h"

namespace Avida {
  namespace Resource {
    class cCellResource : public cAnonymousRatedResource
    {
      
    protected:
      int m_cell_id;
      
    public:
      
      explicit cCellResource(int cell_id);
      cCellResource(const cCellResource& _res);
      cCellResource& operator=(const cCellResource& _res);
      
      virtual ~cCellResource() override {}
      
      inline ResID GetCellID() const { return m_cell_id; }
      
    };
  }
}




#endif /* cCellResource_h */

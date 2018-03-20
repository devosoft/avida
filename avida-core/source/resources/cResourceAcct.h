//
//  cAbstractAccountant.hpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#ifndef cResourceAcct_h
#define cResourceAcct_h

#include "cResource.h"
#include "cSpatialCountElem.h"
#include "cOffsetLinearGrid.h"
#include "resources/Types.h"

class cAvidaContext;

namespace Avida {
  namespace Resource {
    
    using cCellElements =  Apto::Array<cSpatialCountElem>;
    using cResourceGrid = Apto::Array<CellResAmount>;
    

    
    class cResourceAcct
    { 
    public:
      
      virtual TotalResAmount GetTotalAmount() const = 0;
      virtual void SetTotalAmount(const TotalResAmount& ) = 0;
      virtual ResAmount GetCellAmount(int cell_id) const = 0;
      virtual void AddResource(ResAmount amount) = 0;
      virtual void ScaleResource(double scale) = 0;
      virtual ResAmount operator[](int cell_id) const = 0;
      virtual ResAmount operator()(int cell_id) const = 0;
      virtual ResAmount operator()(int x, int y) const = 0;
      
      virtual void Update(cAvidaContext& ctx) = 0;
    };
    
    
    
    class cAbstractSpatialResourceAcct : public cResourceAcct
    {
    protected:
      cOffsetLinearGrid<CellResAmount> m_abundance;
      
      
    public:
      cAbstractSpatialResourceAcct(const cResource& res)
      : m_abundance(res.m_cbox, 0.0)
      {
      }
      
      virtual TotalResAmount GetTotalAmount() const override;  
      virtual void SetTotalAmount(const TotalResAmount& val) override;
      
      virtual ResAmount GetCellAmount(int cell_id) const override;
      void SetCellAmount(int cell_id, ResAmount amount);
      
      virtual void AddResource(ResAmount amount) override;
      
      virtual void ScaleResource(double scale) override;
      
      virtual ResAmount operator[](int cell_id) const override;
      virtual ResAmount operator()(int cell_id) const override;
      virtual ResAmount operator()(int x, int y) const override;
      
      inline int GetSize() const;
    };
    
    inline int cAbstractSpatialResourceAcct::GetSize() const
    {
      return m_abundance.GetSize();
    }
    
    
  }
}

#endif /* cAbstractAccountant_h */

//
//  cNonSpatialResource.hpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#ifndef cNonSpatialResource_h
#define cNonSpatialResource_h

#include <sstream>

#include "cResource.h"
#include "cResourceAcct.h"



namespace Avida {
  namespace Resource {
    
    class cNonSpatialResource : public cRatedResource
    {
      friend cNonSpatialResourceAcct;
      
      std::unique_ptr<cNonSpatialResourceAcct> m_acct;
      
      
    public:
      explicit cNonSpatialResource(ResID res_id, const cString& name, 
                                   const cOffsetCellBox& cbox, Avida::Feedback& fb);
      
      virtual ~cNonSpatialResource() {}
      
      cNonSpatialResource(const cNonSpatialResource& _res);
      cNonSpatialResource& operator=(const cNonSpatialResource& _res);
      
      virtual ResDescr ToString() const override;
      
      virtual cResourceAcct* GetBasicAcct() const override;
      
      cNonSpatialResourceAcct* GetFullAcct() const;
    };
    
    
    
    class cNonSpatialResourceAcct : public cResourceAcct
    {  
    protected:
      
      const cNonSpatialResource& m_resource;
      
      static const double UPDATE_STEP;
      static const double EPSILON;
      static const int PRECALC_DISTANCE;
      
      static double m_update_time;
      static int m_calc_steps;
      
      TotalResAmount m_current;
      
      Apto::Array<double> m_decay_precalc;
      Apto::Array<double> m_inflow_precalc;
      
    public:
      
      explicit cNonSpatialResourceAcct(const cNonSpatialResource& res);
      cNonSpatialResourceAcct(const cNonSpatialResourceAcct&) = delete;
      cNonSpatialResourceAcct& operator=(const cNonSpatialResourceAcct&) = delete;
      
      virtual ~cNonSpatialResourceAcct() {}
      
      virtual TotalResAmount GetTotalAmount() const override;
      
      virtual void SetTotalAmount(const TotalResAmount& amount) override;
      
      virtual ResAmount GetCellAmount(int cell_id) const override;
      
      virtual void AddResource(ResAmount amount) override;
      
      virtual void ScaleResource(double scale) override;
      
      virtual ResAmount operator[](int cell_id) const override;
      
      virtual ResAmount operator()(int cell_id) const override;
      
      virtual ResAmount operator()(int x, int y) const override;
      
      virtual void Update(cAvidaContext& ctx) override;
      
      static void AddTime(double tt);
      
    };
    
    
    TotalResAmount cNonSpatialResourceAcct::GetTotalAmount() const 
    { 
      return m_current; 
    }
    
    void cNonSpatialResourceAcct::SetTotalAmount(const TotalResAmount& amount) 
    { 
      m_current = amount; 
    }
    
    ResAmount cNonSpatialResourceAcct::GetCellAmount(int cell_id) const 
    { 
      return (m_resource.IsPresent(cell_id)) ? m_current : 0.0; 
    }
    
    void cNonSpatialResourceAcct::AddResource(ResAmount amount)
    {
      m_current = (m_current + amount >= 0.0) ? m_current+amount : 0.0;
    }
    
    void cNonSpatialResourceAcct::ScaleResource(double scale)
    {
      assert(scale > 0.0);
      m_current = m_current * scale;
    }
    
    ResAmount cNonSpatialResourceAcct::operator[](int cell_id) const
    {
      return GetCellAmount(cell_id);
    }
    
    ResAmount cNonSpatialResourceAcct::operator()(int cell_id) const
    {
      return GetCellAmount(cell_id);
    }
    
    ResAmount cNonSpatialResourceAcct::operator()(int x, int y) const
    {
      return GetCellAmount(y*x+x);
    }
    
  }
}




#endif /* cNonSpatialResource_hpp */

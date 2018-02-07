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




class cNonSpatialResource : public cRatedResource
{
  friend cNonSpatialResourceAcct;
  
  public:
    explicit cNonSpatialResource(int id, const cString& name, Avida::Feedback& fb) 
    : cRatedResource(id, name, fb) 
    {}
  
    virtual ~cNonSpatialResource() {}
    
    cNonSpatialResource(const cNonSpatialResource& _res);
    cNonSpatialResource& operator=(const cNonSpatialResource& _res);
    
    virtual ResDescr ToString() const;
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
  
    ResAmount m_current;
    
    Apto::Array<double> m_decay_precalc;
    Apto::Array<double> m_inflow_precalc;
    
  public:
  
    explicit cNonSpatialResourceAcct(const cNonSpatialResource& res);
    cNonSpatialResourceAcct(const cNonSpatialResourceAcct&) = delete;
    cNonSpatialResourceAcct& operator=(const cNonSpatialResourceAcct&) = delete;
    
    virtual ~cNonSpatialResourceAcct() {}
    
    
    
    virtual ResAmount GetTotalAmount() const { return m_current; }
    
    virtual void SetTotalAmount(const ResAmount& amount) { m_current = amount; }
    
    virtual ResAmount GetCellAmount(int cell_id) const 
    { 
      return (m_resource.IsPresent(cell_id)) ? m_current : 0.0; 
    }
    
    virtual void AddResource(ResAmount amount)
    {
      m_current = (m_current + amount >= 0.0) ? m_current+amount : 0.0;
    }
    
    virtual void ScaleResource(double scale)
    {
      assert(scale > 0.0);
      m_current = m_current * scale;
    }
    
    virtual ResAmount operator[](int cell_id) const
    {
      return GetCellAmount(cell_id);
    }
    
    virtual ResAmount operator()(int cell_id) const
    {
      return GetCellAmount(cell_id);
    }
    
    virtual ResAmount operator()(int x, int y) const
    {
      return GetCellAmount(y*x+x);
    }
    
    virtual void Update();
    
    static void AddTime(double tt);
    
};


#endif /* cNonSpatialResource_hpp */

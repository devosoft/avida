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
  
  private:
    cNonSpatialResource();
    cNonSpatialResource(const cNonSpatialResource&);
    cNonSpatialResource& operator=(const cNonSpatialResource&);
    
  protected:
    cNonSpatialResourceAcct* m_accountant;
  
  public:
    cNonSpatialResource(int id, const cString& name, Avida::Feedback& fb) 
    : cRatedResource(id, name, fb) 
    , m_accountant(nullptr)
    {}
  
    virtual ~cNonSpatialResource() {}
    
    cString ToString() const
    {
      std::ostringstream sot;
      sot << "RESOURCE " << m_name << ":"
          << "initial=" << m_initial << ":"
          << "inflow=" << m_inflow << ":"
          << "outflow=" << m_outflow;
      return cString(sot.str().c_str());
    }
    
    virtual void AddAcctountant(cNonSpatialResourceAcct* acct)
    {
      m_accountant = acct;
    }
    
    virtual cNonSpatialResourceAcct* GetAccountant()
    {
      return m_accountant;
    }
    
  
};


class cNonSpatialResourceAcct : public cAbstractResourceAcct
{  
  protected:
    
    const cNonSpatialResource& m_resource;
    
    static const double UPDATE_STEP;
    static const double EPSILON;
    static const int PRECALC_DISTANCE;
    
    static double m_update_time;
    static int m_calc_steps;
  
    double m_current;
    
    Apto::Array<double> m_decay_precalc;
    Apto::Array<double> m_inflow_precalc;
    
  public:
  
    cNonSpatialResourceAcct(const cNonSpatialResource& res);
    
    virtual ~cNonSpatialResourceAcct() {}
    
    double GetTotalAbundance() const { return m_current; }
    
    virtual double GetCellAbundance(int cell_id) const 
    { 
      return (m_resource.IsPresent(cell_id)) ? m_current : 0.0; 
    }
    
    virtual void AddResource(double amount)
    {
      m_current = (m_current + amount >= 0.0) ? m_current+amount : 0.0;
    }
    
    virtual void ScaleResource(double scale)
    {
      assert(scale > 0.0);
      m_current = m_current * scale;
    }
    
    void Update();
    
    static void AddTime(double tt);
    
};


#endif /* cNonSpatialResource_hpp */

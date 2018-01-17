//
//  cNonSpatialResource.hpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#ifndef cNonSpatialResource_h
#define cNonSpatialResource_h

#include <sstream>

#include "cAbstractResource.h"
#include "cAbstractResourceAcct.h"


class cNonSpatialResource : public cRatedResource
{
  friend cNonSpatialResourceAcct;
  
  private:
    cNonSpatialResource();
    cNonSpatialResource(const cNonSpatialResource&);
    cNonSpatialResource& operator=(const cNonSpatialResource&);
    
  protected:
  
  public:
    cNonSpatialResource(int id, const cString& name, Avida::Feedback& fb) 
    : cRatedResource(id, name, fb) 
    {}
    
    cString ToString() const
    {
      std::ostringstream sot;
      sot << "RESOURCE " << m_name << ":"
          << "initial=" << m_initial << ":"
          << "inflow=" << m_inflow << ":"
          << "outflow=" << m_outflow;
      return cString(sot.str().c_str());
    }
  
};


class cNonSpatialResourceAcct : public cAbstractResourceAcct
{
  friend cNonSpatialResource;
  
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
    
    double GetTotalAbundance() const { return m_current; }
    
    virtual double GetCellAbundance(int cell_id) const 
    { 
      return (m_resource.IsPresent(cell_id)) ? m_current : 0.0; 
    }
    
    void Update();
    
    static void AddTime(double tt);
    
};

#endif /* cNonSpatialResource_hpp */

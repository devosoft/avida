//
//  cAbstractResource.h
//  Avida-Core
//
//  Created by Matthew Rupp on 1/14/18.
//

#ifndef cAbstractResource_h
#define cAbstractResource_h

#include "cString.h"
#include "cCellBox.h"

class cAbstractResoureAcct;
class cNonSpatialResourceAcct;
class cSpatialResourceAcct;
class cGradientResourceAcct;
  

class cAbstractResource
{

  const static int NONE = -99;

  friend cNonSpatialResourceAcct;
  friend cSpatialResourceAcct;
  friend cGradientResourceAcct;
  
  public:
    #define ADD_RESOURCE_PROP(TYPE, NAME, M_NAME) \
    void Set ## NAME(TYPE k) { M_NAME = k ;} \
    TYPE Get ## NAME () const { return M_NAME ;}
    
  private:
    cAbstractResource();
    cAbstractResource(const cAbstractResource&);
    cAbstractResource& operator=(const cAbstractResource&);

  protected:
    int m_id;
    cString m_name;
    cCellBox m_cbox;
    bool m_is_deme_resource;
    bool m_is_org_resource;
    bool m_hgt_metabolize;
    bool m_is_collectable;
    bool m_is_deadly;
    bool m_is_path;
    bool m_is_hammer;
    double m_damage;
    double m_prob_detect;
    
  public:
    cAbstractResource(int id, const cString& name="")
    : m_id(id)
    , m_name(name)
    {}
    
    void SetCellBox(const cCellBox& cbox) { m_cbox = cbox; }
    cCellBox GetCellBox() { return m_cbox; }
    
    
    ADD_RESOURCE_PROP(bool, AsDemeResource, m_is_deme_resource);
    ADD_RESOURCE_PROP(bool, AsOrgResource, m_is_org_resource);
    ADD_RESOURCE_PROP(bool, HGTMetabolize, m_hgt_metabolize);
    ADD_RESOURCE_PROP(bool, IsCollectable, m_is_collectable);
    ADD_RESOURCE_PROP(bool, IsDeadly, m_is_deadly);
    ADD_RESOURCE_PROP(bool, IsHammer, m_is_hammer);
    
    ADD_RESOURCE_PROP(double, ProbDetect, m_prob_detect);
    
  
    bool IsPresent(int cell_id, int world_x, int world_y) const
    {
      return (m_cbox.IsDefined()) ? m_cbox.InCellBox(cell_id, world_x, world_y) : true;
    }
    
    bool IsPresent(int xx, int yy) const
    { 
      return (m_cbox.IsDefined()) ? m_cbox.InCellBox(xx,yy) : true;
    }
    
    virtual cString ToString() const = 0;
};


class cRatedResource : public cAbstractResource
{
  friend cNonSpatialResourceAcct;
  friend cSpatialResourceAcct;
  
  private:
    cRatedResource();
    cRatedResource(const cRatedResource&);
    cRatedResource& operator=(const cRatedResource&);

  protected:
    double m_initial;
    double m_inflow;
    double m_outflow;
    double m_decay;
    
    
  public:
    cRatedResource(int id, const cString& name="") 
    : cAbstractResource(id, name) {};
    
    void SetInitial(double initial) { m_initial = initial; }
    void SetInflow(double inflow) { m_inflow = inflow; }
    void SetOutflow(double outflow) { m_outflow = outflow; m_decay = 1.0 - outflow;}
    void SetDecay(double decay) { m_outflow = 1.0 - decay; m_decay = decay;}
    
    double GetInitial() const { return m_initial; }
    double GetInflow() const { return m_inflow; }
    double GetOutflow() const { return m_outflow; }
    double GetDecay() const { return 1.0 - m_outflow; }
};

#endif /* cAbstractResource_h */

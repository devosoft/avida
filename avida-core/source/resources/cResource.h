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
#include "avida/core/Feedback.h"
#include "resources/Types.h"

class cResoureAcct;
class cNonSpatialResourceAcct;
class cSpatialResourceAcct;
class cGradientResourceAcct;


class cAbstractResource
{

  const static int NONE = -99;

  // All accountants need to be friends since parent's aren't a child's friend
  friend cNonSpatialResourceAcct;
  friend cSpatialResourceAcct;
  friend cGradientResourceAcct;
  
  public:
    // A macro to quickly add properties
    #define ADD_RESOURCE_PROP(TYPE, NAME, M_NAME) \
    void Set ## NAME(TYPE k) { M_NAME = k ;} \
    TYPE Get ## NAME () const { return M_NAME ;}
    

  protected:
    Avida::Feedback& m_feedback;
    
    ResID m_id;
    ResName m_name;
    cOffsetCellBox m_cbox;
    bool m_is_deme_resource;
    bool m_is_org_resource;
    bool m_hgt_metabolize;
    bool m_is_collectable;
    
  public:
    explicit cAbstractResource(ResID id, const ResName& name, Avida::Feedback& fb)
    : m_feedback(fb)
    , m_id(id)
    , m_name(name)
    , m_is_deme_resource(false)
    , m_hgt_metabolize(false)
    , m_is_collectable(false)
    {}
    
    cAbstractResource(const cAbstractResource&);
    cAbstractResource& operator=(const cAbstractResource&);
    
    virtual ~cAbstractResource()
    {}
    
    ResID GetID() const { return m_id; }
    ResName GetName() const { return m_name; }
    
    void SetCellBox(const cOffsetCellBox& cbox) { m_cbox = cbox; }
    cOffsetCellBox GetCellBox() { return m_cbox; }
    
    
    ADD_RESOURCE_PROP(bool, AsDemeResource, m_is_deme_resource);
    ADD_RESOURCE_PROP(bool, AsOrgResource, m_is_org_resource);
    ADD_RESOURCE_PROP(bool, HGTMetabolize, m_hgt_metabolize);
    ADD_RESOURCE_PROP(bool, IsCollectable, m_is_collectable);
    
  
    bool IsPresent(int cell_id) const
    {
      return (m_cbox.IsDefined()) ? m_cbox.InCellBox(cell_id) : true;
    }
    
    bool IsPresent(int xx, int yy) const
    { 
      return (m_cbox.IsDefined()) ? m_cbox.InCellBox(xx,yy) : true;
    }
    
    virtual ResDescr ToString() const = 0;
};


class cRatedResource : public cAbstractResource
{
  friend cNonSpatialResourceAcct;
  friend cSpatialResourceAcct;


  protected:
    ResAmount m_initial;
    ResAmount m_inflow;
    ResRate m_outflow;
    ResRate m_decay;
    
    
  public:
    explicit cRatedResource(ResID id, const ResName& name, Avida::Feedback& fb) 
    : cAbstractResource(id, name, fb) 
    {};
    
    cRatedResource(const cRatedResource& _res);
    cRatedResoure& operator=(const cRatedResource& _res);
    
    virtual ~cRatedResource() {}
    
    void SetInitial(ResAmount initial) { m_initial = initial; }
    void SetInflow(ResAmount inflow) { m_inflow = inflow; }
    void SetOutflow(ResRate outflow) { m_outflow = outflow; m_decay = 1.0 - outflow;}
    void SetDecay(ResRate decay) { m_outflow = 1.0 - decay; m_decay = decay;}
    
    ResAmount GetInitial() const { return m_initial; }
    ResAmount GetInflow() const { return m_inflow; }
    ResRate GetOutflow() const { return m_outflow; }
    ResRate GetDecay() const { return 1.0 - m_outflow; }
};

#endif /* cAbstractResource_h */

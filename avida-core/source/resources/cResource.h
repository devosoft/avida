//
//  cResource.h
//  Avida-Core
//
//  Created by Matthew Rupp on 1/14/18.
//

#ifndef cResource_h
#define cResource_h

#include "cString.h"
#include "cCellBox.h"
#include "avida/core/Feedback.h"
#include "resources/Types.h"

class cResoureAcct;
class cNonSpatialResourceAcct;
class cSpatialResourceAcct;
class cGradientResourceAcct;


class cResource
{

  const static int NONE = -99;

  // All accountants need to be friends since parent's aren't a child's friend
  friend cNonSpatialResourceAcct;
  friend cSpatialResourceAcct;
  friend cGradientResourceAcct;
  
  public:
    // A macro to quickly add properties
    //TYPE is the type of the property
    //NAME proces a GetNAME (const) and SetNAME method
    //M_NAME is the data member being set.
    //Child classes may also use this macro.
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
    explicit cResource(ResID id, const ResName& name, Avida::Feedback& fb)
    : m_feedback(fb)
    , m_id(id)
    , m_name(name)
    , m_is_deme_resource(false)
    , m_hgt_metabolize(false)
    , m_is_collectable(false)
    {}
    
    cResource(const cResource&);
    cResource& operator=(const cResource&);
    
    virtual ~cResource()
    {}
    
    ResID GetID() const;
    ResName GetName() const;
    
    void SetCellBox(const cOffsetCellBox& cbox);
    cOffsetCellBox GetCellBox();
    
    //The ADD_RESOURCE_PROP macro adds a Get (const) and Set method
    //for each of the named properties.  For example GetIsCollectable
    //and SetIsCollectable are generated.
    ADD_RESOURCE_PROP(bool, IsDemeResource, m_is_deme_resource);
    ADD_RESOURCE_PROP(bool, IsOrgResource, m_is_org_resource);
    ADD_RESOURCE_PROP(bool, IsHGTMetabolize, m_hgt_metabolize);
    ADD_RESOURCE_PROP(bool, IsCollectable, m_is_collectable);
    
  
    bool IsPresent(int cell_id) const;
    bool IsPresent(int xx, int yy) const;
    
    virtual ResDescr ToString() const = 0;
};

ResID cResource::GetID() const 
{ 
  return m_id; 
}

ResName cResource::GetName() const
{
  return m_name;
}

bool cResource::IsPresent(int cell_id) const
{
  return (m_cbox.IsDefined()) ? m_cbox.InCellBox(cell_id) : true;
}

bool cResource::IsPresent(int xx, int yy) const
{
  return (m_cbox.IsDefined()) ? m_cbox.InCellBox(xx,yy) : true;
}





class cRatedResource : public cResource
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
    : cResource(id, name, fb) 
    {};
    
    cRatedResource(const cRatedResource& _res);
    cRatedResource& operator=(const cRatedResource& _res);
    
    virtual ~cRatedResource() override {}
    
    void SetInitial(ResAmount initial);
    void SetInflow(ResAmount inflow);
    void SetOutflow(ResRate outflow);
    void SetDecay(ResRate decay);
    
    ResAmount GetInitial() const;
    ResAmount GetInflow() const;
    ResRate GetOutflow() const;
    ResRate GetDecay() const;
};


void cRatedResource::SetInitial(ResAmount initial)
{
  m_initial = initial;
}

void cRatedResource::SetInflow(ResAmount inflow)
{
  m_inflow = inflow; 
}

void cRatedResource::SetOutflow(ResRate outflow)
{
  m_outflow = outflow; m_decay = 1.0 - outflow;
}

void cRatedResource::SetDecay(ResRate decay)
{
  m_outflow = 1.0 - decay; m_decay = decay;
}

ResAmount cRatedResource::GetInitial() const
{
  return m_initial;
}

ResAmount cRatedResource::GetInflow() const
{
  return m_inflow;
}

ResRate cRatedResource::GetOutflow() const
{
  return m_outflow;
}

ResRate cRatedResource::GetDecay() const
{
  return 1.0 - m_outflow;
}


#endif /* cResource_h */

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



namespace Avida {
  namespace Resource{
    
    class cResoureAcct;
    class cAbstractSpatialResourceAcct;
    class cNonSpatialResourceAcct;
    class cSpatialResourceAcct;
    class cGradientResourceAcct;
    
    class cResource
    {
      
      const static int NONE = -99;
      
      // All accountants need to be friends since parent's aren't a child's friend
      friend cResourceAcct;
      friend cAbstractSpatialResourceAcct;
      friend cNonSpatialResourceAcct;
      friend cSpatialResourceAcct;
      friend cGradientResourceAcct;
      
      friend cResourceRegistry;
      
      
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
      
      void SetID(ResID id);
      
    public:
      explicit cResource(ResID res_id, const ResName& name, Avida::Feedback& fb)
      : m_feedback(fb)
      , m_id(res_id)
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
      
      
      //The ADD_RESOURCE_PROP macro adds a Get (const) and Set method
      //for each of the named properties.  For example GetIsCollectable
      //and SetIsCollectable are generated.
      ADD_RESOURCE_PROP(cOffsetCellBox, CellBox, m_cbox);
      ADD_RESOURCE_PROP(bool, IsDemeResource, m_is_deme_resource);
      ADD_RESOURCE_PROP(bool, IsOrgResource, m_is_org_resource);
      ADD_RESOURCE_PROP(bool, IsHGTMetabolize, m_hgt_metabolize);
      ADD_RESOURCE_PROP(bool, IsCollectable, m_is_collectable);
      
      
      bool IsPresent(int cell_id) const;
      bool IsPresent(int xx, int yy) const;
      
      virtual ResDescr ToString() const = 0;
      
      virtual cResourceAcct* GetBasicAcct() const = 0;
    };
    
    
    
    class cAnonymousRatedResource
    {
      friend cNonSpatialResourceAcct;
      friend cSpatialResourceAcct;
      
    protected:
      ResAmount m_initial;
      ResAmount m_inflow;
      ResRate m_outflow;
      ResRate m_decay;
      
    public:
      
      cAnonymousRatedResource();
      cAnonymousRatedResource(const cAnonymousRatedResource& _res);
      cAnonymousRatedResource& operator=(const cAnonymousRatedResource& _res);
      
      virtual ~cAnonymousRatedResource();
      
      void SetInitial(ResAmount initial);
      void SetInflow(ResAmount inflow);
      void SetOutflow(ResRate outflow);
      void SetDecay(ResRate decay);
      
      ResAmount GetInitial() const;
      ResAmount GetInflow() const;
      ResRate GetOutflow() const;
      ResRate GetDecay() const;
      
    };
    
    
    class cRatedResource : public cResource, public cAnonymousRatedResource
    {
      friend cNonSpatialResourceAcct;
      friend cSpatialResourceAcct;
      
    public:
      explicit cRatedResource(ResID res_id, const ResName& name, Avida::Feedback& fb) 
      : cResource(res_id, name, fb) 
      {};
      
      cRatedResource(const cRatedResource& _res);
      cRatedResource& operator=(const cRatedResource& _res);
      
      virtual ~cRatedResource() override {}
    };  
    
  }
}


#endif /* cResource_h */

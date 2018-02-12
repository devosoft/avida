//
//  cSpatialResourceAcct.hpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#ifndef cSpatialResource_h
#define cSpatialResource_h

#include "cResource.h"
#include "cResourceAcct.h"
#include "cSpatialCountElem.h"
#include "cCellResource.h"
#include "nGeometry.h"

class cSpatialResource : public cRatedResource
{
  friend cSpatialResourceAcct;
  
  protected:
    int m_geometry;
    double m_diffuse_x;
    double m_diffuse_y;
    double m_gravity_x;
    double m_gravity_y;
    Apto::Array<cCellBox> m_inflow_boxes;
    Apto::Array<cCellBox> m_outflow_boxes;
    Apto::Array<cCellResource> m_cell_list;
    
  
  public:
    explicit cSpatialResource(int id, const cString& name, Avida::Feedback& fb)
    : cRatedResource(id, name, fb) 
    {}
    
    cSpatialResource(const cSpatialResource&);
    cSpatialResource& operator=(const cSpatialResource&);
    
    virtual ~cSpatialResource() override {}
    
    void AddInflowBox(int x1, int x2, int y1, int y2);
    void AddOutflowBox(int x1, int x2, int y1, int y2);
   
    void AddInflowCellBox(const cCellBox& cbox);
    void AddOutflowCellBox(const cCellBox& cbox);
    
    //Property generation macro in cResource.h
    ADD_RESOURCE_PROP(int, Geometry, m_geometry);
    ADD_RESOURCE_PROP(double, XDiffuse, m_diffuse_x);
    ADD_RESOURCE_PROP(double, YDiffuse, m_diffuse_y);
    ADD_RESOURCE_PROP(double, XGravity, m_gravity_x);
    ADD_RESOURCE_PROP(double, YGravity, m_gravity_y);
    
    virtual ResDescr ToString() const override {return "Not Implemented.";};
};




class cSpatialResourceAcct : public cAbstractSpatialResourceAcct
{
  
  protected:
     // A few constants to describe update process...
    static const double UPDATE_STEP;   // Fraction of an update per step
    static const double EPSILON;       // Tolorance for round off errors
    static const int PRECALC_DISTANCE; // Number of steps to precalculate
    
    // Temporal information
    static int& m_stats_update;
    static int m_last_calc_update;
    
    const cSpatialResource& m_resource;
    cOffsetLinearGrid<cSpatialCountElem> m_cells;
    
    virtual void Update() override;
    
  
  public:
  
    static void Initialize(int& stats_update);
    
    
    explicit cSpatialResourceAcct(cSpatialResource& res, int size_x, int size_y, const cCellBox& cellbox)
    : cAbstractSpatialResourceAcct(size_x, size_y, cellbox)
    , m_resource(res)
    , m_cells(size_x, size_y, cellbox, cSpatialCountElem(0.0))
    {
    }
    
    void SetupGeometry();
    
    void SetInflow(int cell_id, double inflow);
    void SetInflow(int x, int y, double inflow);
    void SetInflowAll(double inflow);
    
    void SetState(int cell_id);
    void SetState(int x, int y);
    void SetStateAll();
    
    void FlowAll();
    
    void StateAll();
    void State(int x);
    void State(int x, int y);
    
    void CellInflow();
    void Source(double amount);
    void Rate(int x, double ratein);
    void Rate(int x, int y, double ratein);
    
    void CellOutflow();
    void Sink(double decay);
    void ResetResourceCounts();
};



#endif /* cSpatialResourceAcct_h */

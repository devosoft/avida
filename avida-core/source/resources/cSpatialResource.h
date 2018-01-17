//
//  cSpatialResourceAcct.hpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#ifndef cSpatialResource_h
#define cSpatialResource_h

#include "cAbstractResource.h"
#include "cAbstractResourceAcct.h"
#include "cSpatialCountElem.h"
#include "cCellResource.h"
#include "nGeometry.h"

class cSpatialResource : public cRatedResource
{
  friend cSpatialResourceAcct;
  
  private:
    cSpatialResource();
    cSpatialResource(const cSpatialResource&);
    cSpatialResource& operator=(const cSpatialResource&);
  
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
    cSpatialResource(int id, const cString& name, Avida::Feedback& fb)
    : cRatedResource(id, name, fb) 
    {}
    
    void AddInflowBox(int x1, int x2, int y1, int y2)
    {
      int width = abs(x2-x1) + 1;
      int height = abs(y2-y1) + 1;
      m_inflow_boxes.Push(cCellBox(x1,y1,width,height));
    }
    
    void AddOutflowBox(int x1, int x2, int y1, int y2)
    {
      int width = abs(x2-x1) + 1;
      int height = abs(y2-y1) + 1;
      m_outflow_boxes.Push(cCellBox(x1,y1,width,height));
    }
    
    void AddInflowCellBox(const cCellBox& cbox)
    {
      m_inflow_boxes.Push(cbox);
    }
    
    void AddOutflowCellBox(const cCellBox& cbox)
    {
      m_outflow_boxes.Push(cbox);
    }
    
    ADD_RESOURCE_PROP(int, Geometry, m_geometry);
    ADD_RESOURCE_PROP(double, XDiffuse, m_diffuse_x);
    ADD_RESOURCE_PROP(double, YDiffuse, m_diffuse_y);
    ADD_RESOURCE_PROP(double, XGravity, m_gravity_x);
    ADD_RESOURCE_PROP(double, YGravity, m_gravity_y);
    
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
    
    virtual void Update();
    
  
  public:
  
    static void Initialize(int& stats_update)
    {
      m_stats_update = stats_update;
    }
    
    
    cSpatialResourceAcct(cSpatialResource& res, int size_x, int size_y, const cCellBox& cellbox)
    : cAbstractSpatialResourceAcct(size_x, size_y, cellbox)
    , m_resource(res)
    , m_cells(size_x, size_y, cellbox, cSpatialCountElem(0.0))
    {
    }
    
    void SetupGeometry();
    
    void SetInflow(int cell_id, double inflow) { m_cells(cell_id).Rate(inflow); }
    void SetInflow(int x, int y, double inflow) { m_cells(x,y).Rate(inflow); }
    void SetInflowAll(double inflow);
    
    void SetState(int cell_id) { m_cells(cell_id).State(); }
    void SetState(int x, int y) { m_cells(x,y).State(); }
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

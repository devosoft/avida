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

#include <memory>


namespace Avida {
  namespace Resource {
    
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
      Apto::Array< std::unique_ptr<cCellResource> > m_cell_list;
      
      
    public:
      explicit cSpatialResource(ResID res_id, const cString& name, Avida::Feedback& fb)
      : cRatedResource(res_id, name, fb) 
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
      
      virtual ResDescr ToString() const override {return "Not Implemented.";}
      
    };
    
    
    
    
    class cSpatialResourceAcct : public cAbstractSpatialResourceAcct
    {
      
    protected:
      // A few constants to describe update process...
      static const double UPDATE_STEP;   // Fraction of an update per step
      static const double EPSILON;       // Tolorance for round off errors
      static const int PRECALC_DISTANCE; // Number of steps to precalculate
      
      // Temporal information; all instances of this class share these
      static int& m_stats_update;     //Bound to Avida's update counter
      static int m_last_calc_update;        //The last update calculated
      
      const cSpatialResource& m_resource;
      
      /*
       To the outside looking into this accountant, m_cells behaves as if
       it is a full grid of data.  In reality the accounting system only
       keeps track of what is inside the offset cell box if it is valid/defined.
       Cell boxes that are not valid are considered unset and the entire
       grid is tracked.
       
       An offset cell box is defined by three types of parameters:
       The size of the grid to emulate full tracking (GetSizeX, GetSizeY accessors)
       The location of a corner of the cell box
       (GetX, GetY accessors; by convention the lowest value coordinates for
       these respective axes)
       The width and the height of the cell box from the X,Y coordinates
       (GetWidth, GetHeight).  In conditions where a toroidal topology wraps
       the cellbox around an axis edge, these values are negative.  The absolute
       value of width and height must be at least 1 unit each.
       If a cell box is not set (IsValid returns FALSE -- An undefined cell
       box is simply ignored) then the accounting system will keep track of
       the entire world.
       */
      cOffsetLinearGrid<cSpatialCountElem> m_cells;
      
      int m_size_x;
      int m_size_y;
      
      virtual void Update(cAvidaContext& ctx) override;
      
      
    public:
      
      static void Initialize(int& stats_update);
      
      
      explicit cSpatialResourceAcct(cSpatialResource& res)
      : cAbstractSpatialResourceAcct(res)
      , m_resource(res)
      , m_cells(res.m_cbox, cSpatialCountElem(0.0))
      , m_size_x(res.m_cbox.GetWidth())
      , m_size_y(res.m_cbox.GetHeight())
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
    
  }
}





#endif /* cSpatialResourceAcct_h */

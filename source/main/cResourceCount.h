//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_COUNT_HH
#define RESOURCE_COUNT_HH

#ifndef SPATIAL_RES_COUNT_HH
#include "cSpatialResCount.h"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef TARRAY_HH
#include "tArray.hh"
#endif
#ifndef TMATRIX_HH
#include "tMatrix.hh"
#endif

template <class T> class tArray; // aggregate
template <class T> class tMatrix; // aggregate
class cSpatialResCount; // aggregate
class cString; // aggregate

class cResourceCount {
private:
  mutable tArray<double> resource_count;  // Current quantity of each resource
  tArray<double> decay_rate;      // Multiplies resource count at each step
  tArray<double> inflow_rate;     // An increment for resource at each step
  tMatrix<double> decay_precalc;  // Precalculation of decay values
  tMatrix<double> inflow_precalc; // Precalculation of inflow values
  tArray<int> geometry;           // Spatial layout of each resource
  mutable tArray<cSpatialResCount> spatial_resource_count;
  mutable tArray<double> curr_grid_res_cnt;
  mutable tArray< tArray<double> > curr_spatial_res_cnt;

  // Setup the update process to use lazy evaluation...
  mutable double update_time;     // Portion of an update compleated...
  mutable double spatial_update_time;
  void DoUpdates() const;         // Update resource count based on update time

  // A few constants to describe update process...
  static const double UPDATE_STEP;   // Fraction of an update per step
  static const double EPSILON;       // Tolorance for round off errors
  static const int PRECALC_DISTANCE; // Number of steps to precalculate
public:
  cResourceCount(int num_resources=0);
  cResourceCount(const cResourceCount &);
  ~cResourceCount();

  const cResourceCount &operator=(const cResourceCount &);

  void SetSize(int num_resources);

  void Setup(int id, cString name, double initial, double inflow,
             double decay, int in_geometry, double in_xdiffuse,
             double in_xgravity, double in_ydiffuse,
             double in_ygravity, int in_inflowX1,
             int in_inflowX2, int in_inflowY1,
             int in_inflowY2, int in_outflowX1,
             int in_outflowX2, int in_outflowY1,
             int in_outflowY);
  void Update(double in_time);

  int GetSize(void) const { return resource_count.GetSize(); }
  const tArray<double> & ReadResources(void) const { return resource_count; }
  const tArray<double> & GetResources() const;
  const tArray<double> & GetCellResources(int cell_id) const;
  const tArray<int> & GetResourcesGeometry() const;
  const tArray< tArray<double> > & GetSpatialRes();
  void Modify(const tArray<double> & res_change);
  void Modify(int id, double change);
  void ModifyCell(const tArray<double> & res_change, int cell_id);
  void Set(int id, double new_level);
  double Get(int id) const{
    assert(id < resource_count.GetSize());
    return resource_count[id]; }
  void ResizeSpatialGrids(int in_x, int in_y);
  cSpatialResCount GetSpatialResource(int id)
       { return spatial_resource_count[id]; }
};

#endif

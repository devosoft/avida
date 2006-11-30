/*
 *  cSpatialResCount.h
 *  Avida
 *
 *  Called "spatial_res_count.hh" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 */

#ifndef cSpatialResCount_h
#define cSpatialResCount_h

#ifndef cSpatialCountElem_h
#include "cSpatialCountElem.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

#ifndef cResource_h
#include "cResource.h"
#endif

class cSpatialResCount
{
private:
  tArray<cSpatialCountElem> grid;
  double xdiffuse, xgravity, ydiffuse, ygravity;
  int    inflowX1, inflowX2, inflowY1, inflowY2;
  int    outflowX1, outflowX2, outflowY1, outflowY2;
  int    geometry;
  int    world_x, world_y, num_cells;
  /* instead of creating a new array use the existing one from cResource */
  tArray<cCellResource> *cell_list_ptr;
  
public:
  cSpatialResCount();
  cSpatialResCount(int inworld_x, int inworld_y, int ingeometry);
  cSpatialResCount(int inworld_x, int inworld_y, int ingeometry, 
                   double inxdiffuse, double inydiffuse,
                   double inxgravity, double inygravity);
  
  void ResizeClear(int inworld_x, int inworld_y, int ingeometry);
  void SetPointers();
  void CheckRanges();
  void SetCellList(tArray<cCellResource> *in_cell_list_ptr);
  int GetSize() { return grid.GetSize(); }
  int GetX() { return world_x; }
  int GetY() { return world_y; }
  int GetCellListSize() { return cell_list_ptr->GetSize(); }
  cSpatialCountElem Element(int x) { return grid[x]; }
  void Rate(int x, double ratein) const;
  void Rate(int x, int y, double ratein) const;
  void State(int x);
  void State(int x, int y);
  const double GetAmount(int x) const;
  const double GetAmount(int x, int y) const;
  void RateAll(double ratein);
  void StateAll();
  void FlowAll();
  const double SumAll() const;
  void Source(double amount) const;
  void CellInflow() const;
  void Sink(double percent) const;
  void CellOutflow() const;
  void SetGeometry(int in_geometry) { geometry = in_geometry; }
  void SetXdiffuse(double in_xdiffuse) { xdiffuse = in_xdiffuse; }
  void SetXgravity(double in_xgravity) { xgravity = in_xgravity; }
  void SetYdiffuse(double in_ydiffuse) { ydiffuse = in_ydiffuse; }
  void SetYgravity(double in_ygravity) { ygravity = in_ygravity; }
  void SetInflowX1(int in_inflowX1) { inflowX1 = in_inflowX1; }
  void SetInflowX2(int in_inflowX2) { inflowX2 = in_inflowX2; }
  void SetInflowY1(int in_inflowY1) { inflowY1 = in_inflowY1; }
  void SetInflowY2(int in_inflowY2) { inflowY2 = in_inflowY2; }
  void SetOutflowX1(int in_outflowX1) { outflowX1 = in_outflowX1; }
  void SetOutflowX2(int in_outflowX2) { outflowX2 = in_outflowX2; }
  void SetOutflowY1(int in_outflowY1) { outflowY1 = in_outflowY1; }
  void SetOutflowY2(int in_outflowY2) { outflowY2 = in_outflowY2; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nSpatialResCount {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

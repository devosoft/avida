//
//  cSpatialResourceAcct.cpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#include "cSpatialResource.h"
#include "AvidaTools.h"

using namespace AvidaTools;

void cSpatialResourceAcct::SetupGeometry()
{
    /* Pointer 0 will point to the cell above and to the left the current cell
     and will go clockwise around the cell.                               */

  int     i,ii;
  double  SQRT2 = sqrt(2.0);

  /* First treat all cells like they are in a torus */

  for (i = 0; i < m_cells.GetSize(); i++) {
    m_cells[i].SetPtr(0 ,GridNeighbor(i, m_size_x, m_size_y, -1, -1), -1, -1, SQRT2);
    m_cells[i].SetPtr(1 ,GridNeighbor(i, m_size_x, m_size_y,  0, -1),  0, -1, 1.0);
    m_cells[i].SetPtr(2 ,GridNeighbor(i, m_size_x, m_size_y, +1, -1), +1, -1, SQRT2);
    m_cells[i].SetPtr(3 ,GridNeighbor(i, m_size_x, m_size_y, +1,  0), +1,  0, 1.0);
    m_cells[i].SetPtr(4 ,GridNeighbor(i, m_size_x, m_size_y, +1, +1), +1, +1, SQRT2);
    m_cells[i].SetPtr(5 ,GridNeighbor(i, m_size_x, m_size_y,  0, +1),  0, +1, 1.0);
    m_cells[i].SetPtr(6 ,GridNeighbor(i, m_size_x, m_size_y, -1, +1), -1, +1, SQRT2);
    m_cells[i].SetPtr(7 ,GridNeighbor(i, m_size_x, m_size_y, -1,  0), -1,  0, 1.0);
  }
 
  /* Fix links for top, bottom and sides for non-torus */
  
  if (m_resource.m_geometry == nGeometry::GRID) {
    /* Top and bottom */

    for (i = 0; i < m_size_x; i++) {
      m_cells[i].SetPtr(0, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
      m_cells[i].SetPtr(1, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
      m_cells[i].SetPtr(2, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
      ii = num_cells-1-i;
      m_cells[ii].SetPtr(4, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
      m_cells[ii].SetPtr(5, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
      m_cells[ii].SetPtr(6, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
    }

    /* fix links for right and left sides */

    for (i = 0; i < m_size_y; i++) {
      ii = i * m_size_x;    
      m_cells[ii].SetPtr(0, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
      m_cells[ii].SetPtr(7, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
      m_cells[ii].SetPtr(6, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
      ii = ((i + 1) * m_size_x) - 1;
      m_cells[ii].SetPtr(2, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
      m_cells[ii].SetPtr(3, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
      m_cells[ii].SetPtr(4, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE, cAbstractResource::NONE);
    }
  }
}



void cSpatialResourceAcct::Update()
{
  int num_updates = m_stats_update - m_last_calc_update;
  for (int kk=0; kk < num_updates; kk++){
    //spatial_resource_count[res_id]->UpdateCount(ctx);  //Only for Gradient Resources
    m_->Source(inflow_rate[res_id]);  
    spatial_resource_count[res_id]->Sink(decay_rate[res_id]);   
    if (spatial_resource_count[res_id]->GetCellListSize() > 0) {  // Only for CELL resources?
      spatial_resource_count[res_id]->CellInflow();
      spatial_resource_count[res_id]->CellOutflow();
    }
    spatial_resource_count[res_id]->FlowAll();
    spatial_resource_count[res_id]->StateAll();
    // BDB: resource_count[res_ndx] = spatial_resource_count[i]->SumAll();
  }
}

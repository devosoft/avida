//
//  cSpatialResourceAcct.cpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#include "cSpatialResource.h"
#include "AvidaTools.h"

using namespace AvidaTools;




cSpatialResource::cSpatialResource(const cSpatialResource& _res)
: cRatedResource(_res)
{
  *this = _res;
}

cSpatialResource& cSpatialResource::operator=(const cSpatialResource& _res)
{
  this->cRatedResource::operator=(_res);
  m_geometry = _res.m_geometry;
  m_diffuse_x = _res.m_diffuse_x;
  m_diffuse_y = _res.m_diffuse_y;
  m_gravity_x = _res.m_gravity_x;
  m_gravity_y = _res.m_gravity_y;
  m_inflow_boxes = _res.m_inflow_boxes;
  m_outflow_boxes = _res.m_outflow_boxes;
  m_cell_list = _res.m_cell_list;
  return *this;
}

void cSpatialResource::AddInflowBox(int x1, int x2, int y1, int y2)
{
  int width = abs(x2-x1) + 1;
  int height = abs(y2-y1) + 1;
  m_inflow_boxes.Push(cCellBox(x1,y1,width,height));
}

void cSpatialResource::AddOutflowBox(int x1, int x2, int y1, int y2)
{
  int width = abs(x2-x1) + 1;
  int height = abs(y2-y1) + 1;
  m_outflow_boxes.Push(cCellBox(x1,y1,width,height));
}

void cSpatialResource::AddInflowCellBox(const cCellBox& cbox)
{
  m_inflow_boxes.Push(cbox);
}

void cSpatialResource::AddOutflowCellBox(const cCellBox& cbox)
{
  m_outflow_boxes.Push(cbox);
}





void cSpatialResourceAcct::Initialize(int& stats_update)
{
  cSpatialResourceAcct::m_stats_update = stats_update;
}


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
      m_cells[i].SetPtr(0, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
      m_cells[i].SetPtr(1, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
      m_cells[i].SetPtr(2, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
      ii = m_cells.GetSize()-1-i;
      m_cells[ii].SetPtr(4, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
      m_cells[ii].SetPtr(5, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
      m_cells[ii].SetPtr(6, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
    }
    
    /* fix links for right and left sides */
    
    for (i = 0; i < m_size_y; i++) {
      ii = i * m_size_x;    
      m_cells[ii].SetPtr(0, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
      m_cells[ii].SetPtr(7, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
      m_cells[ii].SetPtr(6, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
      ii = ((i + 1) * m_size_x) - 1;
      m_cells[ii].SetPtr(2, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
      m_cells[ii].SetPtr(3, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
      m_cells[ii].SetPtr(4, cResource::NONE, cResource::NONE, cResource::NONE, cResource::NONE);
    }
  }
}


void cSpatialResourceAcct::SetInflow(int cell_id, double inflow)
{ 
  m_cells(cell_id).Rate(inflow); 
}


void cSpatialResourceAcct::SetInflow(int x, int y, double inflow) 
{ 
  m_cells(x,y).Rate(inflow); 
}


void cSpatialResourceAcct::SetState(int cell_id) 
{ 
  m_cells(cell_id).State(); 
}


void cSpatialResourceAcct::SetState(int x, int y) 
{ 
  m_cells(x,y).State(); 
}



void cSpatialResourceAcct::Update(cAvidaContext& ctx)
{
  int num_updates = m_stats_update - m_last_calc_update;
  for (int kk=0; kk < num_updates; kk++){
    //spatial_resource_count[res_id]->UpdateCount(ctx);  //Only for Gradient Resources
    Source(m_resource.m_inflow);  
    Sink(m_resource.m_decay);   
    if (m_resource.m_cell_list.GetSize() > 0) {  // Only for CELL resources?
      CellInflow();
      CellOutflow();
    }
    FlowAll();
    StateAll();
  }
}


void cSpatialResourceAcct::FlowAll()
{
  if (m_resource.m_diffuse_x == 0 && m_resource.m_diffuse_y == 0
      && m_resource.m_gravity_x == 0 && m_resource.m_gravity_y == 0)
    return;
  
  int     i,k,ii,xdist,ydist;
  double  dist;
  
  for (i = 0; i < m_cells.GetSize(); i++) {
    
    /* because flow is two way we must check only half the neighbors to 
     prevent double flow calculations */
    
    for (k = 3; k <= 6; k++) {
      ii = m_cells[i].GetElemPtr(k);
      xdist = m_cells[i].GetPtrXdist(k);
      ydist = m_cells[i].GetPtrYdist(k);
      dist = m_cells[i].GetPtrDist(k);
      if (ii >= 0) {
        FlowMatter(m_cells[i],m_cells[ii],
                   m_resource.m_diffuse_x, m_resource.m_diffuse_y,
                   m_resource.m_gravity_x, m_resource.m_gravity_y,
                   xdist, ydist, dist);
      }
    }
  }
}

void cSpatialResourceAcct::StateAll()
{
  for (int i = 0; i < m_cells.GetSize(); i++) {
    m_cells[i].State();
  } 
}

void cSpatialResourceAcct::Source(double amount)
{
  
  for (int b=0; b < m_resource.m_inflow_boxes.GetSize(); b++)
  {
    const cCellBox in_box = m_resource.m_inflow_boxes[b];
    int inflowX1 = in_box.GetX();
    int inflowX2 = inflowX1 + in_box.GetWidth();
    int inflowY1 = in_box.GetY();
    int inflowY2 = in_box.GetHeight();
    double totalcells = (inflowY2 - inflowY1 + 1) * (inflowX2 - inflowX1 + 1) * 1.0;
    amount /= totalcells;
    
    for (int i = inflowY1; i <= inflowY2; i++) {
      for (int j = inflowX1; j <= inflowX2; j++) {
        int size_x = m_cells.GetHeight();
        int size_y = m_cells.GetWidth();
        int elem = 
        (Mod(i,size_y) * size_x) + Mod(j,size_x);
        Rate(elem,amount); 
      }
    }
  }
}

void cSpatialResourceAcct::Rate(int cell_id, double ratein)
{
  if (cell_id >= 0 && cell_id < m_cells.GetSize()) {
    m_cells[cell_id].Rate(ratein);
  } else {
    assert(false); // x not valid id
  }
}

void cSpatialResourceAcct::Rate(int x, int y, double ratein)
{ 
  int size_x = m_cells.GetWidth();
  int size_y = m_cells.GetHeight();
  if (x >= 0 && x < m_size_x && y>= 0 && y < size_y) {
    m_cells[y * size_x + x].Rate(ratein);
  } else {
    assert(false); // x or y not valid id
  }
}


void cSpatialResourceAcct::CellOutflow()
{
  double deltaamount = 0.0;
  
  const Apto::Array<cCellResource>& cell_list = m_resource.m_cell_list;
  for (int i=0; i < cell_list.GetSize(); i++) {
    const int cell_id = cell_list[i].GetCellID();
    
    /* Be sure the user entered a valid cell id or if the the program is loading
     the resource for the testCPU that does not have a grid set up */
    
    if (cell_id >= 0 && cell_id < m_cells.GetSize()) {
      deltaamount = Apto::Max(
                              ( (*this)(cell_id) * cell_list[i].GetOutflow()), 0.0);
    }                     
    Rate(cell_id, -deltaamount); 
  }
}

void cSpatialResourceAcct::Sink(double decay)
{
  if (m_resource.m_outflow_boxes.GetSize() == 0)
    return;
  
  int size_x = m_cells.GetWidth();
  int size_y = m_cells.GetHeight();
  for (int b=0; b < m_resource.m_outflow_boxes.GetSize(); b++)
  {
    cCellBox cbox = m_resource.m_outflow_boxes[b];
    int outflowX1 = cbox.GetX();
    int outflowX2 = outflowX1 + cbox.GetWidth();
    int outflowY1 = cbox.GetY();
    int outflowY2 = outflowY1 + cbox.GetHeight();
    
    for (int i = outflowY1; i <= outflowY2; i++) {
      for (int j = outflowX1; j <= outflowX2; j++) {
        int elem = (Mod(i,size_y) * size_x) + Mod(j,size_x);
        double deltaamount = Apto::Max( (*this)(elem) * (1.0 - decay), 0.0);
        Rate(elem,-deltaamount); 
      }
    }
  }
}

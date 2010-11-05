#include "cGradientCount.h"
#include <cmath>
#include <iostream>

cGradientCount::cGradientCount(int in_peakx, int in_peaky, double in_spread, double in_height, int in_updatestep, int in_worldx, int in_worldy, int in_geometry) : m_peakx(in_peakx),
m_peaky(in_peaky), m_spread(in_spread), m_height(in_height), m_updatestep(in_updatestep)
{
  ResizeClear(in_worldx, in_worldy, in_geometry);
  m_counter = m_updatestep;
  m_counter2 = 0;
  UpdateCount();
  m_clock = 100;
}


int cGradientCount::MapToWorld(int wx,int wy,int x,int y)
{
  int place;
  if ((x <= GetX()) && (y <= GetY())) {	
    place = y * wx;
    place = place + x;
    place = place - 1;
    place = (y-1) * wx + (x-1);
  } 
  
  else {
    place = -1;
  }
  
  return place;
}


double cGradientCount::Distance(double x1, double y1, double x2, double y2) //calculate linear distance from cone peak
{
  return(sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
}

double cGradientCount::Linmap(double dy, double dx1, double dx2, double rx1, double rx2) //dy=dist from cone center; dx1=???; dx2=spread; rx1=max height for entire cone; rx2=min cone height
{
  double ry;
  if (dx1 != dx2) {
    ry = (dy - dx1) * ((rx2 - rx1) / (dx2 - dx1)) + rx1;
  } 
  
  else {
    ry = 0.0;
  }
  
  return(ry);
}

void cGradientCount::UpdateCount()
{
  m_counter++;
  
  if(GetModified()){                    //once a resource cone has been 'bitten', start the clock that counts down to when the entire peak will be refreshed (carcass rots for only so long before disappearing)
    if(m_counter2++ < m_clock) return;
    m_counter = m_updatestep;
    m_counter2 = 0;
  }
       
  if(m_counter < m_updatestep) return;  //only update resource values at declared update timesteps
  
  double thisdist, thisheight;
  double min_height = 0;
  
  for (int ii = 0; ii < GetX(); ii++) {
    for (int jj = 0; jj < GetY(); jj++) {
      thisdist = Distance(ii, jj, m_peakx, m_peaky);
      if(m_spread >= thisdist) {
        thisheight = Linmap(thisdist, 0.0, m_spread, m_height, min_height);
        if(thisdist != 0) {
          thisheight = m_height / thisdist; //divide the height at the center of the cone (peak) by the distance from the center to get the height for this cell
          if(thisheight < 0) {
            thisheight = 0;  //keep resources from going negative
          }
        }
      } 
      else {
        thisheight = 0;
      }
      
      int thiscell = MapToWorld(GetX(),GetY(),ii,jj);
      Element(jj*GetX()+ii).SetInitial(thisheight);
      Element(jj*GetX()+ii).SetAmount(thisheight);
      
      
    }
  }   
  ResetResourceCounts();
  m_counter = 0;  //reset timestep counter after cone resources updated
}

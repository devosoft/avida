#ifndef cGradientCount_h
#define cGradientCount_h

#ifndef cSpatialResCount_h
#include "cSpatialResCount.h"
#endif


class cGradientCount : public cSpatialResCount
{
private:
  int m_peakx;
  int m_peaky;
  double m_height;
  double m_spread;
  int m_updatestep;
  int m_counter;
  int m_clock;
  int m_counter2;
  int MapToWorld(int wx,int wy,int x,int y);
  double Distance(double x1, double x2, double y1, double y2);
  double Linmap(double dy, double dx1, double dx2, double rx1, double rx2);
  double Max(double r1, double r2);
  void UpdateCount();

public:
  cGradientCount(int in_peakx, int in_peaky, double in_spread, double in_height, int in_updatestep, int in_worldx, int in_worldy, int in_geometry);
  
};

#endif

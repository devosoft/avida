#ifndef cGradientCount_h
#define cGradientCount_h

#ifndef cSpatialResCount_h
#include "cSpatialResCount.h"
#endif

class cWorld;

class cGradientCount : public cSpatialResCount
{
private:
  cWorld* m_world;
  int m_peakx;
  int m_peaky;
  double m_height;
  double m_spread;
  double m_plateau;
  int m_decay;
  int m_max_x;
  int m_max_y;
  int m_min_x;
  int m_min_y;
  double m_move_a_scaler;
  double moveYscaler;
  int m_updatestep;
  int m_counter;
  int m_counter2;
  double movesignx;
  double movesigny;
  double Distance(double x1, double x2, double y1, double y2);
  void UpdateCount();


public:

  cGradientCount(cWorld* world, int in_peakx, int in_peaky, double in_height, double in_spread, double in_plateau, int in_decay,                     //APW random mapping 'cWorld* world, '
                 int in_max_x, int in_max_y, int in_min_x, int in_min_y, double in_move_a_scaler, int in_updatecount, 
                 int in_worldx, int in_worldy, int in_geometry);

};

#endif

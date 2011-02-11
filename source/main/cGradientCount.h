#ifndef cGradientCount_h
#define cGradientCount_h

#ifndef cSpatialResCount_h
#include "cSpatialResCount.h"
#endif

#ifndef cAvidaContext_h
#include "cAvidaContext.h"
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
  int m_updatestep;
  int m_halo;
  int m_halo_inner_radius;
  int m_halo_width;
  int m_halo_anchor_x;
  int m_halo_anchor_y;
  int m_move_speed;
  double moveYscaler;
  
  int m_counter;
  int move_counter;
  double movesignx;
  double movesigny;
  double Distance(double x1, double x2, double y1, double y2);
  void UpdateCount(cAvidaContext* ctx);
  int orbit_shift;
  int angle_shift;
  int halo_dir;
  int changling;


public:

  cGradientCount(cWorld* world, int in_peakx, int in_peaky, double in_height, double in_spread, double in_plateau, int in_decay,              
                 int in_max_x, int in_max_y, int in_min_x, int in_min_y, double in_move_a_scaler, int in_updatestep, 
                 int in_worldx, int in_worldy, int in_geometry,int in_halo, int in_halo_inner_radius, int in_halo_width,
                 int in_halo_anchor_x, int in_halo_anchor_y, int in_move_speed);

};

#endif

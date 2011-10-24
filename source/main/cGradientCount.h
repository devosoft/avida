/*
 *  cGradientCount.h
 *  Avida
 *
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors: Aaron Wagner <apwagner@msu.edu>
 *
 */

#ifndef cGradientCount_h
#define cGradientCount_h

#include "cSpatialResCount.h"

class cWorld;


class cGradientCount : public cSpatialResCount
{
private:
  cWorld* m_world;
  
  // Configuration Arguments
  int m_peakx;
  int m_peaky;
  int m_height;
  int m_spread;
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
  double m_plateau_inflow;
  double m_plateau_outflow;
  int m_is_plateau_common;
  double m_floor;
  int m_habitat;
  int m_min_size;
  int m_max_size; 
  int m_config; 
  int m_count;
  double m_resistance;
  
  // Internal Values
  double m_move_y_scaler;
  
  int m_counter;
  int m_move_counter;
  int m_topo_counter;
  int m_movesignx;
  int m_movesigny;
  
  int m_halo_dir;
  int m_changling;
  bool m_just_reset;
  double m_past_height;
  double m_current_height;
  double m_ave_plat_cell_loss;
  double m_common_plat_height;
  tArray<double> m_plateau_array;
  tArray<int> m_plateau_cell_IDs;
  
public:
  cGradientCount(cWorld* world, int peakx, int peaky, int height, int spread, double plateau, int decay,              
                 int max_x, int max_y, int min_x, int min_y, double move_a_scaler, int updatestep, 
                 int worldx, int worldy, int geometry,int halo, int halo_inner_radius, int halo_width,
                 int halo_anchor_x, int halo_anchor_y, int move_speed, double plateau_inflow, double plateau_outflow,
                 int is_plateau_common, double floor, int habitat, int min_size, int max_size, int config, int count, 
                 double resistance);

  void UpdateCount(cAvidaContext& ctx);
  void StateAll();
  
private:
  void refreshResourceValues();
  void generatePeak(cAvidaContext& ctx);
  void getCurrentPlatValues();
  void generateBarrier(cAvidaContext& ctx);
  void generateHills(cAvidaContext& ctx);  
  
};

#endif

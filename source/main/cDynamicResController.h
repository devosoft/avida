/*
 *  cDynamicResController.h
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
 *  Authors: Aaron P Wagner <apwagner@msu.edu>
 *
 */

#ifndef cDynamicResController_h
#define cDynamicResController_h

#include "cDynamicRes.h"

class cWorld;

class cDynamicResController : public cDynamicRes
{
private:
  cWorld* m_world;
  int m_res_def;
  
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
  double m_cone_inflow;
  double m_cone_outflow;
  double m_res_inflow;
  int m_is_plateau_common;
  double m_floor;
  int m_habitat;
  int m_min_size;
  int m_max_size; 
  int m_config; 
  int m_count;
  double m_initial_plat;
  
  int m_geometry;
  
  // Internal Values
  bool m_initial;
  
  double m_move_y_scaler;
  
  int m_counter;
  int m_move_counter;
  int m_topo_counter;
  int m_movesignx;
  int m_movesigny;
  
  int m_old_peakx;
  int m_old_peaky;
  
  int m_halo_dir;
  int m_changling;
  bool m_just_reset;
  double m_past_height;
  double m_current_height;
  double m_ave_plat_cell_loss;
  double m_common_plat_height;
  int m_skip_moves;
  int m_skip_counter;
  Apto::Array<double> m_plateau_array;
  Apto::Array<int> m_plateau_cell_IDs;
  Apto::Array<int> m_wall_cells;
  
  double m_mean_plat_inflow;
  double m_var_plat_inflow;
  
  bool m_predator;
  double m_pred_odds;
  int m_guarded_juvs_per_adult;
  
  bool m_probabilistic;
  Apto::Array<int> m_prob_res_cells;

  int m_min_usedx;
  int m_min_usedy;
  int m_max_usedx;
  int m_max_usedy;
    
public:
  cDynamicResController(cWorld* world, int peakx, int peaky, int height, int spread, double plateau, int decay,              
                 int max_x, int max_y, int min_x, int min_y, double move_a_scaler, int updatestep, 
                 int worldx, int worldy, int geometry,int halo, int halo_inner_radius, int halo_width,
                 int halo_anchor_x, int halo_anchor_y, int move_speed, double plateau_inflow, double plateau_outflow,
                 double cone_inflow, double cone_outflow, double res_inflow, int is_plateau_common,
                 double floor, int habitat, int min_size, int max_size, int config, int count, 
                 double init_plat);
  ~cDynamicResController();

  void UpdateDynamicRes(cAvidaContext& ctx);
  void ResetDynamicRes(cAvidaContext& ctx, int worldx, int worldy);
  
  void SetPlatVarInflow(double mean, double variance, int type);
  
  void SetPredatoryResource(double odds, int juvsper);
  void UpdatePredatoryRes(cAvidaContext& ctx); 
  
  void SetProbabilisticResource(cAvidaContext& ctx, double initial, double inflow, double outflow, double lambda, double theta, int x, int y, int num_cells);
  void BuildProbabilisticRes(cAvidaContext& ctx, double lambda, double theta, int x, int y, int num_cells);
  void UpdateProbabilisticRes();
 
  Apto::Array<int>* GetWallCells() { return &m_wall_cells; }
  int GetMinUsedX() { return m_min_usedx; }
  int GetMinUsedY() { return m_min_usedy; }
  int GetMaxUsedX() { return m_max_usedx; }
  int GetMaxUsedY() { return m_max_usedy; }
  
private:
  void fillinResourceValues();
  void updatePeakRes(cAvidaContext& ctx);
  void moveRes(cAvidaContext& ctx);
  int setHaloOrbit(cAvidaContext& ctx, int current_orbit);
  void setPeakMoveMovement(cAvidaContext& ctx);
  void moveHaloPeak(int current_orbit);
  void movePeak();
  void generatePeak(cAvidaContext& ctx);
  void getCurrentPlatValues();
  void generateBarrier(cAvidaContext& ctx);
  void generateHills(cAvidaContext& ctx);    
  void updateBounds(int x, int y);
  void resetUsedBounds();
  void clearExistingProbRes();
};

#endif

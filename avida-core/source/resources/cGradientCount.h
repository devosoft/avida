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
 *  Authors: Aaron P Wagner <apwagner@msu.edu>
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
  int m_move_resistance;
  double m_plateau_inflow;
  double m_plateau_outflow;
  double m_cone_inflow;
  double m_cone_outflow;
  double m_gradient_inflow;
  int m_is_plateau_common;
  double m_floor;
  int m_habitat;
  int m_min_size;
  int m_max_size; 
  int m_config; 
  int m_count;
  double m_initial_plat;
  double m_threshold;
  double m_damage;
  
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
  
  double m_pred_odds;
  bool m_predator;
  double m_death_odds;
  bool m_deadly;
  int m_path;
  int m_hammer;
  int m_guarded_juvs_per_adult;
  
  bool m_probabilistic;
  Apto::Array<int> m_prob_res_cells;

  int m_min_usedx;
  int m_min_usedy;
  int m_max_usedx;
  int m_max_usedy;
    
public:
  cGradientCount(cWorld* world, int peakx, int peaky, int height, int spread, double plateau, int decay,              
                 int max_x, int max_y, int min_x, int min_y, double move_a_scaler, int updatestep, 
                 int worldx, int worldy, int geometry,int halo, int halo_inner_radius, int halo_width,
                 int halo_anchor_x, int halo_anchor_y, int move_speed, int move_resistance, double plateau_inflow, double plateau_outflow,
                 double cone_inflow, double cone_outflow, double gradient_inflow, int is_plateau_common, 
                 double floor, int habitat, int min_size, int max_size, int config, int count, 
                 double init_plat, double threshold, double damage, double death_odds, int path, int hammer);
  ~cGradientCount();

  void UpdateCount(cAvidaContext& ctx);
  void StateAll();
  
  void SetGradInitialPlat(double plat_val) { m_initial_plat = plat_val; m_initial = true; }
  void SetGradPeakX(int peakx) { m_peakx = peakx; }
  void SetGradPeakY(int peaky) { m_peaky = peaky; }
  void SetGradHeight(int height) { m_height = height; }
  void SetGradSpread(int spread) { m_spread = spread; }
  void SetGradPlateau(double plateau) { m_plateau = plateau; }
  void SetGradDecay(int decay) { m_decay = decay; }
  void SetGradMaxX(int max_x) { m_max_x = max_x; }
  void SetGradMaxY(int max_y) { m_max_y = max_y; }
  void SetGradMinX(int min_x) { m_min_x = min_x; }
  void SetGradMinY(int min_y) { m_min_y = min_y; }
  void SetGradMoveScaler(double move_a_scaler) { m_move_a_scaler = move_a_scaler; }
  void SetGradUpdateStep(int updatestep) { m_updatestep = updatestep; }
  void SetGradIsHalo(bool halo) { m_halo = halo; }
  void SetGradHaloInnerRad(int halo_inner_radius) { m_halo_inner_radius = halo_inner_radius; }
  void SetGradHaloWidth(int halo_width) { m_halo_width = halo_width; }
  void SetGradHaloX(int halo_anchor_x) { m_halo_anchor_x = halo_anchor_x; }
  void SetGradHaloY(int halo_anchor_y) { m_halo_anchor_y = halo_anchor_y; }
  void SetGradMoveSpeed(int move_speed) { m_move_speed = move_speed; }
  void SetGradMoveResistance(int move_resistance) { m_move_resistance = move_resistance; }
  void SetGradPlatInflow(double plateau_inflow) { m_plateau_inflow = plateau_inflow; }
  void SetGradPlatOutflow(double plateau_outflow) { m_plateau_outflow = plateau_outflow; }
  void SetGradConeInflow(double cone_inflow) { m_cone_inflow = cone_inflow; }
  void SetGradConeOutflow(double cone_outflow) { m_cone_outflow = cone_outflow; }
  void SetGradientInflow(double gradient_inflow) { m_gradient_inflow = gradient_inflow; }
  void SetGradPlatIsCommon(bool is_plateau_common) { m_is_plateau_common = is_plateau_common; }
  void SetGradFloor(double floor) { m_floor = floor; }
  void SetGradHabitat(int habitat) { m_habitat = habitat; }
  void SetGradMinSize(int min_size) { m_min_size = min_size; }
  void SetGradMaxSize(int max_size) { m_max_size = max_size; }
  void SetGradConfig(int config) { m_config = config; }
  void SetGradCount(int count) { m_count = count; }
 
  void SetGradPlatVarInflow(cAvidaContext& ctx, double mean, double variance, int type);
  
  void SetPredatoryResource(double odds, int juvsper);
  void UpdatePredatoryRes(cAvidaContext& ctx);

  void UpdateDamagingRes(cAvidaContext& ctx);
  void SetDeadlyRes(double odds) { m_death_odds = odds; m_deadly = (m_death_odds != 0); }
  void SetIsPath(bool path) { m_path = path; }
  void UpdateDeadlyRes(cAvidaContext& ctx);
  
  void SetProbabilisticResource(cAvidaContext& ctx, double initial, double inflow, double outflow, double lambda, double theta, int x, int y, int num_cells);
  void BuildProbabilisticRes(cAvidaContext& ctx, double lambda, double theta, int x, int y, int num_cells);
  void UpdateProbabilisticRes();
 
  void ResetGradRes(cAvidaContext& ctx, int worldx, int worldy); 
  
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
  void confirmHaloPeak();
  void movePeak();
  void generatePeak(cAvidaContext& ctx);
  void getCurrentPlatValues();
  void generateBarrier(cAvidaContext& ctx);
  void generateHills(cAvidaContext& ctx);    
  void updateBounds(int x, int y);
  void resetUsedBounds();
  void clearExistingProbRes();
  
  inline void setHaloDirection(cAvidaContext& ctx);
};

#endif

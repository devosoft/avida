/*
 *  cDynamicRes.h
 *  Avida
 *
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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
 */

/*! Class to keep track of amounts of localized resources. */

#ifndef cDynamicRes_h
#define cDynamicRes_h

#include "cResource.h"

class cAvidaContext;
class cResourceDef;
class cWorld;

class cDynamicRes : public cResource
{
public:
  cDynamicRes(cWorld* world, cResourceDef& res_def, int worldx, int worldy);

  virtual ~cDynamicRes();
  
// base class
public:
  void UpdateDynamicRes(cAvidaContext& ctx);
  void ResetDynamicRes(cAvidaContext& ctx, int worldx, int worldy);
  void BuildProbabilisticRes(cAvidaContext& ctx, double lambda, double theta, int x, int y, int num_cells);
  void SetPlatVarInflow(double mean, double variance, int type);

  int GetCurrPeakX() { return curr_peakx; }
  int GetCurrPeakY() { return curr_peaky; }
  
  Apto::Array<int>* GetWallCells() { return &m_wall_cells; }
  int GetMinUsedX() { return m_min_usedx; }
  int GetMinUsedY() { return m_min_usedy; }
  int GetMaxUsedX() { return m_max_usedx; }
  int GetMaxUsedY() { return m_max_usedy; }
    
  void SetCurrPeakX(int in_curr_x) { curr_peakx = in_curr_x; }
  void SetCurrPeakY(int in_curr_y) { curr_peaky = in_curr_y; }

// internal variables
private:
  cWorld* m_world;
  int curr_peakx, curr_peaky;
  cResourceDef& m_res_def;
  
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
  
  Apto::Array<int> m_prob_res_cells;

  int m_min_usedx;
  int m_min_usedy;
  int m_max_usedx;
  int m_max_usedy;
    
// internal functions
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
  void updatePredatoryRes(cAvidaContext& ctx);
  void updateProbabilisticRes();
};

#endif

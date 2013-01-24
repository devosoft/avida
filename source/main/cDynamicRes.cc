/*
 *  cDynamicRes.cc
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
 */

#include "cDynamicRes.h"

#include "avida/core/Feedback.h"
#include "avida/core/UniverseDriver.h"

#include "cPopulation.h"
#include "cStats.h"

#include <cmath>
#include <iostream>

using namespace Avida;

cDynamicRes::cDynamicRes(cWorld* world, cResourceDef& res_def, int worldx, int worldy)
  : cResource()
  , m_world(world)
  , m_res_def(res_def)
  , m_initial(false)
  , m_move_y_scaler(0.5)
  , m_counter(0)
  , m_move_counter(1)
  , m_topo_counter(res_def.GetUpdateStep())
  , m_movesignx(0)
  , m_movesigny(0)
  , m_old_peakx(res_def.GetPeakX())
  , m_old_peaky(res_def.GetPeakY())
  , m_just_reset(true)
  , m_past_height(0.0)
  , m_current_height(0.0)
  , m_ave_plat_cell_loss(0.0)
  , m_common_plat_height(0.0)
  , m_skip_moves(0)
  , m_skip_counter(0)
  , m_mean_plat_inflow(res_def.GetPlateauInflow())
  , m_var_plat_inflow(0)
  , m_min_usedx(-1)
  , m_min_usedy(-1)
  , m_max_usedx(-1)
  , m_max_usedy(-1)
{
  m_world->GetEnvironment().GetOutputSize();
  ResetDynamicRes(m_world->GetDefaultContext(), worldx, worldy);
}

cDynamicRes::~cDynamicRes() { ; }


void cDynamicRes::UpdateDynamicRes(cAvidaContext& ctx)
{ 
  m_old_peakx = m_res_def.GetPeakX();
  m_old_peaky = m_res_def.GetPeakY();
  if (m_res_def.GetHabitat() == 2) generateBarrier(m_world->GetDefaultContext());
  else if (m_res_def.GetHabitat() == 1) generateHills(m_world->GetDefaultContext());
  else if (m_res_def.IsProbabilistic()) updateProbabilisticRes();
  else updatePeakRes(ctx);
}

void cDynamicRes::updatePeakRes(cAvidaContext& ctx)
{  
  bool has_edible = false; 
  
  // determine if there is any edible food left in the peak (don't refresh the peak values until decay kicks in if there is edible food left) 
  // to speed things up, we only check cells within the possible spread of the peak
  // and we only need to do this if decay > 1 (if decay == 1, we're going to reset everything regardless of the amount left)
  // if decay = 1 and the resource IS depletable, that means we have a moving depleting resource! Odd, but useful.
  if (m_res_def.GetDecay() > 1) {
    int max_pos_x = min(m_res_def.GetPeakX() + m_res_def.GetSpread() + 1, GetX() - 1);
    int min_pos_x = max(m_res_def.GetPeakX() - m_res_def.GetSpread() - 1, 0);
    int max_pos_y = min(m_res_def.GetPeakY() + m_res_def.GetSpread() + 1, GetY() - 1);
    int min_pos_y = max(m_res_def.GetPeakY() - m_res_def.GetSpread() - 1, 0);
    for (int ii = min_pos_x; ii < max_pos_x + 1; ii++) {
      for (int jj = min_pos_y; jj < max_pos_y + 1; jj++) {
        if (Element(jj * GetX() + ii).GetAmount() >= 1) {
          has_edible = true;
          break;
        }
      }
    }
  }
  
  // once a resource cone has been 'bitten', start the clock that counts down to when the entire peak will be
  // refreshed (carcass rots for only so long before disappearing)
  if (has_edible && GetModified() && m_res_def.GetDecay() > 1) m_counter++;

  // only update resource values at declared update timesteps if there is resource left in the cone
  if (has_edible && m_counter < m_res_def.GetDecay() && GetModified()) {
    if (m_res_def.IsPredatory()) updatePredatoryRes(ctx);
    return;
  } 
                   
  // before we move anything, if we have a depletable resource, we need to get the current plateau cell values 
  if (m_res_def.GetDecay() == 1) getCurrentPlatValues();

  // When the counter matches decay, regenerate resource peak
  if (m_counter == m_res_def.GetDecay()) generatePeak(ctx);
  
  // if we are working with moving peaks, calculate the y-scaler
  if (m_res_def.GetMoveScaler() > 1) m_move_y_scaler = m_res_def.GetMoveScaler() * m_move_y_scaler * (1 - m_move_y_scaler);   

  // if working with moving resources, check if we are moving once per update or less frequently
  if (m_skip_counter == m_skip_moves) moveRes(ctx);
  else m_skip_counter++;
  
  // to speed things up, we only check cells within the possible spread of the peak
  // and we only do this if the resource is set to actually move, has inflow/outflow to update, or
  // we just reset a non-moving resource
  if (m_res_def.GetMoveScaler() > 1 || m_res_def.GetPlateauInflow() != 0 || m_res_def.GetPlateauOutflow() != 0 || m_res_def.GetConeInflow() != 0 || m_res_def.GetConeOutflow() != 0
  || m_res_def.GetDynamicResInflow() != 0 || (m_res_def.GetMoveScaler() == 1 && m_just_reset)) fillinResourceValues();

  if (m_res_def.IsPredatory()) updatePredatoryRes(ctx);
}

void cDynamicRes::generatePeak(cAvidaContext& ctx)
{
  // Get initial peak cell x, y coordinates and movement directions.
  Apto::Random& rng = ctx.GetRandom();
  int temp_height = 0;
  if (m_res_def.GetPlateau() < 0) temp_height = 1;
  else temp_height = m_res_def.GetHeight();
  // If no initial config set m_res_def.GetPeakX() and m_res_def.GetPeakY(), get a random location
  if (!m_res_def.IsHalo()) {
    if (m_res_def.GetPeakX() == -1) m_res_def.GetPeakX() = rng.GetUInt(m_res_def.GetMinX() + temp_height, m_res_def.GetMaxX() - temp_height + 1);
    if (m_res_def.GetPeakY() == -1) m_res_def.GetPeakY() = rng.GetUInt(m_res_def.GetMinY() + temp_height, m_res_def.GetMaxY() - temp_height + 1);

    if (m_res_def.GetMoveScaler() > 1) {
      // Get a random direction for movement on the x-axis
      m_movesignx = rng.GetInt(-1,2);
      // If x-axis movement direction is 0, we want to make sure y-axis movement is not also 0
      if (m_movesignx == 0) {
        m_movesigny = (rng.GetUInt(0,2) == 1) ? -1 : 1;
      } else {
        m_movesigny = rng.GetInt(-1,2);
      }
    }
  } 
  // for halo's we generate a random location on the orbit,
  else if (m_res_def.IsHalo()) {
    if (m_res_def.GetMoveScaler() > 1) {
      m_halo_dir = (rng.GetUInt(0,2) == 1) ? -1 : 1;
      m_changling = (rng.GetUInt(0,2) == 1) ? -1 : 1;
    }
    const int chooseUpDown = rng.GetUInt(0,2);
    if (chooseUpDown == 0) {
    int chooseEW = rng.GetUInt(0,2);
      if (chooseEW == 0) {
        m_res_def.GetPeakX() = rng.GetUInt(max(0,m_res_def.GetHaloX() - m_res_def.GetHaloInnerRadius() - m_res_def.GetHaloWidth() + temp_height),
                              m_res_def.GetHaloX() - m_res_def.GetHaloInnerRadius() - temp_height + 1);
      } else {
        m_res_def.GetPeakX() = rng.GetUInt(max(0,m_res_def.GetHaloX() + m_res_def.GetHaloInnerRadius() + temp_height),
                              m_res_def.GetHaloX() + m_res_def.GetHaloInnerRadius() + m_res_def.GetHaloWidth() - temp_height + 1); 
      }
      m_res_def.GetPeakY() = rng.GetUInt(max(0,m_res_def.GetHaloY() - m_res_def.GetHaloInnerRadius() - m_res_def.GetHaloWidth() + temp_height),
                            m_res_def.GetHaloY() + m_res_def.GetHaloInnerRadius() + m_res_def.GetHaloWidth() - temp_height + 1);
    }
    else {
      int chooseNS = rng.GetUInt(0,2);
      if (chooseNS == 0) { 
        m_res_def.GetPeakY() = rng.GetUInt(max(0,m_res_def.GetHaloY() - m_res_def.GetHaloInnerRadius() - m_res_def.GetHaloWidth() + temp_height),
                              m_res_def.GetHaloY() - m_res_def.GetHaloInnerRadius() - temp_height + 1);
      } else {
        m_res_def.GetPeakY() = rng.GetUInt(max(0,m_res_def.GetHaloY() + m_res_def.GetHaloInnerRadius() + temp_height),
                              m_res_def.GetHaloY() + m_res_def.GetHaloInnerRadius() + m_res_def.GetHaloWidth() - temp_height + 1);
      }
      m_res_def.GetPeakX() = rng.GetUInt(max(0,m_res_def.GetHaloX() - m_res_def.GetHaloInnerRadius() - m_res_def.GetHaloWidth() + temp_height),
                            m_res_def.GetHaloX() + m_res_def.GetHaloInnerRadius() + m_res_def.GetHaloWidth() - temp_height + 1);
    }
  }
  assert(m_res_def.GetPeakX() >= 0 && m_res_def.GetPeakY() >= 0 && m_res_def.GetPeakX() < GetX() && m_res_def.GetPeakY() < GetY());

  SetModified(false);
  m_counter = 0;
  m_skip_counter = 0;
  m_just_reset = true;
  fillinResourceValues();
}

void cDynamicRes::fillinResourceValues()
{  
  int max_pos_x;
  int min_pos_x;
  int max_pos_y;
  int min_pos_y;
  resetUsedBounds();

  // if we are resetting a resource, we need to calculate new values for the whole world so we can wipe away any residue
  if (m_just_reset) {
    if (m_min_usedx == -1 || m_min_usedy == -1 || m_max_usedx == -1 || m_max_usedy == -1) {
      max_pos_x = GetX() - 1;
      min_pos_x = 0;
      max_pos_y = GetY() - 1;
      min_pos_y = 0;
    }
    else {
      max_pos_x = m_max_usedx;
      min_pos_x = m_min_usedx;
      max_pos_y = m_max_usedy;
      min_pos_y = m_min_usedy;
    }
  } else {
    // otherwise we only need to update values within the possible range of the peak 
    // we check all the way back to move_speed to make sure we're not leaving any old residue behind
    max_pos_x = min(m_res_def.GetPeakX() + m_res_def.GetSpread() + m_res_def.GetMoveSpeed() + 1, GetX() - 1);
    min_pos_x = max(m_res_def.GetPeakX() - m_res_def.GetSpread() - m_res_def.GetMoveSpeed() - 1, 0);
    max_pos_y = min(m_res_def.GetPeakY() + m_res_def.GetSpread() + m_res_def.GetMoveSpeed() + 1, GetY() - 1);
    min_pos_y = max(m_res_def.GetPeakY() - m_res_def.GetSpread() - m_res_def.GetMoveSpeed() - 1, 0);
  }

  if (m_res_def.IsPlateauCommon() == 1 && !m_just_reset && m_world->GetStats().GetUpdate() > 0) {
    // with common depletion, new peak height is not the plateau heights, but the delta in plateau heights applied to 
    // peak height from the last time
    m_current_height = m_current_height - m_ave_plat_cell_loss + m_res_def.GetPlateauInflow() - (m_current_height * m_res_def.GetPlateauOutflow());
    m_common_plat_height = m_common_plat_height - m_ave_plat_cell_loss + m_res_def.GetPlateauInflow() - (m_current_height * m_res_def.GetPlateauOutflow());
    if (m_common_plat_height > m_res_def.GetPlateau() && m_res_def.GetPlateau() >= 0) m_common_plat_height = m_res_def.GetPlateau();
    if (m_common_plat_height < 0 && m_res_def.GetPlateau() >=0) m_common_plat_height = 0;
    if (m_current_height > m_res_def.GetHeight() && m_res_def.GetPlateau() >= 0) m_current_height = m_res_def.GetHeight();
    if (m_current_height < 0 && m_res_def.GetPlateau() >= 0) m_current_height = 0;
  }
  else {
    m_current_height = m_res_def.GetHeight();
  }

  int plateau_cell = 0;
  for (int ii = min_pos_x; ii < max_pos_x + 1; ii++) {
    for (int jj = min_pos_y; jj < max_pos_y + 1; jj++) {
      double thisheight = 0.0;
      double thisdist = sqrt((double) (m_res_def.GetPeakX() - ii) * (m_res_def.GetPeakX() - ii) + (m_res_def.GetPeakY() - jj) * (m_res_def.GetPeakY() - jj));
      if (m_res_def.GetSpread() >= thisdist) {
        // determine theoretical individual cells values and add one to distance from center 
        // (so that center point = radius 1, not 0)
        // also used to distinguish plateau cells
        
        thisheight = m_current_height / (thisdist + 1);
        
        // set the floor values
        // plateaus will override this so that plateaus can hit 0 when being eaten
        if (thisheight < m_res_def.GetFloor()) thisheight = m_res_def.GetFloor();
        
        // create cylindrical profiles of resources whereever thisheight would be >1 (area where thisdist + 1 <= m_res_def.GetHeight())
        // and slopes outside of that range
        // plateau = -1 turns off this option; if activated, causes 'peaks' to be flat plateaus = plateau value 
        bool is_plat_cell = ((m_res_def.GetHeight() / (thisdist + 1)) >= 1);
        // apply plateau inflow(s) and outflow 
        if ((is_plat_cell && m_res_def.GetPlateau() >= 0) || (m_res_def.GetPlateau() < 0 && thisdist == 0 && m_plateau_array.GetSize())) {
          if (m_just_reset || m_world->GetStats().GetUpdate() <= 0) {
            m_past_height = m_res_def.GetHeight();
            if (m_res_def.GetPlateau() >= 0.0) {
              thisheight = m_res_def.GetPlateau();
            } 
            else {
              thisheight = m_res_def.GetHeight();
            }
          } 
          else { 
            if (m_res_def.IsPlateauCommon() == 0) {
              m_past_height = m_plateau_array[plateau_cell]; 
              thisheight = m_past_height + m_res_def.GetPlateauInflow() - (m_past_height * m_res_def.GetPlateauOutflow());
              thisheight += m_res_def.GetDynamicResInflow() / (thisdist + 1);
              if (thisheight > m_res_def.GetPlateau() && m_res_def.GetPlateau() >= 0) {
                thisheight = m_res_def.GetPlateau();
              } 
              if (m_res_def.GetPlateau() < 0 && thisdist == 0 && thisheight > m_res_def.GetHeight()) {
                thisheight = m_res_def.GetHeight();
              }
            }
            else if (m_res_def.IsPlateauCommon() == 1) {   
              thisheight = m_common_plat_height;
            }
          }
          if (m_initial && m_res_def.GetInitialPlatVal() != -1) thisheight = m_res_def.GetInitialPlatVal();
          if (thisheight < 0) thisheight = 0;
          m_plateau_array[plateau_cell] = thisheight;
          m_plateau_cell_IDs[plateau_cell] = jj * GetX() + ii;
          plateau_cell ++;
         }
        // now apply any off-plateau inflow(s) and outflow
        else if (!is_plat_cell && (m_res_def.GetConeInflow() > 0 || m_res_def.GetConeOutflow() > 0 || m_res_def.GetDynamicResInflow() > 0)) {
          if (!m_just_reset && m_world->GetStats().GetUpdate() > 0) {
            int offsetx = m_old_peakx - m_res_def.GetPeakX();
            int offsety = m_old_peaky - m_res_def.GetPeakY();
            
            int old_cell_x = ii + offsetx;
            int old_cell_y = jj + offsety;
            
            // cone cells that were previously off the world and moved onto world, start at 0
            if ( old_cell_x < 0 || old_cell_y < 0 || (old_cell_y > (GetY() - 1)) || (old_cell_x > (GetX() - 1)) ) {
              thisheight = 0;
            }
            else {
              double past_height = Element(old_cell_y * GetX() + old_cell_x).GetAmount(); 
              double newheight = past_height; 
              if (m_res_def.GetConeInflow() > 0 || m_res_def.GetConeOutflow() > 0) newheight += m_res_def.GetConeInflow() - (past_height * m_res_def.GetConeOutflow());
              if (m_res_def.GetDynamicResInflow() > 0) newheight += m_res_def.GetDynamicResInflow() / (thisdist + 1);
              // don't exceed expected slope value
              if (newheight < thisheight) thisheight = newheight;
              if (thisheight < 0) thisheight = 0;
            }
          }
        }
      }
      Element(jj * GetX() + ii).SetAmount(thisheight);
      if (thisheight > 0) updateBounds(ii, jj);
    }
  }         
  SetCurrPeakX(m_res_def.GetPeakX());
  SetCurrPeakY(m_res_def.GetPeakY());
  m_just_reset = false;
}

void cDynamicRes::getCurrentPlatValues()
{ 
  int temp_height = 0;
  if (m_res_def.GetPlateau() < 0) temp_height = 1;
  else temp_height = m_res_def.GetHeight();
  int plateau_box_min_x = m_res_def.GetPeakX() - temp_height - 1;
  int plateau_box_max_x = m_res_def.GetPeakX() + temp_height + 1;
  int plateau_box_min_y = m_res_def.GetPeakY() - temp_height - 1;
  int plateau_box_max_y = m_res_def.GetPeakY() + temp_height + 1;
  int plateau_cell = 0;
  double amount_devoured = 0.0;
  for (int ii = plateau_box_min_x; ii < plateau_box_max_x + 1; ii++) {
    for (int jj = plateau_box_min_y; jj < plateau_box_max_y + 1; jj++) { 
      double thisdist = sqrt((double) (m_res_def.GetPeakX() - ii) * (double) (m_res_def.GetPeakX() - ii) + (double) (m_res_def.GetPeakY() - jj) * (double) (m_res_def.GetPeakY() - jj));
      double find_plat_dist = temp_height / (thisdist + 1);
      if ((find_plat_dist >= 1 && m_res_def.GetPlateau() >= 0) || (m_res_def.GetPlateau() < 0 && thisdist == 0 && m_plateau_array.GetSize() > 0)) {
        double past_cell_height = m_plateau_array[plateau_cell];
        double pre_move_height = Element(m_plateau_cell_IDs[plateau_cell]).GetAmount();  
        if (pre_move_height < past_cell_height) {
          m_plateau_array[plateau_cell] = pre_move_height; 
          amount_devoured = amount_devoured + past_cell_height - pre_move_height;
        }
        plateau_cell ++;
      }
    }        
  }
  m_ave_plat_cell_loss = amount_devoured / plateau_cell;
} 

void cDynamicRes::moveRes(cAvidaContext& ctx)
{
  // for halo peaks, find current orbit. Add 1 to distance to account for the anchor grid cell
  int current_orbit = max(abs(m_res_def.GetHaloX() - m_res_def.GetPeakX()), abs(m_res_def.GetHaloY() - m_res_def.GetPeakY())) + 1;  

  // if we are working with moving resources and it's time to update direction
  if (m_move_counter == m_res_def.GetUpdateStep() && m_res_def.GetMoveScaler() > 1) {
    m_move_counter = 1;
    if (m_res_def.IsHalo() == 1) current_orbit = setHaloOrbit(ctx, current_orbit);
    else setPeakMoveMovement(ctx);
  }    
  else m_move_counter++;
  
  if (m_res_def.GetMoveScaler() > 1) {
    if (m_res_def.IsHalo() == 1 && m_res_def.GetMoveScaler() > 1) moveHaloPeak(current_orbit);
    else movePeak();
  }
  m_skip_counter = 0;
}

void cDynamicRes::setPeakMoveMovement(cAvidaContext& ctx)
{
  int choosesign = ctx.GetRandom().GetInt(1,3);
  if (choosesign == 1) {
    if (m_movesignx == -1) m_movesignx = ctx.GetRandom().GetInt(-1,1); 
    else if (m_movesignx == 1) m_movesignx = ctx.GetRandom().GetInt(0,2);
    else m_movesignx = ctx.GetRandom().GetInt(-1,2);
  }      
  else if (choosesign == 2){ 
    if (m_movesigny == -1) m_movesigny = ctx.GetRandom().GetInt(-1,1); 
    else if (m_movesigny == 1) m_movesigny = ctx.GetRandom().GetInt(0,2);
    else m_movesigny = ctx.GetRandom().GetInt(-1,2);
  }
} 

int cDynamicRes::setHaloOrbit(cAvidaContext& ctx, int current_orbit)
{    
  // move cones by moving m_res_def.GetPeakX() & m_res_def.GetPeakY() 
  // halo resources orbit at a fixed org walking distance from an anchor point
  // if halo width > the height of the halo resource, the resource will be bounded inside the halo but the orbit can vary within those bounds
  // halo's are actually square in avida because, at a given orbit, this keeps a constant distance (in number of steps and org would have to take)
  //    between the anchor point and any orbit
  
  //choose to change orbit (0) or direction (1)    
  int random_shift = ctx.GetRandom().GetUInt(0,2);
  // if changing orbit, choose to go in or out one orbit
  // then figure out if we need change the x or the y to shift orbit (based on what quadrant we're in)
  int temp_height = 0;
  if (m_res_def.GetPlateau() < 0) temp_height = 1;
  else temp_height = m_res_def.GetHeight();

  if (random_shift == 0) {
    //do nothing unless there's room to change orbit
    if (m_res_def.GetHaloWidth() > (temp_height * 2)) {
      int orbit_shift = 1;
      if (!ctx.GetRandom().GetUInt(0,2)) orbit_shift = -1;
      current_orbit += orbit_shift;
      
      // we have to check that we are still going to be within the halo after an orbit change
      // if we go out of bounds, we need to go the other way instead, taking two steps back on current_orbit
      if (current_orbit > (m_res_def.GetHaloInnerRadius() + m_res_def.GetHaloWidth() - temp_height + 2) ||
          current_orbit < (m_res_def.GetHaloInnerRadius() + temp_height + 2) ||
          m_res_def.GetHaloY() + current_orbit - 1 >= GetY() ||
          m_res_def.GetHaloX() + current_orbit - 1 >= GetX() ||
          m_res_def.GetHaloY() + current_orbit - 1 < 0 ||
          m_res_def.GetHaloX() + current_orbit - 1 < 0) {
        orbit_shift *= -1;
        current_orbit += 2 * orbit_shift;
      }

      if (abs(m_res_def.GetHaloY() - m_res_def.GetPeakY()) > abs(m_res_def.GetHaloX() - m_res_def.GetPeakX())) m_res_def.GetPeakY() = m_old_peaky + orbit_shift;
      else m_res_def.GetPeakX() = m_old_peakx + orbit_shift;  
    }
    // if there was no room to change orbit, change the direction instead of the orbit
    else random_shift = 1;
  }
  // if changing direction of rotation, we just switch sign of rotation
  if (random_shift == 1) m_halo_dir = m_halo_dir * -1;
  return current_orbit;
}

void cDynamicRes::moveHaloPeak(int current_orbit)
{
  // what quadrant we are in determines whether we are changing x's or y's (= changling)
  // if we are on a corner, we just stick with the current changling
  if (abs(m_res_def.GetHaloY() - m_res_def.GetPeakY()) > abs(m_res_def.GetHaloX() - m_res_def.GetPeakX()))
    m_changling = 1;
  else if (abs(m_res_def.GetHaloY() - m_res_def.GetPeakY()) < abs(m_res_def.GetHaloX() - m_res_def.GetPeakX()))
    m_changling = -1;
  
  if (m_changling == 1) {
    // check to make sure the move will not put peak beyond the bounds (at corner) of the orbit
    // if it will go beyond the bounds of the orbit, turn the corner (e.g. if move = 5 & space to move on x =2, move 2 on x and 3 on y)
    int next_posx = m_res_def.GetPeakX() + (m_halo_dir * m_res_def.GetMoveSpeed());
    int max_orbit_x = m_res_def.GetHaloX() + current_orbit - 1;
    int min_orbit_x = m_res_def.GetHaloX() - current_orbit + 1;
    int current_x = m_res_def.GetPeakX();
    if (next_posx > max_orbit_x) {
      m_res_def.GetPeakX() = max_orbit_x;
      if (m_res_def.GetPeakY() > m_res_def.GetHaloY()) {      
        // turning this corner means changing the sign of the movement once we switch from moving along x to moving along y
        m_halo_dir *= -1;
        m_res_def.GetPeakY() = m_res_def.GetPeakY() + m_halo_dir * (m_res_def.GetMoveSpeed() - abs(m_res_def.GetPeakX() - current_x)); 
      } else {
        m_res_def.GetPeakY() = m_res_def.GetPeakY() + m_halo_dir * (m_res_def.GetMoveSpeed() - abs(m_res_def.GetPeakX() - current_x));
      }
      m_changling *= -1;
    }
    else if (next_posx < min_orbit_x) { 
      m_res_def.GetPeakX() = min_orbit_x;          
      if (m_res_def.GetPeakY() > m_res_def.GetHaloY()) {
        m_res_def.GetPeakY() = m_res_def.GetPeakY() + m_halo_dir * (m_res_def.GetMoveSpeed() - abs(m_res_def.GetPeakX() - current_x));
      } else {
        m_halo_dir *= -1;
        m_res_def.GetPeakY() = m_res_def.GetPeakY() + m_halo_dir * (m_res_def.GetMoveSpeed() - abs(m_res_def.GetPeakX() - current_x));
      }
      m_changling *= -1;          
    }
    else m_res_def.GetPeakX() = m_res_def.GetPeakX() + (m_halo_dir * m_res_def.GetMoveSpeed());   
  } 
  else {
    int next_posy = m_res_def.GetPeakY() + (m_halo_dir * m_res_def.GetMoveSpeed());
    int max_orbit_y = m_res_def.GetHaloY() + current_orbit - 1;
    int min_orbit_y = m_res_def.GetHaloY() - current_orbit + 1;
    int current_y = m_res_def.GetPeakY();
    
    if (next_posy > max_orbit_y) {
      m_res_def.GetPeakY() = max_orbit_y;
      if (m_res_def.GetPeakX() < m_res_def.GetHaloX()) {
        m_res_def.GetPeakX() = m_res_def.GetPeakX() + m_halo_dir * (m_res_def.GetMoveSpeed() - abs(m_res_def.GetPeakY() - current_y));
      } else {
        m_halo_dir *= -1;
        m_res_def.GetPeakX() = m_res_def.GetPeakX() + m_halo_dir * (m_res_def.GetMoveSpeed() - abs(m_res_def.GetPeakY() - current_y));
      }
      m_changling *= -1;      
    } else if (next_posy < min_orbit_y) { 
      m_res_def.GetPeakY() = min_orbit_y;          
      if (m_res_def.GetPeakX() < m_res_def.GetHaloX()) { 
        m_halo_dir *= -1;
        m_res_def.GetPeakX() = m_res_def.GetPeakX() + m_halo_dir * (m_res_def.GetMoveSpeed() - abs(m_res_def.GetPeakY() - current_y));
      } else {
        m_res_def.GetPeakX() = m_res_def.GetPeakX() + m_halo_dir * (m_res_def.GetMoveSpeed() - abs(m_res_def.GetPeakY() - current_y));
      }
      m_changling *= -1;
    } else {
      m_res_def.GetPeakY() = m_res_def.GetPeakY() + (m_halo_dir * m_res_def.GetMoveSpeed());
    }
  }
  assert(m_res_def.GetPeakX() >= 0 && m_res_def.GetPeakY() >= 0 && m_res_def.GetPeakX() < GetX() && m_res_def.GetPeakY() < GetY());
}

void cDynamicRes::movePeak()
{
  // for non-halo peaks keep cones inside their bounding boxes, bouncing them if they hit the edge 
  int temp_height = 0;
  if (m_res_def.GetPlateau() < 0) temp_height = 1;
  else temp_height = m_res_def.GetHeight();

  int temp_peakx = m_res_def.GetPeakX() + (int)(m_move_y_scaler + 0.5) * m_movesignx;
  int temp_peaky = m_res_def.GetPeakY() + (int)(m_move_y_scaler + 0.5) * m_movesigny;
  
  if (temp_peakx > (m_res_def.GetMaxX() - temp_height)) m_movesignx = -1;
  if (temp_peakx < (m_res_def.GetMinX() + temp_height + 1)) m_movesignx = 1; 
  
  if (temp_peaky > (m_res_def.GetMaxY() - temp_height)) m_movesigny = -1;
  if (temp_peaky < (m_res_def.GetMinY() + temp_height + 1)) m_movesigny = 1;
  
  m_res_def.GetPeakX() = (int) (m_res_def.GetPeakX() + (m_movesignx * m_move_y_scaler) + .5);
  m_res_def.GetPeakY() = (int) (m_res_def.GetPeakY() + (m_movesigny * m_move_y_scaler) + .5);
}  

void cDynamicRes::generateBarrier(cAvidaContext& ctx)
// If habitat == 2 we are creating barriers to movement (walls), not really gradient resources
{ 
  // generate/regenerate walls when counter == config updatestep
  if (m_topo_counter == m_res_def.GetUpdateStep()) {
    resetUsedBounds();
    // reset counter
    m_topo_counter = 1;
    // clear any old resource
    if (m_wall_cells.GetSize()) {
      for (int i = 0; i < m_wall_cells.GetSize(); i++) {
        Element(m_wall_cells[i]).SetAmount(0);
      }
    }
    else {
      for (int ii = 0; ii < GetX(); ii++) {
        for (int jj = 0; jj < GetY(); jj++) {
          Element(jj * GetX() + ii).SetAmount(0);
        }
      }
    }
    m_wall_cells.Resize(0);
    // generate number barriers equal to count 
    for (int i = 0; i < m_res_def.GetCount(); i++) {
      // drop the anchor/first block for current barrier
      int start_randx = 0;
      int start_randy = 0;   
      if (m_res_def.GetConfig() == 3 || m_res_def.GetConfig() == 4) {
        start_randx = m_res_def.GetPeakX();
        start_randy = m_res_def.GetPeakY();
      }
      else {
        start_randx = ctx.GetRandom().GetUInt(0, GetX());
        start_randy = ctx.GetRandom().GetUInt(0, GetY());  
      }
      Element(start_randy * GetX() + start_randx).SetAmount(m_res_def.GetPlateau());
      if (m_res_def.GetPlateau() > 0) updateBounds(start_randx, start_randy);
      m_wall_cells.Push(start_randy * GetX() + start_randx);

      int randx = start_randx;
      int randy = start_randy;
      int prev_blockx = randx;
      int prev_blocky = randy;
      int cornerx = prev_blockx;
      int cornery = prev_blocky;
      bool place_corner = false;

      // decide the size of the current barrier
      int rand_block_count = ctx.GetRandom().GetUInt(m_res_def.GetMinSize(), m_res_def.GetMaxSize() + 1);
      // for vertical or horizontal wall building, pick a random direction once for the whole wall
      int direction = ctx.GetRandom().GetUInt(0,2);
      
      for (int num_blocks = 0; num_blocks < rand_block_count; num_blocks++) {
        // if config == 0, build random shaped walls
        if (m_res_def.GetConfig() == 0) {
          prev_blockx = randx;
          prev_blocky = randy;
          cornerx = prev_blockx;
          cornery = prev_blocky;
          place_corner = false;
          // move one cell in chosen direction
          // on diagonals, we need to place a block at chosen spot + 1 block in corner to prevent porous diagonal walls
          if (direction == 0) {
            randy = randy - 1;
          }
          else if (direction == 1) {
            randy = randy - 1;
            randx = randx + 1;
            place_corner = true;
            cornery -= 1;
          }
          else if (direction == 2) {
            randx = randx + 1;
          }
          else if (direction == 3) {
            randy = randy + 1;
            randx = randx + 1;
            place_corner = true;
            cornerx += 1;
          }
          else if (direction == 4) {
            randy = randy + 1;
          }
          else if (direction == 5) {
            randy = randy + 1;
            randx = randx - 1;
            place_corner = true;
            cornerx -= 1;
          }
          else if (direction == 6) {
            randx = randx - 1;
          }
          else if (direction == 7) {
            randy = randy - 1;
            randx = randx - 1;
            place_corner = true;
            cornery += 1;
          }
          // choose a direction for next block with fixed 90% probability of not changing direction (~1 of 20 blocks will be in new direction)
          if(ctx.GetRandom().GetUInt(0, 21) == 20) direction = ctx.GetRandom().GetUInt(0, 8);
        }
        // if config == 1, build randomly placed vertical walls
        else if (m_res_def.GetConfig() == 1) {
          // choose up/down build direction
          if (direction == 0) randy = randy - 1;
          else randy = randy + 1;
        }
        // if config == 2, build randonly placed horizontal walls
        else if (m_res_def.GetConfig() == 2) {
          // choose left/right build direction
          if (direction == 0) randx = randx - 1;
          else randx = randx + 1;
        }       
        // if config == 3, build vertical walls from north to south
        else if (m_res_def.GetConfig() == 3) randy = randy + 1;
        // if config == 4, build horizontal walls from west to east
        else if (m_res_def.GetConfig() == 4) randx = randx + 1;
        
        bool count_block = true;
        // place the new block(s) if not off edge of world
        if (randy < GetY() && randy >= 0 && randx < GetX() && randx >= 0) {
          // if we are trying to build across an inner_radius 
          // or for random walls, if there is already a block here
          // don't count or place this one (continue walking across inner_radius)
          if ((randx < (m_res_def.GetHaloX() + m_res_def.GetHaloInnerRadius()) && 
               randy < (m_res_def.GetHaloY() + m_res_def.GetHaloInnerRadius()) && 
               randx > (m_res_def.GetHaloX() - m_res_def.GetHaloInnerRadius()) && 
               randy > (m_res_def.GetHaloY() - m_res_def.GetHaloInnerRadius())) || 
              (m_res_def.GetConfig() == 0 && Element(randy * GetX() + randx).GetAmount())) {
            num_blocks --;
            count_block = false;
          }
          if (count_block) {
            Element(randy * GetX() + randx).SetAmount(m_res_def.GetPlateau());
            if (m_res_def.GetPlateau() > 0) updateBounds(randx, randy);
            m_wall_cells.Push(randy * GetX() + randx);
            if (place_corner) {
              if (cornery < GetY() && cornery >= 0 && cornerx < GetX() && cornerx >= 0) {
                if ( ! ((cornerx < (m_res_def.GetHaloX() + m_res_def.GetHaloInnerRadius()) && 
                     cornery < (m_res_def.GetHaloY() + m_res_def.GetHaloInnerRadius()) && 
                     cornerx > (m_res_def.GetHaloX() - m_res_def.GetHaloInnerRadius()) && 
                     cornery > (m_res_def.GetHaloY() - m_res_def.GetHaloInnerRadius()))) ){
                  Element(cornery * GetX() + cornerx).SetAmount(m_res_def.GetPlateau());
                  if (m_res_def.GetPlateau() > 0) updateBounds(cornerx, cornery);
                  m_wall_cells.Push(randy * GetX() + randx);
                }
              }
            }
          }
        }
        // if the wall is horizontal or vertical build and we went off the world edge, build from the opposite direction
        else if (m_res_def.GetConfig() == 1 || m_res_def.GetConfig() == 2) {
          randx = start_randx; 
          randy = start_randy; 
          direction = abs(direction - 1);
          num_blocks --;
        }
        // if a random build and we went off the world edge, backup a block and try again
        else if (m_res_def.GetConfig() == 0) {
          randx = prev_blockx;
          randy = prev_blocky;
          num_blocks --;
        }
      }  
    }
  }
  else m_topo_counter++; 
}

void cDynamicRes::generateHills(cAvidaContext& ctx)
// If habitat == 1 we are creating hills which slow movement, not really gradient resources
{ 
  // generate/regenerate hills when counter == config updatestep
  if (m_topo_counter == m_res_def.GetUpdateStep()) {
    resetUsedBounds();
    // reset counter
    m_topo_counter = 1;
    // since we are potentially plotting more than one hill per resource, we need to wipe the world before we start
    if (m_min_usedx == -1 || m_min_usedy == -1 || m_max_usedx == -1 || m_max_usedy == -1) {
      for (int ii = 0; ii < GetX(); ii++) {
        for (int jj = 0; jj < GetY(); jj++) {
          Element(jj * GetX() + ii).SetAmount(0);
        }
      }
    }
    else {
      for (int ii = m_min_usedx; ii < m_max_usedx + 1; ii++) {
        for (int jj = m_min_usedy; jj < m_max_usedy + 1; jj++) {
          Element(jj * GetX() + ii).SetAmount(0);
        }
      }
    }

    Apto::Random& rng = ctx.GetRandom();
    // generate number hills equal to count
    for (int i = 0; i < m_res_def.GetCount(); i++) {
      // decide the size of the current hill
      int rand_hill_radius = ctx.GetRandom().GetUInt(m_res_def.GetMinSize(), m_res_def.GetMaxSize() + 1);
      
      // generate random hills, if config == 0, otherwise generate 1 hill at peakx X peaky
      if (m_res_def.GetConfig() == 0) {
        // choose the peak center for current hill, keeping the entire hill outside of any inner_radius
        int chooseEW = rng.GetUInt(0,2);
        if (chooseEW == 0) {
          m_res_def.GetPeakX() = rng.GetUInt(rand_hill_radius, m_res_def.GetHaloX() - m_res_def.GetHaloInnerRadius() - rand_hill_radius);
        } else {
          m_res_def.GetPeakX() = rng.GetUInt(m_res_def.GetHaloX() + m_res_def.GetHaloInnerRadius() + rand_hill_radius, GetX() - 1 - rand_hill_radius);
        }
        int chooseNS = rng.GetUInt(0,2);
        if (chooseNS == 0) { 
          m_res_def.GetPeakY() = rng.GetUInt(rand_hill_radius, m_res_def.GetHaloY() - m_res_def.GetHaloInnerRadius() - rand_hill_radius);
        } else {
          m_res_def.GetPeakY() = rng.GetUInt(m_res_def.GetHaloY() + m_res_def.GetHaloInnerRadius() + rand_hill_radius, GetY() - 1 - rand_hill_radius);
        }
      }
      
      // figure the coordinate extent of each hill (box)
      int max_pos_x = min(m_res_def.GetPeakX() + rand_hill_radius + 1, GetX() - 1);
      int min_pos_x = max(m_res_def.GetPeakX() - rand_hill_radius - 1, 0);
      int max_pos_y = min(m_res_def.GetPeakY() + rand_hill_radius + 1, GetY() - 1);
      int min_pos_y = max(m_res_def.GetPeakY() - rand_hill_radius - 1, 0);

      // look to place new cell values within a box around the hill center
      for (int ii = min_pos_x; ii < max_pos_x + 1; ii++) {
        for (int jj = min_pos_y; jj < max_pos_y + 1; jj++) {
          double thisheight = 0.0;
          double thisdist = sqrt((double) (m_res_def.GetPeakX() - ii) * (m_res_def.GetPeakX() - ii) + (m_res_def.GetPeakY() - jj) * (m_res_def.GetPeakY() - jj));
          // only plot values when within set config radius & if no larger amount has already been plotted for another overlapping hill
          if ((thisdist <= rand_hill_radius) && (Element(jj * GetX() + ii).GetAmount() <  m_res_def.GetPlateau() / (thisdist + 1))) {
          thisheight = m_res_def.GetPlateau() / (thisdist + 1);
          Element(jj * GetX() + ii).SetAmount(thisheight);
          if (thisheight > 0) updateBounds(ii, jj);
          }
        }
      }
    }
  }
  else m_topo_counter++; 
}

void cDynamicRes::ResetDynamicRes(cAvidaContext& ctx, int worldx, int worldy)
{
  for (int i = 0; i < GetSize(); i++) {
    cResourceElement tmpelem;
    Element(i) = tmpelem;
  } 

  if ((m_res_def.GetMoveSpeed() >= (2 * (m_res_def.GetHaloInnerRadius() + m_res_def.GetHaloWidth()))) && ((m_res_def.GetHaloInnerRadius() + m_res_def.GetHaloWidth()) != 0)
      && m_res_def.GetMoveSpeed() != 0) {
    m_world->GetDriver().Feedback().Error("Move speed greater or equal to 2*Radius");
    return;
  }
  if (m_res_def.IsHalo() == 1 && (m_res_def.GetHaloWidth() < (2 * m_res_def.GetHeight()) && m_res_def.GetPlateau() >= 0)) {
    m_world->GetDriver().Feedback().Error("Halo width < 2 * height (aka plateau radius)");
    return;
  }
  if (m_res_def.GetMoveSpeed() < 0) {
    m_skip_moves = abs(m_res_def.GetMoveSpeed());
    m_res_def.GetMoveSpeed() = 1;
  }
  m_plateau_array.Resize(int(4 * m_res_def.GetHeight() * m_res_def.GetHeight() + 0.5));
  m_plateau_array.SetAll(0);
  m_plateau_cell_IDs.Resize(int(4 * m_res_def.GetHeight() * m_res_def.GetHeight() + 0.5));
  m_plateau_cell_IDs.SetAll(0);
  m_prob_res_cells.Resize(0);
  m_wall_cells.Resize(0);
  m_current_height = m_res_def.GetHeight();
  m_common_plat_height = m_res_def.GetPlateau();
  m_mean_plat_inflow = m_res_def.GetPlateauInflow();
  m_var_plat_inflow = 0;
  resetUsedBounds();
  
  m_initial = true;
  ResizeClear(worldx, worldy, m_res_def.GetGeometry());
  if (m_res_def.GetHabitat() == 2) {
    generateBarrier(ctx);
  }
  else if (m_res_def.GetHabitat() == 1) {
    generateHills(ctx);
  }
  else {
    generatePeak(ctx);
    UpdateDynamicRes(ctx);
  }
  // set m_initial to false now that we have reset the resource
  m_initial = false;
}

void cDynamicRes::SetPlatVarInflow(double mean, double variance, int type)
{
  if (variance > 0) {
    m_mean_plat_inflow = mean;
    m_var_plat_inflow = variance;
    double the_inflow = 0;
    if (type == 0) {
      the_inflow = abs(m_world->GetRandom().GetRandNormal(mean, variance));
      GetResDef()->SetPlateauInflow(the_inflow);
    }
    else if (type < 0) { 
      the_inflow = abs(m_world->GetRandom().GetRandNormal(0, variance));
      if (mean - the_inflow < 0) the_inflow = mean;
      GetResDef()->SetPlateauInflow(mean - the_inflow);
    }
    else if (type == 1) {
      the_inflow = abs(m_world->GetRandom().GetRandNormal(0, variance));
      GetResDef()->SetPlateauInflow(mean + the_inflow);
    }
    else if (type == 2) {
      the_inflow = m_world->GetRandom().GetRandNormal(0, variance);
      if (mean + the_inflow < 0) the_inflow = mean;
      GetResDef()->SetPlateauInflow(mean + the_inflow);
    }
  }
  else GetResDef()->SetPlateauInflow(mean);
}

void cDynamicRes::updatePredatoryRes(cAvidaContext& ctx)
{
  // kill off up to 1 org per update within the predator radius (plateau area), with prob of death for selected prey = m_pred_odds
  if (m_res_def.IsPredatory()) {
    for (int i = 0; i < m_plateau_cell_IDs.GetSize(); i ++) {
      if (Element(m_plateau_cell_IDs[i]).GetAmount() >= 1) {
        m_world->GetPopulation().GetResources().ExecutePredatoryResource(ctx, m_plateau_cell_IDs[i], m_res_def.GetPredatorResOdds(), m_res_def.GetJuvAdultGuardRatio());
      }
    }
  }
}

void cDynamicRes::BuildProbabilisticRes(cAvidaContext& ctx, double lambda, double theta, int x, int y, int num_cells)
{
  if (m_min_usedx != -1) clearExistingProbRes();
  resetUsedBounds();
  int cells_used = 0;
  const int worldx = GetX();
  const int worldy = GetY();
  int world_size = worldx * worldy;
  int max_idx = world_size - 1;
  int max_tries = min(1000, world_size);
  
  Apto::Array <int> cell_id_array;
  cell_id_array.ResizeClear(world_size);
  for (int i = 0; i < cell_id_array.GetSize(); i++) cell_id_array[i] = i;
  
  if (x == -1) m_res_def.GetPeakX() = ctx.GetRandom().GetUInt(0, worldx);
  else m_res_def.GetPeakX() = x;
  if (y == -1) m_res_def.GetPeakY() = ctx.GetRandom().GetUInt(0, worldy); 
  else m_res_def.GetPeakY() = y;
  
  if (num_cells != -1) m_prob_res_cells.ResizeClear(num_cells);

  // only if theta == 1 do want want a 'hill' with resource for certain in the center
  if (theta == 0) {
    Element(m_res_def.GetPeakY() * worldx + m_res_def.GetPeakX()).SetAmount(m_res_def.GetInitialPlatVal());
    if (m_res_def.GetInitialPlatVal() > 0) updateBounds(m_res_def.GetPeakX(), m_res_def.GetPeakY());
    if (m_res_def.GetPlateauOutflow() > 0 || m_res_def.GetPlateauInflow() > 0) { 
      if (num_cells == -1) m_prob_res_cells.Push(m_res_def.GetPeakY() * worldx + m_res_def.GetPeakX());
      else m_prob_res_cells[cells_used] = m_res_def.GetPeakY() * worldx + m_res_def.GetPeakX();
    }  
    cells_used++;
    // no need to pop this cell off the array, just move it and don't check that far anymore
    cell_id_array.Swap(m_res_def.GetPeakY() * worldx + m_res_def.GetPeakX(), max_idx--);
  }
  
  // prevent looping when num_cells not specified
  bool loop_once = false;
  if (num_cells == -1) {
    loop_once = true;
    num_cells = cells_used + 1;
  }
  
  int max_unused_idx = max_idx;
  while (max_tries) {   // emergency exit
    // allow looping through multiple times until num_cells quota is filled
    if (max_unused_idx == 0) {
      max_tries--;
      if (!loop_once) max_unused_idx = max_idx;
    }
    
    int cell_idx = m_world->GetRandom().GetUInt(max_unused_idx + 1);
    int cell_id = cell_id_array[cell_idx];
    int this_x = cell_id % worldx;
    int this_y = cell_id / worldx;  
    double cell_dist = sqrt((double) (m_res_def.GetPeakX() - this_x) * (m_res_def.GetPeakX() - this_x) + (m_res_def.GetPeakY() - this_y) * (m_res_def.GetPeakY() - this_y));
    // use a half normal
    double this_prob = (1/lambda) * (sqrt(2 / 3.14159)) * exp(-0.5 * pow(((cell_dist - theta) / lambda), 2));
    
    if (ctx.GetRandom().P(this_prob)) {
      Element(cell_id).SetAmount(m_res_def.GetInitialPlatVal());
      if (m_res_def.GetInitialPlatVal() > 0) updateBounds(this_x, this_y);
      if (m_res_def.GetPlateauOutflow() > 0 || m_res_def.GetPlateauInflow() > 0) {
        if (loop_once) m_prob_res_cells.Push(cell_id);
        else m_prob_res_cells[cells_used] = cell_id;
      }              
      cells_used++;
      cell_id_array.Swap(cell_idx, max_idx--);
    }
    // just push this cell out of the way for this loop, but keep it around for next time
    else { 
      Element(cell_id).SetAmount(0); 
      cell_id_array.Swap(cell_idx, max_unused_idx--);
    }

    if (cells_used >= num_cells && !loop_once) break;
    if (max_unused_idx <= 0 && loop_once) break;
    if (max_idx <= 0) break;
  }
  // in case we couldn't fill the quota...
  while (m_prob_res_cells.GetSize() > cells_used) m_prob_res_cells.Pop();
}

void cDynamicRes::updateProbabilisticRes()
{
  if (m_res_def.GetPlateauOutflow() > 0 || m_res_def.GetPlateauInflow() > 0) {
    for (int i = 0; i < m_prob_res_cells.GetSize(); i++) {
      double curr_val = Element(m_prob_res_cells[i]).GetAmount();
      double amount = curr_val + m_res_def.GetPlateauInflow() - (curr_val * m_res_def.GetPlateauOutflow());
      Element(m_prob_res_cells[i]).SetAmount(amount); 
      if (amount > 0) updateBounds(m_prob_res_cells[i] % GetX(), m_prob_res_cells[i] / GetX());
    }
  }
}

void cDynamicRes::clearExistingProbRes()
{
  for (int x = m_min_usedx; x < m_max_usedx + 1; x ++) {
    for (int y = m_min_usedy; y < m_max_usedy + 1; y ++) {
      Element(y * GetX() + x).SetAmount(0);
    }
  }
}

void cDynamicRes::updateBounds(int x, int y)
{
  if (x < m_min_usedx || m_min_usedx == -1) m_min_usedx = x;
  if (y < m_min_usedy || m_min_usedy == -1) m_min_usedy = y;
  if (x > m_max_usedx || m_max_usedx == -1) m_max_usedx = x;
  if (y > m_max_usedy || m_max_usedy == -1) m_max_usedy = y;
}

void cDynamicRes::resetUsedBounds()
{
  m_min_usedx = -1;
  m_min_usedy = -1;
  m_max_usedx = -1;
  m_max_usedy = -1;
}

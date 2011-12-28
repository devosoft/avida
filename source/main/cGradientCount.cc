/*
 *  cGradientCount.cc
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

#include "cGradientCount.h"

#include "avida/core/WorldDriver.h"

#include "cAvidaContext.h"
#include "cStats.h"
#include "cWorld.h"

#include <cmath>
#include <iostream>

using namespace Avida;


/* cGradientCount is designed to give moving peaks of resources. Peaks are <optionally> capped with plateaus. The slope of the peaks
is height / distance. Consequently, when height = distance from center of peak, the value at that cell = 1. This was 
designed this way because the organims used for this could only consume resources when the value is >= 1. Thus, height also 
gives radius of 'edible' resources (aka the plateau). If plateaus are >1, you get sloped edges leading up to plateau 
cylinders.
  Spread gives the radius of the entire resource peak to the outside of the sloped edge. Organisms could detect resources 
all along the spread, but only consume that portion on the plateau. Thus, spread - plateau = sense radius (smell) while 
plateau = consumable radius (actual food).
  Peaks move within the boundaries set by min/max x and y. If the plateau / edible portion of the peak hits the boundary, the peak 
'bounces' (sign of direction of movement changes).
  Smoothness of the movement is controlled by move_a_scaler which is the A in eq1 in Morrison & DeJong 1999. A-values 
need to be between 1 and 4. Values of 1 to ~3 give smooth movements. Larger values should yield chaotic moves. However, beyond 
establishing that peaks don't move when the value = 1 and do move when the value > 1, the effects of A-values have not really been 
evaluated.
  If depletable (via reaction) peaks stop moving when they are first bitten.
  Depletable peaks will be refreshed when either all edible portions (>=1) are consumed or when the decay timestep (in 
updates) is reached, whichever comes first.
  Once bitten, depletable peaks will not move again until refreshed.
  Peak values are refreshed to match initial height, spread, and plateau, but for non-halo peaks, the placement of the 
refreshed peak is random within the min/max x and y area. For halo peaks, the peak is currently refreshed at the SE 
corner of the orbit.
cGradientCount cannot access the random number generator at the very first update. Thus, it uses the DefaultContext initially.
  We use movesign to determine direction of peak movement
  First, to get smooth movements, for non-halo resources we only allow either the x or y direction change to be evaluated in 
 a single update. For halo resources, we only evaluate either the orbit or the direction in a given update.
  Second, we then decide the change of direction based on the current direction, e.g. so that non-halo peak movesigns can't 'jump' 
from -1 to 1, without first changing to 0
  Finally, we only toy with movement direction when # updates since last change = updatestep.
 */

cGradientCount::cGradientCount(cWorld* world, int peakx, int peaky, int height, int spread, double plateau, int decay, 
                               int max_x, int max_y, int min_x, int min_y, double move_a_scaler, int updatestep,  
                               int worldx, int worldy, int geometry, int halo, int halo_inner_radius, int halo_width,
                               int halo_anchor_x, int halo_anchor_y, int move_speed, 
                               double plateau_inflow, double plateau_outflow, int is_plateau_common, double floor,
                               int habitat, int min_size, int max_size, int config, int count, double resistance)
  : m_world(world)
  , m_peakx(peakx), m_peaky(peaky)
  , m_height(height), m_spread(spread), m_plateau(plateau), m_decay(decay)
  , m_max_x(max_x), m_max_y(max_y), m_min_x(min_x), m_min_y(min_y)
  , m_move_a_scaler(move_a_scaler), m_updatestep(updatestep)
  , m_halo(halo), m_halo_inner_radius(halo_inner_radius), m_halo_width(halo_width)
  , m_halo_anchor_x(halo_anchor_x), m_halo_anchor_y(halo_anchor_y)
  , m_move_speed(move_speed), m_plateau_inflow(plateau_inflow), m_plateau_outflow(plateau_outflow)
  , m_is_plateau_common(is_plateau_common), m_floor(floor) 
  , m_habitat(habitat), m_min_size(min_size), m_max_size(max_size), m_config(config), m_count(count), m_resistance(resistance)
  , m_geometry(geometry)
  , m_move_y_scaler(0.5)
  , m_counter(0)
  , m_move_counter(1)
  , m_topo_counter(updatestep)
  , m_movesignx(0)
  , m_movesigny(0)
  , m_just_reset(true)
  , m_past_height(0.0)
  , m_current_height(0.0)
  , m_ave_plat_cell_loss(0.0)
  , m_common_plat_height(0.0)
  , m_skip_moves(0)
  , m_skip_counter(0)
{
  ResetGradRes(m_world->GetDefaultContext(), worldx, worldy);
}

void cGradientCount::StateAll()
{
  return;
}


void cGradientCount::UpdateCount(cAvidaContext& ctx)
{ 
  if (m_habitat == 2) {
    generateBarrier(m_world->GetDefaultContext());
    return;
  }
  else if (m_habitat == 1) {
    generateHills(m_world->GetDefaultContext());
    return;
  }  
  bool has_edible = false;

  // determine if there is any edible food left in the peak (don't refresh the peak values until decay kicks in if there is edible food left) 
  // to speed things up, we only check cells within the possible spread of the peak
  // and we only need to do this if decay > 1 (if decay == 1, we're going to reset everything regardless of the amount left)
  // if decay = 1 and the resource IS depletable, that means we have a moving depleting resource! Odd, but useful.
  if (m_decay > 1) {
    int max_pos_x = min(m_peakx + m_spread + 1, GetX() - 1);
    int min_pos_x = max(m_peakx - m_spread - 1, 0);
    int max_pos_y = min(m_peaky + m_spread + 1, GetY() - 1);
    int min_pos_y = max(m_peaky - m_spread - 1, 0);
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
  if (has_edible && GetModified()) m_counter++;
  
  // only update resource values at declared update timesteps if there is resource left in the cone
  if (has_edible && m_counter < m_decay && GetModified()) return; 
                    
  // before we move anything, if we have a depletable resource, we need to get the current plateau cell values 
  if (m_decay == 1) getCurrentPlatValues();

  // When the counter matches decay, regenerate resource peak
  if (m_counter == m_decay) generatePeak(ctx);
  
  // if we are working with moving peaks, calculate the y-scaler
  if (m_move_a_scaler > 1) m_move_y_scaler = m_move_a_scaler * m_move_y_scaler * (1 - m_move_y_scaler);   

  // for halo peaks, find current orbit. Add 1 to distance to account for the anchor grid cell
  int current_orbit = max(abs(m_halo_anchor_x - m_peakx), abs(m_halo_anchor_y - m_peaky)) + 1;
  
  // if working with moving resources, check if we are moving once per update or less frequently
  if (m_skip_counter == m_skip_moves) {
    // if we are working with moving resources and it's time to update direction
    if (m_move_counter == m_updatestep && m_move_a_scaler > 1) { 
      m_move_counter = 1;
      // move cones by moving m_peakx & m_peaky 
      // halo resources orbit at a fixed org walking distance from an anchor point
      // if halo width > the height of the halo resource, the resource will be bounded inside the halo but the orbit can vary within those bounds
      // halo's are actually square in avida because, at a given orbit, this keeps a constant distance (in number of steps and org would have to take)
      //    between the anchor point and any orbit
      if (m_halo == 1) {    //choose to change orbit (0) or direction (1)    
        int old_peakx = m_peakx;
        int old_peaky = m_peaky;
        int random_shift = ctx.GetRandom().GetUInt(0,2);
        // if changing orbit, choose to go in or out one orbit
        // then figure out if we need change the x or the y to shift orbit (based on what quadrant we're in)
        int temp_height = 0;
        if (m_plateau < 0) temp_height = 1;
        else temp_height = m_height;
        if (random_shift == 0) {
          //do nothing unless there's room to change orbit
          if (m_halo_width > (temp_height * 2)) {
            int orbit_shift = ctx.GetRandom().GetUInt(0,2); 
            if (orbit_shift == 0) {
              current_orbit = current_orbit - 1; 
              if (abs(m_halo_anchor_y - m_peaky) > abs(m_halo_anchor_x - m_peakx))
                m_peaky = old_peaky - 1;
              else 
                m_peakx = old_peakx - 1;
            }
            else if (orbit_shift == 1) {
              current_orbit = current_orbit + 1;  
              if (abs(m_halo_anchor_y - m_peaky) > abs(m_halo_anchor_x - m_peakx))
                m_peaky = old_peaky + 1;
              else 
                m_peakx = old_peakx + 1;
            }
            // we have to check that we are still going to be within the halo after an orbit change       
            if (current_orbit > (m_halo_inner_radius + m_halo_width - temp_height + 2)) {
              // if we go out of bounds, we need to go the other way instead (taking two steps back on orbit since we already took one in the wrong direction)
              current_orbit = current_orbit - 2;
              if (abs(m_halo_anchor_y - old_peaky) > abs(m_halo_anchor_x - old_peakx))
                m_peaky = old_peaky + 1;
              else 
                m_peakx = old_peakx + 1;
            }
            else if (current_orbit < (m_halo_inner_radius + temp_height + 2)) {
              current_orbit = current_orbit + 2;
              if (abs(m_halo_anchor_y - old_peaky) > abs(m_halo_anchor_x - old_peakx))
                m_peaky = old_peaky - 1;
              else 
                m_peakx = old_peakx - 1;          
            }
          }
          // if there was no room to change orbit, change the direction instead of the orbit
          else random_shift = 1;
        }
        // if changing direction of rotation, we just switch sign of rotation
        else if (random_shift == 1) {
          m_halo_dir = m_halo_dir * -1; 
        }
      }
      // for non-halo peaks
      else {
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
    } 
    else {
      m_move_counter++;
    }
    
    if (m_halo == 1 && m_move_a_scaler > 1) { 
      // what quadrant we are in determines whether we are changing x's or y's (= changling)
      // if we are on a corner, we just stick with the current changling
      if (abs(m_halo_anchor_y - m_peaky) > abs(m_halo_anchor_x - m_peakx))
        m_changling = 1;
      else if (abs(m_halo_anchor_y - m_peaky) < abs(m_halo_anchor_x - m_peakx))
        m_changling = -1;
      
      if (m_changling == 1) {
        // check to make sure the move will not put peak beyond the bounds (at corner) of the orbit
        // if it will go beyond the bounds of the orbit, turn the corner (e.g. if move = 5 & space to move on x =2, move 2 on x and 3 on y)
        int next_posx = m_peakx + (m_halo_dir * m_move_speed);
        int max_orbit_x = m_halo_anchor_x + current_orbit - 1;
        int min_orbit_x = m_halo_anchor_x - current_orbit + 1;
        int current_x = m_peakx;
        if (next_posx > max_orbit_x) {
          m_peakx = max_orbit_x;
          if (m_peaky > m_halo_anchor_y) {      
            // turning this corner means changing the sign of the movement once we switch from moving along x to moving along y
            m_halo_dir *= -1;
            m_peaky = m_peaky + m_halo_dir * (m_move_speed - abs(m_peakx - current_x)); 
          } else {
            m_peaky = m_peaky + m_halo_dir * (m_move_speed - abs(m_peakx - current_x));
          }
          m_changling *= -1;
        }
        else if (next_posx < min_orbit_x) { 
          m_peakx = min_orbit_x;          
          if (m_peaky > m_halo_anchor_y) {
            m_peaky = m_peaky + m_halo_dir * (m_move_speed - abs(m_peakx - current_x));
          } else {
            m_halo_dir *= -1;
            m_peaky = m_peaky + m_halo_dir * (m_move_speed - abs(m_peakx - current_x));
          }
          m_changling *= -1;          
        }
        else m_peakx = m_peakx + (m_halo_dir * m_move_speed);   
      } else {
        int next_posy = m_peaky + (m_halo_dir * m_move_speed);
        int max_orbit_y = m_halo_anchor_y + current_orbit - 1;
        int min_orbit_y = m_halo_anchor_y - current_orbit + 1;
        int current_y = m_peaky;
        
        if (next_posy > max_orbit_y) {
          m_peaky = max_orbit_y;
          if (m_peakx < m_halo_anchor_x) {
            m_peakx = m_peakx + m_halo_dir * (m_move_speed - abs(m_peaky - current_y));
          } else {
            m_halo_dir *= -1;
            m_peakx = m_peakx + m_halo_dir * (m_move_speed - abs(m_peaky - current_y));
          }
          m_changling *= -1;      
        } else if (next_posy < min_orbit_y) { 
          m_peaky = min_orbit_y;          
          if (m_peakx < m_halo_anchor_x) { 
            m_halo_dir *= -1;
            m_peakx = m_peakx + m_halo_dir * (m_move_speed - abs(m_peaky - current_y));
          } else {
            m_peakx = m_peakx + m_halo_dir * (m_move_speed - abs(m_peaky - current_y));
          }
          m_changling *= -1;
        } else {
          m_peaky = m_peaky + (m_halo_dir * m_move_speed);
        }
      }
    } else {
      // for non-halo peaks keep cones inside their bounding boxes, bouncing them if they hit the edge 
      int temp_height = 0;
      if (m_plateau < 0) temp_height = 1;
      else temp_height = m_height;
      if (m_move_a_scaler > 1) {
        int temp_peakx = m_peakx + (int)(m_move_y_scaler + 0.5) * m_movesignx;
        int temp_peaky = m_peaky + (int)(m_move_y_scaler + 0.5) * m_movesigny;
        
        if (temp_peakx > (m_max_x - temp_height)) m_movesignx = -1;
        if (temp_peakx < (m_min_x + temp_height + 1)) m_movesignx = 1; 
        
        if (temp_peaky > (m_max_y - temp_height)) m_movesigny = -1;
        if (temp_peaky < (m_min_y + temp_height + 1)) m_movesigny = 1;
        
        m_peakx = (int) (m_peakx + (m_movesignx * m_move_y_scaler) + .5);
        m_peaky = (int) (m_peaky + (m_movesigny * m_move_y_scaler) + .5);
      }  
    } 
    m_skip_counter = 0;
  }
  else m_skip_counter++;
  // to speed things up, we only check cells within the possible spread of the peak
  // and we only do this if the resource is set to actually move, has inflow/outflow to update, or
  // we just reset a non-moving resource
  if (m_move_a_scaler > 1 || m_plateau_inflow != 0 || m_plateau_outflow != 0 || (m_move_a_scaler == 1 && m_just_reset)) refreshResourceValues();

  m_counter = 0;  // reset decay counter after cone resources updated
}

void cGradientCount::generatePeak(cAvidaContext& ctx)
{
  // Get initial peak cell x, y coordinates and movement directions.
  cRandom& rng = ctx.GetRandom();
  int temp_height = 0;
  if (m_plateau < 0) temp_height = 1;
  else temp_height = m_height;
  // If we are not moving the resource we default to the config input m_peakx and m_peaky for 'normal' gradient resources
  // for non-moving halo's we generate a random location on the orbit,
  //   otherwise we get a random location and direction.
  if (m_move_a_scaler > 1) {
    if (!m_halo) {
      m_peakx = rng.GetUInt(m_min_x + temp_height, m_max_x - temp_height + 1);                 
      m_peaky = rng.GetUInt(m_min_y + temp_height, m_max_y - temp_height + 1);
      // Get a random direction for movement on the x-axis  
      m_movesignx = rng.GetInt(-1,2);
      // If x-axis movement is 0, we want to make sure y-axis movement is not also 0  
      if (m_movesignx == 0) {
        m_movesigny = (rng.GetUInt(0,2) == 1) ? -1 : 1;
      } else {
        m_movesigny = rng.GetInt(-1,2);
      }
    } else if (m_halo) {
      m_halo_dir = (rng.GetUInt(0,2) == 1) ? -1 : 1;
      m_changling = (rng.GetUInt(0,2) == 1) ? -1 : 1;
    }
  }
  if (m_halo) {
    int chooseEW = rng.GetUInt(0,2);
    if (chooseEW == 0) {
      m_peakx = rng.GetUInt(m_halo_anchor_x - m_halo_inner_radius - m_halo_width + temp_height, 
                            m_halo_anchor_x - m_halo_inner_radius - temp_height + 1);
    } else {
      m_peakx = rng.GetUInt(m_halo_anchor_x + m_halo_inner_radius + temp_height, 
                            m_halo_anchor_x + m_halo_inner_radius + m_halo_width - temp_height + 1);
    }
    int chooseNS = rng.GetUInt(0,2);
    if (chooseNS == 0) { 
      m_peaky = rng.GetUInt(m_halo_anchor_y - m_halo_inner_radius - m_halo_width + temp_height, 
                            m_halo_anchor_y - m_halo_inner_radius - temp_height + 1);
    } else {
      m_peaky = rng.GetUInt(m_halo_anchor_y + m_halo_inner_radius + temp_height, 
                            m_halo_anchor_y + m_halo_inner_radius + m_halo_width - temp_height + 1);
    }
  }
  
  SetModified(false);
  m_counter = 0;
  m_skip_counter = 0;
  m_just_reset = true;
  refreshResourceValues();
}

void cGradientCount::refreshResourceValues()
{  
  int max_pos_x;
  int min_pos_x;
  int max_pos_y;
  int min_pos_y;
  
  // if we are resetting a resource, we need to calculate new values for the whole world so we can wipe away any residue
  if (m_just_reset) {
    max_pos_x = GetX() - 1;
    min_pos_x = 0;
    max_pos_y = GetY() - 1;
    min_pos_y = 0;
  } else {
    // otherwise we only need to update values within the possible range of the peak 
    // we check all the way back to move_speed to make sure we're not leaving any old residue behind
    max_pos_x = min(m_peakx + m_spread + m_move_speed + 1, GetX() - 1);
    min_pos_x = max(m_peakx - m_spread - m_move_speed - 1, 0);
    max_pos_y = min(m_peaky + m_spread + m_move_speed + 1, GetY() - 1);
    min_pos_y = max(m_peaky - m_spread - m_move_speed - 1, 0);
  }

  if (m_is_plateau_common == 1 && !m_just_reset && m_world->GetStats().GetUpdate() > 0) {
    // with common depletion, new peak height is not the plateau heights, but the delta in plateau heights applied to 
    // peak height from the last time
    m_current_height = m_current_height - m_ave_plat_cell_loss + m_plateau_inflow - (m_current_height * m_plateau_outflow);
    m_common_plat_height = m_common_plat_height - m_ave_plat_cell_loss + m_plateau_inflow - (m_current_height * m_plateau_outflow);
    if (m_common_plat_height > m_plateau && m_plateau >= 0) m_common_plat_height = m_plateau;
    if (m_common_plat_height < 0 && m_plateau >=0) m_common_plat_height = 0;
    if (m_current_height > m_height && m_plateau >= 0) m_current_height = m_height;
    if (m_current_height < 0 && m_plateau >= 0) m_current_height = 0;
  } else {
    m_current_height = m_height;
  }

  int plateau_cell = 0;
  for (int ii = min_pos_x; ii < max_pos_x + 1; ii++) {
    for (int jj = min_pos_y; jj < max_pos_y + 1; jj++) {
      double thisheight = 0.0;
      double thisdist = sqrt((double) (m_peakx - ii) * (m_peakx - ii) + (m_peaky - jj) * (m_peaky - jj));
      if (m_spread >= thisdist) {
        // determine theoretical individual cells values and add one to distance from center 
        // (so that center point = radius 1, not 0)
        // also used to distinguish plateau cells
  
        thisheight = m_current_height / (thisdist + 1);
        
        // set the floor values
        // plateaus will override this so that plateaus can hit 0 when being eaten
        if (thisheight < m_floor) thisheight = m_floor;
        
        // create cylindrical profiles of resources whereever thisheight would be >1 (area where thisdist + 1 <= m_height)
        // and slopes outside of that range
        // plateau = -1 turns off this option; if activated, causes 'peaks' to be flat plateaus = plateau value 
        // this is where we apply inflow and outflow...we are only applying it to plateau cells 
        double find_plat_dist = m_height / (thisdist + 1);
        if ((find_plat_dist >= 1 && m_plateau >= 0) || (m_plateau < 0 && thisdist == 0)) { 
          if (m_just_reset || m_world->GetStats().GetUpdate() <= 0) {
            m_past_height = m_height;
            if (m_plateau >= 0.0) {
              thisheight = m_plateau;
            } 
            else {
              thisheight = m_height;
            }
          } 
          else { 
            if (m_is_plateau_common == 0) {
              m_past_height = m_plateau_array[plateau_cell]; 
              thisheight = m_past_height + m_plateau_inflow - (m_past_height * m_plateau_outflow);
              if (thisheight > m_plateau && m_plateau >= 0) {
                thisheight = m_plateau;
              } 
            }
            else if (m_is_plateau_common == 1) {   
              thisheight = m_common_plat_height;
            }
          } 
          if (thisheight < 0) thisheight = 0;
          m_plateau_array[plateau_cell] = thisheight;
          m_plateau_cell_IDs[plateau_cell] = jj * GetX() + ii;
          plateau_cell ++;
        }
      }
      Element(jj * GetX() + ii).SetAmount(thisheight);
    }
  }         
  SetCurrPeakX(m_peakx);
  SetCurrPeakY(m_peaky);
  m_just_reset = false;
}

void cGradientCount::getCurrentPlatValues()
{ 
  int temp_height = 0;
  if (m_plateau < 0) temp_height = 1;
  else temp_height = m_height;
  int plateau_box_min_x = m_peakx - temp_height - 1;
  int plateau_box_max_x = m_peakx + temp_height + 1;
  int plateau_box_min_y = m_peaky - temp_height - 1;
  int plateau_box_max_y = m_peaky + temp_height + 1;
  int plateau_cell = 0;
  double amount_devoured = 0.0;
  for (int ii = plateau_box_min_x; ii < plateau_box_max_x + 1; ii++) {
    for (int jj = plateau_box_min_y; jj < plateau_box_max_y + 1; jj++) { 
      double thisdist = sqrt((double) (m_peakx - ii) * (double) (m_peakx - ii) + (double) (m_peaky - jj) * (double) (m_peaky - jj));
      double find_plat_dist = temp_height / (thisdist + 1);
      if ((find_plat_dist >= 1 && m_plateau >= 0) || (m_plateau < 0 && thisdist == 0)) {
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

void cGradientCount::generateBarrier(cAvidaContext& ctx)
// If habitat == 2 we are creating barriers to movement (walls), not really gradient resources
{ 
  // generate/regenerate walls when counter == config updatestep
  if (m_topo_counter == m_updatestep) { 
    // reset counter
    m_topo_counter = 1;
    // clear any old resource
    for (int ii = 0; ii < GetX(); ii++) {
      for (int jj = 0; jj < GetY(); jj++) {
        Element(jj * GetX() + ii).SetAmount(0);
      }
    }
    // generate number barriers equal to count 
    for (int i = 0; i < m_count; i++) {
      // drop the anchor/first block for current barrier
      int start_randx = 0;
      int start_randy = 0;   
      if (m_config == 3 || m_config == 4) { 
        start_randx = m_peakx;
        start_randy = m_peaky;
      }
      else {
        start_randx = ctx.GetRandom().GetUInt(0, GetX());
        start_randy = ctx.GetRandom().GetUInt(0, GetY());  
      }
      Element(start_randy * GetX() + start_randx).SetAmount(m_plateau);

      int randx = start_randx;
      int randy = start_randy;
      int prev_blockx = randx;
      int prev_blocky = randy;
      int cornerx = prev_blockx;
      int cornery = prev_blocky;
      bool place_corner = false;

      // decide the size of the current barrier
      int rand_block_count = ctx.GetRandom().GetUInt(m_min_size, m_max_size + 1);
      // for vertical or horizontal wall building, pick a random direction once for the whole wall
      int direction = ctx.GetRandom().GetUInt(0,2);
      
      for (int num_blocks = 0; num_blocks < rand_block_count; num_blocks++) {
        // if config == 0, build random shaped walls
        if (m_config == 0) {
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
        // if config == 1, build vertical walls
        else if (m_config == 1) {
          // choose up/down build direction
          if (direction == 0) randy = randy - 1;
          else randy = randy + 1;
        }
        // if config == 2, build horizontal walls
        else if (m_config == 2) {
          // choose left/right build direction
          if (direction == 0) randx = randx - 1;
          else randx = randx + 1;
        }       
        // if config == 3, build vertical walls from north to south
        else if (m_config == 3) randy = randy + 1;
        // if config == 4, build horizontal walls from west to east
        else if (m_config == 4) randx = randx + 1;
        
        bool count_block = true;
        // place the new block(s) if not off edge of world
        if (randy < GetY() && randy >= 0 && randx < GetX() && randx >= 0) {
          // if we are trying to build across an inner_radius 
          // or for random walls, if there is already a block here
          // don't count or place this one (continue walking across inner_radius)
          if ((randx < (m_halo_anchor_x + m_halo_inner_radius) && 
               randy < (m_halo_anchor_y + m_halo_inner_radius) && 
               randx > (m_halo_anchor_x - m_halo_inner_radius) && 
               randy > (m_halo_anchor_y - m_halo_inner_radius)) || 
              (m_config == 0 && Element(randy * GetX() + randx).GetAmount())) {
            num_blocks --;
            count_block = false;
          }
          if (count_block) {
            Element(randy * GetX() + randx).SetAmount(m_plateau);
            if (place_corner) {
              if (cornery < GetY() && cornery >= 0 && cornerx < GetX() && cornerx >= 0) {
                if ( ! ((cornerx < (m_halo_anchor_x + m_halo_inner_radius) && 
                     cornery < (m_halo_anchor_y + m_halo_inner_radius) && 
                     cornerx > (m_halo_anchor_x - m_halo_inner_radius) && 
                     cornery > (m_halo_anchor_y - m_halo_inner_radius))) ){
                  Element(cornery * GetX() + cornerx).SetAmount(m_plateau);
                }
              }
            }
          }
        }
        // if the wall is horizontal or vertical build and we went off the world edge, build from the opposite direction
        else if (m_config == 1 || m_config == 2) {
          randx = start_randx; 
          randy = start_randy; 
          direction = direction * -1;
          num_blocks --;
        }
        // if a random build and we went off the world edge, backup a block and try again
        else if (m_config == 0) {
          randx = prev_blockx;
          randy = prev_blocky;
          num_blocks --;
        }
      }      
    }
  }
  else m_topo_counter++; 
}

void cGradientCount::generateHills(cAvidaContext& ctx)
// If habitat == 1 we are creating hills which slow movement, not really gradient resources
{ 
  cRandom& rng = ctx.GetRandom();
  // generate/regenerate hills when counter == config updatestep
  if (m_topo_counter == m_updatestep) { 
    // reset counter
    m_topo_counter = 1;
    // since we are potentially plotting more than one hill per resource, we need to wipe the world before we start
    for (int ii = 0; ii < GetX(); ii++) {
      for (int jj = 0; jj < GetY(); jj++) {
        Element(jj * GetX() + ii).SetAmount(0);
      }
    }

    // generate number hills equal to count
    for (int i = 0; i < m_count; i++) {
      // decide the size of the current hill
      int rand_hill_radius = ctx.GetRandom().GetUInt(m_min_size, m_max_size + 1);
      
      // choose the peak center for current hill, keeping the entire hill outside of any inner_radius
      int chooseEW = rng.GetUInt(0,2);
      if (chooseEW == 0) {
        m_peakx = rng.GetUInt(rand_hill_radius, m_halo_anchor_x - m_halo_inner_radius - rand_hill_radius);
      } else {
        m_peakx = rng.GetUInt(m_halo_anchor_x + m_halo_inner_radius + rand_hill_radius, GetX() - 1 - rand_hill_radius);
      }
      int chooseNS = rng.GetUInt(0,2);
      if (chooseNS == 0) { 
        m_peaky = rng.GetUInt(rand_hill_radius, m_halo_anchor_y - m_halo_inner_radius - rand_hill_radius);
      } else {
        m_peaky = rng.GetUInt(m_halo_anchor_y + m_halo_inner_radius + rand_hill_radius, GetY() - 1 - rand_hill_radius);
      }

      // figure the coordinate extent of each hill (box)
      int max_pos_x = min(m_peakx + rand_hill_radius + 1, GetX() - 1);
      int min_pos_x = max(m_peakx - rand_hill_radius - 1, 0);
      int max_pos_y = min(m_peaky + rand_hill_radius + 1, GetY() - 1);
      int min_pos_y = max(m_peaky - rand_hill_radius - 1, 0);

      // look to place new cell values within a box around the hill center
      for (int ii = min_pos_x; ii < max_pos_x + 1; ii++) {
        for (int jj = min_pos_y; jj < max_pos_y + 1; jj++) {
          double thisheight = 0.0;
          double thisdist = sqrt((double) (m_peakx - ii) * (m_peakx - ii) + (m_peaky - jj) * (m_peaky - jj));
          // only plot values when within set config radius & if no larger amount has already been plotted for another overlapping hill
          if ((thisdist <= rand_hill_radius) && (Element(jj * GetX() + ii).GetAmount() <  m_plateau / (thisdist + 1))) {
          thisheight = m_plateau / (thisdist + 1);
          Element(jj * GetX() + ii).SetAmount(thisheight);
          }
        }
      }
    }
  }
  else m_topo_counter++; 
}

void cGradientCount::ResetGradRes(cAvidaContext& ctx, int worldx, int worldy)
{
  if ((m_move_speed >= (2 * (m_halo_inner_radius + m_halo_width))) && ((m_halo_inner_radius + m_halo_width) != 0)
      && m_move_speed != 0) {
    m_world->GetDriver().RaiseFatalException(-1, "Move speed greater or equal to 2*Radius");    
  }
  if (m_halo == 1 && (m_halo_width < (2 * m_height) && m_plateau >= 0)) {
    m_world->GetDriver().RaiseFatalException(-1, "Halo width < 2 * height (aka plateau radius)");
  }
  if (m_move_speed < 0) {
    m_skip_moves = abs(m_move_speed);
    m_move_speed = 1;
  }
  m_plateau_array.Resize(int(4 * m_height * m_height + 0.5));
  m_plateau_array.SetAll(0);
  m_plateau_cell_IDs.Resize(int(4 * m_height * m_height + 0.5));
  m_plateau_cell_IDs.SetAll(0);
  m_current_height = m_height;
  m_common_plat_height = m_plateau;
  ResizeClear(worldx, worldy, m_geometry);
  if (m_habitat == 2) {
    generateBarrier(ctx);
  }
  else if (m_habitat == 1) {
    generateHills(ctx);
  }
  else {
    generatePeak(ctx);
    UpdateCount(ctx);
  }
}

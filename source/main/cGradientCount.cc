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

#include "avida/core/cWorldDriver.h"

#include "cAvidaContext.h"
#include "cStats.h"
#include "cWorld.h"

#include <cmath>
#include <iostream>


/* cGradientCount is designed to give moving peaks of resources. Peaks are capped with plateaus. The slope of the peaks
is height / distance. Consequently, when height = distance from center of peak, the value at that cell = 1. This was 
designed this way because the organims used for this could only consume resources when the value is >= 1. Thus, height also 
gives radius of 'edible' resources (aka the plateau). If plateaus are >1, you get sloped edges leading up to plateau 
cylinders.
  Spread gives the radius of the entire resource peak to the outside of the sloped edge. Organisms could detect resources 
all along the spread, but only consume that portion on the plateau. Thus, spread - plateau = sense radius (smell) while 
plateau = consumable radius (actual food).
  Peaks move within the boundaries set by min/max x and y. If the edible portion of the peak hits the boundary, the peak 
'bounces' (sign of direction of movement changes).
  Smoothness of the movement is controlled by move_a_scaler which is the A in eq1 in Morrison & DeJong 1999. A-values 
need to be between 1 and 4. Values of 1 to ~3 give smooth movements. Larger values should yield chaotic moves.
  Peaks stop moving when they are first bitten.
  Peak values will only be refreshed when either all edible portions (>=1) are consumed or when the decay timestep (in 
updates) is reached, whichever comes first.
  Once bitten, peaks will not move again until refreshed.
  Peak values are refreshed to match initial height, spread, and plateau, but for non-halo peaks, the placement of the 
refreshed peak is random within the min/max x and y area. For halo peaks, the peak is currently refreshed at the SE 
corner of the orbit.
cGradientCount cannot access the random number generator at the very first update. Thus, it uses the DefaultContext initially*/

cGradientCount::cGradientCount(cWorld* world, int peakx, int peaky, double height, double spread, double plateau, int decay, 
                               int max_x, int max_y, int min_x, int min_y, double move_a_scaler, int updatestep,  
                               int worldx, int worldy, int geometry, int halo, int halo_inner_radius, int halo_width,
                               int halo_anchor_x, int halo_anchor_y, int move_speed, 
                               double plateau_inflow, double plateau_outflow)
  : m_world(world)
  , m_peakx(peakx), m_peaky(peaky)
  , m_height(height), m_spread(spread), m_plateau(plateau), m_decay(decay)
  , m_max_x(max_x), m_max_y(max_y), m_min_x(min_x), m_min_y(min_y)
  , m_move_a_scaler(move_a_scaler), m_updatestep(updatestep)
  , m_halo(halo), m_halo_inner_radius(halo_inner_radius), m_halo_width(halo_width)
  , m_halo_anchor_x(halo_anchor_x), m_halo_anchor_y(halo_anchor_y)
  , m_move_speed(move_speed), m_plateau_inflow(plateau_inflow), m_plateau_outflow(plateau_outflow)
  , m_move_y_scaler(0.5)
  , m_counter(0)
  , m_move_counter(1)
  , m_just_reset(true)
  , m_past_height(0.0)
{
  if ((m_move_speed >= 2 * (m_halo_inner_radius + m_halo_width)) && ((m_halo_inner_radius + m_halo_width) != 0)
      && m_move_speed != 0) {
    m_world->GetDriver().RaiseFatalException(-1, "Move speed greater or equal to 2*Radius");
  }				
  m_plateau_array.Resize(4 * m_height * m_height);
  m_plateau_array.SetAll(0);
  m_plateau_cell_IDs.Resize(4 * m_height * m_height);
  m_plateau_cell_IDs.SetAll(0);
  ResizeClear(worldx, worldy, geometry);
  generatePeak(m_world->GetDefaultContext());
  UpdateCount(m_world->GetDefaultContext());
}

void cGradientCount::UpdateCount(cAvidaContext& ctx)
{
  bool has_edible = false;
  bool has_been_bitten = false;
  
  // determine if there is any edible food left in the peak (don't refresh the peak values until decay kicks in if there is edible food left) 
  // to speed things up, we only check cells within the possible spread of the peak
  // and we only need to do this if decay > 1 (if decay == 1, we're going to reset everything regardless of the amount left)
  // if decay = 1 and the resource IS depletable, that means we have a moving depleating resource! Odd, but useful.
  if (m_decay > 1) {
    int max_pos_x = min(int(m_peakx + m_spread + 1), GetX() - 1);
    int min_pos_x = max(int(m_peakx - m_spread - 1), 0);
    int max_pos_y = min(int(m_peaky + m_spread + 1), GetY() - 1);
    int min_pos_y = max(int(m_peaky - m_spread - 1), 0);
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
    
  // before we move anything, if we have moving depletable resource, we need to get the current plateau cell values  
  if (m_move_a_scaler > 1 && m_decay == 1) {
    int plateau_box_min_x = m_peakx - m_height - 1;
    int plateau_box_max_x = m_peakx + m_height + 1;
    int plateau_box_min_y = m_peaky - m_height - 1;
    int plateau_box_max_y = m_peaky + m_height + 1;
    int plateau_cell = 0;
    for (int ii = plateau_box_min_x; ii < plateau_box_max_x + 1; ii++) {
      for (int jj = plateau_box_min_y; jj < plateau_box_max_y + 1; jj++) {        
        double thisdist = sqrt((m_peakx - ii) * (m_peakx - ii) + (m_peaky - jj) * (m_peaky - jj));
        double thisheight = m_height / (thisdist + 1);
        if (thisheight >= 1 && m_plateau >= 0.0) {
          m_past_height = m_plateau_array[plateau_cell];
          double pre_move_height = Element(m_plateau_cell_IDs[plateau_cell]).GetAmount();              
           if (pre_move_height < m_past_height) {
            m_plateau_array[plateau_cell] = Element(m_plateau_cell_IDs[plateau_cell]).GetAmount();    
          }
          plateau_cell ++;
        }
      }        
    }
  } 
  
  // When the counter matches decay, regenerate resource peak
  if (m_counter == m_decay) generatePeak(ctx);
  
  // move cones by moving m_peakx & m_peaky, but only if the cone has not been bitten 
  // keep cones inside their bounding boxes; bounce them if they hit the edge
  m_move_y_scaler = m_move_a_scaler * m_move_y_scaler * (1 - m_move_y_scaler); 
  
  // we use movesign to determine direction of peak movement
  // first, to get smooth movements, we only allow either the x or y direction change to be evaluated in a single update
  // second, we then decide the change of direction based on the current direction so that peak can't 'jump' from -1 to 1, 
  // without first changing to 0
  // finally, we only do this only when # updates since last change = updatestep to slow the frequency of path changes


  //we add 1 to distance to account for the anchor grid cell
  int current_orbit = max(abs(m_halo_anchor_x - m_peakx), abs(m_halo_anchor_y - m_peaky)) + 1;

  if (m_move_counter == m_updatestep) { 
    m_move_counter = 1;
    //halo resources orbit at a fixed org walking distance from an anchor point
    //if halo width > the height of the halo resource, the resource will be bounded inside the halo but the orbit can vary within those bounds
    //halo's are actually square in avida because, at a given orbit, this keeps a constant distance (in number of steps and org would have to take)
    //    between the anchor point and any orbit
    if (m_halo == 1) {    //choose to change orbit (0) or direction (1)    
      int old_peakx = m_peakx;
      int old_peaky = m_peaky;
      int random_shift = ctx.GetRandom().GetUInt(0,2);
      //if changing orbit, choose to go in or out one orbit
      //then figure out if we need change the x or the y to shift orbit (based on what quadrant we're in)
      if (random_shift == 0) {
        //do nothing unless there's room to change orbit
        if (m_halo_width > (m_height * 2)) {
          m_orbit_shift = ctx.GetRandom().GetUInt(0,2); 
          if (m_orbit_shift == 0) {
            current_orbit = current_orbit - 1; 
            if (abs(m_halo_anchor_y - m_peaky) > abs(m_halo_anchor_x - m_peakx))
              m_peaky = old_peaky - 1;
            else 
              m_peakx = old_peakx - 1;
          }
          else if (m_orbit_shift == 1) {
            current_orbit = current_orbit + 1;  
            if (abs(m_halo_anchor_y - m_peaky) > abs(m_halo_anchor_x - m_peakx))
              m_peaky = old_peaky + 1;
            else 
              m_peakx = old_peakx + 1;
          }
          //we have to check that we are still going to be within the halo after an orbit change       
          if (current_orbit > (m_halo_inner_radius + m_halo_width - m_height + 2)) {
            //if we go out of bounds, we need to go the other way instead (taking two steps back on orbit since we already took one in the wrong direction)
            current_orbit = current_orbit - 2;
            if (abs(m_halo_anchor_y - old_peaky) > abs(m_halo_anchor_x - old_peakx))
              m_peaky = old_peaky + 1;
            else 
              m_peakx = old_peakx + 1;
          }
          else if (current_orbit < (m_halo_inner_radius + m_height + 2)) {
            current_orbit = current_orbit + 2;
            if (abs(m_halo_anchor_y - old_peaky) > abs(m_halo_anchor_x - old_peakx))
              m_peaky = old_peaky - 1;
            else 
              m_peakx = old_peakx - 1;          
          }
        }
        //if there was no room to change orbit, change the direction instead of the orbit
        else random_shift = 1;
      }
      //if changing direction of rotation, we just switch sign of rotation
      else if (random_shift == 1) {
        m_halo_dir = m_halo_dir * -1; 
      }
    }
    //for non-halo peaks
    else {
      int choosesign = ctx.GetRandom().GetInt(0,3);
      if (choosesign == 1) {
        if (m_movesignx == -1) m_movesignx = ctx.GetRandom().GetInt(-1,1); 
        else if (m_movesignx == 1) m_movesignx = ctx.GetRandom().GetInt(0,2);
        else m_movesignx = ctx.GetRandom().GetInt(-1,2);
      }      
      if (choosesign == 2){ 
        if (m_movesigny == -1) m_movesigny = ctx.GetRandom().GetInt(-1,1); 
        else if (m_movesigny == 1) m_movesigny = ctx.GetRandom().GetInt(0,2);
        else m_movesigny = ctx.GetRandom().GetInt(-1,2);
      }
    }
  } 
  else {
    m_move_counter++;
  }
  
  if (m_halo == 1) { 
    //what quadrant we are in determines whether we are changing x's or y's (= changling)
    //if we are on a corner, we just stick with the current changling
    if (abs(m_halo_anchor_y - m_peaky) > abs(m_halo_anchor_x - m_peakx))
      m_changling = 1;
    else if (abs(m_halo_anchor_y - m_peaky) < abs(m_halo_anchor_x - m_peakx))
      m_changling = -1;
    
    if (m_changling == 1) {
      //check to make sure the move will not put peak beyond the bounds (at corner) of the orbit
      //if it will go beyond the bounds of the orbit, turn the corner (e.g. if move = 5 & space to move on x =2, move 2 on x and 3 on y)
      int next_posx = m_peakx + (m_halo_dir * m_move_speed);
      int max_orbit_x = m_halo_anchor_x + current_orbit - 1;
      int min_orbit_x = m_halo_anchor_x - current_orbit + 1;
      int current_x = m_peakx;
      if (next_posx > max_orbit_x) {
        m_peakx = max_orbit_x;
        if (m_peaky > m_halo_anchor_y) {      
          //turning this corner means changing the sign of the movement once we switch from moving along x to moving along y
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
    // for non-halo peaks
    double temp_peakx = m_peakx + (m_move_y_scaler * m_movesignx);
    double temp_peaky = m_peaky + (m_move_y_scaler * m_movesigny);
    
    if (temp_peakx > (m_max_x - m_height)) m_movesignx = -1.0;
    if (temp_peakx < (m_min_x + m_height)) m_movesignx = 1.0; 
    
    if (temp_peaky > (m_max_y - m_height)) m_movesigny = -1.0;
    if (temp_peaky < (m_min_y + m_height)) m_movesigny = 1.0;
    
    m_peakx = m_peakx + (m_movesignx * m_move_y_scaler) + .5;
    m_peaky = m_peaky + (m_movesigny * m_move_y_scaler) + .5; 
  } 
  
  // to speed things up, we only check cells within the possible spread of the peak
  // and we only do this if the resource is set to actually move (or we just reset a non-moving resource)
  if (m_move_a_scaler > 1 || (m_move_a_scaler == 1 && m_just_reset)) refreshResourceValues();
  
  m_counter = 0;  // reset decay counter after cone resources updated
}

void cGradientCount::generatePeak(cAvidaContext& ctx)
{
  // Get initial peak cell x, y coordinates and movement directions.
  cRandom& rng = ctx.GetRandom();
  
  // If we are not moving the resource we can skip creating a peak location and use the config input m_peakx and m_peaky.
  if (!m_halo) {
    if (m_move_a_scaler > 1) {
      m_peakx = rng.GetUInt(m_min_x + m_height, m_max_x - m_height + 1);                 
      m_peaky = rng.GetUInt(m_min_y + m_height, m_max_y - m_height + 1);
    }
    m_movesignx = rng.GetInt(-1,2);  
    if (m_movesignx == 0) {
      if (rng.GetUInt(0,2)) {
        m_movesigny = 1;
      } else {
        m_movesigny = -1;
      }
    } else {
      m_movesigny = rng.GetInt(-1,2);
    }
    
  } else if (m_halo) {
    if (m_move_a_scaler > 1) {
      m_halo_dir = (rng.GetUInt(0,2) == 1) ? -1 : 1;
      m_changling = (rng.GetUInt(0,2) == 1) ? -1 : 1;
    }
    int chooseEW = rng.GetUInt(0,2);
    if (chooseEW == 0) {
      m_peakx = rng.GetUInt(m_halo_anchor_x - m_halo_inner_radius - m_halo_width + m_height, 
                            m_halo_anchor_x - m_halo_inner_radius);
    } else {
      m_peakx = rng.GetUInt(m_halo_anchor_x + m_halo_inner_radius, 
                            m_halo_anchor_x + m_halo_inner_radius + m_halo_width - m_height + 1);
    }
    int chooseNS = rng.GetUInt(0,2);
    if (chooseNS == 0) { 
      m_peaky = rng.GetUInt(m_halo_anchor_y - m_halo_inner_radius - m_halo_width + m_height, 
                            m_halo_anchor_y - m_halo_inner_radius);
    } else {
      m_peaky = rng.GetUInt(m_halo_anchor_y + m_halo_inner_radius, 
                            m_halo_anchor_y + m_halo_inner_radius + m_halo_width - m_height + 1);
    }
  }
  SetModified(false);
  m_counter = 0;
  m_just_reset = true;
  refreshResourceValues();
  return;
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
    max_pos_x = min(int(m_peakx + m_spread + m_move_speed + 1), GetX() - 1);
    min_pos_x = max(int(m_peakx - m_spread - m_move_speed - 1), 0);
    max_pos_y = min(int(m_peaky + m_spread + m_move_speed + 1), GetY() - 1);
    min_pos_y = max(int(m_peaky - m_spread - m_move_speed - 1), 0);
  }
  
  int plateau_cell = 0;
  for (int ii = min_pos_x; ii < max_pos_x + 1; ii++) {
    for (int jj = min_pos_y; jj < max_pos_y + 1; jj++) {
      double thisheight = 0.0;
      double thisdist = sqrt((m_peakx - ii) * (m_peakx - ii) + (m_peaky - jj) * (m_peaky - jj));
      
      if (m_spread >= thisdist) {
        // determine individual cells values and add one to distance from center (e.g. so that center point = radius 1, not 0)
        thisheight = m_height / (thisdist + 1);
        
        // create cylindrical profiles of resources whereever thisheight would be >1 (area where thisdist + 1 <= m_height)
        // and slopes outside of that range
        // plateau = -1 turns off this option; if activated, causes 'peaks' to be flat plateaus = plateau value 
        // this is where we apply inflow and outflow...we are only applying it to plateau cells (so we don't have to worry 
        // about changing slope values).
        if (thisheight >= 1 && m_plateau >= 0.0) {
          if (m_just_reset || m_world->GetStats().GetUpdate() <= 0) {
            thisheight = m_plateau;
          } else {
            m_past_height = m_plateau_array[plateau_cell]; 
            thisheight = m_past_height + m_plateau_inflow - (m_past_height * m_plateau_outflow); 
          }
          if (thisheight > m_plateau) {
            thisheight = m_plateau;
          }
          m_plateau_array[plateau_cell] = thisheight;
          m_plateau_cell_IDs[plateau_cell] = jj * GetX() + ii;
          plateau_cell ++;
        }
      }
      Element(jj * GetX() + ii).SetAmount(thisheight);
    }
  }
  m_just_reset = false;
}

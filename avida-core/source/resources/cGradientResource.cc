//
//  cGradientResource.cpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/15/18.
//

#include "cGradientResource.h"
#include "cAvidaContext.h"


#include <cmath>


cGradientResource::cGradientResource(const cGradientResource& _res)
: cAbstractResource(_res)
{
  *this = _res;
}

cGradientResource& cGradientResource::operator=(const cGradientResource& _res)
{
  #define cp(NAME) m_ ## NAME = _res.m_ ## NAME ;
  cp(geometry);
  cp(peaks);
  cp(min_height);
  cp(min_radius);
  cp(radius_range);
  cp(ah);
  cp(ar);
  cp(acx);
  cp(acy);
  cp(update_step);
  cp(hstepscale);
  cp(rstepscale);
  cp(cstepscalex);
  cp(cstepscaley);
  cp(hstep);
  cp(rstep);
  cp(cstepx);
  cp(cstepy);
  cp(update_dynamic);
  cp(peakx);
  cp(peaky);
  cp(height);
  cp(spread);
  cp(plateau);
  cp(decay);
  cp(max_x);
  cp(max_y);
  cp(min_x)
  cp(min_y);
  cp(move_a_scaler);
  cp(updatestep);
  cp(halo);
  cp(halo_inner_radius);
  cp(halo_width);
  cp(halo_anchor_x);
  cp(halo_anchor_y);
  cp(skip_moves);
  cp(move_speed);
  cp(move_resistance);
  cp(plateau_inflow);
  cp(plateau_outflow);
  cp(cone_inflow);
  cp(cone_outflow);
  cp(gradient_inflow);
  cp(is_plateau_common)
  cp(floor);
  cp(habitat);
  cp(min_size);
  cp(max_size);
  cp(config);
  cp(count);
  cp(resistance);
  cp(threshold);
  cp(refuge);
  cp(predator_odds);
  cp(predator);
  cp(is_path);
  #undef cp
  return *this;
}


/* 
cGradientResourceAcct is designed to give moving peaks of resources.
Peaks are <optionally> capped with plateaus. The slope of the peaks is
height / distance. Consequently, when height = distance from center of
peak, the value at that cell = 1. This was designed this way because the
organims used for this could only consume resources when the value is >=
1. Thus, height also gives radius of 'edible' resources (aka the
plateau). If plateaus are >1, you get sloped edges leading up to plateau
cylinders.

Spread gives the radius of the entire resource peak to the outside of
the sloped edge. Organisms could detect resources all along the spread,
but only consume that portion on the plateau. Thus, spread - plateau =
sense radius (smell) while plateau = consumable radius (actual food).

Peaks move within the boundaries set by min/max x and y. If the plateau
/ edible portion of the peak hits the boundary, the peak 'bounces' (sign
of direction of movement changes).

Smoothness of the movement is controlled by move_a_scaler which is the A
in eq1 in Morrison & DeJong 1999. A-values need to be between 1 and 4.
Values of 1 to ~3 give smooth movements. Larger values should yield
chaotic moves. However, beyond establishing that peaks don't move when
the value = 1 and do move when the value > 1, the effects of A-values
have not really been evaluated.

If depletable (via reaction) peaks stop moving when they are first
bitten.

Depletable peaks will be refreshed when either all edible portions (>=1)
are consumed or when the decay timestep (in updates) is reached,
whichever comes first.

Once bitten, depletable peaks will not move again until refreshed.

Peak values are refreshed to match initial height, spread, and plateau,
but for non-halo peaks, the placement of the refreshed peak is random
within the min/max x and y area. For halo peaks, the peak is currently
refreshed at the SE corner of the orbit. cGradientCount cannot access
the random number generator at the very first update. Thus, it uses the
DefaultContext initially.

We use movesign to determine direction of peak movement

First, to get smooth movements, for non-halo resources we only allow
either the x or y direction change to be evaluated in a single update.
For halo resources, we only evaluate either the orbit or the direction
in a given update.

Second, we then decide the change of direction based on the current
direction, e.g. so that non-halo peak movesigns can't 'jump' from -1 to
1, without first changing to 0

Finally, we only toy with movement direction when # updates since last
change = updatestep.
 */

cGradientResourceAcct::cGradientResourceAcct(cGradientResource& res, int size_x, int size_y, const cCellBox& cellbox,
    cPopulation* pop)
    : cAbstractSpatialResourceAcct(size_x, size_y, cellbox)
    , m_res(res)
    , m_cells(size_x, size_y, cellbox, cSpatialCountElem())
    , m_peakx(res.m_peakx)
    , m_peaky(res.m_peaky)
    , m_modified(false)
{
}

void cGradientResourceAcct::UpdateCount(cAvidaContext& ctx)
{ 
  m_old_peakx = m_peakx;
  m_old_peaky = m_peaky;
  if (m_res.m_habitat == 2) GenerateBarrier(ctx);
  else if (m_res.m_habitat == 1) GenerateHills(ctx);
  else if (m_probabilistic) UpdateProbabilisticResource();
  else UpdatePeakRes(ctx);
}

void cGradientResourceAcct::SetGradInitialPlat(double plat_val) 
{
  m_initial_plat = plat_val; 
  m_initial = true; 
}


void cGradientResourceAcct::SetGradPlatVarInflow(cAvidaContext& ctx, double mean, double variance, int type)
{
  if (variance > 0) {
    m_mean_plat_inflow = mean;
    m_var_plat_inflow = variance;
    double the_inflow = 0;
    if (type == 0) {
      the_inflow = abs(ctx.GetRandom().GetRandNormal(mean, variance));
      m_res.SetPlateauInflow(the_inflow);
    }
    else if (type < 0) { 
      the_inflow = abs(ctx.GetRandom().GetRandNormal(0, variance));
      if (mean - the_inflow < 0) the_inflow = mean;
      m_res.SetPlateauInflow(mean - the_inflow);
    }
    else if (type == 1) {
      the_inflow = abs(ctx.GetRandom().GetRandNormal(0, variance));
      m_res.SetPlateauInflow(mean + the_inflow);
    }
    else if (type == 2) {
      the_inflow = ctx.GetRandom().GetRandNormal(0, variance);
      if (mean + the_inflow < 0) the_inflow = mean;
      m_res.SetPlateauInflow(mean + the_inflow);
    }
  }
  else m_res.SetPlateauInflow(mean);
}


void cGradientResourceAcct::SetPredatoryResource(double odds, int juvsper)
{
  m_predator = true;
  m_pred_odds = odds;
  m_guarded_juvs_per_adult = juvsper;
}



void cGradientResourceAcct::SetProbabilisticResource(cAvidaContext& ctx, double initial, double inflow,
    double outflow, double lambda, double theta, int x, int y, int num_cells)
{
  m_probabilistic = true;
  m_initial_plat = initial;
  m_res.m_plateau_inflow = inflow;
  m_res.m_plateau_outflow = outflow;
  
  BuildProbabilisticResource(ctx, lambda, theta, x , y, num_cells);
}


void cGradientResourceAcct::BuildProbabilisticResource(cAvidaContext& ctx, double lambda, double theta,
    int x, int y, int num_cells)
{
  if (m_min_usedx != -1) ClearExistingProbResource();
  ResetUsedBounds();
  int cells_used = 0;
  const int worldx = m_size_x;
  const int worldy = m_size_y;
  int world_size = worldx * worldy;
  int max_idx = world_size - 1;
  int max_tries = std::min(1000, world_size);
  
  Apto::Array <int> cell_id_array;
  cell_id_array.ResizeClear(world_size);
  for (int i = 0; i < cell_id_array.GetSize(); i++) cell_id_array[i] = i;
  
  if (x == -1) m_peakx = ctx.GetRandom().GetUInt(0, worldx);
  else m_peakx = x;
  if (y == -1) m_peaky = ctx.GetRandom().GetUInt(0, worldy); 
  else m_peaky = y;
  
  if (num_cells != -1) m_prob_res_cells.ResizeClear(num_cells);

  // only if theta == 1 do want want a 'hill' with resource for certain in the center
  if (theta == 0) {
    m_abundance[m_peaky * worldx + m_peakx] = m_initial_plat;
    if (m_initial_plat > 0) UpdateBounds(m_peakx, m_peaky);
    if (m_res.m_plateau_outflow > 0 || m_res.m_plateau_inflow > 0) { 
      if (num_cells == -1) m_prob_res_cells.Push(m_peaky * worldx + m_peakx);
      else m_prob_res_cells[cells_used] = m_peaky * worldx + m_peakx;
    }  
    cells_used++;
    // no need to pop this cell off the array, just move it and don't check that far anymore
    cell_id_array.Swap(m_peaky * worldx + m_peakx, max_idx--);
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
    
    int cell_idx = ctx.GetRandom().GetUInt(max_unused_idx + 1);
    int cell_id = cell_id_array[cell_idx];
    int this_x = cell_id % worldx;
    int this_y = cell_id / worldx;  
    double cell_dist = sqrt((double) (m_peakx - this_x) * (m_peakx - this_x) + (m_peaky - this_y) * (m_peaky - this_y));
    // use a half normal
    double this_prob = (1/lambda) * (sqrt(2 / 3.14159)) * exp(-0.5 * pow(((cell_dist - theta) / lambda), 2));
    
    if (ctx.GetRandom().P(this_prob)) {
      m_abundance[cell_id] = m_initial_plat;
      if (m_initial_plat > 0) UpdateBounds(this_x, this_y);
      if (m_res.m_plateau_outflow > 0 || m_res.m_plateau_inflow > 0) {
        if (loop_once) m_prob_res_cells.Push(cell_id);
        else m_prob_res_cells[cells_used] = cell_id;
      }              
      cells_used++;
      cell_id_array.Swap(cell_idx, max_idx--);
    }
    // just push this cell out of the way for this loop, but keep it around for next time
    else { 
      m_abundance[cell_id] = 0.0; 
      cell_id_array.Swap(cell_idx, max_unused_idx--);
    }

    if (cells_used >= num_cells && !loop_once) break;
    if (max_unused_idx <= 0 && loop_once) break;
    if (max_idx <= 0) break;
  }
  // in case we couldn't fill the quota...
  while (m_prob_res_cells.GetSize() > cells_used) m_prob_res_cells.Pop();
}


void cGradientResourceAcct::UpdateProbabilisticResource()
{
  if (m_res.m_plateau_outflow > 0 || m_res.m_plateau_inflow > 0) {
    for (int i = 0; i < m_prob_res_cells.GetSize(); i++) {
      double curr_val = m_abundance[m_prob_res_cells[i]];
      double amount = curr_val + m_res.m_plateau_inflow - (curr_val * m_res.m_plateau_outflow);
      m_abundance[m_prob_res_cells[i]] = amount; 
      if (amount > 0) UpdateBounds(m_prob_res_cells[i] % m_size_x, m_prob_res_cells[i] / m_size_x);
    }
  }
}

void cGradientResourceAcct::ResetGradRes(cAvidaContext& ctx, int world_x, int world_y)
{
  if ((m_res.m_move_speed >= (2 * (m_res.m_halo_inner_radius + m_res.m_halo_width))) && ((m_res.m_halo_inner_radius + m_res.m_halo_width) != 0)
      && m_res.m_move_speed != 0) {
    m_res.m_feedback.Error("Move speed greater or equal to 2*Radius");
    return;
  }
  if (m_res.m_halo == 1 && (m_res.m_halo_width < (2 * m_res.m_height) && m_res.m_plateau >= 0)) {
    m_res.m_feedback.Error("Halo width < 2 * height (aka plateau radius)");
    return;
  }
  if (m_res.m_move_speed < 0) {
    m_res.m_skip_moves = std::abs(m_res.m_move_speed);
   m_res. m_move_speed = 1;
  }
  m_plateau_array.Resize(int(4 * m_res.m_height * m_res.m_height + 0.5));
  m_plateau_array.SetAll(0);
  m_plateau_cell_IDs.Resize(int(4 * m_res.m_height * m_res.m_height + 0.5));
  m_plateau_cell_IDs.SetAll(0);
  m_prob_res_cells.Resize(0);
  m_wall_cells.Resize(0);
  m_current_height = m_res.m_height;
  m_common_plat_height = m_res.m_plateau;
  m_mean_plat_inflow = m_res.m_plateau_inflow;
  m_var_plat_inflow = 0;
  ResetUsedBounds();
  
  m_initial = true;
  m_abundance.Clear();
  if (m_res.m_habitat == 2) {
    m_topo_counter = m_res.m_updatestep;
    GenerateBarrier(ctx);
  }
  else if (m_res.m_habitat == 1) {
    m_topo_counter = m_res.m_updatestep;
    GenerateHills(ctx);
  }
  else {
    GeneratePeak(ctx);
    UpdateCount(ctx);
  }
  
  // set m_initial to false now that we have reset the resource
  m_initial = false;
}


void cGradientResourceAcct::FillInResourceValues()
{
  int max_pos_x;
  int min_pos_x;
  int max_pos_y;
  int min_pos_y;
  ResetUsedBounds();

  // if we are resetting a resource, we need to calculate new values for the whole world so we can wipe away any residue
  if (m_just_reset) {
    if (m_min_usedx == -1 || m_min_usedy == -1 || m_max_usedx == -1 || m_max_usedy == -1) {
      max_pos_x = m_size_x - 1;
      min_pos_x = 0;
      max_pos_y = m_size_y - 1;
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
    max_pos_x = std::min(m_peakx + m_res.m_spread + m_res.m_move_speed + 1, m_size_x - 1);
    min_pos_x = std::max(m_peakx - m_res.m_spread - m_res.m_move_speed - 1, 0);
    max_pos_y = std::min(m_peaky + m_res.m_spread + m_res.m_move_speed + 1, m_size_y - 1);
    min_pos_y = std::max(m_peaky - m_res.m_spread - m_res.m_move_speed - 1, 0);
  }

  if (m_res.m_is_plateau_common == 1 && !m_just_reset && cGradientResourceAcct::m_update > 0) {
    // with common depletion, new peak height is not the plateau heights, but the delta in plateau heights applied to 
    // peak height from the last time
    m_current_height = m_current_height - m_ave_plat_cell_loss + m_res.m_plateau_inflow - (m_current_height * m_res.m_plateau_outflow);
    m_common_plat_height = m_common_plat_height - m_ave_plat_cell_loss + m_res.m_plateau_inflow - (m_current_height * m_res.m_plateau_outflow);
    if (m_common_plat_height > m_res.m_plateau && m_res.m_plateau >= 0) m_common_plat_height = m_res.m_plateau;
    if (m_common_plat_height < 0 && m_res.m_plateau >=0) m_common_plat_height = 0;
    if (m_current_height > m_res.m_height && m_res.m_plateau >= 0) m_current_height = m_res.m_height;
    if (m_current_height < 0 && m_res.m_plateau >= 0) m_current_height = 0;
  }
  else {
    m_current_height = m_res.m_height;
  }

  int plateau_cell = 0;
  for (int ii = min_pos_x; ii < max_pos_x + 1; ii++) {
    for (int jj = min_pos_y; jj < max_pos_y + 1; jj++) {
      double thisheight = 0.0;
      double thisdist = sqrt((double) (m_peakx - ii) * (m_peakx - ii) + (m_peaky - jj) * (m_peaky - jj));
      if (m_res.m_spread >= thisdist) {
        // determine theoretical individual cells values and add one to distance from center 
        // (so that center point = radius 1, not 0)
        // also used to distinguish plateau cells
        
        thisheight = m_current_height / (thisdist + 1);
        
        // set the floor values
        // plateaus will override this so that plateaus can hit 0 when being eaten
        if (thisheight < m_res.m_floor) thisheight = m_res.m_floor;
        
        // create cylindrical profiles of resources whereever thisheight would be >1 (area where thisdist + 1 <= m_height)
        // and slopes outside of that range
        // plateau = -1 turns off this option; if activated, causes 'peaks' to be flat plateaus = plateau value 
        bool is_plat_cell = ((m_res.m_height / (thisdist + 1)) >= 1);
        // apply plateau inflow(s) and outflow 
        if ((is_plat_cell && m_res.m_plateau >= 0) || (m_res.m_plateau < 0 && thisdist == 0 && m_plateau_array.GetSize())) { 
          if (m_just_reset || cGradientResourceAcct::m_update <= 0) {
            m_past_height = m_res.m_height;
            if (m_res.m_plateau >= 0.0) {
              thisheight = m_res.m_plateau;
            } 
            else {
              thisheight = m_res.m_height;
            }
          } 
          else { 
            if (m_res.m_is_plateau_common == 0) {
              m_past_height = m_plateau_array[plateau_cell]; 
              thisheight = m_past_height + m_res.m_plateau_inflow - (m_past_height * m_res.m_plateau_outflow);
              thisheight += m_res.m_gradient_inflow / (thisdist + 1);
              if (thisheight > m_res.m_plateau && m_res.m_plateau >= 0) {
                thisheight = m_res.m_plateau;
              } 
              if (m_res.m_plateau < 0 && thisdist == 0 && thisheight > m_res.m_height) {
                thisheight = m_res.m_height;
              }
            }
            else if (m_res.m_is_plateau_common == 1) {   
              thisheight = m_common_plat_height;
            }
          }
          if (m_initial && m_initial_plat != -1) thisheight = m_initial_plat;
          if (thisheight < 0) thisheight = 0;
          m_plateau_array[plateau_cell] = thisheight;
          m_plateau_cell_IDs[plateau_cell] = jj * m_size_x + ii;
          plateau_cell ++;
         }
        // now apply any off-plateau inflow(s) and outflow
        else if (!is_plat_cell && (m_res.m_cone_inflow > 0 || m_res.m_cone_outflow > 0 || m_res.m_gradient_inflow > 0)) {
          if (!m_just_reset && cGradientResourceAcct::m_update > 0) {
            int offsetx = m_old_peakx - m_peakx;
            int offsety = m_old_peaky - m_peaky;
            
            int old_cell_x = ii + offsetx;
            int old_cell_y = jj + offsety;
            
            // cone cells that were previously off the world and moved onto world, start at 0
            if ( old_cell_x < 0 || old_cell_y < 0 || (old_cell_y > (m_size_y - 1)) || (old_cell_x > (m_size_x - 1)) ) {
              thisheight = 0;
            }
            else {
              double past_height = m_abundance[old_cell_y * m_size_x + old_cell_x]; 
              double newheight = past_height; 
              if (m_res.m_cone_inflow > 0 || m_res.m_cone_outflow > 0) 
                newheight += m_res.m_cone_inflow - (past_height * m_res.m_cone_outflow);
              if (m_res.m_gradient_inflow > 0) 
                newheight += m_res.m_gradient_inflow / (thisdist + 1); 
              // don't exceed expected slope value
              if (newheight < thisheight) thisheight = newheight;
              if (thisheight < 0) thisheight = 0;
            }
          }
        }
      }
      m_abundance[jj * m_size_x + ii] = thisheight;
      if (thisheight > 0) UpdateBounds(ii, jj);
    }
  }         
  //SetCurrPeakX(m_peakx);  @MRR
  //SetCurrPeakY(m_peaky);
  m_just_reset = false;
}


void cGradientResourceAcct::UpdatePeakRes(cAvidaContext& ctx)
{
  bool has_edible = false; 
  
  // determine if there is any edible food left in the peak (don't refresh the peak values until decay kicks in if there is edible food left) 
  // to speed things up, we only check cells within the possible spread of the peak
  // and we only need to do this if decay > 1 (if decay == 1, we're going to reset everything regardless of the amount left)
  // if decay = 1 and the resource IS depletable, that means we have a moving depleting resource! Odd, but useful.
  if (m_res.m_decay > 1) {
    int max_pos_x = std::min(m_peakx + m_res.m_spread + 1, m_size_x - 1);
    int min_pos_x = std::max(m_peakx - m_res.m_spread - 1, 0);
    int max_pos_y = std::min(m_peaky + m_res.m_spread + 1, m_size_y - 1);
    int min_pos_y = std::max(m_peaky - m_res.m_spread - 1, 0);
    for (int ii = min_pos_x; ii < max_pos_x + 1; ii++) {
      for (int jj = min_pos_y; jj < max_pos_y + 1; jj++) {
        if (m_abundance[jj * m_size_x + ii] >= 1) {
          has_edible = true;
          break;
        }
      }
    }
  }
  
  // once a resource cone has been 'bitten', start the clock that counts down to when the entire peak will be
  // refreshed (carcass rots for only so long before disappearing)
  if (has_edible && m_modified && m_res.m_decay > 1) m_counter++;

  if (has_edible && m_counter < m_res.m_decay && m_modified) {
    if (m_predator) UpdatePredatoryRes<cPopulation*>(ctx, m_pop);
    if (m_damage) UpdateDamagingRes<cPopulation*>(ctx, m_pop);
    if (m_deadly) UpdateDeadlyRes<cPopulation*>(ctx, m_pop);
    return;
  } 
                   
  // only update resource values at declared update timesteps if there is resource left in the cone

  // before we move anything, if we have a depletable resource, we need to get the current plateau cell values
  if (m_res.m_decay == 1) GetCurrentPlatValues();

  // When the counter matches decay, regenerate resource peak
  if (m_counter == m_res.m_decay) GeneratePeak(ctx);
  
  // if we are working with moving peaks, calculate the y-scaler
  if (m_res.m_move_a_scaler > 1) 
    m_move_y_scaler = m_res.m_move_a_scaler * m_move_y_scaler * (1 - m_move_y_scaler);   

  // if working with moving resources, check if we are moving once per update or less frequently
  if (m_skip_counter == m_skip_moves) MoveRes(ctx);
  else m_skip_counter++;
  
  // to speed things up, we only check cells within the possible spread of the peak
  // and we only do this if the resource is set to actually move, has inflow/outflow to update, or
  // we just reset a non-moving resource
  if (m_res.m_move_a_scaler > 1 ||m_res. m_plateau_inflow != 0 || 
      m_res.m_plateau_outflow != 0 || m_res.m_cone_inflow != 0 || m_res.m_cone_outflow != 0 ||
      m_res.m_gradient_inflow != 0 || (m_res.m_move_a_scaler == 1 && m_just_reset)) 
  FillInResourceValues();

  if (m_predator) UpdatePredatoryRes<cPopulation*>(ctx, m_pop);
  if (m_damage) UpdateDamagingRes<cPopulation*>(ctx, m_pop);
  if (m_deadly) UpdateDeadlyRes<cPopulation*>(ctx, m_pop);
}


void cGradientResourceAcct::MoveRes(cAvidaContext& ctx)
{
  // for halo peaks, find current orbit. Add 1 to distance to account for the anchor grid cell
  int current_orbit = 
    std::max(std::abs(m_res.m_halo_anchor_x - m_peakx), std::abs(m_res.m_halo_anchor_y - m_peaky));

  // if we are working with moving resources and it's time to update direction
  if (m_move_counter == m_res.m_updatestep && m_res.m_move_a_scaler > 1) {
    m_move_counter = 1;
    if (m_res.m_halo == 1) 
      current_orbit = SetHaloOrbit(ctx, current_orbit);
    else 
      SetPeakMoveMovement(ctx);
  }    
  else m_move_counter++;
  
  if (m_res.m_move_a_scaler > 1) {
    if (m_res.m_halo == 1 && m_res.m_move_a_scaler > 1) 
      MoveHaloPeak(current_orbit);
    else 
      MovePeak();
  }
  m_skip_counter = 0;
}

void cGradientResourceAcct::SetPeakMoveMovement(cAvidaContext& ctx)  
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


int cGradientResourceAcct::SetHaloOrbit(cAvidaContext& ctx, int current_orbit)
{
  // move cones by moving m_peakx & m_peaky 
  // halo resources orbit at a fixed org walking distance from an anchor point
  // if halo width > the height of the halo resource, the resource will be bounded inside the halo but the orbit can vary within those bounds
  // halo's are actually square in avida because, at a given orbit, this keeps a constant distance (in number of steps and org would have to take)
  //    between the anchor point and any orbit

  //choose to change orbit (0) or direction (1)
  int random_shift = ctx.GetRandom().GetUInt(0,2);
  // if changing orbit, choose to go in or out one orbit
  // then figure out if we need change the x or the y to shift orbit (based on what quadrant we're in)
  int temp_height = 0;
  if (m_res.m_plateau < 0) temp_height = 1;
  else temp_height = m_res.m_height;

  if (random_shift == 0) {
    //do nothing unless there's room to change orbit
    if (m_res.m_halo_width > (temp_height * 2)) {
      int orbit_shift = 1;
      if (!ctx.GetRandom().GetUInt(0,2)) orbit_shift = -1;
      current_orbit += orbit_shift;
      // we have to check that we are still going to be within the halo after an orbit change
      // if we go out of bounds, we need to go the other way instead, taking two steps back on current_orbit
      if (current_orbit > (m_res.m_halo_inner_radius + m_res.m_halo_width - temp_height - 1) ||
          current_orbit < (m_res.m_halo_inner_radius + temp_height + 1) ||
          m_res.m_halo_anchor_y + current_orbit - 1 >= m_size_y ||
          m_res.m_halo_anchor_x + current_orbit - 1 >= m_size_x ||
          m_res.m_halo_anchor_y - current_orbit + 1 < 0 ||
         m_res. m_halo_anchor_x - current_orbit + 1 < 0) {
        orbit_shift *= -1;
        current_orbit += 2 * orbit_shift;
      }
      
      if (current_orbit < 0) current_orbit = std::abs(current_orbit); // went passed anchor origin to the other side

      if (std::abs(m_res.m_halo_anchor_y - m_peaky) > std::abs(m_res.m_halo_anchor_x - m_peakx)) {
        m_peaky = (m_res.m_halo_anchor_y > m_peaky) ? m_old_peaky - orbit_shift : m_old_peaky + orbit_shift;
      }
      else 
        m_peakx = (m_res.m_halo_anchor_x > m_peakx) ? m_old_peakx - orbit_shift : m_old_peakx + orbit_shift;
    }
    // if there was no room to change orbit, change the direction instead of the orbit
    else random_shift = 1;
  }
  // if changing direction of rotation, we just switch sign of rotation
  if (random_shift == 1) SetHaloDirection(ctx);
  return current_orbit;
}


void cGradientResourceAcct::MoveHaloPeak(int current_orbit)
{
  // what quadrant we are in determines whether we are changing x's or y's (= changling)
  // if we are on a corner, we just stick with the current changling
  if (std::abs(m_res.m_halo_anchor_y - m_peaky) > std::abs(m_res.m_halo_anchor_x - m_peakx)) 
    m_changling = 1;
  else if (std::abs(m_res.m_halo_anchor_y - m_peaky) < std::abs(m_res.m_halo_anchor_x - m_peakx)) 
    m_changling = -1;
  
  if (m_changling == 1) {
    // check to make sure the move will not put peak beyond the bounds (at corner) of the orbit
    // if it will go beyond the bounds of the orbit, turn the corner (e.g. if move = 5 & space to move on x =2, move 2 on x and 3 on y)
    int max_orbit_x = m_res.m_halo_anchor_x + current_orbit - 1;
    int min_orbit_x = m_res.m_halo_anchor_x - current_orbit + 1;
    int next_posx = m_peakx + (m_halo_dir * m_res.m_move_speed);
    int current_x = m_peakx;
    if (next_posx > max_orbit_x) {
      // turning this corner means changing the sign of the movement once we switch from moving along x to moving along y
      m_peakx = max_orbit_x;
      if (m_peaky > m_res.m_halo_anchor_y) m_halo_dir *= -1;
      m_peaky = m_peaky + m_halo_dir * (m_res.m_move_speed - abs(m_peakx - current_x));
      m_changling *= -1;
    }
    else if (next_posx < min_orbit_x) {
      m_peakx = min_orbit_x;
      if (!(m_peaky > m_res.m_halo_anchor_y)) m_halo_dir *= -1;
      m_peaky = m_peaky + m_halo_dir * (m_res.m_move_speed - abs(m_peakx - current_x));
      m_changling *= -1;
    }
    else m_peakx = m_peakx + (m_halo_dir * m_res.m_move_speed);
  }
  else {
    int max_orbit_y = m_res.m_halo_anchor_y + current_orbit - 1;
    int min_orbit_y = m_res.m_halo_anchor_y - current_orbit + 1;
    int next_posy = m_peaky + (m_halo_dir * m_res.m_move_speed);
    int current_y = m_peaky;
    if (next_posy > max_orbit_y) {
      m_peaky = max_orbit_y;
      if (!(m_peakx < m_res.m_halo_anchor_x)) m_halo_dir *= -1;
      m_peakx = m_peakx + m_halo_dir * (m_res.m_move_speed - abs(m_peaky - current_y));
      m_changling *= -1;
    }
    else if (next_posy < min_orbit_y) {
      m_peaky = min_orbit_y;
      if (m_peakx < m_res.m_halo_anchor_x) m_halo_dir *= -1;
      m_peakx = m_peakx + m_halo_dir * (m_res.m_move_speed - abs(m_peaky - current_y));
      m_changling *= -1;
    }
    else {
      m_peaky = m_peaky + (m_halo_dir * m_res.m_move_speed);
    }
  }
  ConfirmHaloPeak();
}


void cGradientResourceAcct::ConfirmHaloPeak()
{
 // this function corrects for situations where a change in orbit and direction and changling at the same time caused the halo to jump out of it's orbital bounds
  if (m_changling == 1) {
    int l_y_min = m_res.m_halo_anchor_y - m_res.m_halo_inner_radius - m_res.m_halo_width + m_res.m_height - 1;
    int l_y_max = m_res.m_halo_anchor_y - m_res.m_halo_inner_radius - m_res.m_height + 1;
    int r_y_min = m_res.m_halo_anchor_y + m_res.m_halo_inner_radius + m_res.m_height - 1;
    int r_y_max = m_res.m_halo_anchor_y + m_res.m_halo_inner_radius + m_res.m_halo_width - m_res.m_height + 1;
    // top
    if (m_peaky < m_res.m_halo_anchor_y) {
      if (m_peaky < l_y_min) m_peaky = l_y_min;
      else if (m_peaky > l_y_max) m_peaky = l_y_max;
    } // bottom
    else if (m_peaky > m_res.m_halo_anchor_y) {
      if (m_peaky < r_y_min) m_peaky = r_y_min;
      else if (m_peaky > r_y_max) m_peaky = r_y_max;
    }
  }
  else {
    int l_x_min = m_res.m_halo_anchor_x - m_res.m_halo_inner_radius - m_res.m_halo_width + m_res.m_height - 1;
    int l_x_max = m_res.m_halo_anchor_x - m_res.m_halo_inner_radius - m_res.m_height + 1;
    int r_x_min = m_res.m_halo_anchor_x + m_res.m_halo_inner_radius + m_res.m_height - 1;
    int r_x_max = m_res.m_halo_anchor_x + m_res.m_halo_inner_radius + m_res.m_halo_width - m_res.m_height + 1;
    // left
    if (m_peakx < m_res.m_halo_anchor_x) {
      if (m_peakx < l_x_min) m_peakx = l_x_min;
      else if (m_peakx > l_x_max) m_peakx = l_x_max;
    } // right
    else if (m_peakx > m_res.m_halo_anchor_x) {
      if (m_peakx < r_x_min) m_peakx = r_x_min;
      else if (m_peakx > r_x_max) m_peakx = r_x_max;
    }
  }
  assert(m_peakx >= 0 && m_peaky >= 0 && m_peakx < m_size_x && m_peaky < m_size_y);
  assert(std::max(std::abs(m_res.m_halo_anchor_x - m_peakx), std::abs(m_res.m_halo_anchor_y - m_peaky)) > m_res.m_halo_inner_radius);
  assert(std::max(std::abs(m_res.m_halo_anchor_x - m_peakx), std::abs(m_res.m_halo_anchor_y - m_peaky)) < m_res.m_halo_inner_radius + m_res.m_halo_width);
}


void cGradientResourceAcct::MovePeak()
{
  // for non-halo peaks keep cones inside their bounding boxes, bouncing them if they hit the edge
  int temp_height = 0;
  if (m_res.m_plateau < 0) temp_height = 1;
  else temp_height = m_res.m_height;
  
  int num_steps = m_res.m_move_speed > 1 ? m_res.m_move_speed : 1;
  for (int i = 0; i < num_steps; i++) {
    int temp_peakx = m_peakx + (int)(m_move_y_scaler + 0.5) * m_movesignx;
    int temp_peaky = m_peaky + (int)(m_move_y_scaler + 0.5) * m_movesigny;
    
    if ((temp_height * 2) < std::abs(m_res.m_max_x - m_res.m_min_x)) {
      if (temp_peakx > (m_res.m_max_x - temp_height)) m_movesignx = -1;
      if (temp_peakx < (m_res.m_min_x + temp_height + 1)) m_movesignx = 1;
    }
    else {
      m_movesignx = 0;
      temp_peakx = m_peakx;
    }
    
    if ((temp_height * 2) < std::abs(m_res.m_max_y - m_res.m_min_y)) {
      if (temp_peaky > (m_res.m_max_y - temp_height)) m_movesigny = -1;
      if (temp_peaky < (m_res.m_min_y + temp_height + 1)) m_movesigny = 1;
    }
    else {
      m_movesigny = 0;
      temp_peaky = m_peaky;
    }
    if ((temp_height * 2) < std::abs(m_res.m_max_x - m_res.m_min_x)) m_peakx = (int) (m_peakx + (m_movesignx * m_move_y_scaler) + .5);
    if ((temp_height * 2) < std::abs(m_res.m_max_y - m_res.m_min_y)) m_peaky = (int) (m_peaky + (m_movesigny * m_move_y_scaler) + .5);
  }
}


void cGradientResourceAcct::GeneratePeak(cAvidaContext& ctx)
{
  // Get initial peak cell x, y coordinates and movement directions.
  Apto::Random& rng = ctx.GetRandom();
  int temp_height = 0;
  if (m_res.m_plateau < 0) temp_height = 1;
  else temp_height = m_res.m_height;
  // If no initial config set m_peakx and m_peaky, get a random location
  if (!m_res.m_halo) {
    if (m_peakx == -1) m_peakx = rng.GetUInt(m_res.m_min_x + temp_height, m_res.m_max_x - temp_height + 1);
    if (m_peaky == -1) m_peaky = rng.GetUInt(m_res.m_min_y + temp_height, m_res.m_max_y - temp_height + 1);

    if (m_res.m_move_a_scaler > 1) {
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
  else if (m_res.m_halo) {
    if (m_res.m_move_a_scaler > 1) {
      m_halo_dir = (rng.GetUInt(0,2) == 0) ? -1 : 1;
      SetHaloDirection(ctx);
      
      m_changling = (rng.GetUInt(0,2) == 1) ? -1 : 1;
    }
    const int chooseUpDown = rng.GetUInt(0,2);
    if (chooseUpDown == 0) {
      int chooseEW = rng.GetUInt(0,2);
      if (chooseEW == 0) {
        m_peakx = rng.GetUInt(
          std::max(0, m_res.m_halo_anchor_x - m_res.m_halo_inner_radius - m_res.m_halo_width + temp_height + 1),
                              m_res.m_halo_anchor_x - m_res.m_halo_inner_radius - temp_height);
      } else {
        m_peakx = rng.GetUInt(std::max(0, m_res.m_halo_anchor_x + m_res.m_halo_inner_radius + temp_height + 1),
                              m_res.m_halo_anchor_x + m_res.m_halo_inner_radius + m_res.m_halo_width - temp_height);
      }
      m_peaky = rng.GetUInt(
                  std::max(0,m_res.m_halo_anchor_y - m_res.m_halo_inner_radius - m_res.m_halo_width + temp_height + 1),
                            m_res.m_halo_anchor_y + m_res.m_halo_inner_radius + m_res.m_halo_width - temp_height);
    }
    else {
      int chooseNS = rng.GetUInt(0,2);
      if (chooseNS == 0) {
        m_peaky = rng.GetUInt(
                  std::max(0, m_res.m_halo_anchor_y - m_res.m_halo_inner_radius - m_res.m_halo_width + temp_height + 1),
                              m_res.m_halo_anchor_y - m_res.m_halo_inner_radius - temp_height);
      } else {
        m_peaky = rng.GetUInt(
                  std::max(0, m_res.m_halo_anchor_y + m_res.m_halo_inner_radius + temp_height + 1),
                              m_res.m_halo_anchor_y + m_res.m_halo_inner_radius + m_res.m_halo_width - temp_height);
      }
      m_peakx = rng.GetUInt(
                  std::max(0, m_res.m_halo_anchor_x - m_res.m_halo_inner_radius - m_res.m_halo_width + temp_height + 1),
                            m_res.m_halo_anchor_x + m_res.m_halo_inner_radius + m_res.m_halo_width - temp_height);
    }
  }
  assert(m_peakx >= 0 && m_peaky >= 0 && m_peakx < m_size_x && m_peaky < m_size_y);

  m_modified = false;
  m_counter = 0;
  m_skip_counter = 0;
  m_just_reset = true;
  FillInResourceValues();
}


void cGradientResourceAcct::GetCurrentPlatValues()
{
  int temp_height = 0;
  if (m_res.m_plateau < 0) temp_height = 1;
  else temp_height = m_res.m_height;
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
      if ((find_plat_dist >= 1 && m_res.m_plateau >= 0) || (m_res.m_plateau < 0 && thisdist == 0 && m_plateau_array.GetSize() > 0)) {
        double past_cell_height = m_plateau_array[plateau_cell];
        double pre_move_height = m_abundance[m_plateau_cell_IDs[plateau_cell]];  
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


void cGradientResourceAcct::GenerateBarrier(cAvidaContext& ctx)
// If habitat == 2 we are creating barriers to movement (walls)
{ 
  // generate/regenerate walls when counter == config updatestep
  if (m_topo_counter == m_res.m_updatestep) { 
    ResetUsedBounds();
    // reset counter
    m_topo_counter = 1;
    // clear any old resource
    if (m_wall_cells.GetSize()) {
      for (int i = 0; i < m_wall_cells.GetSize(); i++) {
        m_abundance[m_wall_cells[i]] = 0.0;
      }
    }
    else {
      for (int ii = 0; ii < m_size_x; ii++) {
        for (int jj = 0; jj < m_size_y; jj++) {
          m_abundance[jj * m_size_x + ii] = 0.0;
        }
      }
    }
    m_wall_cells.Resize(0);
    // generate number barriers equal to count 
    for (int i = 0; i < m_res.m_count; i++) {
      // drop the anchor/first block for current barrier
      int start_randx = 0;
      int start_randy = 0;   
      if (m_res.m_config == 3 || m_res.m_config == 4) { 
        start_randx = m_peakx;
        start_randy = m_peaky;
      }
      else {
        start_randx = ctx.GetRandom().GetUInt(0, m_size_x);
        start_randy = ctx.GetRandom().GetUInt(0, m_size_y);  
      }
      m_abundance[start_randy * m_size_x + start_randx] = m_res.m_plateau;
      // if (m_plateau > 0) updateBounds(start_randx, start_randy);
      UpdateBounds(start_randx, start_randy);
      m_wall_cells.Push(start_randy * m_size_x + start_randx);

      int randx = start_randx;
      int randy = start_randy;
      int prev_blockx = randx;
      int prev_blocky = randy;
      int cornerx = prev_blockx;
      int cornery = prev_blocky;
      bool place_corner = false;

      // decide the size of the current barrier
      int rand_block_count = ctx.GetRandom().GetUInt(m_res.m_min_size, m_res.m_max_size + 1);
      // for vertical or horizontal wall building, pick a random direction once for the whole wall
      int direction = ctx.GetRandom().GetUInt(0,2);
      
      for (int num_blocks = 0; num_blocks < rand_block_count; num_blocks++) {
        // if config == 0, build random shaped walls
        if (m_res.m_config == 0) {
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
        else if (m_res.m_config == 1) {
          // choose up/down build direction
          if (direction == 0) randy = randy - 1;
          else randy = randy + 1;
        }
        // if config == 2, build randonly placed horizontal walls
        else if (m_res.m_config == 2) {
          // choose left/right build direction
          if (direction == 0) randx = randx - 1;
          else randx = randx + 1;
        }       
        // if config == 3, build vertical walls from north to south
        else if (m_res.m_config == 3) randy = randy + 1;
        // if config == 4, build horizontal walls from west to east
        else if (m_res.m_config == 4) randx = randx + 1;
        
        bool count_block = true;
        // place the new block(s) if not off edge of world
        if (randy < m_size_y && randy >= 0 && randx < m_size_x && randx >= 0) {
          // if we are trying to build across an inner_radius 
          // or for random walls, if there is already a block here
          // don't count or place this one (continue walking across inner_radius)
          if ((randx < (m_res.m_halo_anchor_x + m_res.m_halo_inner_radius) && 
               randy < (m_res.m_halo_anchor_y + m_res.m_halo_inner_radius) && 
               randx > (m_res.m_halo_anchor_x - m_res.m_halo_inner_radius) && 
               randy > (m_res.m_halo_anchor_y - m_res.m_halo_inner_radius)) || 
              (m_res.m_config == 0 && m_abundance[randy * m_size_x + randx])) {
            num_blocks --;
            count_block = false;
          }
          if (count_block) {
            m_abundance[randy * m_size_x + randx] = m_res.m_plateau;
            if (m_res.m_plateau > 0) UpdateBounds(randx, randy);
            m_wall_cells.Push(randy * m_size_x + randx);
            if (place_corner) {
              if (cornery < m_size_y && cornery >= 0 && cornerx < m_size_x && cornerx >= 0) {
                if ( ! ((cornerx < (m_res.m_halo_anchor_x + m_res.m_halo_inner_radius) && 
                     cornery < (m_res.m_halo_anchor_y + m_res.m_halo_inner_radius) && 
                     cornerx > (m_res.m_halo_anchor_x - m_res.m_halo_inner_radius) && 
                     cornery > (m_res.m_halo_anchor_y - m_res.m_halo_inner_radius))) ){
                  m_abundance[cornery * m_size_x + cornerx] = m_res.m_plateau;
                  if (m_res.m_plateau > 0) UpdateBounds(cornerx, cornery);
                  m_wall_cells.Push(randy * m_size_x + randx);
                }
              }
            }
          }
        }
        // if the wall is horizontal or vertical build and we went off the world edge, build from the opposite direction
        else if (m_res.m_config == 1 || m_res.m_config == 2) {
          randx = start_randx; 
          randy = start_randy; 
          direction = std::abs(direction - 1);
          num_blocks --;
        }
        // if a random build and we went off the world edge, backup a block and try again
        else if (m_res.m_config == 0) {
          randx = prev_blockx;
          randy = prev_blocky;
          num_blocks --;
        }
      }  
    }
  }
  else m_topo_counter++; 
}


void cGradientResourceAcct::GenerateHills(cAvidaContext& ctx)
// If habitat == 1 we are creating hills which slow movement, not really gradient resources
{ 
  // generate/regenerate hills when counter == config updatestep
  if (m_topo_counter == m_res.m_updatestep) { 
    ResetUsedBounds();
    // reset counter
    m_topo_counter = 1;
    // since we are potentially plotting more than one hill per resource, we need to wipe the world before we start
    if (m_min_usedx == -1 || m_min_usedy == -1 || m_max_usedx == -1 || m_max_usedy == -1) {
      for (int ii = 0; ii < m_size_x; ii++) {
        for (int jj = 0; jj < m_size_y; jj++) {
          m_abundance[jj * m_size_x + ii] = 0.0;
        }
      }
    }
    else {
      for (int ii = m_min_usedx; ii < m_max_usedx + 1; ii++) {
        for (int jj = m_min_usedy; jj < m_max_usedy + 1; jj++) {
          m_abundance[jj * m_size_x + ii] = 0.0;
        }
      }
    }

    Apto::Random& rng = ctx.GetRandom();
    // generate number hills equal to count
    for (int i = 0; i < m_res.m_count; i++) {
      // decide the size of the current hill
      int rand_hill_radius = ctx.GetRandom().GetUInt(m_res.m_min_size, m_res.m_max_size + 1);
      
      // generate random hills, if config == 0, otherwise generate 1 hill at peakx X peaky
      if (m_res.m_config == 0) {
        // choose the peak center for current hill, keeping the entire hill outside of any inner_radius
        int chooseEW = rng.GetUInt(0,2);
        if (chooseEW == 0) {
          m_peakx = rng.GetUInt(rand_hill_radius, m_res.m_halo_anchor_x - m_res.m_halo_inner_radius - rand_hill_radius);
        } else {
          m_peakx = rng.GetUInt(m_res.m_halo_anchor_x + m_res.m_halo_inner_radius + rand_hill_radius, m_size_x - 1 - rand_hill_radius);
        }
        int chooseNS = rng.GetUInt(0,2);
        if (chooseNS == 0) { 
          m_peaky = rng.GetUInt(rand_hill_radius, m_res.m_halo_anchor_y - m_res.m_halo_inner_radius - rand_hill_radius);
        } else {
          m_peaky = rng.GetUInt(m_res.m_halo_anchor_y + m_res.m_halo_inner_radius + rand_hill_radius, m_size_y - 1 - rand_hill_radius);
        }
      }
      
      // figure the coordinate extent of each hill (box)
      int max_pos_x = std::min(m_peakx + rand_hill_radius + 1, m_size_x - 1);
      int min_pos_x = std::max(m_peakx - rand_hill_radius - 1, 0);
      int max_pos_y = std::min(m_peaky + rand_hill_radius + 1, m_size_y - 1);
      int min_pos_y = std::max(m_peaky - rand_hill_radius - 1, 0);

      // look to place new cell values within a box around the hill center
      for (int ii = min_pos_x; ii < max_pos_x + 1; ii++) {
        for (int jj = min_pos_y; jj < max_pos_y + 1; jj++) {
          double thisheight = 0.0;
          double thisdist = sqrt((double) (m_peakx - ii) * (m_peakx - ii) + (m_peaky - jj) * (m_peaky - jj));
          // only plot values when within set config radius & if no larger amount has already been plotted for another overlapping hill
          if ((thisdist <= rand_hill_radius) && 
              (m_abundance[jj * m_size_x + ii] <  m_res.m_plateau / (thisdist + 1))) {
          thisheight = m_res.m_plateau / (thisdist + 1);
          m_abundance[jj * m_size_x + ii] = thisheight;
          if (thisheight > 0) UpdateBounds(ii, jj);
          }
        }
      }
    }
  }
  else m_topo_counter++; 
}


void cGradientResourceAcct::UpdateBounds(int x, int y)
{
  if (x < m_min_usedx || m_min_usedx == -1) m_min_usedx = x;
  if (y < m_min_usedy || m_min_usedy == -1) m_min_usedy = y;
  if (x > m_max_usedx || m_max_usedx == -1) m_max_usedx = x;
  if (y > m_max_usedy || m_max_usedy == -1) m_max_usedy = y;
}


void cGradientResourceAcct::ResetUsedBounds()
{
  m_min_usedx = -1;
  m_min_usedy = -1;
  m_max_usedx = -1;
  m_max_usedy = -1;
}


void cGradientResourceAcct::ClearExistingProbResource()
{
  for (int x = m_min_usedx; x < m_max_usedx + 1; x ++) {
    for (int y = m_min_usedy; y < m_max_usedy + 1; y ++) {
      m_abundance[y * m_size_x + x] = 0.0;
    }
  }
}


inline void cGradientResourceAcct::SetHaloDirection(cAvidaContext& ctx)
{
  int move_rand = 0;
  // Move resistance adds a bias for remaining in place and makes directional adjustment random
  if (m_res.m_move_resistance > 0) move_rand = ctx.GetRandom().GetUInt(2 + m_res.m_move_resistance);
  else move_rand = ctx.GetRandom().GetUInt(3);
  
  switch (move_rand) {
    case 0: m_halo_dir = -1; break;
    case 1: m_halo_dir = 1; break;
    default: m_halo_dir = 0; break;
  }
}


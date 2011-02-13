#include "cGradientCount.h"
#include "cWorld.h"
#include "cWorldDriver.h"
#include "cStats.h"
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
  Peak values are refreshed to match initial height, spread, and plateau, but the placement of the refreshed peak is random
within the min/max x and y area. */
/*cGradientCount cannot access the random number generator at the very first update. Thus, it uses the DefaultContext initially*/

cGradientCount::cGradientCount(cWorld* world, int in_peakx, int in_peaky, double in_height, double in_spread, double in_plateau, int in_decay, 
                               int in_max_x, int in_max_y, int in_min_x, int in_min_y, double in_move_a_scaler, int in_updatestep,  
                               int in_worldx, int in_worldy, int in_geometry, int in_halo, int in_halo_inner_radius, int in_halo_width,
                               int in_halo_anchor_x, int in_halo_anchor_y, int in_move_speed) : 
                               m_world(world), m_peakx(in_peakx), m_peaky(in_peaky), m_height(in_height), m_spread(in_spread), m_plateau(in_plateau), 
                               m_decay(in_decay), m_max_x(in_max_x), m_max_y(in_max_y), m_min_x(in_min_x), m_min_y(in_min_y), 
                               m_move_a_scaler(in_move_a_scaler), m_updatestep(in_updatestep), 
                               m_halo(in_halo), m_halo_inner_radius(in_halo_inner_radius), m_halo_width(in_halo_width),
                               m_halo_anchor_x(in_halo_anchor_x), m_halo_anchor_y(in_halo_anchor_y), m_move_speed(in_move_speed)
{
  if(m_move_speed >= 2*(m_halo_inner_radius+m_halo_width) && m_halo_inner_radius+m_halo_width != 0
    && m_move_speed != 0){
    m_world->GetDriver().RaiseFatalException(-1, "Move speed greater or equal to 2*Radius");
    assert(false);
  }				

  ResizeClear(in_worldx, in_worldy, in_geometry);
  m_counter = 0;
  move_counter = 1;
  moveYscaler = 0.5;
  movesignx = m_world->GetDefaultContext().GetRandom().GetInt(-1,2);
  if (movesignx == 0) {
    if (m_world->GetDefaultContext().GetRandom().GetUInt(0,2)) {
      movesigny = 1;
    }
    else movesigny = -1;
  }
  else movesigny = m_world->GetDefaultContext().GetRandom().GetInt(-1,2);
  UpdateCount(&m_world->GetDefaultContext());
  if (m_halo == 1) {
    m_peakx = m_halo_anchor_x + m_halo_inner_radius + m_halo_width/2 + 1;
    m_peaky = m_halo_anchor_y + m_halo_inner_radius + m_halo_width/2 + 1;
    if (m_world->GetDefaultContext().GetRandom().GetUInt(0,2) == 1) halo_dir = -1;
    else halo_dir = 1;  
    if (m_world->GetDefaultContext().GetRandom().GetUInt(0,2) == 1) changling = -1;
    else changling = 1;
  }
  else {
    m_peakx = in_peakx;
    m_peaky = in_peaky;
  }
  UpdateCount(&m_world->GetDefaultContext());
}

double cGradientCount::Distance(double x1, double y1, double x2, double y2) //calculate linear distance from cone peak
{
  return(sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
}

void cGradientCount::UpdateCount(cAvidaContext* ctx)
{  
  if(ctx == NULL) ctx = &(m_world->GetDefaultContext());
  bool has_edible = false;
  // determine if there is any edible food left in the peak (don't refresh the peak values until decay kicks in if there is edible food left)
  for (int ii = 0; ii < GetX() && !has_edible; ii++) {
    for (int jj = 0; jj < GetY(); jj++) {
      if (Element(jj * GetX() + ii).GetAmount() >= 1) {
        has_edible = true;
        break;
      }
    }
  }

  // once a resource cone has been 'bitten', start the clock that counts down to when the entire peak will be
  // refreshed (carcass rots for only so long before disappearing)
  if (has_edible && GetModified()) m_counter++;
  
  // only update resource values at declared update timesteps if there is resource left in the cone
  if (has_edible && m_counter < m_decay && GetModified()) return; 
  
  if (m_counter == m_decay) {
    if (m_halo != 1) {
      m_peakx = ctx->GetRandom().GetUInt(m_min_x + m_height, m_max_x - m_height + 1);                 
      m_peaky = ctx->GetRandom().GetUInt(m_min_y + m_height, m_max_y - m_height + 1);
      movesignx = ctx->GetRandom().GetInt(-1,2);  
      if (movesignx == 0) {
        if (ctx->GetRandom().GetUInt(0,2)) {
          movesigny = 1;
        }
        else movesigny = -1;
      }
      else movesigny = ctx->GetRandom().GetInt(-1,2);
    }
    else if (m_halo == 1) {
      int chooseEW = ctx->GetRandom().GetUInt(0,2);
        if (chooseEW == 0)
          m_peakx = ctx->GetRandom().GetUInt(m_halo_anchor_x - m_halo_inner_radius, m_halo_anchor_x - m_halo_inner_radius - m_halo_width + 1);
        else m_peakx = ctx->GetRandom().GetUInt(m_halo_anchor_x + m_halo_inner_radius, m_halo_anchor_x + m_halo_inner_radius + m_halo_width - 1); 
      int chooseNS = ctx->GetRandom().GetUInt(0,2);
        if (chooseNS == 0) 
          m_peaky = ctx->GetRandom().GetUInt(m_halo_anchor_y - m_halo_inner_radius, m_halo_anchor_y - m_halo_inner_radius - m_halo_width + 1);
        else m_peaky = ctx->GetRandom().GetUInt(m_halo_anchor_y + m_halo_inner_radius, m_halo_anchor_y + m_halo_inner_radius + m_halo_width - 1);
    }
    SetModified(false);
  }
  
  // move cones by moving m_peakx & m_peaky, but only if the cone has not been bitten 
  // keep cones inside their bounding boxes; bounce them if they hit the edge
  moveYscaler = m_move_a_scaler * moveYscaler * (1 - moveYscaler); 
  
  //we use movesign to determine direction of peak movement
  //first, to get smooth movements, we only allow either the x or y direction change to be evaluated in a single update
  //second, we then decide the change of direction based on the current direction so that peak can't 'jump' from -1 to 1, 
  //without first changing to 0
  //finally, we only do this only when # updates since last change = updatestep to slow the frequency of path changes
  int choosesign = ctx->GetRandom().GetInt(0,3);

  //we add 1 to distance to account for the anchor grid cell
  int current_orbit = max(abs(m_halo_anchor_x - m_peakx), abs(m_halo_anchor_y - m_peaky)) + 1;
  
  if (move_counter == m_updatestep) {
    move_counter = 1;
    //halo resources orbit at a fixed org walking distance from an anchor point
    //if halo width > the height of the halo resource, the resource will be bounded inside the halo but the orbit can vary within those bounds
    //halo's are actually square in avida because, at a given orbit, this keeps a constant distance (in number of steps and org would have to take)
    //    between the anchor point and any orbit
    if (m_halo == 1) {    //choose to change orbit (0) or direction (1)    
      int old_peakx = m_peakx;
      int old_peaky = m_peaky;
      int random_shift = ctx->GetRandom().GetUInt(0,2);
      //if changing orbit, choose to go in or out one orbit
      //then figure out if we need change the x or the y to shift orbit (based on what quadrant we're in)
      if (random_shift == 0) {
        //do nothing unless there's room to change orbit
        if (m_halo_width > (m_height * 2)) {
          orbit_shift = ctx->GetRandom().GetUInt(0,2); 
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
        halo_dir = halo_dir * -1; 
      }
    }
    //for non-halo peaks
    else {
      if (choosesign == 1) {
        if (movesignx == -1) movesignx = ctx->GetRandom().GetInt(-1,1); 
        else if (movesignx == 1) movesignx = ctx->GetRandom().GetInt(0,2);
        else movesignx = ctx->GetRandom().GetInt(-1,2);
      }
      
      if (choosesign == 2){ 
        if (movesigny == -1) movesigny = ctx->GetRandom().GetInt(-1,1); 
        else if (movesigny == 1) movesigny = ctx->GetRandom().GetInt(0,2);
        else movesigny = ctx->GetRandom().GetInt(-1,2);
      }
    }
  }
  else move_counter = move_counter++;
  
  //for non-halo peaks
  double temp_peakx = m_peakx + (moveYscaler * movesignx);
  double temp_peaky = m_peaky + (moveYscaler * movesigny);
  
  if (m_halo == 1) { 
    //what quadrant we are in determines whether we are changing x's or y's (= changling)
    if (abs(m_halo_anchor_y - m_peaky) > abs(m_halo_anchor_x - m_peakx))
      changling = 1;
    else 
      changling = -1;
    
    if (changling == 1) {
      //check to make sure the move will not put peak beyond the bounds (at corner) of the orbit
      //if it will go beyond the bounds of the orbit, turn the corner (e.g. if move = 5 & space to move on x =2, move 2 on x and 3 on y)
      int next_posx = m_peakx + (halo_dir * m_move_speed);
      int max_orbit_x = m_halo_anchor_x + current_orbit - 1;
      int min_orbit_x = m_halo_anchor_x - current_orbit + 1;
      int current_x = m_peakx;
      if (next_posx > max_orbit_x) {
        m_peakx = max_orbit_x;
        if (m_peaky > m_halo_anchor_y) {
          //turning this corner means changing the sign of the movement once we switch from moving along x to moving along y
          halo_dir = halo_dir * -1;
          m_peaky = m_peaky + halo_dir * (m_move_speed - abs(m_peakx - current_x)); 
        }
        else
          m_peaky = m_peaky + halo_dir * (m_move_speed - abs(m_peakx - current_x));
        changling = changling * -1;
      }
      else if (next_posx < min_orbit_x) { 
        m_peakx = min_orbit_x;          
        if (m_peaky > m_halo_anchor_y) 
          m_peaky = m_peaky + halo_dir * (m_move_speed - abs(m_peakx - current_x));
        else {
          halo_dir = halo_dir * -1;
          m_peaky = m_peaky + halo_dir * (m_move_speed - abs(m_peakx - current_x));
        }
        changling = changling * -1;          
      }
      else {
        m_peakx = m_peakx + (halo_dir * m_move_speed);     
      }
    }
    else {
      int next_posy = m_peaky + (halo_dir * m_move_speed);
      int max_orbit_y = m_halo_anchor_y + current_orbit - 1;
      int min_orbit_y = m_halo_anchor_y - current_orbit + 1;
      int current_y = m_peaky;
      if (next_posy > max_orbit_y) {
        m_peaky = max_orbit_y;
        if (m_peakx < m_halo_anchor_x) 
          m_peakx = m_peakx + halo_dir * (m_move_speed - abs(m_peaky - current_y));
        else {
          halo_dir = halo_dir * -1;
          m_peakx = m_peakx + halo_dir * (m_move_speed - abs(m_peaky - current_y));
        }
        changling = changling * -1; 
      }
      else if (next_posy < min_orbit_y) { 
        m_peaky = min_orbit_y;          
        if (m_peakx < m_halo_anchor_x) { 
          halo_dir = halo_dir * -1;
          m_peakx = m_peakx + halo_dir * (m_move_speed - abs(m_peaky - current_y));
        }
        else 
          m_peakx = m_peakx + halo_dir * (m_move_speed - abs(m_peaky - current_y));
        changling = changling * -1;      
      }
      else {
        m_peaky = m_peaky + (halo_dir * m_move_speed);  
      }
    }
  } 
  
  else {
    if (temp_peakx > (m_max_x - m_height)) movesignx = -1.0;
    if (temp_peakx < (m_min_x + m_height)) movesignx = 1.0; 
    
    if (temp_peaky > (m_max_y - m_height)) movesigny = -1.0;
    if (temp_peaky < (m_min_y + m_height)) movesigny = 1.0;
    
    m_peakx = m_peakx + (movesignx * moveYscaler) + .5;
    m_peaky = m_peaky + (movesigny * moveYscaler) + .5; 
  } 
  
  /*  //some defunct code from when halo's were round (but not equidistant travel from anchor)  
   //if changing nothing (move_counter < updatestep), continue to rotate in same direction on current orbit
   // in order to move one cell along the orbit ring, we need to rotate at least 360 / # cells in orbit circumference
   const double PI = 3.14159265358979323846;
   double min_rotate_angle = 360 / (8 * (current_orbit - 1)); 
   m_peakx = (cos((deg_current_angle + halo_dir * min_rotate_angle) * PI/180)) * current_orbit + m_halo_anchor_x + .5;
   m_peaky = (sin((deg_current_angle + halo_dir * min_rotate_angle) * PI/180)) * current_orbit + m_halo_anchor_y + .5; 
   rad_current_angle = atan2(m_peaky - m_halo_anchor_y, m_peakx - m_halo_anchor_x);
   deg_current_angle = rad_current_angle * 180/PI;
   double rad_current_angle = atan2(m_peaky - m_halo_anchor_y, m_peakx - m_halo_anchor_x);
   double deg_current_angle = rad_current_angle * 180/PI;        
   int max_orbit_x = m_halo_anchor_x + m_halo_inner_radius + m_halo_width - m_height + 2;
   int min_orbit_x = m_halo_anchor_x - (m_halo_inner_radius + m_halo_width - m_height + 2);
   */
  
  
  //to speed things up, we only check cells within the possible spread of the peak
  //note that, at the moment, you must also create a global dummy resource (id = 0) with a negative or 0 initial value 'under' gradient resources 
  //    otherwise you end up with an unwanted additional gradient resource forming at 0,0 because Avida doesn't like having cells with res value = Null
  double thisdist;
  double thisheight = 0.0;
  int max_pos_x = min(int(m_peakx + m_spread + 1), GetX() - 1);
  int min_pos_x = max(int(m_peakx - m_spread - 1), 0);
  int max_pos_y = min(int(m_peaky + m_spread + 1), GetY() - 1);
  int min_pos_y = max(int(m_peaky - m_spread - 1), 0);
  
  for (int ii = min_pos_x; ii < max_pos_x + 1; ii++) {
    for (int jj = min_pos_y; jj < max_pos_y + 1; jj++) {
      thisdist = Distance(ii, jj, m_peakx, m_peaky);
      if (m_spread >= thisdist) {
  // determine individual cells values and add one to distance from center (e.g. so that center point = radius 1, not 0)
          thisheight = m_height / (thisdist + 1);       
  // create cylindrical profiles of resources whereever thisheight would be >1 (area where thisdist + 1 <= m_height) and slopes outside of that range
  // plateau = -1 turns off this option; if activated, causes 'peaks' to be flat plateaus = plateau value 
        if (thisheight >= 1 && m_plateau >= 0.0) thisheight = m_plateau; 
      }
      else
        thisheight = 0;

      Element(jj*GetX()+ii).SetInitial(thisheight);
      Element(jj*GetX()+ii).SetAmount(thisheight);
    }
  }   
  ResetResourceCounts();
  m_counter = 0;                            //reset decay counter after cone resources updated
}

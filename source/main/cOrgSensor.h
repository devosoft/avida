/*
 *  cOrgSensor.h
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

#ifndef cOrgSensor_h
#define cOrgSensor_h

#include "cCoords.h"
#include "cOrganism.h"
#include "cResourceLib.h"
#include "cWorld.h"

struct sOrgDisplay 
{
  int distance;
  int direction;
  int thing_id;
  int value;
  int message;
};

class cOrgSensor
{
  protected:
  cWorld* m_world;
  cOrganism* m_organism;     // Organism using this sensor
  
  private:
  int m_use_avatar;
  bool m_return_rel_facing; 
  sOrgDisplay m_last_seen_display;
  bool m_has_seen_display;
  
  void ResetOrgSensor();

  public:
  cOrgSensor(cWorld* world, cOrganism* in_organism);
  
  // ---------- Some Instruction Helpers -----------
  struct sSearchInfo {
    double amountFound;
    int resource_id;
    bool has_edible;
  };
  struct sLookInit {
    int habitat;
    int distance;
    int search_type;
    int id_sought;
  };
  struct sLookOut {
    int report_type;
    int habitat;
    int distance;
    int search_type;
    int id_sought;
    int count;
    int value;
    int group;
    int forage;
  }; 
  struct sBounds {
    int min_x;
    int min_y;
    int max_x;
    int max_y;
  };
  
  void Reset() { ResetOrgSensor(); }

  const sLookOut SetLooking(cAvidaContext& ctx, sLookInit& in_defs, int facing, int cell_id, bool use_ft);
  sSearchInfo TestCell(cAvidaContext& ctx, const cResourceLib& resource_lib, const int habitat_used, const int search_type, 
                      const cCoords target_cell_coords, const Apto::Array<int, Apto::Smart>& val_res, bool first_step);  
  sLookOut WalkCells(cAvidaContext& ctx, const cResourceLib& resource_lib, const int habitat_used, const int search_type, const int distance_sought, const int id_sought, const int facing, const int cell_id);
  sLookOut FindOrg(cOrganism* target_org, const int distance, const int facing);
  sLookOut GlobalVal(cAvidaContext& ctx, const int habitat_used, const int id_sought, const int search_type);
  int TestResDist(const int dist_used, const int search_type, const int id_sought, const int facing, const int cell);
  int GetMinDist(cAvidaContext& ctx, const int worldx, sBounds& bounds, const int cell_id, const int distance_sought, 
                 const int facing);
  int GetMaxDist(const int worldx, const int cell_id, const int distance_sought, sBounds& res_bounds);
  sBounds GetBounds(cAvidaContext& ctx, const cResourceLib& resource_lib, const int res_id, const int search_type);
  bool TestBounds(const cCoords cell_id, sBounds& bounds_set);
  Apto::Array <int, Apto::Smart> BuildResArray(const int habitat_used, const int id_sought, const cResourceLib& resource_lib, bool single_bound);
  
  void SetReturnRelativeFacing(bool do_set) { m_return_rel_facing = do_set; }
  int ReturnRelativeFacing(cOrganism* sighted_org);
  
  void SetLastSeenDisplay(sOrgDisplay* seen_display);
  bool HasSeenDisplay() { return m_has_seen_display; }
  inline sOrgDisplay& GetLastSeenDisplay() { return m_last_seen_display; }
  void SetPotentialDisplayData(sLookOut& stuff_seen);
  
  int FindDirFromHome();
  int FindDistanceFromHome();
};

#endif

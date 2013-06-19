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

  const cResourceLib& m_res_lib;
  
  void ResetOrgSensor();
  
  public:
  cOrgSensor(cWorld* world, cOrganism* in_organism);
  
  // ---------- Some Instruction Helpers -----------
  struct sSearchInfo {
    double amountFound;
    int resource_id;
    bool has_edible:1;
    bool has_some:1;
    
    inline sSearchInfo() : amountFound(0.0), resource_id(-9), has_edible(false), has_some(false) { ; }
  };
  struct sLookInit {
    int habitat;
    int distance;
    int search_type;
    int id_sought;
    
    inline sLookInit()
      : habitat(std::numeric_limits<int>::min()), distance(std::numeric_limits<int>::max())
      , search_type(0), id_sought(-1)
    {
    }
    
    inline void Clear()
    {
      habitat = std::numeric_limits<int>::min();
      distance = std::numeric_limits<int>::max();
      search_type = 0;
      id_sought = -1;
    }
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
    int deviance;
    
    inline sLookOut()
      : report_type(0), habitat(0), distance(-1), search_type(0), id_sought(-1), count(0), value(0)
      , group(-9), forage(-9), deviance(0)
    {
    }
  }; 
  struct sBounds {
    int min_x;
    int min_y;
    int max_x;
    int max_y;
  };
  Apto::Array<sBounds> m_soloBounds;
  
  void Reset() { ResetOrgSensor(); }
  const sLookOut SetLooking(cAvidaContext& ctx, sLookInit& in_defs, int facing, int cell_id, bool use_ft);
  sSearchInfo TestCell(cAvidaContext& ctx, sLookInit& in_defs, const Apto::Coord<int>& target_cell_coords,
                      const Apto::Array<int, Apto::Smart>& val_res, bool first_step, bool stop_at_first_found);
  sLookOut WalkCells(cAvidaContext& ctx, sLookInit& in_defs, const int facing, const int cell_id);
  sLookOut FindOrg(cAvidaContext& ctx, cOrganism* target_org, const int distance, const int facing);
  sLookOut FindResCenter(cAvidaContext& ctx, const int res_id, const int distance_sought, const int facing);
  void FindThing(int target_cell, const int distance_sought, const int facing, cOrgSensor::sLookOut& org_search, cOrganism* target_org = NULL);
  sLookOut GlobalVal(cAvidaContext& ctx, sLookInit& in_defs);
  int TestResDist(sLookInit& in_defs, const int facing, const int cell);
  int GetMinDist(const int worldx, sBounds& bounds, const int cell_id, const int distance_sought, const int facing);
  int GetMaxDist(const int worldx, const int cell_id, const int distance_sought, sBounds& res_bounds);
  sBounds GetBounds(cAvidaContext& ctx, const int res_id);
  inline bool TestBounds(const Apto::Coord<int>& cell_id, sBounds& bounds_set);
  Apto::Array<int, Apto::Smart> BuildResArray(sLookInit& in_defs, bool single_bound);
  
  void SetReturnRelativeFacing(bool do_set) { m_return_rel_facing = do_set; }
  int ReturnRelativeFacing(cOrganism* sighted_org);
  
  void SetLastSeenDisplay(sOrgDisplay* seen_display);
  bool HasSeenDisplay() { return m_has_seen_display; }
  inline sOrgDisplay& GetLastSeenDisplay() { return m_last_seen_display; }
  void SetPotentialDisplayData(sLookOut& stuff_seen);
  
  int FindDirFromHome();
  int FindDistanceFromHome();
};

inline bool cOrgSensor::TestBounds(const Apto::Coord<int>& cell_id, sBounds& bounds)
{
  const int curr_x = cell_id.X();
  const int curr_y = cell_id.Y();
  if (curr_x < bounds.min_x || curr_y < bounds.min_y || curr_x > bounds.max_x || curr_y > bounds.max_y) return false;
  return true;
}

#endif

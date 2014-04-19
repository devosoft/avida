/*
 *  hardware/features/VisualSensor.h
 *  avida-core
 *
 *  Created by David on 7/24/13 based on cOrgSensor.
 *  Copyright 2013 Michigan State University. All rights reserved.
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaHardwareFeaturesVisualSensor_h
#define AvidaHardwareFeaturesVisualSensor_h

#include "avida/environment/Types.h"
#include "avida/hardware/Feature.h"


namespace Avida {
  namespace Hardware {
    namespace Features {
      
      // Hardware::Features::VisualSensor
      // --------------------------------------------------------------------------------------------------------------
      
      class VisualSensor : public Feature
      {
      public:
        struct OrgDisplay
        {
          int distance;
          int direction;
          int thing_id;
          int value;
          int message;
        };

        struct LookSettings
        {
          int habitat;
          int distance;
          int search_type;
          int id_sought;
          
          LIB_EXPORT inline LookSettings()
          : habitat(std::numeric_limits<int>::min()), distance(std::numeric_limits<int>::max()), search_type(0), id_sought(-1)
          {
          }
          
          LIB_EXPORT inline void Clear()
          {
            habitat = std::numeric_limits<int>::min();
            distance = std::numeric_limits<int>::max();
            search_type = 0;
            id_sought = -1;
          }
        };
        
        struct LookResults
        {
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
          
          LIB_EXPORT inline LookResults()
          : report_type(0), habitat(0), distance(-1), search_type(0), id_sought(-1), count(0), value(0)
          , group(-9), forage(-9), deviance(0)
          {
          }
        };
        
        struct Bounds
        {
          int min_x;
          int min_y;
          int max_x;
          int max_y;
        };

      private:
        static int s_feature_idx;
        
      private:
        int m_use_avatar;
        bool m_return_rel_facing;

        bool m_has_seen_display;
        OrgDisplay m_last_seen_display;
        
        Apto::Array<Bounds> m_soloBounds;

      
      public:
        LIB_EXPORT inline VisualSensor(Base* hw) : Feature(hw) { resetOrgSensor(); }
        LIB_EXPORT ~VisualSensor();
        
        LIB_EXPORT static inline VisualSensor& Of(Base* hw);

        LIB_EXPORT void Reset(Context& ctx);
        
        
        LIB_EXPORT LookResults PerformLook(Context& ctx, LookSettings& in_defs, int facing, int cell_id, bool use_ft);
        
        
        LIB_EXPORT inline void SetReturnRelativeFacing(bool do_set) { m_return_rel_facing = do_set; }
        LIB_EXPORT int ReturnRelativeFacing(Structure::Element& element);
        
        LIB_EXPORT void SetLastSeenDisplay(OrgDisplay* seen_display);
        LIB_EXPORT inline bool HasSeenDisplay() { return m_has_seen_display; }
        LIB_EXPORT inline OrgDisplay& GetLastSeenDisplay() { return m_last_seen_display; }
        
        
      private:
        struct SearchInfo
        {
          double amountFound;
          int resource_id;
          bool has_edible:1;
          bool has_some:1;
          
          inline SearchInfo() : amountFound(0.0), resource_id(-9), has_edible(false), has_some(false) { ; }
        };

        
        void resetOrgSensor();
        
        LIB_LOCAL SearchInfo testCell(Context& ctx, const Environment::ResourceManager& resource_lib, const int habitat_used, const int search_type,
                             const Apto::Coord<int>& target_cell_coords, const Apto::Array<int, Apto::Smart>& val_res, bool first_step,
                             bool stop_at_first_found);
        LIB_LOCAL LookResults walkCells(Context& ctx, const Environment::ResourceManager& resource_lib, const int habitat_used, const int search_type,
                              const int distance_sought, const int id_sought, const int facing, const int cell_id);
        LIB_LOCAL LookResults findOrg(Structure::Element& target_org, const int distance, const int facing);
        LIB_LOCAL LookResults globalVal(Context& ctx, const int habitat_used, const int id_sought, const int search_type);

        LIB_LOCAL int getMinDist(const int worldx, Bounds& bounds, const int cell_id, const int distance_sought, const int facing);
        LIB_LOCAL int getMaxDist(const int worldx, const int cell_id, const int distance_sought, Bounds& res_bounds);
        LIB_LOCAL Bounds getBounds(Context& ctx, const int res_id);
        LIB_LOCAL inline bool testBounds(const Apto::Coord<int>& cell_id, Bounds& bounds_set);
        
        LIB_LOCAL int findDirFromHome();
        LIB_LOCAL int findDistanceFromHome();

        LIB_LOCAL void buildResArray(Apto::Array <int, Apto::Smart>& res_array, LookSettings& in_defs, const Environment::ResourceManager& resource_lib, bool single_bound);

        LIB_LOCAL void setPotentialDisplayData(LookResults& stuff_seen);
      };
      
      
      inline VisualSensor& VisualSensor::Of(Base* hw)
      {
        assert(dynamic_cast<VisualSensor*>(featureOf(s_feature_idx, hw)));
        return *static_cast<VisualSensor*>(featureOf(s_feature_idx, hw));
      }
      
    }
  }
}


#endif

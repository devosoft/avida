/*
 *  private/environment/resources/Dynamic.h
 *  avida-core
 *
 *  Created by David on 1/29/13.
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

#ifndef AvidaEnvironmentResourcesDynamic_h
#define AvidaEnvironmentResourcesDynamic_h

#include "avida/environment/Resource.h"


namespace Avida {
  namespace Environment {
    namespace Resources {
      
      class Dynamic : public Resource
      {
      private:
        int curr_peakx, curr_peaky;
        
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
        
        
      public:
        LIB_EXPORT ~Dynamic();
        
        LIB_EXPORT static void Initialize();
        
        
        LIB_EXPORT ResourceQuantity AmountAt(const Structure::Coord& location, Update current_update);
        LIB_EXPORT void SetAmountAt(ResourceQuantity amount, const Structure::Coord& location, Update current_update);
        LIB_EXPORT ResourceQuantity ModifyAmountAt(ResourceQuantity adjust_amount, const Structure::Coord& location,
                                                   Update current_update);
        
        LIB_EXPORT void PerformUpdate(Avida::Context& ctx, Update current_update);
        LIB_EXPORT void DefinitionChanged(Update current_update);
        
        
      private:
        LIB_LOCAL Dynamic(ResourceDefinition& def);
        
        LIB_LOCAL static Dynamic* Create(ResourceDefinition& def, Structure::Controller& structure);
        
      };
      
    };
  };
};

#endif

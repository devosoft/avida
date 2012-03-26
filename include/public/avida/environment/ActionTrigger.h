/*
 *  environment/ActionTrigger.h
 *  avida-core
 *
 *  Created by David on 6/24/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
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

#ifndef AvidaEnvironmentActionTrigger_h
#define AvidaEnvironmentActionTrigger_h

#include "avida/environment/Types.h"

namespace Avida {
  namespace Environment {
    
    // Environment::ActionTrigger - Direct resource production resulting from an action
    // --------------------------------------------------------------------------------------------------------------

    class ActionTrigger
    {
      friend class Manager;
    private:
      const ActionTriggerID m_id;
      const Apto::String m_desc;
      ConstProductPtr m_product;
      int m_tmp_order;
      
      LIB_LOCAL inline ActionTrigger(const ActionTriggerID& trigger_id, const Apto::String& desc, ConstProductPtr product,
                                     int tmp_order = -1)
        : m_id(trigger_id), m_desc(desc), m_product(product), m_tmp_order(tmp_order) { ; }

    public:
      LIB_EXPORT inline ~ActionTrigger() { ; }
      
      LIB_EXPORT inline const ActionTriggerID& GetID() const { return m_id; }
      LIB_EXPORT inline const Apto::String& GetDescription() const { return m_desc; }
      LIB_EXPORT inline ConstProductPtr GetProduct() const { return m_product; }

      // Transitionary methods
      LIB_EXPORT inline int TempOrdering() const { return m_tmp_order; }
    };
    
  };
};

#endif

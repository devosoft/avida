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
#include "avida/util/ArgParser.h"

#include <cassert>


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
      ActionTest m_action_test;
      int m_tmp_order;
      Util::Args* m_args;

      Apto::String m_prop_id_ave;
      Apto::String m_prop_id_count;

      
      LIB_LOCAL inline ActionTrigger(const ActionTriggerID& trigger_id, const Apto::String& desc,
                                     ActionTest test, int tmp_order = -1, Util::Args* args = NULL)
        : m_id(trigger_id), m_desc(desc), m_action_test(test), m_tmp_order(tmp_order), m_args(args)
      {
        m_prop_id_ave = Apto::FormatStr("environment.triggers.%s.average", (const char*)trigger_id);
        m_prop_id_count = Apto::FormatStr("environment.triggers.%s.count", (const char*)trigger_id);
      }

    public:
      LIB_EXPORT inline ~ActionTrigger() { ; }
      
      LIB_EXPORT inline const ActionTriggerID& ID() const { return m_id; }
      LIB_EXPORT inline const Apto::String& Description() const { return m_desc; }
      
      LIB_EXPORT inline double Test(/* test arguments*/) { return m_action_test(/* test_arguments */); }
      
      
      LIB_EXPORT inline const Apto::String& AveragePropertyID() const { return m_prop_id_ave; }
      LIB_EXPORT inline const Apto::String& CountPropertyID() const { return m_prop_id_count; }

      bool HasArguments() const { return (m_args != NULL); }
      Avida::Util::Args& Arguments() const { assert(m_args); return *m_args; }


      // Transitionary methods
      LIB_EXPORT inline int TempOrdering() const { return m_tmp_order; }
    };
    
  };
};

#endif

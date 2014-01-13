/*
 *  hardware/InstArchCPU.h
 *  avida-core
 *
 *  Created by David on 7/11/13.
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

#ifndef AvidaHardwareInstArchCPU_h
#define AvidaHardwareInstArchCPU_h

#include "avida/core/InstructionSequence.h"
#include "avida/hardware/Base.h"
#include "avida/hardware/InstSet.h"


namespace Avida {
  namespace Hardware {
    
    // Hardware::InstArchCPU
    // --------------------------------------------------------------------------------------------------------------
    
    class InstArchCPU : public Base
    {
    protected:
      InstSet* m_instset;
     
      
    private:
      struct {
        bool m_has_pre_hooks:1;
        bool m_has_post_hooks:1;
      };
      
      
    public:
      LIB_EXPORT InstArchCPU(Context& ctx, ConfigPtr cfg, Biota::OrganismPtr owner);
      LIB_EXPORT virtual ~InstArchCPU() = 0;
      
      
    protected:
      LIB_EXPORT inline bool PreInstructionHook(Context& ctx, Instruction cur_inst, int hw_context);
      LIB_EXPORT inline void PostInstructionHook(Context& ctx, Instruction cur_inst, int hw_context);
      
    private:
      LIB_EXPORT bool preInstHook(Context& ctx, Instruction cur_inst, int hw_context);
      LIB_EXPORT void postInstHook(Context& ctx, Instruction cur_inst, int hw_context);
    };

    
    inline bool InstArchCPU::PreInstructionHook(Context& ctx, Instruction cur_inst, int hw_context)
    {
      return (m_has_pre_hooks) ? preInstHook(ctx, cur_inst, hw_context) : true;
    }
    
    inline void InstArchCPU::PostInstructionHook(Context& ctx, Instruction cur_inst, int hw_context)
    {
      if (m_has_post_hooks) postInstHook(ctx, cur_inst, hw_context);
    }

  };
};

#endif

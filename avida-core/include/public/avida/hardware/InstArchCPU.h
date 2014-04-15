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
    
    // Hardware::InstructionHook
    // --------------------------------------------------------------------------------------------------------------
    
    class InstructionHook
    {
    public:
      LIB_EXPORT virtual ~InstructionHook() = 0;
      
      LIB_EXPORT virtual bool PreInstruction(Context& ctx, Instruction cur_inst, int hw_context);
      LIB_EXPORT virtual void PostInstruction(Context& ctx, Instruction cur_inst, int hw_context);
    };
    
    
    // Hardware::InstArchCPU
    // --------------------------------------------------------------------------------------------------------------
    
    class InstArchCPU : public Base
    {
    protected:
      InstSet* m_instset;
     
      
    private:
      Apto::Array<InstructionHook*> m_pre_hooks;
      Apto::Array<InstructionHook*> m_post_hooks;
      
      
    public:
      LIB_EXPORT InstArchCPU(Context& ctx, ConfigPtr cfg, Biota::OrganismPtr owner);
      LIB_EXPORT virtual ~InstArchCPU() = 0;
      
      LIB_EXPORT void Reset(Context& ctx);
      
      
      LIB_EXPORT void AttachPreInstructionHook(InstructionHook* hook);
      LIB_EXPORT void DetachPreInstructionHook(InstructionHook* hook);
      LIB_EXPORT void AttachPostInstructionHook(InstructionHook* hook);
      LIB_EXPORT void DetachPostInstructionHook(InstructionHook* hook);
      
    protected:
      LIB_EXPORT inline bool PreInstructionHook(Context& ctx, Instruction cur_inst, int hw_context);
      LIB_EXPORT inline void PostInstructionHook(Context& ctx, Instruction cur_inst, int hw_context);
      
    private:
      LIB_EXPORT bool preInstHook(Context& ctx, Instruction cur_inst, int hw_context);
      LIB_EXPORT void postInstHook(Context& ctx, Instruction cur_inst, int hw_context);
    };

    
    inline bool InstArchCPU::PreInstructionHook(Context& ctx, Instruction cur_inst, int hw_context)
    {
      return (m_pre_hooks.GetSize()) ? preInstHook(ctx, cur_inst, hw_context) : true;
    }
    
    inline void InstArchCPU::PostInstructionHook(Context& ctx, Instruction cur_inst, int hw_context)
    {
      if (m_post_hooks.GetSize()) postInstHook(ctx, cur_inst, hw_context);
    }

  };
};

#endif

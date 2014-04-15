/*
 *  hardware/InstArchCPU.cc
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

#include "avida/hardware/InstArchCPU.h"


Avida::Hardware::InstArchCPU::~InstArchCPU() { ; }


void Avida::Hardware::InstArchCPU::Reset(Context& ctx)
{
  Base::Reset(ctx);
}


void Avida::Hardware::InstArchCPU::AttachPreInstructionHook(InstructionHook* hook)
{
  // Just pushes onto the array for now... should this verify it has not already been attached?
  m_pre_hooks.Push(hook);
}

void Avida::Hardware::InstArchCPU::DetachPreInstructionHook(InstructionHook* hook)
{
  for (int i = 0; i < m_pre_hooks.GetSize(); i++) {
    if (m_pre_hooks[i] == hook) {
      if (m_pre_hooks.GetSize() > 1) m_pre_hooks[i] = m_pre_hooks[m_pre_hooks.GetSize() - 1];
      m_pre_hooks.Resize(m_pre_hooks.GetSize() - 1);
    }
  }
}

void Avida::Hardware::InstArchCPU::AttachPostInstructionHook(InstructionHook* hook)
{
  // Just pushes onto the array for now... should this verify it has not already been attached?
  m_post_hooks.Push(hook);
}

void Avida::Hardware::InstArchCPU::DetachPostInstructionHook(InstructionHook* hook)
{
  for (int i = 0; i < m_post_hooks.GetSize(); i++) {
    if (m_post_hooks[i] == hook) {
      if (m_post_hooks.GetSize() > 1) m_post_hooks[i] = m_post_hooks[m_post_hooks.GetSize() - 1];
      m_post_hooks.Resize(m_post_hooks.GetSize() - 1);
    }
  }
}


bool Avida::Hardware::InstArchCPU::preInstHook(Context& ctx, Instruction cur_inst, int hw_context)
{
  bool success = true;
  for (int i = 0; i < m_pre_hooks.GetSize(); i++) {
    if (!m_pre_hooks[i]->PreInstruction(ctx, cur_inst, hw_context)) success = false;
  }
  
  return success;
}


void Avida::Hardware::InstArchCPU::postInstHook(Context& ctx, Instruction cur_inst, int hw_context)
{
  for (int i = 0; i < m_pre_hooks.GetSize(); i++) m_post_hooks[i]->PostInstruction(ctx, cur_inst, hw_context);
}


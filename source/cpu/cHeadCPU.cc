/*
 *  cHeadCPU.cc
 *  Avida
 *
 *  Called "head_cpu.cc" prior to 11/30/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cHeadCPU.h"

#include <cassert>


void cHeadCPU::fullAdjust(int mem_size)
{
  assert(m_mem_space >= 0);
  // Ensure that m_mem_space is valid
  if (m_mem_space != m_cached_ms) {
    if (m_mem_space >= m_hardware->GetNumMemSpaces()) m_mem_space %= m_hardware->GetNumMemSpaces();
    m_cached_ms = m_mem_space;
    m_memory = &m_hardware->GetMemory(m_mem_space);
  }
  
  if (mem_size < 0) mem_size = GetMemSize();
  
  // If we are still in range, stop here!
  if (m_position >= 0 && m_position < mem_size) return;
  
  // If the memory is gone, just stick it at the begining of its parent.
  // @DMB - note: this violates the circularity of memory spaces.  You can loop forward, but not backward.
  if (mem_size == 0 || m_position < 0) {
    m_position = 0;
    return;
  }
  
  // position back at the begining of the creature as necessary.
  if (m_position < (2 * mem_size)) m_position -= mem_size;
  else m_position %= mem_size;
}


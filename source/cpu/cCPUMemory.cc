/*
 *  cCPUMemory.cc
 *  Avida
 *
 *  Called "cpu_memory.cc" prior to 11/22/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#include "cCPUMemory.h"

using namespace std;


cCPUMemory::cCPUMemory(const cCPUMemory& in_memory) : cSequence(in_memory), m_flag_array(in_memory.GetSize())
{
  for (int i = 0; i < m_flag_array.GetSize(); i++) m_flag_array[i] = in_memory.m_flag_array[i];
}


void cCPUMemory::adjustCapacity(int new_size)
{
  cSequence::adjustCapacity(new_size);
  if (m_seq.GetSize() != m_flag_array.GetSize()) m_flag_array.Resize(m_seq.GetSize()); 
}


void cCPUMemory::prepareInsert(int pos, int num_sites)
{
  assert(pos >= 0 && pos <= m_active_size); // Must insert at a legal position!
  assert(num_sites > 0); // Must insert positive number of lines!
  
  // Re-adjust the size...
  const int old_size = m_active_size;
  const int new_size = m_active_size + num_sites;
  adjustCapacity(new_size);
  
  // Shift any sites needed...
  for (int i = old_size - 1; i >= pos; i--) m_seq[i + num_sites] = m_seq[i];
  for (int i = old_size - 1; i >= pos; i--) m_flag_array[i + num_sites] = m_flag_array[i];
}


void cCPUMemory::Reset(int new_size)
{
  assert(new_size >= 0);

  adjustCapacity(new_size);
  Clear();
}


void cCPUMemory::Resize(int new_size)
{
  assert(new_size >= 0);

  const int old_size = m_active_size;
  adjustCapacity(new_size);
  
  for (int i = old_size; i < new_size; i++) {
    m_seq[i].SetOp(0);
    m_flag_array[i] = 0;
  }
}


void cCPUMemory::ResizeOld(int new_size)
{
  assert(new_size >= 0);

  const int old_size = m_active_size;
  adjustCapacity(new_size);

  for (int i = old_size; i < new_size; i++) m_flag_array[i] = 0;
}


void cCPUMemory::Copy(int to, int from)
{
  assert(to >= 0);
  assert(to < m_seq.GetSize());
  assert(from >= 0);
  assert(from < m_seq.GetSize());
  
  m_seq[to] = m_seq[from];
  m_flag_array[to] = m_flag_array[from];
}


void cCPUMemory::Insert(int pos, const cInstruction& inst)
{
  assert(pos >= 0);
  assert(pos <= m_seq.GetSize());

  prepareInsert(pos, 1);
  m_seq[pos] = inst;
  m_flag_array[pos] = 0;
}

void cCPUMemory::Insert(int pos, const cSequence& genome)
{
  assert(pos >= 0);
  assert(pos <= m_seq.GetSize());

  prepareInsert(pos, genome.GetSize());
  for (int i = 0; i < genome.GetSize(); i++) {
    m_seq[i + pos] = genome[i];
    m_flag_array[i + pos] = 0;
  }
}

void cCPUMemory::Remove(int pos, int num_sites)
{
  assert(num_sites > 0);                    // Must remove something...
  assert(pos >= 0);                         // Removal must be in genome.
  assert(pos + num_sites <= m_active_size); // Cannot extend past end of genome.

  const int new_size = m_active_size - num_sites;
  for (int i = pos; i < new_size; i++) {
    m_seq[i] = m_seq[i + num_sites];
    m_flag_array[i] = m_flag_array[i + num_sites];
  }
  adjustCapacity(new_size);
}

void cCPUMemory::Replace(int pos, int num_sites, const cSequence& genome)
{
  assert(pos >= 0);                         // Replace must be in genome
  assert(num_sites >= 0);                   // Cannot replace negative
  assert(pos + num_sites <= m_active_size); // Cannot extend past end!
  
  const int size_change = genome.GetSize() - num_sites;
  
  // First, get the size right
  if (size_change > 0) prepareInsert(pos, size_change);
  else if (size_change < 0) Remove(pos, -size_change);
  
  // Now just copy everything over!
  for (int i = 0; i < genome.GetSize(); i++) {
    m_seq[i + pos] = genome[i];
    m_flag_array[i + pos] = 0;
  }
}


void cCPUMemory::operator=(const cCPUMemory& other_memory)
{
  adjustCapacity(other_memory.m_active_size);
  
  // Fill in the new information...
  for (int i = 0; i < m_active_size; i++) {
    m_seq[i] = other_memory.m_seq[i];
    m_flag_array[i] = other_memory.m_flag_array[i];
  }
}


void cCPUMemory::operator=(const cSequence& other_genome)
{
  adjustCapacity(other_genome.GetSize());
  
  // Fill in the new information...
  for (int i = 0; i < m_active_size; i++) {
    m_seq[i] = other_genome[i];
    m_flag_array[i] = 0;
  }
}


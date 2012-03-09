/*
 *  cCPUMemory.h
 *  Avida
 *
 *  Called "cpu_memory.hh" prior to 11/22/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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
 */

#ifndef cCPUMemory_h
#define cCPUMemory_h

#include "avida/core/InstructionSequence.h"


class cCPUMemory : public Avida::InstructionSequence
{
private:
	static const unsigned char MASK_COPIED   = 0x01;
	static const unsigned char MASK_MUTATED  = 0x02;
	static const unsigned char MASK_EXECUTED = 0x04;
	static const unsigned char MASK_POINTMUT = 0x08;
	static const unsigned char MASK_COPYMUT  = 0x10;
	static const unsigned char MASK_INJECTED = 0x20;
	static const unsigned char MASK_UNUSED1  = 0x40; // unused bit
	static const unsigned char MASK_UNUSED2  = 0x80; // unused bit
  
  Apto::Array<unsigned char> m_flag_array;

  void adjustCapacity(int new_size);
  void prepareInsert(int pos, int num_sites);

public:
  cCPUMemory(const cCPUMemory& in_memory);
  cCPUMemory(const InstructionSequence& in_genome) : InstructionSequence(in_genome), m_flag_array(in_genome.GetSize()) { ; }
  explicit cCPUMemory(int size = 1)  : InstructionSequence(size), m_flag_array(size) { ClearFlags(); }
  cCPUMemory(const Apto::String& in_string) : InstructionSequence(in_string), m_flag_array(in_string.GetSize()) { ; }
  ~cCPUMemory() { ; }

  inline bool FlagCopied(int pos) const     { return (MASK_COPIED   & m_flag_array[pos]) != 0; }
  inline bool FlagMutated(int pos) const    { return (MASK_MUTATED  & m_flag_array[pos]) != 0; }
  inline bool FlagExecuted(int pos) const   { return (MASK_EXECUTED & m_flag_array[pos]) != 0; }
  inline bool FlagPointMut(int pos) const   { return (MASK_POINTMUT & m_flag_array[pos]) != 0; }
  inline bool FlagCopyMut(int pos) const    { return (MASK_COPYMUT  & m_flag_array[pos]) != 0; }
  inline bool FlagInjected(int pos) const   { return (MASK_INJECTED & m_flag_array[pos]) != 0; }
  
  inline void SetFlagCopied(int pos)     { m_flag_array[pos] |= MASK_COPIED;   }
  inline void SetFlagMutated(int pos)    { m_flag_array[pos] |= MASK_MUTATED;  }
  inline void SetFlagExecuted(int pos)   { m_flag_array[pos] |= MASK_EXECUTED; }
  inline void SetFlagPointMut(int pos)   { m_flag_array[pos] |= MASK_POINTMUT; }
  inline void SetFlagCopyMut(int pos)    { m_flag_array[pos] |= MASK_COPYMUT;  }
  inline void SetFlagInjected(int pos)   { m_flag_array[pos] |= MASK_INJECTED; }
	
	inline void ClearFlagCopied(int pos)     { m_flag_array[pos] &= ~MASK_COPIED;   }
	inline void ClearFlagMutated(int pos)    { m_flag_array[pos] &= ~MASK_MUTATED;  }
	inline void ClearFlagExecuted(int pos)   { m_flag_array[pos] &= ~MASK_EXECUTED; }
	inline void ClearFlagPointMut(int pos)   { m_flag_array[pos] &= ~MASK_POINTMUT; }
	inline void ClearFlagCopyMut(int pos)    { m_flag_array[pos] &= ~MASK_COPYMUT;  }
  inline void ClearFlagInjected(int pos)   { m_flag_array[pos] &= ~MASK_INJECTED; }
  
  
  void Clear()
	{
		for (int i = 0; i < m_active_size; i++) {
			m_seq[i].SetOp(0);
			m_flag_array[i] = 0;
		}
	}
  inline void ClearFlags() { m_flag_array.SetAll(0); }
  void Reset(int new_size);     // Reset size, clearing contents...
  void ResizeOld(int new_size); // Reset size, save contents, init to previous
    
  
  void Resize(int new_size);
  void Copy(int to, int from);
  void Insert(int pos, const Avida::Instruction& inst);
  void Insert(int pos, const InstructionSequence& genome);
  void Remove(int pos, int num_sites = 1);
  void Replace(int pos, int num_sites, const InstructionSequence& genome);

  void operator=(const cCPUMemory& other_memory);
  void operator=(const InstructionSequence& other_genome);
};

#endif

/*
 *  cHeadCPU.h
 *  Avida
 *
 *  Called "head_cpu.hh" prior to 11/30/05.
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

#ifndef cHeadCPU_h
#define cHeadCPU_h

#include "Avida.h"

#include "cCPUMemory.h"
#include "cHardwareBase.h"
#include "cInstSet.h"

/**
 * The cHeadCPU class contains a pointer to locations in memory for a CPU.
 **/

class cCodeLabel;
class cSequence;
class cInstruction;
class cString;


class cHeadCPU
{
protected:
  cHardwareBase* m_hardware;
  int m_position;
  int m_mem_space;
  int m_cached_ms;
  cCPUMemory* m_memory;
  
  void fullAdjust(int mem_size = -1);

  int FindLabel_Forward(const cCodeLabel& search_label, const cSequence& search_mem, int pos);
  int FindLabel_Backward(const cCodeLabel& search_label, const cSequence& search_mem, int pos);
  

public:
  inline cHeadCPU(cHardwareBase* hw = NULL, int pos = 0, int ms = 0);
  inline cHeadCPU(const cHeadCPU& in_cpu_head);
  ~cHeadCPU() { ; }
  
  inline const cCPUMemory& GetMemory() const { return *m_memory; }
  inline cCPUMemory& GetMemory() { return *m_memory; }
  inline int GetMemSize() const { return m_memory->GetSize(); }
  
  inline void Adjust() { if (m_mem_space != m_cached_ms || m_position < 0 || m_position >= GetMemSize()) fullAdjust(); }
  inline void Reset(cHardwareBase* hw, int ms = 0) { m_hardware = hw; m_position = 0; m_mem_space = ms; if (hw) Adjust(); }
  
  inline int GetMemSpace() const { return m_mem_space; }
  inline int GetPosition() const { return m_position; }
  inline int GetFullLocation() const { return (m_position & 0xFFFFFF) | (m_mem_space << 24); }
  
  inline void Set(int pos, int ms = 0) { m_position = pos; m_mem_space = ms; Adjust(); }
  inline void SetFullLocation(int loc) { m_position = loc & 0xFFFFFF; m_mem_space = (loc >> 24); Adjust(); }
  inline void Set(const cHeadCPU& in_head) { m_position = in_head.m_position; m_mem_space = in_head.m_mem_space; }
  inline void AbsSet(int new_pos) { m_position = new_pos; }

  inline void Jump(int jump) { m_position += jump; Adjust(); }
  inline void AbsJump(int jump) { m_position += jump; }
  inline void LoopJump(int jump);
  
  inline void Advance() { m_position++; Adjust(); }
  inline void Retreat() { m_position--; Adjust(); }

  inline const cInstruction& GetInst() const { return GetMemory()[m_position]; }
  inline const cInstruction& GetInst(int offset) const { return GetMemory()[m_position + offset]; }
  inline cInstruction GetNextInst() const;

  inline void SetInst(const cInstruction& value) { GetMemory()[m_position] = value; }
  inline void InsertInst(const cInstruction& inst) { GetMemory().Insert(m_position, inst); }
  inline void RemoveInst() { GetMemory().Remove(m_position); }

  inline void SetFlagCopied() { return GetMemory().SetFlagCopied(m_position); }
  inline void SetFlagMutated() { return GetMemory().SetFlagMutated(m_position); }
  inline void SetFlagExecuted() { return GetMemory().SetFlagExecuted(m_position); }
  inline void SetFlagBreakpoint() { return GetMemory().SetFlagBreakpoint(m_position); }
  inline void SetFlagPointMut() { return GetMemory().SetFlagPointMut(m_position); }
  inline void SetFlagCopyMut() { return GetMemory().SetFlagCopyMut(m_position); }
  
  inline void ClearFlagCopied() { return GetMemory().ClearFlagCopied(m_position); }
  inline void ClearFlagMutated() { return GetMemory().ClearFlagMutated(m_position); }
  inline void ClearFlagExecuted() { return GetMemory().ClearFlagExecuted(m_position); }
  inline void ClearFlagBreakpoint() { return GetMemory().ClearFlagBreakpoint(m_position); }
  inline void ClearFlagPointMut() { return GetMemory().ClearFlagPointMut(m_position); }
  inline void ClearFlagCopyMut() { return GetMemory().ClearFlagCopyMut(m_position); }
  
  // Operator Overloading...
  inline cHeadCPU& operator=(const cHeadCPU& in_cpu_head);
  inline cHeadCPU& operator++() { m_position++; Adjust(); return *this; }
  inline cHeadCPU& operator--() { m_position--; Adjust(); return *this; }
  inline cHeadCPU& operator++(int) { return operator++(); }
  inline cHeadCPU& operator--(int) { return operator--(); }
  inline int operator-(const cHeadCPU& in_cpu_head) { return m_position - in_cpu_head.m_position; }
  inline bool operator==(const cHeadCPU& in_cpu_head) const;

  // Bool Tests...
  inline bool AtFront() const { return (m_position == 0); }
  inline bool AtEnd() const { return (m_position + 1 == GetMemory().GetSize()); }
  inline bool InMemory() const { return (m_position >= 0 && m_position < GetMemory().GetSize()); }
};


inline cHeadCPU::cHeadCPU(cHardwareBase* hw, int pos, int ms)
  : m_hardware(hw), m_position(pos), m_mem_space(ms), m_cached_ms(-1)
{
  if (hw) {
    if (pos || ms) Adjust();
    else {
      m_cached_ms = 0;
      m_memory = &m_hardware->GetMemory(0);
    }
  }
}

inline cHeadCPU::cHeadCPU(const cHeadCPU& in_cpu_head)
{
  m_hardware = in_cpu_head.m_hardware;
  m_position = in_cpu_head.m_position;
  m_mem_space = in_cpu_head.m_mem_space;
  m_cached_ms = in_cpu_head.m_cached_ms;
  m_memory = in_cpu_head.m_memory;
}

inline void cHeadCPU::LoopJump(int jump)
{
  m_position += jump;
  
  // keep in range
  m_position %= GetMemory().GetSize();
  if (m_position <= 0) m_position += GetMemory().GetSize();
}

inline cHeadCPU& cHeadCPU::operator=(const cHeadCPU& in_cpu_head)
{
  m_hardware = in_cpu_head.m_hardware;
  m_position = in_cpu_head.m_position;
  m_mem_space = in_cpu_head.m_mem_space;
  return *this;
}

inline bool cHeadCPU::operator==(const cHeadCPU& in_cpu_head) const
{
  return (m_hardware == in_cpu_head.m_hardware) && (m_position == in_cpu_head.m_position) &&
  (m_mem_space == in_cpu_head.m_mem_space);
}

inline cInstruction cHeadCPU::GetNextInst() const
{
  return (AtEnd()) ? m_hardware->GetInstSet().GetInstError() : GetMemory()[m_position + 1];
}

#endif

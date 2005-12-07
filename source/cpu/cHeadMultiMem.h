/*
 *  cHeadMultiMem.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cHeadMultiMem_h
#define cHeadMultiMem_h

#ifndef cHeadCPU_h
#include "cHeadCPU.h"
#endif

class cHeadMultiMem : public cHeadCPU {
private:
  int mem_space;

public:
  cHeadMultiMem() : cHeadCPU() { mem_space = 0; }
  cHeadMultiMem(cHardwareBase* hw, int pos = 0, int ms = 0) : cHeadCPU(hw, pos) { mem_space = ms; }
  cHeadMultiMem(const cHeadMultiMem& in_head) : cHeadCPU(in_head) { mem_space = in_head.mem_space; }

  void Adjust();
  void Reset(int in_mem_space = 0, cHardwareBase* new_hardware = NULL);
  void Set(int new_pos, int in_mem_space = 0, cHardwareBase * in_hardware = NULL);
  void Set(const cHeadMultiMem& in_head);
  void LoopJump(int jump);
  const cCPUMemory& GetMemory() const;
  cCPUMemory& GetMemory();
  const cInstruction& GetInst() const;
  const cInstruction& GetInst(int offset) const;

  int GetMemSpace() const { return mem_space; }

  void SetInst(const cInstruction & value);
  void InsertInst(const cInstruction & in_char);
  void RemoveInst();
  const cInstruction& GetNextInst();

  bool& FlagCopied();
  bool& FlagMutated();
  bool& FlagExecuted();
  bool& FlagBreakpoint();
  bool& FlagPointMut();
  bool& FlagCopyMut();

  // Operator Overloading...
  cHeadMultiMem& operator=(const cHeadMultiMem & in_cpu_head);
  bool operator==(const cHeadMultiMem & in_cpu_head) const; 
  bool AtEnd() const;
  bool InMemory() const;
};

#endif

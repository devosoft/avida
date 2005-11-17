/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HEAD_MULTI_MEM_HH
#define HEAD_MULTI_MEM_HH

#ifndef HEAD_CPU_HH
#include "cHeadCPU.h"
#endif
#ifndef DEFS_HH
#include "defs.h"
#endif

class cCodeLabel;
class cCPUMemory;
class cGenome;
class cHardwareBase;
class cInstruction;

class cHeadMultiMem : public cHeadCPU {
private:
  int mem_space;

public:
  cHeadMultiMem();
  cHeadMultiMem(cHardwareBase* in_hardware, int in_pos = 0, int mem_space = 0);
  cHeadMultiMem(const cHeadMultiMem& in_cpu_head);

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

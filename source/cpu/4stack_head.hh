/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef FOURSTACK_HEAD_HH
#define FOURSTACK_HEAD_HH

#ifndef CPU_HEAD_HH
#include "cpu_head.hh"
#endif
#ifndef DEFS_HH
#include "defs.hh"
#endif

class cCodeLabel;
class cCPUMemory;
class cGenome;
class cHardwareBase;
class cInstruction;

class c4StackHead : public cCPUHead {
private:
  int mem_space;

public:

  c4StackHead();
  c4StackHead(cHardwareBase * in_hardware, int in_pos = 0, int mem_space = 0);
  c4StackHead(const c4StackHead & in_cpu_head);

  void Adjust();
  void Reset(int in_mem_space=0, cHardwareBase * new_hardware = NULL);
  void Set(int new_pos, int in_mem_space = 0, cHardwareBase * in_hardware = NULL);
  void Set(const c4StackHead & in_head);
  void LoopJump(int jump);
  const cCPUMemory & GetMemory() const;
  cCPUMemory & GetMemory();
  const cInstruction & GetInst() const;
  const cInstruction & GetInst(int offset) const;

  int GetMemSpace() const { return mem_space; }

  void SetInst(const cInstruction & value);
  void InsertInst(const cInstruction & in_char);
  void RemoveInst();
  const cInstruction & GetNextInst();

  bool & FlagCopied();
  bool & FlagMutated();
  bool & FlagExecuted();
  bool & FlagBreakpoint();
  bool & FlagPointMut();
  bool & FlagCopyMut();

  // Operator Overloading...
  c4StackHead & operator=(const c4StackHead & in_cpu_head);
  bool operator==(const c4StackHead & in_cpu_head) const; 
  bool AtEnd() const;
  bool InMemory() const;
};

#endif

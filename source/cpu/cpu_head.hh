/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CPU_HEAD_HH
#define CPU_HEAD_HH

#ifndef DEFS_HH
#include "defs.hh"
#endif

class cHardwareBase;
class cCodeLabel;
class cGenome;
class cCPUMemory;

/**
 * The cCPUHead class contains a pointer to locations in memory for a CPU.
 **/

class cHardwareBase;
class cCodeLabel;
class cGenome;
class cInstruction;

class cCPUHead {
protected:
  cHardwareBase * main_hardware;
  cHardwareBase * cur_hardware;
  int position;

  int FindLabel_Forward(const cCodeLabel & search_label,
			const cGenome & search_mem, int pos);
  int FindLabel_Backward(const cCodeLabel & search_label,
			 const cGenome & search_mem, int pos);
public:
  cCPUHead();
  cCPUHead(cHardwareBase * in_hardware, int in_pos = 0);
  cCPUHead(const cCPUHead & in_cpu_head);
  virtual ~cCPUHead() { ; }
  
  /**
   * This function keeps the position within the range of the current memory.
   **/
  virtual void Adjust();

  virtual void Reset(cHardwareBase * new_hardware = NULL);
  
  /**
   * Set the new position of the head (and adjust it into range in Set()).
   **/

  virtual void Set(int new_pos, cHardwareBase * in_hardware = NULL);
  
  void AbsSet(int new_pos) { position = new_pos; }
  
  virtual void Set(const cCPUHead & in_head) {
    position = in_head.position;
    cur_hardware  = in_head.cur_hardware;
  }

  /**
   * Increment the new position of the head by 'jump'.
   **/

  void Jump(int jump);
  virtual void LoopJump(int jump);
  void AbsJump(int jump);
  
  // Other manipulation functions.
  void Advance();
  void Retreat();
  cCPUHead FindLabel(const cCodeLabel & label, int direction=1);

  // Accessors.
  int GetPosition() const { return position; }
  virtual const cCPUMemory & GetMemory() const;
  cHardwareBase * GetCurHardware() const { return cur_hardware; }
  cHardwareBase * GetMainHardware() const { return main_hardware; }
  virtual const cInstruction & GetInst() const;
  virtual const cInstruction & GetInst(int offset) const;
  // int GetFlag(int id) const;

  virtual void SetInst(const cInstruction & value);
  virtual void InsertInst(const cInstruction & in_char);
  virtual void RemoveInst();
  virtual const cInstruction & GetNextInst();

  virtual bool & FlagCopied();
  virtual bool & FlagMutated();
  virtual bool & FlagExecuted();
  virtual bool & FlagBreakpoint();
  virtual bool & FlagPointMut();
  virtual bool & FlagCopyMut();

  // Operator Overloading...
  virtual cCPUHead & operator=(const cCPUHead & in_cpu_head);
  cCPUHead & operator++();
  cCPUHead & operator--();
  cCPUHead & operator++(int);
  cCPUHead & operator--(int);

  inline int operator-(const cCPUHead & in_cpu_head) {
    if (cur_hardware != in_cpu_head.cur_hardware) return 0;
    else return position - in_cpu_head.position;
  }
  virtual bool operator==(const cCPUHead & in_cpu_head) const;

  // Bool Tests...
  inline bool AtFront() const { return (position == 0); }
  virtual bool AtEnd() const;
  virtual bool InMemory() const;

  // Test functions...
  int TestParasite() const;
};

#endif

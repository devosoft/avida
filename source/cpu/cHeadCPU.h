/*
 *  cHeadCPU.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cHeadCPU_h
#define cHeadCPU_h

#ifndef defs_h
#include "defs.h"
#endif

/**
 * The cHeadCPU class contains a pointer to locations in memory for a CPU.
 **/

class cHardwareBase;
class cCodeLabel;
class cCPUMemory;
class cGenome;
class cInstruction;

class cHeadCPU
{
protected:
  cHardwareBase* main_hardware;
  cHardwareBase* cur_hardware;
  int position;

  int FindLabel_Forward(const cCodeLabel& search_label, const cGenome& search_mem, int pos);
  int FindLabel_Backward(const cCodeLabel& search_label, const cGenome& search_mem, int pos);

public:
  cHeadCPU();
  cHeadCPU(cHardwareBase* in_hardware, int in_pos = 0);
  cHeadCPU(const cHeadCPU& in_cpu_head);
  virtual ~cHeadCPU() { ; }
  
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
  
  virtual void Set(const cHeadCPU & in_head) {
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
  cHeadCPU FindLabel(const cCodeLabel & label, int direction=1);

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

  virtual void SetFlagCopied();
  virtual void SetFlagMutated();
  virtual void SetFlagExecuted();
  virtual void SetFlagBreakpoint();
  virtual void SetFlagPointMut();
  virtual void SetFlagCopyMut();
  
  virtual void ClearFlagCopied();
  virtual void ClearFlagMutated();
  virtual void ClearFlagExecuted();
  virtual void ClearFlagBreakpoint();
  virtual void ClearFlagPointMut();
  virtual void ClearFlagCopyMut();
    
  // Operator Overloading...
  virtual cHeadCPU& operator=(const cHeadCPU& in_cpu_head);
  cHeadCPU & operator++();
  cHeadCPU & operator--();
  cHeadCPU & operator++(int);
  cHeadCPU & operator--(int);

  inline int operator-(const cHeadCPU & in_cpu_head) {
    if (cur_hardware != in_cpu_head.cur_hardware) return 0;
    else return position - in_cpu_head.position;
  }
  virtual bool operator==(const cHeadCPU & in_cpu_head) const;

  // Bool Tests...
  inline bool AtFront() const { return (position == 0); }
  virtual bool AtEnd() const;
  virtual bool InMemory() const;

  // Test functions...
  int TestParasite() const;


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif

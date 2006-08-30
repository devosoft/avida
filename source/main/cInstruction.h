/*
 *  cInstruction.h
 *  Avida
 *
 *  Called "inst.hh" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cInstruction_h
#define cInstruction_h

#include <assert.h>

#ifndef defs_h
#include "defs.h"
#endif

class cInstruction
{
private:
  unsigned char m_operand;

public:
  // Constructors and Destructor...
  cInstruction() : m_operand(0) { ; }
  cInstruction(const cInstruction& inst) { *this = inst; }
  explicit cInstruction(int in_op) { SetOp(in_op); }
  ~cInstruction() { ; }
  
  // Accessors...
  int GetOp() const { return static_cast<int>(m_operand); }
  void SetOp(int in_op) { assert(in_op < 256); m_operand = in_op; }

  // Operators...
  void operator=(const cInstruction& inst) { if (this != &inst) m_operand = inst.m_operand; }
  bool operator==(const cInstruction& inst) const { return (m_operand == inst.m_operand); }
  bool operator!=(const cInstruction& inst) const { return !(operator==(inst)); }

  // Some extra methods to convert too and from alpha-numeric symbols...
  char GetSymbol() const;
  void SetSymbol(char symbol);
};


#ifdef ENABLE_UNIT_TESTS
namespace nInstruction {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

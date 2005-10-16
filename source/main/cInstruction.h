//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INSTRUCTION_HH
#define INSTRUCTION_HH

#include <assert.h>

#ifndef DEFS_HH
#include "defs.h"
#endif

class cInstruction {
private:
  UCHAR operand;

public:
  // Accessors...
  int GetOp() const { return (int) operand; }
  void SetOp(int in_op) { assert(in_op < 256); operand = in_op; }

  // Operators...
  void operator=(const cInstruction & inst)
    { if (this != &inst) operand = inst.operand; }
  bool operator==(const cInstruction & inst) const
    { return (operand == inst.operand); }
  bool operator!=(const cInstruction & inst) const
    { return !(operator==(inst)); }

  // Constructors and Destructor...
  cInstruction() { operand = 0; }
  cInstruction(const cInstruction & _inst) { *this = _inst; }
  explicit cInstruction(int in_op) { SetOp(in_op); }
  ~cInstruction() { ; }

  // Some extra methods to convert too and from alpha-numeric symbols...
  char GetSymbol() const;
  void SetSymbol(char symbol);
};

#endif

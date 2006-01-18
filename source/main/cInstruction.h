/*
 *  cInstruction.h
 *  Avida
 *
 *  Created by David on 12/5/05.
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
  unsigned char operand;

public:
  // Constructors and Destructor...
  cInstruction() { operand = 0; }
  cInstruction(const cInstruction& _inst) { *this = _inst; }
  explicit cInstruction(int in_op) { SetOp(in_op); }
  ~cInstruction() { ; }
  
  // Accessors...
  int GetOp() const { return (int) operand; }
  void SetOp(int in_op) { assert(in_op < 256); operand = in_op; }

  // Operators...
  void operator=(const cInstruction & inst) { if (this != &inst) operand = inst.operand; }
  bool operator==(const cInstruction & inst) const { return (operand == inst.operand); }
  bool operator!=(const cInstruction & inst) const { return !(operator==(inst)); }

  // Some extra methods to convert too and from alpha-numeric symbols...
  char GetSymbol() const;
  void SetSymbol(char symbol);
};

#endif

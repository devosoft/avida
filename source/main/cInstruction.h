/*
 *  cInstruction.h
 *  Avida
 *
 *  Called "inst.hh" prior to 12/5/05.
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

#ifndef cInstruction_h
#define cInstruction_h

#include "Avida.h"

#include <cassert>

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
  // int GetOp() const { return static_cast<int>(m_operand); 
  //                     if (static_cast<int>(m_operand) > 30) { 
  //                       cout << m_operand << "  " << static_cast<int>(m_operand) << endl; 
  //                     }
  //                   }
  void SetOp(int in_op) { assert(in_op < 256); m_operand = in_op; }

  // Operators...
  void operator=(const cInstruction& inst) { if (this != &inst) m_operand = inst.m_operand; }
  bool operator==(const cInstruction& inst) const { return (m_operand == inst.m_operand); }
  bool operator!=(const cInstruction& inst) const { return !(operator==(inst)); }

  // Some extra methods to convert too and from alpha-numeric symbols...
  char GetSymbol() const;
  void SetSymbol(char symbol);
	
	static int ConvertSymbol(const char symbol);
};

#endif

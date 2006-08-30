/*
 *  cInstruction.cc
 *  Avida
 *
 *  Called "inst.cc" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cInstruction.h"


char cInstruction::GetSymbol() const
{
  if (m_operand < 26) return m_operand + 'a';
  if (m_operand < 52) return m_operand - 26 + 'A';
  if (m_operand < 62) return m_operand - 52 + '0';
  if (m_operand == 255) return '_';
  return  '?';
}

void cInstruction::SetSymbol(char symbol)
{
  if (symbol >= 'a' && symbol <= 'z') m_operand = symbol - 'a';
  else if (symbol >= 'A' && symbol <= 'Z') m_operand = symbol - 'A' + 26;
  else if (symbol >= '0' && symbol <= '9') m_operand = symbol - '0' + 52;
  else m_operand = 254;
}


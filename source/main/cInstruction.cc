/*
 *  cInstruction.cc
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cInstruction.h"


char cInstruction::GetSymbol() const
{
  if (operand < 26) return operand + 'a';
  if (operand < 52) return operand - 26 + 'A';
  if (operand < 62) return operand - 52 + '0';
  if (operand == 255) return '_';
  return  '?';
}

void cInstruction::SetSymbol(char symbol)
{
  if (symbol >= 'a' && symbol <= 'z') operand = symbol - 'a';
  else if (symbol >= 'A' && symbol <= 'Z') operand = symbol - 'A' + 26;
  else if (symbol >= '0' && symbol <= '9') operand = symbol - '0' + 52;
  else operand = 254;
}


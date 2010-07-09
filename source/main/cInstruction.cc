/*
 *  cInstruction.cc
 *  Avida
 *
 *  Called "inst.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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

int cInstruction::ConvertSymbol(char symbol)
{
	int retval;
	if (symbol >= 'a' && symbol <= 'z') retval = symbol - 'a';
  else if (symbol >= 'A' && symbol <= 'Z') retval = symbol - 'A' + 26;
  else if (symbol >= '0' && symbol <= '9') retval = symbol - '0' + 52;
  else retval = 254;
	return retval;
}

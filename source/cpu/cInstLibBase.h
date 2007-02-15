/*
 *  cInstLibBase.h
 *  Avida
 *
 *  Called "inst_lib_base.hh" prior to 12/5/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef cInstLibBase_h
#define cInstLibBase_h

#ifndef cInstruction_h
#include "cInstruction.h"
#endif

class cString;

class cInstLibBase
{
public:
  cInstLibBase() { ; }
  virtual ~cInstLibBase() { ; }
  virtual const cString& GetName(const unsigned int id) = 0;
  virtual const cString& GetNopName(const unsigned int id) = 0;
  virtual int GetNopMod(const unsigned int id) = 0;
  virtual int GetNopMod(const cInstruction& inst) = 0;
  virtual int GetSize() = 0;
  virtual int GetNumNops() = 0;
  virtual cInstruction GetInst(const cString& in_name) = 0;
  virtual const cInstruction GetInstDefault() = 0;
  virtual const cInstruction GetInstError() = 0;
};

#endif

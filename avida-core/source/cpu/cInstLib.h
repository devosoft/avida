/*
 *  cInstLib.h
 *  Avida
 *
 *  Called "inst_lib_base.hh" prior to 12/5/05.
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

#ifndef cInstLib_h
#define cInstLib_h

#include "avida/core/InstructionSequence.h"

#include "cInstLibEntry.h"

class cString;

using namespace Avida;


class cInstLib
{
protected:
  const int m_size;
  Apto::Map<Apto::String, int> m_namemap;

  int m_inst_default;
  int m_inst_null;
  
  cInstLib(); // @not_implemented
  
public:
  cInstLib(int size, int inst_default, int inst_null)
    : m_size(size), m_inst_default(inst_default), m_inst_null(inst_null) { ; }
  virtual ~cInstLib() { ; }

  inline int GetSize() const { return m_size; }

  virtual const cInstLibEntry& Get(int i) const = 0;
  inline const cInstLibEntry& operator[](int i) const { return Get(i); }
  inline int GetIndex(const cString& name) const;
  inline Apto::String GetNearMatch(const Apto::String& name) const { return Apto::NearMatch(name, m_namemap.Keys()); }

  inline const cString& GetName(int entry) const { return Get(entry).GetName(); }
  
  virtual const cString& GetNopName(const unsigned int id) = 0;
  virtual int GetNopMod(const unsigned int id) = 0;
  virtual int GetNopMod(const Instruction& inst) = 0;
  

  int GetInstDefault() const { return m_inst_default; }
  int GetInstNull() const { return m_inst_null; }

private:
    inline Instruction GetInst(const cString& name);

};


inline int cInstLib::GetIndex(const cString& name) const
{
  int idx = -1;
  if (m_namemap.Get((const char*)name, idx)) return idx;
  return -1;
}

inline Instruction cInstLib::GetInst(const cString& name)
{
  int idx;
  if (m_namemap.Get((const char*)name, idx)) return Instruction(idx);
  return Instruction(255);
}


#endif

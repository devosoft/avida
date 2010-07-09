/*
 *  cInstLib.h
 *  Avida
 *
 *  Called "inst_lib_base.hh" prior to 12/5/05.
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

#ifndef cInstLib_h
#define cInstLib_h

#ifndef cInstruction_h
#include "cInstruction.h"
#endif
#ifndef tDictionary_h
#include "tDictionary.h"
#endif
#ifndef cInstLibEntry_h
#include "cInstLibEntry.h"
#endif

class cString;


class cInstLib
{
protected:
  const int m_size;
  tDictionary<int> m_namemap;

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
  inline cString GetNearMatch(const cString& name) const { return m_namemap.NearMatch(name); }

  inline const cString& GetName(int entry) const { return Get(entry).GetName(); }
  
  virtual const cString& GetNopName(const unsigned int id) = 0;
  virtual int GetNopMod(const unsigned int id) = 0;
  virtual int GetNopMod(const cInstruction& inst) = 0;
  

  int GetInstDefault() const { return m_inst_default; }
  int GetInstNull() const { return m_inst_null; }

private:
    inline cInstruction GetInst(const cString& name);

};


inline int cInstLib::GetIndex(const cString& name) const
{
  int idx = -1;
  if (m_namemap.Find(name, idx)) return idx;
  return -1;
}

inline cInstruction cInstLib::GetInst(const cString& name)
{
  int idx;
  if (m_namemap.Find(name, idx)) return cInstruction(idx);
  return cInstruction(255);
}


#endif

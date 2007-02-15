/*
 *  tInstLib.h
 *  Avida
 *
 *  Called "tInstLib.hh" prior to 6/4/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef tInstLib_h
#define tInstLib_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef cInstLibBase_h
#include "cInstLibBase.h"
#endif

class cInstruction;

template <class MethodType> class tInstLib : public cInstLibBase
{
private:
  const size_t m_nopmods_array_size;
  const size_t m_function_array_size;
  cString* m_nopmod_names;
  cString* m_function_names;
  const int* m_nopmods;
  MethodType* m_functions;
  const cInstruction inst_error;
  const cInstruction inst_default;


public:
  tInstLib(size_t nopmod_array_size, size_t function_array_size, cString* nopmod_names,
           cString* function_names, const int* nopmods, MethodType* functions,
           const cInstruction& error, const cInstruction& def)
  : m_nopmods_array_size(nopmod_array_size),
    m_function_array_size(function_array_size),
    m_nopmod_names(nopmod_names),
    m_function_names(function_names),
    m_nopmods(nopmods),
    m_functions(functions),
    inst_error(error),
    inst_default(def)
  {
  }
  
  MethodType* GetFunctions(void){ return m_functions; } 
  
  const cString& GetName(const unsigned int id)
  {
    assert(id < m_function_array_size);
    return m_function_names[id];
  }
  const cString& GetNopName(const unsigned int id)
  {
    assert(id < m_nopmods_array_size);
    return m_nopmod_names[id];
  }  
  int GetNopMod(const unsigned int id)
  {
    assert(id < m_nopmods_array_size);
    return m_nopmods[id];
  }
  int GetNopMod(const cInstruction& inst)
  {
    return GetNopMod(inst.GetOp());
  }
  
  int GetSize() { return m_function_array_size; }
  int GetNumNops() { return m_nopmods_array_size; }
  
  cInstruction GetInst(const cString& in_name)
  {
    for (unsigned int i = 0; i < m_function_array_size; i++) {
      if (m_function_names[i] == in_name) return cInstruction(i);
    }
    return tInstLib::GetInstError();
  }
  const cInstruction GetInstDefault() { return inst_default; }
  const cInstruction GetInstError() { return inst_error; }
};

#endif

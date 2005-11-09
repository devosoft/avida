/*
 *  tInstLib.h
 *  Avida
 *
 *  Created by David on 6/4/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef TINSTLIB_H
#define TINSTLIB_H

#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef INST_LIB_BASE_HH
#include "cInstLibBase.h"
#endif

class cInstruction;

template <class MethodType> class tInstLib : public cInstLibBase {
  const size_t m_nopmods_array_size;
  const size_t m_function_array_size;
  cString* m_nopmod_names;
  cString* m_function_names;
  const int* m_nopmods;
  MethodType* m_functions;
  const cInstruction inst_error;
  const cInstruction inst_default;
public:
		tInstLib(
						 size_t nopmod_array_size,
						 size_t function_array_size,
						 cString* nopmod_names,
						 cString* function_names,
						 const int* nopmods,
						 MethodType* functions,
						 const cInstruction& error,
						 const cInstruction& def
						 ):m_nopmods_array_size(nopmod_array_size),
    m_function_array_size(function_array_size),
    m_nopmod_names(nopmod_names),
    m_function_names(function_names),
    m_nopmods(nopmods),
    m_functions(functions),
		inst_error(error),
		inst_default(def)
  {}
  MethodType* GetFunctions(void){ return m_functions; } 
  const cString &GetName(const unsigned int id) {
    assert(id < m_function_array_size);
    return m_function_names[id];
  }
  const cString &GetNopName(const unsigned int id) {
    assert(id < m_nopmods_array_size);
    return m_nopmod_names[id];
  }
  int GetNopMod(const unsigned int id){
    assert(id < m_nopmods_array_size);
    return m_nopmods[id];
  }
  int GetNopMod(const cInstruction & inst){
    return GetNopMod(inst.GetOp());
  }
  int GetSize(){ return m_function_array_size; }
  int GetNumNops(){ return m_nopmods_array_size; }
  cInstruction GetInst(const cString & in_name){
    for (unsigned int i = 0; i < m_function_array_size; i++) {
      if (m_function_names[i] == in_name) return cInstruction(i);
    }
    return tInstLib::GetInstError();
  }
  const cInstruction & GetInstDefault(){ return inst_default; }
  const cInstruction & GetInstError(){ return inst_error; }
};

#endif
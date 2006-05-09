/*
 *  cInstLibCPU.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cInstLibCPU_h
#define cInstLibCPU_h

#ifndef cInstLibBase_h
#include "cInstLibBase.h"
#endif
#ifndef cHardwareCPU_h
#include "cHardwareCPU.h"
#endif
#ifndef cInstruction_h
#include "cInstruction.h"
#endif

class cString;

class cInstLibCPU : public cInstLibBase
{
private:
  const size_t m_nopmods_array_size;
  const size_t m_function_array_size;
  cString *m_nopmod_names;
  cString *m_function_names;
  const int *m_nopmods;
  cHardwareCPU::tHardwareCPUMethod *m_functions;
  static const cInstruction inst_error;
  static const cInstruction inst_default;

protected:
  friend class cHardwareCPU;
  
  cInstLibCPU(
    size_t nopmod_array_size,
    size_t function_array_size,
    cString *nopmod_names,
    cString *function_names,
    const int *nopmods,
    cHardwareCPU::tHardwareCPUMethod *functions
  ):m_nopmods_array_size(nopmod_array_size),
    m_function_array_size(function_array_size),
    m_nopmod_names(nopmod_names),
    m_function_names(function_names),
    m_nopmods(nopmods),
    m_functions(functions)
  {}
  cHardwareCPU::tHardwareCPUMethod *GetFunctions(void){ return m_functions; } 

public:
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
    return cInstLibCPU::GetInstError();
  }
  const cInstruction & GetInstDefault(){ return inst_default; }
  const cInstruction & GetInstError(){ return inst_error; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nInstLibCPU {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  


#endif

//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INST_LIB_BASE_HH
#define INST_LIB_BASE_HH

#ifndef INSTRUCTION_HH
#include "instruction.hh"
#endif

class cString;
class cInstLibBase {
public:
  virtual ~cInstLibBase() { ; }
  virtual const cString &GetName(const unsigned int id) = 0;
  virtual const cString &GetNopName(const unsigned int id) = 0;
  virtual int GetNopMod(const unsigned int id) = 0;
  virtual int GetNopMod(const cInstruction & inst) = 0;
  virtual int GetSize() = 0;
  virtual int GetNumNops() = 0;
  virtual cInstruction GetInst(const cString & in_name) = 0;
  virtual const cInstruction & GetInstDefault() = 0;
  virtual const cInstruction & GetInstError() = 0;
};

#endif

/*
 *  cGenomeUtil.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cInstLibBase_h
#define cInstLibBase_h

#ifndef cInstruction_h
#include "cInstruction.h"
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

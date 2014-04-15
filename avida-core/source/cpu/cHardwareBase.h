/*
 *  cHardwareBase.h
 *  Avida
 *
 *  Called "hardware_base.hh" prior to 11/17/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#ifndef cHardwareBase_h
#define cHardwareBase_h

#include "avida/core/InstructionSequence.h"
#include "avida/output/Types.h"
#include "avida/systematics/Types.h"


class cHardwareBase
{

  int Divide_DoMutations(cAvidaContext& ctx, double mut_multiplier = 1.0, const int maxmut = INT_MAX);
  bool Divide_TestFitnessMeasures(cAvidaContext& ctx);
  
  
  // --------  Mutation  --------
  virtual int PointMutate(cAvidaContext& ctx, double override_mut_rate = 0.0);

  
  // --------  Implicit Repro Check/Instruction  -------- @JEB
  inline void CheckImplicitRepro(cAvidaContext& ctx, bool exec_last_inst = false)
    { if (m_implicit_repro_active) checkImplicitRepro(ctx, exec_last_inst); }
  virtual bool Inst_Repro(cAvidaContext& ctx);

  
  // --------  Mutation Helper Methods  --------
  bool doUniformMutation(cAvidaContext& ctx, InstructionSequence& genome);
  void doUniformCopyMutation(cAvidaContext& ctx, cHeadCPU& head);
  void doSlipMutation(cAvidaContext& ctx, InstructionSequence& genome, int from = -1);
  void doTransMutation(cAvidaContext& ctx, InstructionSequence& genome, int from = -1);
  

  // --------  Organism Execution Property Calculation  --------
  virtual int calcExecutedSize(const int parent_size);
  virtual int calcCopiedSize(const int parent_size, const int child_size) = 0;  
  
  
  // --------  Division Support Methods  --------
  bool Divide_CheckViable(cAvidaContext& ctx, const int parent_size, const int child_size, bool using_repro = false);
  unsigned Divide_DoExactMutations(cAvidaContext& ctx, double mut_multiplier = 1.0, const int pointmut = INT_MAX);
  bool Divide_TestFitnessMeasures1(cAvidaContext& ctx);
  
};


#endif

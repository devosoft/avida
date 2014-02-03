/*
 *  cOrgInterface.h
 *  Avida
 *
 *  Created by David on 3/4/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

// This class is responsible for establishing the interface that links an
// organism back to its population.  It is a base class that is derived in:
//
//   cpu/cTestCPUInterface.{h,cc}      - Test CPU interface
//   main/cPopulationInterface.{h,cc}  - Main population interface.
//
// Make sure that any new function you implment here also has versions in
// those classes.

#ifndef cOrgInterface_h
#define cOrgInterface_h

#include "avida/systematics/Types.h"

namespace Avida {
  class Genome;
  class InstructionSequence;
};

class cAvidaContext;
class cOrganism;
class cOrgSinkMessage;
class cPopulationCell;
class cString;

using namespace Avida;


class cOrgInterface
{
public:
  cOrgInterface() { ; }
  virtual ~cOrgInterface() { ; }

  virtual bool Divide(cAvidaContext& ctx, cOrganism* parent, const Genome& offspring_genome) = 0;
  
  virtual void KillOrganism(cAvidaContext& ctx, int cell_id) = 0;
  
 
  virtual int GetInputAt(int& input_pointer) = 0;
  virtual void ResetInputs(cAvidaContext& ctx) = 0;
  virtual const Apto::Array<int>& GetInputs() const = 0;

  virtual void Die(cAvidaContext& ctx) = 0;
  virtual bool UpdateMerit(double new_merit) = 0;

  virtual bool Move(cAvidaContext& ctx, int src_id, int dest_id) = 0;

};

#endif

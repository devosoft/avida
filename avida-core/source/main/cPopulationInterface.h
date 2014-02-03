/*
 *  cPopulationInterface.h
 *  Avida
 *
 *  Called "pop_interface.hh" prior to 12/5/05.
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

#ifndef cPopulationInterface_h
#define cPopulationInterface_h

#include "avida/core/UniverseDriver.h"
#include "avida/systematics/Unit.h"

#include "cOrgInterface.h"
#include "cWorld.h"
#include "cPopulationCell.h"

class cAvidaContext;
class cPopulation;
class cOrgMessage;
class cOrganism;

using namespace Avida;


class cPopulationInterface : public cOrgInterface
{
private:
  cWorld* m_world;
  int m_cell_id;
  
public:
  cPopulationInterface(cWorld* world);
  virtual ~cPopulationInterface();

  bool Divide(cAvidaContext& ctx, cOrganism* parent, const Genome& offspring_genome);

  int GetInputAt(int& input_pointer);
  void ResetInputs(cAvidaContext& ctx);
  const Apto::Array<int>& GetInputs() const;

  void Die(cAvidaContext& ctx);

  bool UpdateMerit(double new_merit);

  void KillOrganism(cAvidaContext& ctx, int cell_id);
};

#endif

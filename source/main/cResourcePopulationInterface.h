/*
 *  cResourcePopulationInterface.h
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

#ifndef cResourcePopulationInterface_h
#define cResourcePopulationInterface_h

#include "avida/core/Types.h"

class cAvidaContext;
class cDeme;
class cPopulationCell;

class cResourcePopulationInterface
{
public:
  inline cResourcePopulationInterface() { ; }
  virtual ~cResourcePopulationInterface();

  virtual cPopulationCell& GetCell(int cell_id) = 0;
  virtual cDeme& GetDeme(int deme_id) = 0;
  virtual int GetNumDemes() const = 0;
//  virtual const Apto::Array<double>& GetDemeCellResources(int deme_id, int cell_id, cAvidaContext& ctx) const = 0;
  virtual void KillOrganism(cAvidaContext& ctx, int cell_id) = 0;
};

#endif

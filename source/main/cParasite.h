/*
 *  cParasite.h
 *  Avida
 *
 *  Created by David on 12/16/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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

#ifndef cParasite_h
#define cParasite_h

#include "avida/core/Genome.h"
#include "avida/systematics/Unit.h"

#include "cPhenotype.h"

class cWorld;

using namespace Avida;


class cParasite : public Systematics::Unit
{
private:
  Systematics::Source m_src;
  Apto::String m_src_args;
  const Avida::Genome m_initial_genome;
  
  HashPropertyMap m_prop_map;
  
  cPhenotype m_phenotype;
  double virulence;
  
  
public:
  cParasite(cWorld* world, const Avida::Genome& genome, int parent_generation, Systematics::Source src);
  ~cParasite() { ; }
  
  // --------  Systematics::Unit Methods  --------
  Systematics::Source UnitSource() const { return m_src; }
  const Avida::Genome& UnitGenome() const { return m_initial_genome; }  
  
  const PropertyMap& Properties() const { return m_prop_map; }

  // --------  cParasite Methods  --------
  cPhenotype& GetPhenotype() { return m_phenotype; }
  double GetVirulence() { return virulence; }
  void SetVirulence(double v) { virulence = v; }

private:
  cParasite(); // @not_implemented
  cParasite(const cParasite&); // @not_implemented
  cParasite& operator=(const cParasite&); // @not_implemented
};

#endif

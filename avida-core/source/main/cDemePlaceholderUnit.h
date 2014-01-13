/*
 *  cDemePlaceholderUnit.h
 *  Avida
 *
 *  Created by David on 8/9/10.
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
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

#ifndef cDemePlaceholderUnit_h
#define cDemePlaceholderUnit_h

#include "avida/core/Genome.h"
#include "avida/systematics/Unit.h"

#include "cPhenotype.h"
#include "cString.h"

using namespace Avida;


class cDemePlaceholderUnit : public Systematics::Unit
{
private:
  Systematics::Source m_src;
  cString m_src_args;
  Genome m_genome;
  cPhenotype m_phenotype;
  HashPropertyMap m_prop_map;
  
  
public:
  cDemePlaceholderUnit(Systematics::Source src, const Genome& mg) : m_src(src), m_src_args(""), m_genome(mg) { ; }
  ~cDemePlaceholderUnit() { ; }
  
  Systematics::Source UnitSource() const { return m_src; }
  const Genome& UnitGenome() const { return m_genome; }
  
  const PropertyMap& Properties() const { return m_prop_map; }
  
  const cPhenotype& GetPhenotype() const { return m_phenotype; }
};

#endif

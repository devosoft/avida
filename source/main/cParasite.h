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

#ifndef cPhenotype_h
#include "cPhenotype.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cWorld;

using namespace Avida;


class cParasite : public Systematics::Unit
{
private:
  eBioUnitSource m_src;
  cString m_src_args;
  const class Genome m_initial_genome;
  cPhenotype m_phenotype;
  double virulence;
  
  
  cParasite(); // @not_implemented
  cParasite(const cParasite&); // @not_implemented
  cParasite& operator=(const cParasite&); // @not_implemented
  

public:
  cParasite(cWorld* world, const class Genome& genome, int parent_generation, eBioUnitSource src, const cString& src_args);
  ~cParasite() { ; }
  
  // --------  cBioUnit Methods  --------
  eBioUnitSource GetUnitSource() const { return m_src; }
  const cString& GetUnitSourceArgs() const { return m_src_args; }
  const class Genome& GetGenome() const { return m_initial_genome; }  
  const cPhenotype& GetPhenotype() const { return m_phenotype; }

  // --------  cParasite Methods  --------
  cPhenotype& GetPhenotype() { return m_phenotype; }
  double GetVirulence() { return virulence; }
  void SetVirulence(double v) { virulence = v; }

};

#endif

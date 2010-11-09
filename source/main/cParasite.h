/*
 *  cParasite.h
 *  Avida
 *
 *  Created by David on 12/16/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cParasite_h
#define cParasite_h

#ifndef cBioUnit_h
#include "cBioUnit.h"
#endif
#ifndef cGenome_h
#include "cGenome.h"
#endif
#ifndef cPhenotype_h
#include "cPhenotype.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cWorld;


class cParasite : public cBioUnit
{
private:
  eBioUnitSource m_src;
  cString m_src_args;
  const cGenome m_initial_genome;
  cPhenotype m_phenotype;
  
  
  cParasite(); // @not_implemented
  cParasite(const cParasite&); // @not_implemented
  cParasite& operator=(const cParasite&); // @not_implemented
  

public:
  cParasite(cWorld* world, const cGenome& genome, int parent_generation, eBioUnitSource src, const cString& src_args);
  ~cParasite() { ; }
  
  // --------  cBioUnit Methods  --------
  eBioUnitSource GetUnitSource() const { return m_src; }
  const cString& GetUnitSourceArgs() const { return m_src_args; }
  const cGenome& GetGenome() const { return m_initial_genome; }  
  const cPhenotype& GetPhenotype() const { return m_phenotype; }

  // --------  cParasite Methods  --------
  cPhenotype& GetPhenotype() { return m_phenotype; }

};

#endif

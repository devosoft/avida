/*
 *  cDemePlaceholderUnit.h
 *  Avida
 *
 *  Created by David on 8/9/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#ifndef cDemePlaceholderUnit_h
#define cDemePlaceholderUnit_h

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


class cDemePlaceholderUnit : public cBioUnit
{
private:
  eBioUnitSource m_src;
  cString m_src_args;
  cGenome m_genome;
  cPhenotype m_phenotype;
  
public:
  cDemePlaceholderUnit(eBioUnitSource src, const cGenome& mg) : m_src(src), m_src_args(""), m_genome(mg) { ; }
  ~cDemePlaceholderUnit() { ; }
  
  eBioUnitSource GetUnitSource() const { return m_src; }
  const cString& GetUnitSourceArgs() const { return m_src_args; }
  const cGenome& GetGenome() const { return m_genome; }
  const cPhenotype& GetPhenotype() const { return m_phenotype; }
};

#endif

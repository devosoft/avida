/*
 *  cBioUnit.h
 *  Avida
 *
 *  Created by David on 10/7/09.
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

#ifndef cBioUnit_h
#define cBioUnit_h

#include "Avida.h"

#include "tArray.h"

namespace Avida {
  class cGenome;
};

class cBioGroup;
class cPhenotype;
class cString;

using namespace Avida;


class cBioUnit
{
protected:
  tArray<cBioGroup*> m_bio_groups;

  
public:
  cBioUnit() { ; }
  virtual ~cBioUnit();
  
  virtual eBioUnitSource GetUnitSource() const = 0;
  virtual const cString& GetUnitSourceArgs() const = 0;
  virtual const cGenome& GetGenome() const = 0;
  virtual const cPhenotype& GetPhenotype() const = 0;
  virtual bool	IsParasite();
  
  const tArray<cBioGroup*>& GetBioGroups() const { return m_bio_groups; }
  cBioGroup* GetBioGroup(const cString& role) const;
  
  void AddClassification(cBioGroup* bg) { m_bio_groups.Push(bg); }
  void SelfClassify(const tArray<const tArray<cBioGroup*>*>& parents);
  
  void HandleGestation();
};

#endif

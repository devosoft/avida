/*
 *  cBioUnit.h
 *  Avida
 *
 *  Created by David on 10/7/09.
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

#ifndef cBioUnit_h
#define cBioUnit_h

#include "Avida.h"

#include "tArray.h"

class cBioGroup;
class cGenome;
class cPhenotype;
class cString;


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

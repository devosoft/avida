/*
 *  cBioGroup.h
 *  Avida
 *
 *  Created by David on 10/7/09.
 *  Copyright 2009 Michigan State University. All rights reserved.
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

#ifndef cBioGroup_h
#define cBioGroup_h

#ifndef defs_h
#include "defs.h"
#endif
#ifndef cBioUnit_h
#include "cBioUnit.h"
#endif

template<typename T> class tArray;

class cBioGroup
{
protected:
  int m_refs;
  
public:
  cBioGroup() : m_refs(0) { ; }
  virtual ~cBioGroup() = 0;
  
  virtual cBioGroup* ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents = NULL) = 0;
  virtual void RemoveBioUnit(cBioUnit* bu) = 0;
  
  virtual int GetDepth() const = 0;
  
  int GetReferenceCount() const { return m_refs; }
  void AddReference() { m_refs++; }
  void RemoveReference() { m_refs--; }
};

#endif

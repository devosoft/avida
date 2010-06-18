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

#include <typeinfo>

#ifndef defs_h
#include "defs.h"
#endif
#ifndef cBioGroupData_h
#include "cBioGroupData.h"
#endif
#ifndef cBioUnit_h
#include "cBioUnit.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tArrayMap_h
#include "tArrayMap.h"
#endif

class cDataFile;


class cBioGroup
{
protected:
  int m_a_refs;
  int m_p_refs;
  tArrayMap<cString, cBioGroupData*> m_data;
  
  
public:
  cBioGroup() : m_a_refs(0), m_p_refs(0) { ; }
  virtual ~cBioGroup() = 0;
  
  virtual int GetRoleID() const = 0;
  virtual const cString& GetRole() const = 0;
  virtual int GetID() const = 0;
  
  virtual cBioGroup* ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents = NULL) = 0;
  virtual void HandleBioUnitGestation(cBioUnit* bu) = 0;
  virtual void RemoveBioUnit(cBioUnit* bu) = 0;
  
  virtual int GetDepth() const = 0;
  
  virtual void Save(cDataFile& df) = 0;
  
  
  int GetReferenceCount() const { return m_a_refs + m_p_refs; }
  int GetActiveReferenceCount() const { return m_a_refs; }
  void AddActiveReference() { m_a_refs++; assert(m_a_refs >= 0); }
  void RemoveActiveReference() { m_a_refs--; assert(m_a_refs >= 0); }
  int GetPassiveReferenceCount() const { return m_p_refs; }
  void AddPassiveReference() { m_p_refs++; assert(m_p_refs >= 0); }
  void RemovePassiveReference() { m_p_refs--; assert(m_p_refs >= 0); }
  
  template<typename T> void AttachData(T* data)
  {
    delete m_data.GetWithDefault(cString(typeid(data).name()), NULL);
    m_data.Set(typeid(data).name(), (cBioGroupData*)new tBioGroupData<T>(data));
  }
  
  template<typename T> T* GetData() {
    tBioGroupData<T>* data = (tBioGroupData<T>*)m_data.GetWithDefault(cString(typeid(T).name()), NULL);
    return (data) ? data->GetData() : NULL;
  }
};

#endif

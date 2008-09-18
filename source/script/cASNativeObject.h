/*
 *  cASNativeObject.h
 *  Avida
 *
 *  Created by David Bryson on 9/14/08.
 *  Copyright 2008 Michigan State University. All rights reserved.
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

#ifndef cASNativeObject_h
#define cASNativeObject_h

#include "AvidaScript.h"

#include "cASCPPParameter.h"

class cString;

class cASNativeObject 
{
private:
  int m_ref_count;
  
public:
  cASNativeObject() : m_ref_count(1) { ; }
  virtual ~cASNativeObject() { ; }
  
  virtual const char* GetType() = 0;
  
  virtual bool CallMethod(int mid, int argc, cASCPPParameter args[]) = 0;
  
  int LookupValue(const cString& val_name) { return AS_NOT_FOUND; } // @TODO
  int LookupMethod(const cString& meth_name) { return AS_NOT_FOUND; } // @TODO
  
  inline cASNativeObject* GetReference() { m_ref_count++; return this; }
  inline void RemoveReference() { m_ref_count--; if (m_ref_count == 0) delete this; }
  inline bool IsShared() { return (m_ref_count > 1); }
};


template<class NativeClass, const char* TypeName>
class tASNativeObject : public cASNativeObject
{
private:
  NativeClass* m_object;
  
public:
  tASNativeObject(NativeClass* obj) : m_object(obj) { ; }
  ~tASNativeObject() { delete m_object; }

  const char* GetType() { return TypeName; }
  
  bool CallMethod(int mid, int argc, cASCPPParameter args[]) { return false; } // @TODO;  
};
  

#endif

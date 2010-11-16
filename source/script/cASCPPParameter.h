/*
 *  cASCPPParameter.h
 *  Avida
 *
 *  Created by David Bryson on 9/14/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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

#ifndef cASCPPParameter_h
#define cASCPPParameter_h

#include "Avida.h"
#include "AvidaScript.h"

#include "cString.h"

template<class NativeClass> class tASNativeObject;


class cASCPPParameter
{
private:
  union {
    bool m_bool;
    char m_char;
    int m_int;
    double m_float;
    cString* m_string;
    cASNativeObject* m_nobj;
  };
  
public:
  cASCPPParameter() { ; }
  
  void Set(bool val) { m_bool = val; }
  void Set(char val) { m_char = val; }
  void Set(int val) { m_int = val; }
  void Set(double val) { m_float = val; }
  void Set(cString* val) { m_string = val; }
  void Set(const cString& val) { m_string = new cString(val); }
  void Set(cASNativeObject* val) { m_nobj = val; }
  template<class NativeClass> void Set(tASNativeObject<NativeClass>* val) { m_nobj = val; }
  template<class NativeClass> void Set(NativeClass* val) { m_nobj = new tASNativeObject<NativeClass>(val); }
  
  template<typename T> inline T Get() const;
  
};


template<> inline bool cASCPPParameter::Get<bool>() const { return m_bool; }
template<> inline char cASCPPParameter::Get<char>() const { return m_char; }
template<> inline int cASCPPParameter::Get<int>() const { return m_int; }
template<> inline double cASCPPParameter::Get<double>() const { return m_float; }
template<> inline const cString& cASCPPParameter::Get<const cString&>() const { return *m_string; }
template<> inline const cString* cASCPPParameter::Get<const cString*>() const { return m_string; }
template<> inline cString* cASCPPParameter::Get<cString*>() const { return m_string; }
template<> inline cASNativeObject* cASCPPParameter::Get<cASNativeObject*>() const { return m_nobj; }

#endif

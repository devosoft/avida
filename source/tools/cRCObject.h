/*
 *  cRCObject.h
 *  Avida
 *
 *  Created by David on 11/12/08.
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

#ifndef cRCObject_h
#define cRCObject_h


class cRCObject
{
private:
  volatile int m_ref_count;
  volatile bool m_exclusive;
  
public:
  cRCObject() : m_ref_count(0), m_exclusive(false) { ; }
  cRCObject(const cRCObject&) : m_ref_count(0), m_exclusive(false) { ; }
  virtual ~cRCObject() = 0;
  
  cRCObject& operator=(const cRCObject&) { return *this; }
  
  void AddReference() { m_ref_count++; }
  void RemoveReference() { if (--m_ref_count == 0) delete this; }
  
  bool SetExclusive() { if (!(m_ref_count > 1)) m_exclusive = true; return m_exclusive; }
  bool IsExclusive() { return m_exclusive; }
};

#endif

/*
 *  tAutoRelease.h
 *  Avida
 *
 *  Created by David on 7/18/07.
 *  Copyright 2007-2010 Michigan State University. All rights reserved.
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

#ifndef tAutoRelease_h
#define tAutoRelease_h

//! A lightweight pointer wrapper class that automatically deletes the contents on destruction.
template <class T> class tAutoRelease
{
private:
  T* m_value;
  
  tAutoRelease(const tAutoRelease&); // @not_implemented
  tAutoRelease& operator=(const tAutoRelease&); // @not_implemented

  
public:
  explicit inline tAutoRelease() : m_value(NULL) { ; }
  explicit inline tAutoRelease(T* value) : m_value(value) { ; }
  inline ~tAutoRelease() { delete m_value; }
  
  inline void Set(T* value) { delete m_value; m_value = value; }
  inline tAutoRelease<T>& operator=(T* value) { delete m_value; m_value = value; return *this; }
  
  inline bool IsNull() const { return !(m_value); }
  
  //! Access the contents
  inline T* operator->() const { return m_value; }
  inline T& operator*() const { return *m_value; }
  
  //! Take control of the contents
  inline T* Release() { T* value = m_value; m_value = NULL; return value; }
};


#endif

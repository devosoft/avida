/*
 *  tRCPtr.h
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

#ifndef tRCPtr_h
#define tRCPtr_h

template<class T> class tRCPtr
{
private:
  T* m_ptr;
  
  void handleNewPointer();

public:
  tRCPtr(T* ptr = NULL) : m_ptr(ptr) { handleNewPointer(); }
  tRCPtr(const tRCPtr& rhs) : m_ptr(rhs.m_ptr) { handleNewPointer(); }
  ~tRCPtr() { if (m_ptr) m_ptr->RemoveReference(); }
  
  tRCPtr& operator=(const tRCPtr& rhs);
  
  inline bool operator!() const { return !m_ptr; }
  inline operator bool() const { return !operator!(); }
  
  inline T* operator->() const { return m_ptr; }
  inline T& operator*() const { return *m_ptr; }
  
  template<class S> inline S* operator->() const { return m_ptr; }
  template<class S> inline S& operator*() const { return *m_ptr; }
  
  template<class S> operator tRCPtr<S>() { return tRCPtr<S>(m_ptr); }
};

template<class T> inline void tRCPtr<T>::handleNewPointer()
{
  if (!m_ptr) return;
  
  if (m_ptr->IsExclusive()) m_ptr = new T(*m_ptr);
  
  m_ptr->AddReference();
}

template<class T> tRCPtr<T>& tRCPtr<T>::operator=(const tRCPtr& rhs)
{
  if (m_ptr != rhs.m_ptr) {
    T* old_ptr = m_ptr;
    m_ptr = rhs.m_ptr;
    handleNewPointer();
    if (old_ptr) old_ptr->RemoveReference();
  }
  
  return *this;
}

#endif

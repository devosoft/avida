/*
 *  tRLockPtr.h
 *  Avida
 *
 *  Created by David on 1/12/09.
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

#ifndef tRLockPtr_h
#define tRLockPtr_h

#ifndef cRWLock_h
#include "cRWLock.h"
#endif


template<class T> class tRLockPtr
{
private:
  T* m_ptr;
  cRWLock* m_rwlock;
  
  
public:
  tRLockPtr(T* ptr, cRWLock* rwlock) : m_ptr(ptr), m_rwlock(rwlock) { m_rwlock->ReadLock(); }
  tRLockPtr(const tRLockPtr& p) : m_ptr(p.m_ptr), m_rwlock(p.m_rwlock) { m_rwlock->ReadLock(); }
  ~tRLockPtr() { m_rwlock->Unlock(); }
  
  tRLockPtr& operator=(const tRLockPtr& rhs);

  inline bool operator!() const { return !m_ptr; }
  inline operator bool() const { return !operator!(); }
  
  inline T* operator->() const { return m_ptr; }
  inline T& operator*() const { return *m_ptr; }
  
  template<class S> inline S* operator->() const { return m_ptr; }
  template<class S> inline S& operator->() const { return *m_ptr; }
  
  template<class S> operator tRLockPtr<S>() { return tRLockPtr<S>(m_ptr, m_rwlock); }
};

template<class T> tRLockPtr<T>& tRLockPtr<T>::operator=(const tRLockPtr& rhs)
{
  if (m_ptr != rhs.m_ptr) {
    m_rwlock->Unlock();
    m_ptr = rhs.m_ptr;
    m_rwlock = rhs.m_rwlock;
    m_rwlock->ReadLock();
  }
  
  return *this;
}


#endif

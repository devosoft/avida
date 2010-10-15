/*
 *  tThreadSpecific.h
 *  Avida
 *
 *  Created by David on 10/14/08.
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

#ifndef tThreadSpecific_h
#define tThreadSpecific_h


#if AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(UNIX)

// Use POSIX Threads
#include <pthread.h>

template<typename T>
class tThreadSpecific
{
private:
  pthread_key_t m_key;
  static void destroySpecific(void* data) { delete (T*)data; }
  
public:
  tThreadSpecific() { pthread_key_create(&m_key, &destroySpecific); }
  ~tThreadSpecific() { destroySpecific(pthread_getspecific(m_key)); pthread_key_delete(m_key); }
  
  T* Get() { return (T*)pthread_getspecific(m_key); }
  void Set(T* value) { destroySpecific(pthread_getspecific(m_key)); pthread_setspecific(m_key, (void*)value); }
};


#elif AVIDA_PLATFORM(THREADS) && AVIDA_PLATFORM(WINDOWS)

// Use Windows Threading
#include <windows.h>

#error Windows tThreadSpecific not currently implemented

#else

// Disable Threading
template<typename T>
class tThreadSpecific
{
private:
  T* m_value;
  
public:
  tThreadSpecific() : m_value(NULL) { ; }
  ~tThreadSpecific() { delete m_value; }
  
  T* Get() { return m_value; }
  //T* Set(T* value) { T* oldvalue = m_value; m_value = value; return oldvalue; }
  void Set(T* value) { delete m_value; m_value = value; }
};

#endif


#endif

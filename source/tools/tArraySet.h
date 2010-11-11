/*
 *  tArraySet.h
 *  Avida
 *
 *  Created by David on 11/18/09.
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

#ifndef tArraySet_h
#define tArraySet_h

#include <cassert>

template <class T> class tArraySet
{
private:
  T* m_data;  // Data Elements
  int m_size; // Number of Elements

public:
  typedef T* iterator; //!< STL-compatible iterator.
  typedef const T* const_iterator; //!< STL-compatible const_iterator.
  
  tArraySet() : m_data(NULL), m_size(0) { ; }
	tArraySet(const tArraySet& rhs) : m_data(NULL), m_size(0) { this->operator=(rhs); }
  
  ~tArraySet() { delete [] m_data; }
  
  inline iterator begin() { return m_data; }
  inline iterator end() { return m_data + m_size; }
  inline const_iterator begin() const { return m_data; }
  inline const_iterator end() const { return m_data + m_size; }  
  
  inline tArraySet& operator=(const tArraySet& rhs) {
    if (m_size != rhs.GetSize())  ResizeClear(rhs.GetSize());
    for(int i = 0; i < m_size; i++) m_data[i] = rhs[i];
    return *this;
  }
  
  inline tArraySet operator+(const tArraySet& in_array) const {
    tArraySet tmp;
    
    if (m_size >= in_array.m_size) {
      for(int i = 0; i < m_size; i++) tmp.m_data[i] = m_data[i];
      for(int i = 0; i < in_array.m_size; i++) tmp.Add(in_array[i]);
    } else {
      for(int i = 0; i < in_array.m_size; i++) tmp[i] = in_array[i];
      for(int i = 0; i < m_size; i++) tmp.Add(m_data[i]);      
    }
    
    return tmp;
  }
  
  inline int GetSize() const { return m_size; }
  
  
  inline const T& operator[](const int index) const
  {
    assert(index >= 0);     // Lower Bounds Error
    assert(index < m_size); // Upper Bounds Error
    return m_data[index];
  }    
  
  inline void Add(const T& value)
  {
    for (int i = 0; i < m_size; i++) if (m_data[i] == value) return;
    int osize = m_size++;
    T* new_data = new T[m_size];
    for (int i = 0; i < osize; i++) new_data[i] = m_data[i];
    new_data[osize] = value;
    delete [] m_data;
    m_data = new_data;
  }
  
  inline bool Has(const T& value) const
  {
    for (int i = 0; i < m_size; i++) if (m_data[i] == value) return true;    
    return false;
  }
  
  inline void Remove(const T& value)
  {
    int found = -1;
    for (int i = 0; i < m_size; i++) {
      if (m_data[i] == value) {
        found = i;
        break;
      }
    }
    
    if (found > -1) {
      int osize = m_size--;
      T* new_data = new T[m_size];
      for (int i = 0; i < found; i++) new_data[i] = m_data[i];
      for (int i = found + 1; i < osize; i++) new_data[i - 1] = m_data[i];
      delete [] m_data;
      m_data = new_data;
    }
  }
};  

#endif

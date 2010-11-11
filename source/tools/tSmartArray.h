/*
 *  tSmartArray.h
 *  Avida
 *
 *  Created by David on 3/26/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef tSmartArray_h
#define tSmartArray_h

#include "tArray.h"

#include <cassert>

// "I am so smart..."
static const int SMRT_INCREASE_MINIMUM = 10;
static const int SMRT_INCREASE_FACTOR = 2;
static const int SMRT_SHRINK_TEST_FACTOR = 4;

template <class T> class tSmartArray
{
private:
  
  T* m_data;    // Data Array
  int m_size;   // Raw Array Size
  int m_active; // Active Size
  int m_reserve;
  
public:
  explicit tSmartArray(int size = 0, int reserve = 0)
    : m_data(NULL), m_size(0), m_active(0), m_reserve(reserve) { ResizeClear(size); }
  tSmartArray(const tSmartArray& rhs) : m_data(NULL), m_size(0), m_active(0), m_reserve(0) { this->operator=(rhs); }
  tSmartArray(const tArray<T>& rhs) : m_data(NULL), m_size(0), m_active(0), m_reserve(0) { this->operator=(rhs); }

  ~tSmartArray() { delete [] m_data; }
  
  tSmartArray& operator=(const tSmartArray& rhs)
  {
    if (m_active != rhs.GetSize()) Resize(rhs.GetSize());
    for(int i = 0; i < m_active; i++) m_data[i] = rhs[i];
    return *this;
  }
  tSmartArray& operator=(const tArray<T>& rhs)
  {
    if (m_active != rhs.GetSize()) Resize(rhs.GetSize());
    for(int i = 0; i < m_active; i++) m_data[i] = rhs[i];
    return *this;
  }
  
  int GetReserve() const { return m_reserve; }
  void SetReserve(int reserve) { m_reserve = reserve; }
  
  int GetSize() const { return m_active; }
  
  void ResizeClear(const int in_size)
  {
    assert(m_size >= 0);

    m_active = in_size;
    m_size = (in_size >= m_reserve) ? in_size : m_reserve;    
    
    if (m_data != NULL) delete [] m_data;
    
    if (m_size > 0) {
      m_data = new T[m_size];   // Allocate block for data
      assert(m_data != NULL); // Memory allocation error: Out of Memory?
    }
    else m_data = NULL;
  }

  void Resize(int new_size)
  {
    assert(new_size >= 0);
    
    // If we're already at the size we want, don't bother doing anything.
    if (new_size == m_active) return;
    
    // If new size is 0, clean up and go!
    if (new_size == 0) {
      if (m_data != NULL) delete [] m_data;
      m_data = NULL;
      m_size = 0;
      m_active = 0;
      return;
    }
    
    // Determine if we need to adjust the allocated array sizes...
    int shrink_test = new_size * SMRT_SHRINK_TEST_FACTOR;
    if (new_size > m_size || (shrink_test < m_size && shrink_test >= m_reserve)) {
      int new_array_size = new_size * SMRT_INCREASE_FACTOR;
      const int new_array_min = new_size + SMRT_INCREASE_MINIMUM;
      if (new_array_min > new_array_size) new_array_size = new_array_min;
      
      T* new_data = new T[new_array_size];
      assert(new_data != NULL); // Memory Allocation Error: Out of Memory?
      
      // Copy over old data...
      for (int i = 0; i < m_active && i < new_size; i++) {
        new_data[i] = m_data[i];
      }
      if (m_data != NULL) delete [] m_data;  // remove old data if exists
      m_data = new_data;
      
      m_size = new_array_size;
    }
    
    m_active = new_size;
  }
  
  void Resize(int new_size, const T& empty_value)
  {
    int old_size = m_active;
    Resize(new_size);
    for (int i = old_size; i < new_size; i++) m_data[i] = empty_value;
  }
  
  T& operator[](const int index)
  {
    assert(index >= 0);       // Lower Bounds Error
    assert(index < m_active); // Upper Bounds Error
    return m_data[index];
  }
  const T& operator[](const int index) const
  {
    assert(index >= 0);       // Lower Bounds Error
    assert(index < m_active); // Upper Bounds Error
    return m_data[index];
  }
  
  
  // Stack-like Methods...
  void Push(const T& value)
  {
    Resize(m_active + 1);
    m_data[m_active - 1] = value;
  }
  
  T Pop()
  {
    T value = m_data[m_active - 1];
    Resize(m_active - 1);
    return value;
  }
  
  
  void Swap(int idx1, int idx2)
  {
    assert(idx1 >= 0);     // Lower Bounds Error
    assert(idx1 < m_active); // Upper Bounds Error
    assert(idx2 >= 0);     // Lower Bounds Error
    assert(idx2 < m_active); // Upper Bounds Error
    
    T v = m_data[idx1];
    m_data[idx1] = m_data[idx2];
    m_data[idx2] = v;
  }  
  
  
  void SetAll(const T& value)
  {
    for (int i = 0; i < m_active; i++) m_data[i] = value;
  }
};

#endif

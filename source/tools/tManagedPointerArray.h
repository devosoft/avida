/*
 *  tManagedPointerArray.h
 *  Avida
 *
 *  Created by David on 12/7/05.
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

#ifndef tManagedPointerArray_h
#define tManagedPointerArray_h

#include "tArray.h"

#include <cassert>

template <class T> class tManagedPointerArray
{
private:
  T** m_data;  // Data Elements
  int m_size;  // Number of Elements
  
public:
  explicit tManagedPointerArray(const int _size = 0) : m_data(NULL), m_size(0) { ResizeClear(_size); }
  tManagedPointerArray(const tManagedPointerArray& rhs) : m_data(NULL), m_size(0) { this->operator=(rhs); }
  tManagedPointerArray(const tArray<T>& rhs) : m_data(NULL), m_size(0) { this->operator=(rhs); }

  ~tManagedPointerArray()
  { 
    for (int i = 0; i < m_size; i++) delete m_data[i];
    delete [] m_data;
  }
  
  tManagedPointerArray& operator=(const tManagedPointerArray& rhs)
  {
    if (m_size != rhs.GetSize()) Resize(rhs.GetSize());
    for(int i = 0; i < m_size; i++) *m_data[i] = rhs[i];
    return *this;
  }
  tManagedPointerArray& operator=(const tArray<T>& rhs)
  {
    if (m_size != rhs.GetSize()) Resize(rhs.GetSize());
    for(int i = 0; i < m_size; i++) *m_data[i] = rhs[i];
    return *this;
  }
  
  int GetSize() const { return m_size; }
  
  void ResizeClear(const int in_size)
  {
    for (int i = 0; i < m_size; i++) delete m_data[i];
    if (m_data != NULL) delete [] m_data;  // remove old data if exists
    
    m_size = in_size;
    assert(m_size >= 0);  // Invalid size specified for array intialization
    
    if (m_size > 0) {
      m_data = new T*[m_size];   // Allocate block for data
      assert(m_data != NULL); // Memory allocation error: Out of Memory?
      for (int i = 0; i < m_size; i++) m_data[i] = new T;
    }
    else m_data = NULL;
  }
  
  void Resize(int new_size)
  {
    assert(new_size >= 0);
    
    // If we're already at the size we want, don't bother doing anything.
    if (m_size == new_size) return;
    
    // If new size is 0, clean up and go!
    if (new_size == 0) {
      for (int i = 0; i < m_size; i++) delete m_data[i];
      if (m_data != NULL) delete [] m_data;
      m_data = NULL;
      m_size = 0;
      return;
    }
    
    T** new_data = new T*[new_size];
    assert(new_data != NULL); // Memory Allocation Error: Out of Memory?
    
    if (m_size < new_size) {
      // Fill out the new portion of the array, if needed
      for (int i = m_size; i < new_size; i++) new_data[i] = new T;
    } else if (new_size < m_size) {
      // Clean up old portion of the array, if needed
      for (int i = new_size; i < m_size; i++) delete m_data[i];
    }
    
    // Copy over old data...
    for (int i = 0; i < m_size && i < new_size; i++) {
      new_data[i] = m_data[i];
    }
    if (m_data != NULL) delete [] m_data;  // remove old data if exists
    m_data = new_data;
    
    m_size = new_size;
  }
  
  void Resize(int new_size, const T& empty_value)
  {
    int old_size = m_size;
    Resize(new_size);
    for (int i = old_size; i < new_size; i++) *m_data[i] = empty_value;
  }
  
  void Remove(const int index)
  {
    assert(m_size != 0 || index < m_size || index >= 0);

    int new_size = m_size - 1;
    
    // If new size is 0, clean up and go!
    if (new_size == 0) {
      for (int i = 0; i < m_size; i++) delete m_data[i];
      delete [] m_data;
      m_data = NULL;
      m_size = 0;
      return;
    }
    
    T** new_data = new T*[new_size];
    assert(new_data != NULL); // Memory Allocation Error: Out of Memory?
    
    delete m_data[index];
    
    // Copy over old data...
    for (int i = 0; i < index; i++) {
      new_data[i] = m_data[i];
    }
    for (int i = index + 1; i < m_size; i++) {
      new_data[i - 1] = m_data[i];
    }
    delete [] m_data;
    m_data = new_data;

    m_size = new_size;
  }
  
  T& operator[](const int index)
  {
    assert(index >= 0);    // Lower Bounds Error
    assert(index < m_size);  // Upper Bounds Error
    return *m_data[index];
  }
  const T& operator[](const int index) const
  {
    assert(index >= 0);    // Lower Bounds Error
    assert(index < m_size);  // Upper Bounds Error
    return *m_data[index];
  }
  
  void Push(const T& value)
  {
    Resize(m_size + 1);
    *m_data[m_size - 1] = value;
  }
  
  void Swap(int idx1, int idx2)
  {
    // Simple pointer swap, rather than deep copy
    
    assert(idx1 >= 0);     // Lower Bounds Error
    assert(idx1 < m_size); // Upper Bounds Error
    assert(idx2 >= 0);     // Lower Bounds Error
    assert(idx2 < m_size); // Upper Bounds Error
    
    T* v = m_data[idx1];
    m_data[idx1] = m_data[idx2];
    m_data[idx2] = v;
  }
  
  
  void SetAll(const T& value)
  {
    for (int i = 0; i < m_size; i++) *m_data[i] = value;
  }
};

#endif

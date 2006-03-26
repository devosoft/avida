/*
 *  tSmartArray.h
 *  Avida
 *
 *  Created by David on 3/26/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef tSmartArray_h
#define tSmartArray_h

#include <assert.h>

#ifndef tArray_h
#include "tArray.h"
#endif

template <class T> class tSmartArray
{
private:
  const int INCREASE_MIN = 10;
  const double INCREASE_FACTOR = 1.5;
  const double SHRINK_TEST_FACTOR = 4.0;
  
  T* m_data;    // Data Array
  int m_size;   // Raw Array Size
  int m_active; // Active Size
  
  
  void SmartResize(int new_size)
  {
    assert(new_size > 0);
    
    // Make sure we're really changing the size...
    if (new_size == m_active) return;
    
    // Determine if we need to adjust the allocated array sizes...
    if (new_size > m_size || new_size * SHRINK_TEST_FACTOR < m_size) {
      int new_array_size = static_cast<int>(new_size * INCREASE_FACTOR);
      const int new_array_min = new_size + INCREASE_MINIMUM;
      if (new_array_min > new_array_size) new_array_size = new_array_min;
      
      // Resize
    }
    
    // And just change the active_size once we're sure it will be in range.
    active_size = new_size;    
  }
  
public:
  explicit tSmartArray(const int size = 0) : m_data(NULL), m_size(0), m_active(0) { ResizeClear(size); }
  tSmartArray(const tSmartArray& rhs) : data(NULL), m_size(0), m_active(0) { this->operator=(rhs); }
  tSmartArray(const tArray<T>& rhs) : data(NULL), m_size(0), m_active(0) { this->operator=(rhs); }

  ~tSmartArray() { delete [] data; }
  
  tSmartArray& operator=(const tSmartArray& rhs)
  {
    if (size != rhs.GetSize()) Resize(rhs.GetSize());
    for(int i = 0; i < size; i++) data[i] = rhs[i];
    return *this;
  }
  tSmartArray& operator=(const tArray<T>& rhs)
  {
    if (size != rhs.GetSize()) Resize(rhs.GetSize());
    for(int i = 0; i < size; i++) data[i] = rhs[i];
    return *this;
  }
  
  bool Good() const { return (data != NULL); }
  int GetSize() const { return size; }
  
  void ResizeClear(const int in_size)
  {
    size = in_size;
    assert(size >= 0);  // Invalid size specified for array intialization
    if (data != NULL) delete [] data;  // remove old data if exists
    if (size > 0) {
      data = new T[size];   // Allocate block for data
      assert(data != NULL); // Memory allocation error: Out of Memory?
    }
    else data = NULL;
  }

  void Resize(int new_size)
  {
    assert(new_size >= 0);
    
    // If we're already at the size we want, don't bother doing anything.
    if (size == new_size) return;
    
    // If new size is 0, clean up and go!
    if (new_size == 0) {
      delete [] data;
      data = NULL;
      size = 0;
      return;
    }
    
    T* new_data = new T[new_size];
    assert(new_data != NULL); // Memory Allocation Error: Out of Memory?
    
    // Copy over old data...
    for (int i = 0; i < size && i < new_size; i++) {
      new_data[i] = data[i];
    }
    if (data != NULL) delete [] data;  // remove old data if exists
    data = new_data;
    
    size = new_size;
  }
  
  
  void Resize(int new_size, const T& empty_value)
  {
    assert(new_size >= 0);
    int old_size = size;
    Resize(new_size);
    if( new_size > old_size ) {
      for (int i = old_size; i < new_size; i++) {
        data[i] = empty_value;
      }
    }
  }
  
  
  T& ElementAt(const int index)
  {
    assert(index >= 0);    // Lower Bounds Error
    assert(index < size);  // Upper Bounds Error
    return data[index];    // in range, so return element
  }
  
  const T& ElementAt(const int index) const
  {
    assert(index >= 0);    // Lower Bounds Error
    assert(index < size);  // Upper Bounds Error
    return data[index];    // in range, so return element
  }
  
  T& operator[](const int index) { return ElementAt(index); }
  const T& operator[](const int index) const { return ElementAt(index); }
  
  void Push(const T& value)
  {
    Resize(size + 1);
    data[size - 1] = value;
  }
  
  void SetAll(const T& value)
  {
    for (int i = 0; i < size; i++) data[i] = value;
  }
};

#endif

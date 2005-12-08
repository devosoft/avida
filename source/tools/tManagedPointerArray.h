/*
 *  tManagedPointerArray.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef tManagedPointerArray_h
#define tManagedPointerArray_h

#include <assert.h>

#ifndef tArray_h
#include "tArray.h"
#endif

template <class T> class tManagedPointerArray {  
protected:
  T** data;  // Data Elements
  int size;  // Number of Elements
  
public:
  explicit tManagedPointerArray(const int _size = 0) : data(NULL), size(0) { ResizeClear(_size); }
  tManagedPointerArray(const tManagedPointerArray& rhs) : data(NULL), size(0) { this->operator=(rhs); }
  tManagedPointerArray(const tArray<T>& rhs) : data(NULL), size(0) { this->operator=(rhs); }

  ~tManagedPointerArray()
  { 
    for (int i = 0; i < size; i++) delete data[i];
    delete [] data;
  }
  
  tManagedPointerArray& operator=(const tManagedPointerArray& rhs)
  {
    if (size != rhs.GetSize()) Resize(rhs.GetSize());
    for(int i = 0; i < size; i++) *data[i] = rhs[i];
    return *this;
  }
  tManagedPointerArray& operator=(const tArray<T>& rhs)
  {
    if (size != rhs.GetSize()) Resize(rhs.GetSize());
    for(int i = 0; i < size; i++) *data[i] = rhs[i];
    return *this;
  }
  
  bool Good() const { return (data != NULL); }
  int GetSize() const { return size; }
  
  void ResizeClear(const int in_size)
  {
    for (int i = 0; i < size; i++) delete data[i];
    delete [] data;  // remove old data if exists
    
    size = in_size;
    assert(size >= 0);  // Invalid size specified for array intialization
    
    if (size > 0) {
      data = new T*[size];   // Allocate block for data
      assert(data != NULL); // Memory allocation error: Out of Memory?
      for (int i = 0; i < size; i++) data[i] = new T;
    } else
      data = NULL;
  }
  
  void Resize(int new_size)
  {
    assert(new_size >= 0);
    
    // If we're already at the size we want, don't bother doing anything.
    if (size == new_size) return;
    
    // If new size is 0, clean up and go!
    if (new_size == 0) {
      for (int i = 0; i < size; i++) delete data[i];
      delete [] data;
      data = NULL;
      size = 0;
      return;
    }
    
    T** new_data = new T*[new_size];
    assert(new_data != NULL); // Memory Allocation Error: Out of Memory?
    
    if (size < new_size) {
      // Fill out the new portion of the array, if needed
      for (int i = size; i < new_size; i++) new_data[i] = new T;
    } else if (new_size < size) {
      // Clean up old portion of the array, if needed
      for (int i = new_size; i < size; i++) delete data[i];
    }
    
    // Copy over old data...
    for (int i = 0; i < size && i < new_size; i++) {
      new_data[i] = data[i];
    }
    delete [] data;  // remove old data if exists
    data = new_data;
    
    size = new_size;
  }
  
  void Resize(int new_size, const T& empty_value)
  {
    assert(new_size >= 0);
    int old_size = size;
    Resize(new_size);
    if (new_size > old_size)
      for (int i = old_size; i < new_size; i++) *data[i] = empty_value;
  }
  
  T& ElementAt(const int index)
  {
    assert(index >= 0);    // Lower Bounds Error
    assert(index < size);  // Upper Bounds Error
    return *data[index];   // in range, so return element
  }  
  const T& ElementAt(const int index) const
  {
    assert(index >= 0);    // Lower Bounds Error
    assert(index < size);  // Upper Bounds Error
    return *data[index];   // in range, so return element
  }
  
  T& operator[](const int index) { return ElementAt(index); }
  const T& operator[](const int index) const { return ElementAt(index); }
  
  void Push(const T& value)
  {
    Resize(size + 1);
    *data[size - 1] = value;
  }
  
  void SetAll(const T& value)
  {
    for (int i = 0; i < size; i++) *data[i] = value;
  }
};

#endif

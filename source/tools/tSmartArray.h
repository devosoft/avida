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

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif

#ifndef tArray_h
#include "tArray.h"
#endif

#include <assert.h>

// "I am so smart..."
static const int SMRT_INCREASE_MINIMUM = 10;
static const double SMRT_INCREASE_FACTOR = 1.5;
static const double SMRT_SHRINK_TEST_FACTOR = 4.0;

template <class T> class tSmartArray
{
#if USE_tMemTrack
  tMemTrack<tSmartArray<T> > mt;
#endif
private:
  
  T* m_data;    // Data Array
  int m_size;   // Raw Array Size
  int m_active; // Active Size
  
public:
  explicit tSmartArray(const int size = 0) : m_data(NULL), m_size(0), m_active(0) { ResizeClear(size); }
  tSmartArray(const tSmartArray& rhs) : m_data(NULL), m_size(0), m_active(0) { this->operator=(rhs); }
  tSmartArray(const tArray<T>& rhs) : m_data(NULL), m_size(0), m_active(0) { this->operator=(rhs); }

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
  
  bool Good() const { return (m_data != NULL); }
  int GetSize() const { return m_active; }
  
  void ResizeClear(const int in_size)
  {
    assert(m_size >= 0);

    m_active = in_size;
    m_size = in_size;
    
    if (m_data != NULL) delete [] m_data;
    
    if (in_size > 0) {
      m_data = new T[in_size];   // Allocate block for data
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
    if (new_size > m_size || new_size * SMRT_SHRINK_TEST_FACTOR < m_size) {
      int new_array_size = static_cast<int>(new_size * SMRT_INCREASE_FACTOR);
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
  
  void Push(const T& value)
  {
    Resize(m_active + 1);
    m_data[m_active - 1] = value;
  }
  
  void SetAll(const T& value)
  {
    for (int i = 0; i < m_active; i++) m_data[i] = value;
  }

  // Save to archive
  template<class Archive>
  void save(Archive & a, const unsigned int version) const {
    // Save number of elements.
    unsigned int count = m_active;
    a.ArkvObj("count", count);
    // Save elements.
    while(count-- > 0){ 
      a.ArkvObj("item", (*this)[count]);
    } 
  }   
    
    
  // Load from archive
  template<class Archive>
  void load(Archive & a, const unsigned int version){
    // Retrieve number of elements.
    unsigned int count; 
    a.ArkvObj("count", count);
    ResizeClear(count);
    // Retrieve elements.
    while(count-- > 0){
      a.ArkvObj("item", (*this)[count]);
    }
  } 
  
  // Ask archive to handle loads and saves separately
  template<class Archive>
  void serialize(Archive & a, const unsigned int version){
    a.SplitLoadSave(*this, version);
  } 
};

#endif

/*
 *  tArray.h
 *  Avida
 *
 *  Called "tArray.hh" prior to 12/7/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef tArray_h
#define tArray_h

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif

#include <cassert>

#ifndef NULL
#define NULL 0
#endif

template <class T> class tArray
{
#if USE_tMemTrack
  tMemTrack<tArray<T> > mt;
#endif
private:
  T* m_data;  // Data Elements
  int m_size; // Number of Elements

public:
  explicit tArray(const int size = 0) : m_data(NULL), m_size(0) { ResizeClear(size); }
  explicit tArray(const int size = 0, const T& init_val) : m_data(NULL), m_size(0) { Resize(size, init_val); }
	tArray(const tArray& rhs) : m_data(NULL), m_size(0) { this->operator=(rhs); }

  ~tArray() { delete [] m_data; }

  tArray& operator=(const tArray& rhs) {
    if (m_size != rhs.GetSize())  ResizeClear(rhs.GetSize());
    for(int i = 0; i < m_size; i++) m_data[i] = rhs[i];
    return *this;
  }

  bool Good() const { return (m_data != NULL); }
  int GetSize() const { return m_size; }

  void ResizeClear(const int in_size)
  {
    m_size = in_size;
    assert(m_size >= 0);  // Invalid size specified for array intialization
    if (m_data != NULL) delete [] m_data;  // remove old data if exists
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
    if (m_size == new_size) return;

    // If new size is 0, clean up and go!
    if (new_size == 0) {
      if (m_data != NULL) delete [] m_data;
      m_data = NULL;
      m_size = 0;
      return;
    }

    // If new size > 0
    T* new_data = new T[new_size];
    assert(new_data != NULL); // Memory Allocation Error: Out of Memory?

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
    for (int i = old_size; i < new_size; i++) m_data[i] = empty_value;
  }

  T& operator[](const int index)
  {
    assert(index >= 0);     // Lower Bounds Error
    assert(index < m_size); // Upper Bounds Error
    return m_data[index];
  }
  const T& operator[](const int index) const
  {
    assert(index >= 0);     // Lower Bounds Error
    assert(index < m_size); // Upper Bounds Error
    return m_data[index];
  }    

  void Push(const T& value)
  {
    Resize(m_size + 1);
    m_data[m_size - 1] = value;
  }

  void SetAll(const T& value)
  {
    for (int i = 0; i < m_size; i++) m_data[i] = value;
  }

  // Save to archive
  template<class Archive>
  void save(Archive & a, const unsigned int version) const {
    // Save number of elements.
    unsigned int count = GetSize();
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

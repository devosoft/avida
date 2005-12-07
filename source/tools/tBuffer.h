/*
 *  tBuffer.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef tBuffer_h
#define tBuffer_h

#include <assert.h>
#include <iostream>

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

template <class T> class tBuffer {
private:
  tArray<T> data;      // Contents of buffer...
  int offset;          // Position in buffer to next write.
  int total;           // Total inputs ever...
  int last_total;      // Total inputs at time of last ZeroNumAdds.
public:
  tBuffer(const int size) : data(size), offset(0), total(0),
			    last_total(0) { ; }
  tBuffer(const tBuffer<T> & in) : data(in.data), offset(in.offset),
			   total(in.total), last_total(in.last_total) { ; }

  tBuffer & operator= (const tBuffer<T> & in) {
    data = in.data;
    offset = in.offset;
    total = in.total;
    last_total = in.last_total;
    return *this;
  }

  ~tBuffer() { ; }

  void Clear() { offset = 0; total = 0; last_total = 0; }

  void Add(T in){
    data[offset] = in;
    total++;
    offset++;
    while (offset >= data.GetSize()) offset -= data.GetSize();
  }

  void ZeroNumAdds() { total = 0; }

  T operator[] (int i) const {
    int index = offset - i - 1;
    while (index < 0)  index += data.GetSize();
    return data[index];
  }

  int GetCapacity() const { return data.GetSize(); }
  int GetTotal() const { return total; }
  int GetNumStored() const
    { return (total <= data.GetSize()) ? total : data.GetSize(); }
  int GetNum() const { return total - last_total; }

  void SaveState(std::ostream& fp) {
    assert(fp.good());
    fp << "tBuffer" << " ";
    fp << data.GetSize() << " ";
    for (int i = 0; i < data.GetSize(); i++)  fp << data[i] << " ";
    fp << offset << " "  << total << " "  << last_total << " "  << std::endl;
  }
  
  void LoadState(std::istream & fp) {
    assert(fp.good());
    cString foo;  fp >> foo;  assert(foo == "tBuffer");
    int capacity;  fp >> capacity;
    data.Resize(capacity);
    for (int i = 0; i < capacity; i++) {
      fp >> data[i];
    }
    fp  >>  offset  >>  total  >>  last_total;
  }
};

#endif

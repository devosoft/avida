/*
 *  tBuffer.h
 *  Avida
 *
 *  Called "tBuffer.hh" prior to 12/7/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef tBuffer_h
#define tBuffer_h

#include "cString.h"

#include <cassert>
#include <iostream>


template <class T> class tBuffer
{
private:
  Apto::Array<T> data;      // Contents of buffer...
  int offset;          // Position in buffer to next write.
  int total;           // Total inputs ever...
  int last_total;      // Total inputs at time of last ZeroNumAdds.
public:
  explicit tBuffer(const int size) : data(size), offset(0), total(0), last_total(0) { ; }
  tBuffer(const tBuffer<T> & in) : data(in.data), offset(in.offset), total(in.total), last_total(in.last_total) { ; }
  ~tBuffer() { ; }

  tBuffer& operator=(const tBuffer<T>& in)
  {
    data = in.data;
    offset = in.offset;
    total = in.total;
    last_total = in.last_total;
    return *this;
  }

  void Clear() { offset = 0; total = 0; last_total = 0; }
  void ZeroNumAdds() { last_total = total; total = 0; }

  void Add(const T& in_value)
  {
    data[offset] = in_value;
    total++;
    offset++;
    offset %= data.GetSize();
  }

  void Pop()
  {
	  total--;
	  offset--;
	  if (offset < 0) offset += data.GetSize();
  }

  T operator[](int i) const
  {
//    assert(i < total);
    int index = offset - i - 1;
    if (index < 0)  index += data.GetSize();
    assert(index >= 0 && index < data.GetSize());
    return data[index];
  }

  int GetCapacity() const { return data.GetSize(); }
  int GetTotal() const { return total; }
  int GetNumStored() const { return (total <= data.GetSize()) ? total : data.GetSize(); }
  int GetNum() const { return total - last_total; }
};

#endif

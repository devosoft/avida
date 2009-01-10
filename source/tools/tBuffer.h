/*
 *  tBuffer.h
 *  Avida
 *
 *  Called "tBuffer.hh" prior to 12/7/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#ifndef tBuffer_h
#define tBuffer_h

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

#include <cassert>
#include <iostream>


template <class T> class tBuffer
{
#if USE_tMemTrack
  tMemTrack<tBuffer<T> > mt;
#endif
private:
  tArray<T> data;      // Contents of buffer...
  int offset;          // Position in buffer to next write.
  int total;           // Total inputs ever...
  int last_total;      // Total inputs at time of last ZeroNumAdds.
public:
  tBuffer(const int size) : data(size), offset(0), total(0), last_total(0) { ; }
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
    int index = offset - i - 1;
    if (index < 0)  index += data.GetSize();
    return data[index];
  }

  int GetCapacity() const { return data.GetSize(); }
  int GetTotal() const { return total; }
  int GetNumStored() const { return (total <= data.GetSize()) ? total : data.GetSize(); }
  int GetNum() const { return total - last_total; }

  
  template<class Archive>
  void serialize(Archive & a, const unsigned int version){
    a.ArkvObj("data", data);
    a.ArkvObj("offset", offset);
    a.ArkvObj("total", total);
    a.ArkvObj("last_total", last_total);
  }

  /*
  FIXME: I'm replacing the code below with a serializing system, but
  want to keep the old around for reference until I'm sure the new
  system works.
  @kgn
  */
  //void SaveState(std::ostream& fp)
  //{
  //  assert(fp.good());
  //  fp << "tBuffer" << " ";
  //  fp << data.GetSize() << " ";
  //  for (int i = 0; i < data.GetSize(); i++)  fp << data[i] << " ";
  //  fp << offset << " "  << total << " "  << last_total << " "  << std::endl;
  //}
  //
  //void LoadState(std::istream& fp)
  //{
  //  assert(fp.good());
  //  cString foo;  fp >> foo;  assert(foo == "tBuffer");
  //  int capacity;  fp >> capacity;
  //  data.Resize(capacity);
  //  for (int i = 0; i < capacity; i++) {
  //    fp >> data[i];
  //  }
  //  fp  >>  offset  >>  total  >>  last_total;
  //}
};

#endif

/*
 *  cStringIterator.h
 *  Avida
 *
 *  Called "string_iterator.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cStringIterator_h
#define cStringIterator_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

class cStringList;


class cStringIterator
{
private:
  tConstListIterator<cString> list_it;
  static const cString null_str;
  
  
  cStringIterator(); // @not_implemented
  cStringIterator(const cStringIterator&); // @not_implemented
  cStringIterator& operator=(const cStringIterator&); // @not_implemented
  
public:
  cStringIterator(const cStringList& in_list);

  void Reset() { list_it.Reset(); }
  const cString & Get() {
    if (list_it.Get() == NULL) return null_str;
    return *(list_it.Get());
  }
  const cString & Next() { list_it.Next(); return Get(); }
  const cString & Prev() { list_it.Prev(); return Get(); }
  bool AtRoot() const { return list_it.AtRoot(); }
  bool AtEnd() const { return list_it.AtEnd(); }
};

#endif

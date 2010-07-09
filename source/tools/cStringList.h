/*
 *  cStringList.h
 *  Avida
 *
 *  Called "string_list.hh" prior to 12/7/05.
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

#ifndef cStringList_h
#define cStringList_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif


class cStringList
{
private:
  tList<cString> string_list;

  inline cString ReturnString(cString* out_string)
  {
    cString tmp_string(*out_string);
    delete out_string;
    return tmp_string;
  }

public:
  cStringList() { ; }
  cStringList(const cString& _list, char seperator = ' ');
  cStringList(const cStringList& _list);
  ~cStringList();

  cStringList& operator=(const cStringList & _list);

  int GetSize() const { return string_list.GetSize(); }
  cString GetLine(int line_num) const { return *(string_list.GetPos(line_num)); }
  const tList<cString>& GetList() const { return string_list; }

  bool HasString(const cString& test_string) const;
  cString PopString(const cString& test_string);
  cString PopLine(int);
  int LocateString(const cString& test_string) const;
  
  void Push(const cString& _in) { string_list.Push(new cString(_in));}
  void PushRear(const cString& _in) { string_list.PushRear(new cString(_in));}
  cString Pop() { return ReturnString(string_list.Pop()); }
  cString PopRear() { return ReturnString(string_list.PopRear()); }

  void Load(cString _list, char seperator=' ') { while (_list.GetSize() > 0) PushRear(_list.Pop(seperator)); }
  void Clear() { while (string_list.GetSize() > 0) delete string_list.Pop(); }
};

#endif

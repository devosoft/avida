/*
 *  cStringList.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
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


class cStringList {
private:
  tList<cString> string_list;

  inline cString ReturnString(cString * out_string) {
    cString tmp_string(*out_string);
    delete out_string;
    return tmp_string;
  }
public:
  cStringList() { ; }
  cStringList(const cString & _list, char seperator=' ');
  cStringList(const cStringList & _list);
  ~cStringList();

  cStringList & operator=(const cStringList & _list);

  int GetSize() const { return string_list.GetSize(); }
  cString GetLine(int line_num) const { return *(string_list.GetPos(line_num)); }
  const tList<cString> & GetList() const { return string_list; }

  bool HasString(const cString & test_string) const;
  cString PopString(const cString & test_string);

  void Push(const cString & _in) { string_list.Push(new cString(_in));}
  void PushRear(const cString & _in) { string_list.PushRear(new cString(_in));}
  cString Pop() { return ReturnString(string_list.Pop()); }
  cString PopRear() { return ReturnString(string_list.PopRear()); }

  void Load(cString _list, char seperator=' ') {
    while (_list.GetSize() > 0) PushRear(_list.Pop(seperator));
  }
  void Clear() {
    while (string_list.GetSize() > 0) delete string_list.Pop(); 
  }
};

#endif

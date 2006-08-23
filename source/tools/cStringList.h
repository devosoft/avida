/*
 *  cStringList.h
 *  Avida
 *
 *  Called "string_list.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cStringList_h
#define cStringList_h

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif


class cStringList
{
#if USE_tMemTrack
  tMemTrack<cStringList> mt;
#endif
private:
  tList<cString> string_list;

  inline cString ReturnString(cString * out_string)
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

  template<class Archive>
  void serialize(Archive & a, const unsigned int version){
    a.ArkvObj("string_list", string_list);
  } 
};


#ifdef ENABLE_UNIT_TESTS
namespace nStringList {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

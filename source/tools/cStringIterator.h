//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef STRING_ITERATOR_HH
#define STRING_ITERATOR_HH

#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef TLIST_HH
#include "tList.hh"
#endif

class cString; // aggregate
class cStringList;
template <class T> class tConstListIterator; // aggregate

class cStringIterator {
private:
  tConstListIterator<cString> list_it;
  static const cString null_str;
public:
  cStringIterator(const cStringList & in_list);

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

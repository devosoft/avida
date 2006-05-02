/*
 *  cStringIterator.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
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


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif

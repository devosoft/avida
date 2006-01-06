/*
 *  tDataEntry.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef tDataEntry_h
#define tDataEntry_h

#include <iostream>

#ifndef cString_h
#include "cString.h"
#endif
#ifndef cStringUtil_h
#include "cStringUtil.h"
#endif
#ifndef tDataEntryBase_h
#include "tDataEntryBase.h"
#endif

template <class T, class OUT> class tDataEntry : public tDataEntryBase<T> {
protected:
  OUT  (T::*DataRetrieval)() const;
  void (T::*DataSet)(OUT);
  int  (T::*DataCompare)(T*) const;

  int CmpNULL(T *) const { return 0; }
public:
  tDataEntry(const cString & _name, const cString & _desc,
	     OUT (T::*_funR)() const,
	     void (T::*_funS)(OUT _val) = NULL,
	     //int (T::*_funC)(T * _o) const = &T::CompareNULL,
	     int (T::*_funC)(T * _o) const = 0,
	     const cString & _null="0",
	     const cString & _html_cell="align=center")
    : tDataEntryBase<T>(_name, _desc, _null, _html_cell), DataRetrieval(_funR),
      DataSet(_funS), DataCompare(_funC) { ; }

  bool Print(std::ostream& fp) const {
    if (this->target == NULL) return false;
    fp << (this->target->*DataRetrieval)();
    return true;
  }

  //int Compare(T * other) const { return (target->*DataCompare)(other); }
  int Compare(T * other) const {
    return (DataCompare)?((this->target->*DataCompare)(other)):(0);
  }
  bool Set(const cString & value) {
    OUT new_value(0);
    if (DataSet == 0) return false;
    (this->target->*DataSet)( cStringUtil::Convert(value, new_value) );
    return true;
  }
};

#endif

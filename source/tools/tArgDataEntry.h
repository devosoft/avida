/*
 *  tArgDataEntry.h
 *  Avida
 *
 *  Called "tArgDataEntry.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef tArgDataEntry_h
#define tArgDataEntry_h

#include <iostream>

#ifndef tDataEntryBase_h
#include "tDataEntryBase.h"
#endif

class cString;

template <class T, class OUT, class ARG> class tArgDataEntry
  : public tDataEntryBase<T> {
protected:
  OUT (T::*DataRetrieval)(ARG) const;
  int (T::*DataCompare)(T*,ARG) const;
  ARG arg;
public:
  tArgDataEntry(const cString & _name, const cString & _desc,
		OUT (T::*_funR)(ARG) const, ARG _arg,
		//int (T::*_funC)(T * _o, ARG _i) const = &T::CompareArgNULL,
		int (T::*_funC)(T * _o, ARG _i) const = 0,
		const cString & _null="0",
		const cString & _html_cell="align=center")
    : tDataEntryBase<T>(_name, _desc, _null, _html_cell),
      DataRetrieval(_funR), DataCompare(_funC), arg(_arg) { ; }

  bool Print(std::ostream& fp) const {
    if (this->target == NULL) return false;
    fp << (this->target->*DataRetrieval)(arg);
    return true;
  }

  //int Compare(T * other) const { return (target->*DataCompare)(other, arg); }
  int Compare(T * other) const {
    return (DataCompare)?((this->target->*DataCompare)(other, arg)):(0);
  }
};

#endif

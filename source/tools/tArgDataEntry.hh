//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////


#ifndef TARGDATAENTRY_HH
#define TARGDATAENTRY_HH

#include <iostream>

#ifndef TDATAENTRYBASE_HH
#include "tDataEntryBase.hh"
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

  bool Print(std::ostream & fp) const {
    if (target == NULL) return false;
    fp << (target->*DataRetrieval)(arg);
    return true;
  }

  //int Compare(T * other) const { return (target->*DataCompare)(other, arg); }
  int Compare(T * other) const {
    return (DataCompare)?((target->*DataCompare)(other, arg)):(0);
  }
};

#endif

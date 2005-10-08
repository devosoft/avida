//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef TDATAENTRY_HH
#define TDATAENTRY_HH

#include <iostream>

#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef STRING_UTIL_HH
#include "cStringUtil.h"
#endif
#ifndef TDATAENTRYBASE_HH
#include "tDataEntryBase.hh"
#endif

class cString;
struct cStringUtil; // access

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

  bool Print(std::ostream & fp) const {
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

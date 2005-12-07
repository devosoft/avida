/*
 *  tDataEntryBase.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef tDataEntryBase_h
#define tDataEntryBase_h

#include <iostream>
#include <sstream>

#ifndef cDataEntry_h
#include "cDataEntry.h"
#endif

class cString;

template <class T> class tDataEntryBase : public cDataEntry {
protected:
  T * target;
public:
  tDataEntryBase(const cString & _name, const cString & _desc,
		 const cString & _null="0",
		 const cString & _html_cell="align=center")
    : cDataEntry(_name, _desc, _null, _html_cell), target(NULL) { ; }
  
  void SetTarget(T * _target) { target = _target; }

  virtual bool Print(std::ostream& fp) const { (void) fp;  return false; }
  virtual int Compare(T * other) const { (void) other; return 0; }
  virtual bool Set(const cString & value) { (void) value; return false; }

  void HTMLPrint(std::ostream& fp, int compare=0, bool print_text=true) {
    fp << "<td " << GetHtmlCellFlags() << " ";
    if (compare == -2) {
      fp << "bgcolor=\"#FF0000\">";
      if (print_text == true) fp << GetNull() << " ";
      else fp << "&nbsp; ";
      return;
    }

    if (compare == -1)     fp << "bgcolor=\"#FFCCCC\">";
    else if (compare == 0) fp << ">";
    else if (compare == 1) fp << "bgcolor=\"#CCFFCC\">";
    else if (compare == 2) fp << "bgcolor=\"#00FF00\">";
    else {
      std::cerr << "Error! Illegal case in Compare:" << compare << std::endl;
      exit(0);
    }

    if (print_text == true) fp << *this << " ";
    else fp << "&nbsp; ";
  }

  cString AsString() {
    std::stringstream tmp_stream;
    tmp_stream << *this;
    cString out_str;
    tmp_stream >> out_str;
    return out_str;
  }
};

#endif

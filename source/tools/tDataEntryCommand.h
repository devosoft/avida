//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef TDATAENTRYCOMMAND_HH
#define TDATAENTRYCOMMAND_HH

#include <iostream>

#ifndef STRING_LIST_HH
#include "cStringList.h"
#endif
#ifndef TDATAENTRYBASE_HH
#include "tDataEntryBase.h"
#endif

class cString;
class cStringList; // aggregate
template <class T> class tDataEntryBase; // access

template <class T> class tDataEntryCommand {
private:
  tDataEntryBase<T> * data_entry;
  cStringList args;
public:
  tDataEntryCommand(tDataEntryBase<T> * _entry, const cString & _args="")
    : data_entry(_entry), args(_args, ':') { ; }
  
  tDataEntryBase<T> & GetEntry() { return *data_entry; }
  const cStringList & GetArgs() const { return args; }
  bool HasArg(const cString & test_arg) { return args.HasString(test_arg); }

  const cString & GetName() const { return data_entry->GetName(); }
  const cString & GetDesc() const { return data_entry->GetDesc(); }
  const cString & GetNull() const { return data_entry->GetNull(); }
  const cString & GetHtmlCellFlags() const
    { return data_entry->GetHtmlCellFlags(); }

  void SetTarget(T * _target) { data_entry->SetTarget(_target); }
  bool Print(std::ostream & fp) const { return data_entry->Print(fp); }
  int Compare(T * other) const { return data_entry->Compare(other); }
  bool SetValue(const cString & value) { return data_entry->Set(value); }
  void HTMLPrint(std::ostream & fp, int compare=0, bool print_text=true)
    { data_entry->HTMLPrint(fp, compare, print_text); }
};

#endif

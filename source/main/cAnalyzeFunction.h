//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ANALYZE_FUNCTION_HH
#define ANALYZE_FUNCTION_HH

#ifndef ANALYZE_COMMAND_HH
#include "cAnalyzeCommand.h"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef TLIST_HH
#include "tList.hh"
#endif

// cAnalyzeFunction    : User-defined function

class cString; // aggregate
template <class T> class tList; // aggregate
class cAnalyzeCommand;

class cAnalyzeFunction {
private:
  cString name;
  tList<cAnalyzeCommand> command_list;
private:
  // disabled copy constructor.
  cAnalyzeFunction(const cAnalyzeFunction &);
public:
  cAnalyzeFunction(const cString & _name) : name(_name) { ; }
  ~cAnalyzeFunction() { 
    while ( command_list.GetSize() > 0 ) delete command_list.Pop();
  }

  const cString & GetName() { return name; }
  tList<cAnalyzeCommand> * GetCommandList() { return &command_list; }
};

#endif

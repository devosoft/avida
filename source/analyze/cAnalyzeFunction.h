/*
 *  cAnalyzeFunction.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cAnalyzeFunction_h
#define cAnalyzeFunction_h

#ifndef cAnalyzeCommand_h
#include "cAnalyzeCommand.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

// cAnalyzeFunction    : User-defined function

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

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

#endif

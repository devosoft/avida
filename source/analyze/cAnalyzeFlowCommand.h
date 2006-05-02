/*
 *  cAnalyzeFlowCommand.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cAnalyzeFlowCommand_h
#define cAnalyzeFlowCommand_h

#ifndef cAnalyzeCommand_h
#include "cAnalyzeCommand.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

// cAnalyzeFlowCommand : A cAnalyzeCommand containing other commands

class cString;

class cAnalyzeFlowCommand : public cAnalyzeCommand {
protected:
  tList<cAnalyzeCommand> command_list;
public:
  cAnalyzeFlowCommand(const cString & _command, const cString & _args)
    : cAnalyzeCommand(_command, _args) { ; }
  virtual ~cAnalyzeFlowCommand() {
    while ( command_list.GetSize() > 0 ) delete command_list.Pop();
  }

  tList<cAnalyzeCommand> * GetCommandList() { return &command_list; }

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

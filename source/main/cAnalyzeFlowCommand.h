//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ANALYZE_FLOW_COMMAND_HH
#define ANALYZE_FLOW_COMMAND_HH

#ifndef ANALYZE_COMMAND_HH
#include "cAnalyzeCommand.h"
#endif
#ifndef TLIST_HH
#include "tList.hh"
#endif

// cAnalyzeFlowCommand : A cAnalyzeCommand containing other commands

template <class T> class tList; // aggregate
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
};

#endif

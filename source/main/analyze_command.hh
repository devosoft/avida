//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ANALYZE_COMMAND_HH
#define ANALYZE_COMMAND_HH

#ifndef STRING_HH
#include "string.hh"
#endif

// cAnalyzeCommand     : A command in a loaded program

class cString; // aggregate
template <class T> class tList;

class cAnalyzeCommand {
protected:
  cString command;
  cString args;
public:
  cAnalyzeCommand(const cString & _command, const cString & _args)
    : command(_command), args(_args) { command.ToUpper(); }
  virtual ~cAnalyzeCommand() { ; }

  const cString & GetCommand() { return command; }
  const cString & GetArgs() const { return args; }
  cString GetArgs() { return args; }
  virtual tList<cAnalyzeCommand> * GetCommandList() { return NULL; }

  /*
  added to satisfy Boost.Python; the semantics are fairly useless --
  equality of two references means that they refer to the same object.
  */
  bool operator==(const cAnalyzeCommand &in) const { return &in == this; }
};

#endif

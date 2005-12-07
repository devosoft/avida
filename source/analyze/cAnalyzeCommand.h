/*
 *  cAnalyzeCommand.h
 *  Avida
 *
 *  Created by David on 12/1/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cAnalyzeCommand_h
#define cAnalyzeCommand_h

#ifndef cString_h
#include "cString.h"
#endif

// cAnalyzeCommand     : A command in a loaded program

template <class T> class tList;

class cAnalyzeCommand {
protected:
  cString command;
  cString args;
private:
  // disabled copy constructor.
  cAnalyzeCommand(const cAnalyzeCommand &);
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

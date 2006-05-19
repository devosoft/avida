/*
 *  cAnalyzeCommand.h
 *  Avida
 *
 *  Created by David on 12/1/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
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

class cAnalyzeCommand
{
protected:
  cString m_command;
  cString m_args;


private:
  cAnalyzeCommand(); // @not_implemented
  cAnalyzeCommand(const cAnalyzeCommand&); // @not_implemented
  cAnalyzeCommand& operator=(const cAnalyzeCommand&); // @not_implemented


public:
  cAnalyzeCommand(const cString& command, const cString& args) : m_command(command), m_args(args) { ; }
  virtual ~cAnalyzeCommand() { ; }

  const cString& GetCommand() { return m_command; }
  const cString& GetArgs() const { return m_args; }
  cString GetArgs() { return m_args; }
  virtual tList<cAnalyzeCommand>* GetCommandList() { return NULL; }

  /*
  added to satisfy Boost.Python; the semantics are fairly useless --
  equality of two references means that they refer to the same object.
  */
  bool operator==(const cAnalyzeCommand &in) const { return &in == this; }
};

#ifdef ENABLE_UNIT_TESTS
namespace nAnalyzeCommand {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

/*
 *  cAnalyzeCommand.h
 *  Avida
 *
 *  Called "analyze_command.hh" prior to 12/1/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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

#endif

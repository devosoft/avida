/*
 *  cAnalyzeFlowCommand.h
 *  Avida
 *
 *  Called "analyze_flow_command.hh" prior to 12/2/05.
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
};

#endif

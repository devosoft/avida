/*
 *  cAnalyzeFunction.h
 *  Avida
 *
 *  Called "analyze_function.hh" prior to 12/2/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
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
};

#endif

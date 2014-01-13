/*
 *  cAnalyzeCommandDef.h
 *  Avida
 *
 *  Called "analyze_command_def.hh" prior to 12/1/05.
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

#ifndef cAnalyzeCommandDef_h
#define cAnalyzeCommandDef_h

#ifndef cAnalyzeCommandDefBase_h
#include "cAnalyzeCommandDefBase.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cAnalyze;
class cAnalyzeCommand;

class cAnalyzeCommandDef : public cAnalyzeCommandDefBase {
private:
  void (cAnalyze::*CommandFunction)(cString);
public:
  cAnalyzeCommandDef(const cString& name, void (cAnalyze::*cf)(cString))
    : cAnalyzeCommandDefBase(name), CommandFunction(cf) { ; }
  ~cAnalyzeCommandDef() { ; }

  void Run(cAnalyze* analyze, const cString& args, cAnalyzeCommand& command, Feedback&) const
  {
    (void) command; // used in other types of command defininitions.
    (analyze->*CommandFunction)(args);
  }
};

#endif

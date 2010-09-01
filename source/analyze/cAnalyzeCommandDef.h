/*
 *  cAnalyzeCommandDef.h
 *  Avida
 *
 *  Called "analyze_command_def.hh" prior to 12/1/05.
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
  cAnalyzeCommandDef(const cString & _name, void (cAnalyze::*_cf)(cString))
    : cAnalyzeCommandDefBase(_name), CommandFunction(_cf) { ; }
  ~cAnalyzeCommandDef() { ; }

  void Run(cAnalyze * analyze, const cString & args, cAnalyzeCommand & command) const
  {
    (void) command; // used in other types of command defininitions.
    (analyze->*CommandFunction)(args);
  }
};

#endif

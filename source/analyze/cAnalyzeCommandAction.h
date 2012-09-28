/*
 *  cAnalyzeCommandAction.h
 *  Avida
 *
 *  Created by David on 4/10/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#ifndef cAnalyzeCommandAction_h
#define cAnalyzeCommandAction_h

#ifndef cAction_h
#include "cAction.h"
#endif
#ifndef cActionLibrary_h
#include "cActionLibrary.h"
#endif
#ifndef cAnalyzeCommandDefBase_h
#include "cAnalyzeCommandDefBase.h"
#endif
#ifndef cWorld_h
#include "cWorld.h"
#endif

class cAnalyze;
class cAnalyzeCommand;

class cAnalyzeCommandAction : public cAnalyzeCommandDefBase
{
private:
  cWorld* m_world;

public:
  cAnalyzeCommandAction(const cString& in_name, cWorld* world)
    : cAnalyzeCommandDefBase(in_name), m_world(world) { ; }
  ~cAnalyzeCommandAction() { ; }
  
  void Run(cAnalyze*, const cString& args, cAnalyzeCommand&, Feedback& feedback) const
  {
    cAction* action = cActionLibrary::GetInstance().Create((const char*)m_name, m_world, args, feedback);
    cAvidaContext& ctx = m_world->GetDefaultContext();
    action->Process(ctx);
  }
};

#endif

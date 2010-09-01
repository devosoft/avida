/*
 *  cAnalyzeCommandAction.h
 *  Avida
 *
 *  Created by David on 4/10/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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
  
  void Run(cAnalyze* analyze, const cString& args, cAnalyzeCommand& command) const
  {
    cAction* action = cActionLibrary::GetInstance().Create(name, m_world, args);
    cAvidaContext& ctx = m_world->GetDefaultContext();
    action->Process(ctx);
  }
};

#endif

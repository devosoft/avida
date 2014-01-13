/*
 *  cAction.h
 *  Avida
 *
 *  Created by David on 4/8/06.
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

#ifndef cAction_h
#define cAction_h

#include "cString.h"

class cAvidaContext;
class cWorld;

class cAction
{
private:
  cAction();  // @not_implemented
  cAction(const cAction&); // @not_implemented
  cAction& operator=(const cAction&); // @not_implemented
  
protected:
  cWorld* m_world;
  cString m_args;
  
public:
  cAction(cWorld* world, const cString& args) : m_world(world), m_args(args) { ; }
  virtual ~cAction() { ; }
  
  const cString& GetArgs() const { return m_args; }
  
  virtual void Process(cAvidaContext& ctx) = 0;
};

#endif

/*
 *  cAvidaDriver.h
 *  Avida
 *
 *  Created by David on 12/10/05.
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

#ifndef cAvidaDriver_h
#define cAvidaDriver_h

#include "avida/core/cGlobalObject.h"


// This class is an abstract base class from which all driver classes
// in Avida descend.  cAvidaDriver objects are friends with cWorld,
// allowing them to register cWorldDriver's with an instance of cWorld.

class cAvidaDriver : public virtual Avida::cGlobalObject
{
private:
  cAvidaDriver(const cAvidaDriver&); // @not_implemented
  cAvidaDriver& operator=(const cAvidaDriver&); // @not_implemented

public:
  cAvidaDriver() { ; }
  virtual ~cAvidaDriver() { ; }
  
  virtual void Run() = 0;
};

#endif

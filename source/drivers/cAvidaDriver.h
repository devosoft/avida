/*
 *  cAvidaDriver.h
 *  Avida
 *
 *  Created by David on 12/10/05.
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

#ifndef cAvidaDriver_h
#define cAvidaDriver_h

#include "cDMObject.h"


// This class is an abstract base class from which all driver classes
// in Avida descend.  cAvidaDriver objects are friends with cWorld,
// allowing them to register cWorldDriver's with an instance of cWorld.

class cAvidaDriver : public virtual cDMObject
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

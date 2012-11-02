/*
 *  cInstLibEntry.h
 *  Avida
 *
 *  Created by David Bryson on 2/14/07.
 *  Copyright 2007-2011 Michigan State University. All rights reserved.
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

#ifndef tInstLibEntry_h
#define tInstLibEntry_h

#ifndef cInstLibEntry_h
#include "cInstLibEntry.h"
#endif


template <class FUN> class tInstLibEntry : public cInstLibEntry
{
private:
  const FUN m_function;

  tInstLibEntry(); // @not_implemented
  
public:
  tInstLibEntry(const cString& name, FUN function, InstructionClass _class = INST_CLASS_OTHER, unsigned int flags = 0,
    const cString& desc = "", BehavClass _bclass = BEHAV_CLASS_NONE)
    : cInstLibEntry(name, _class, flags, desc, _bclass), m_function(function) { ; }
  
  const FUN GetFunction() const { return m_function; }
};


#endif

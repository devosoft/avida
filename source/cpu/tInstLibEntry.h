/*
 *  cInstLibEntry.h
 *  Avida
 *
 *  Created by David Bryson on 2/14/07.
 *  Copyright 2007-2010 Michigan State University. All rights reserved.
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
  tInstLibEntry(const cString& name, FUN function, unsigned int flags = 0, const cString& desc = "")
    : cInstLibEntry(name, flags, desc), m_function(function) { ; }
  
  const FUN GetFunction() const { return m_function; }
};


#endif

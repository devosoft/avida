/*
 *  cReactionLib.h
 *  Avida
 *
 *  Called "reaction_lib.hh" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#ifndef cReactionLib_h
#define cReactionLib_h

#ifndef tArray_h
#include "tArray.h"
#endif

class cReaction;
class cString;

class cReactionLib
{
private:
  tArray<cReaction*> reaction_array;

  cReactionLib(const cReactionLib&); // @not_implemented
  cReactionLib& operator=(const cReactionLib&); // @not_implemented

public:
  cReactionLib() { ; }
  ~cReactionLib();

  int GetSize() const { return reaction_array.GetSize(); }
  
  cReaction* AddReaction(const cString& name);
  cReaction* GetReaction(const cString& name) const;
  cReaction* GetReaction(int id) const;
};

#endif

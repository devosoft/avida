/*
 *  cReactionLib.h
 *  Avida
 *
 *  Called "reaction_lib.hh" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#ifndef cReactionLib_h
#define cReactionLib_h

#include "avida/core/Types.h"

class cReaction;
class cString;


class cReactionLib
{
private:
  Apto::Array<cReaction*> reaction_array;

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

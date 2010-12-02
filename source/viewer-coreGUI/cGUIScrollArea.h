/*
 *  cGUIScrollArea.h
 *  Avida
 *
 *  Created by Charles on 7-9-07
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

// This is a base class for all GUI widgets that a panels containing widgets and have scollbars.

#ifndef cGUIScrollArea_h
#define cGUIScrollArea_h

#include "cGUIContainer.h"

class cGUIScrollArea : public cGUIContainer {
protected:
public:
  cGUIScrollArea(int x, int y, width, height, name="") : cGUIContainer(x, y, width, height, name) { ; }
  virtual ~cGUIScrollArea() { ; }
};

#endif

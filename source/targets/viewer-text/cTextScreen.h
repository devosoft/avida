/*
 *  cTextScreen.h
 *  Avida
 *
 *  Created by Charles on 7-1-07
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

#ifndef cTextScreen_h
#define cTextScreen_h

#ifndef cViewer_Info_h
#include "cViewer_Info.h"
#endif

#ifndef cTextWindow_h
#include "cTextWindow.h"
#endif

class cTextScreen  {
protected:
  cViewer_Info & m_info;
  cTextWindow & m_window;

public:
  cTextScreen(cViewer_Info & info, cTextWindow & window) : m_info(info), m_window(window) { ; }
  virtual ~cTextScreen() { ; }

  virtual void Draw() = 0;
  virtual void Update() = 0;
  virtual bool DoInput(int input) = 0;
};

#endif

/*
 *  cTextScreen.h
 *  Avida
 *
 *  Created by Charles on 7-1-07
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef cTextScreen_h
#define cTextScreen_h

#ifndef cCoreView_Info_h
#include "cCoreView_Info.h"
#endif

#ifndef cTextWindow_h
#include "cTextWindow.h"
#endif

class cTextScreen  {
protected:
  cCoreView_Info & m_info;
  cTextWindow & m_window;

public:
  cTextScreen(cCoreView_Info & info, cTextWindow & window) : m_info(info), m_window(window) { ; }
  virtual ~cTextScreen() { ; }

  virtual void Draw() = 0;
  virtual void Update() = 0;
  virtual bool DoInput(int input) = 0;
};

#endif

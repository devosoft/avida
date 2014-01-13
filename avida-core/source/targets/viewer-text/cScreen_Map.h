/*
 *  cScreen_Map.h
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

#ifndef cScreen_Map_h
#define cScreen_Map_h

#ifndef cViewer_Map_h
#include "cViewer_Map.h"
#endif

#ifndef cTextScreen_h
#include "cTextScreen.h"
#endif

class cScreen_Map : public cTextScreen {
private:
  cViewer_Map m_map_info;

  void SetGenotypeColor(int color_id);
public:
  cScreen_Map(cViewer_Info & info, cTextWindow & window);
  ~cScreen_Map();

  void Draw();
  void Update();
  bool DoInput(int input);
};

#endif

/*
 *  cScreen_Map.h
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

#ifndef cScreen_Map_h
#define cScreen_Map_h

#ifndef cCoreView_Map_h
#include "cCoreView_Map.h"
#endif

#ifndef cTextScreen_h
#include "cTextScreen.h"
#endif

class cScreen_Map : public cTextScreen {
private:
  cCoreView_Map m_map_info;

  void SetGenotypeColor(int color_id);
public:
  cScreen_Map(cCoreView_Info & info, cTextWindow & window);
  ~cScreen_Map();

  void Draw();
  void Update();
  bool DoInput(int input);
};

#endif

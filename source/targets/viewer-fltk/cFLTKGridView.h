/*
 *  cFLTKGridView.h
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

// This is a class for managing Avida grids in the viewer.

#ifndef cFLTKGridView_h
#define cFLTKGridView_h

#ifndef cFLTKBaseDraw_h
#include "cFLTKBaseDraw.h"
#endif

#ifndef cCoreView_Map_h
#include "cCoreView_Map.h"
#endif

class cFLTKGridView : public cFLTKBaseDraw {
private:
  cCoreView_Info & m_info;
  cCoreView_Map m_map_info;

public:
  cFLTKGridView(cCoreView_Info & info, cGUIContainer & parent,
		int x, int y, int w, int h, const cString & name="")
    : cFLTKBaseDraw(parent, x, y, w, h, name), m_info(info), m_map_info(info)
  {
    cColor::Setup();
    m_map_info.SetScaleMax(cColor::SCALE_BRIGHT.GetSize());
  }
  ~cFLTKGridView() { ; }

  cCoreView_Info & GetInfo() { return m_info; }
  cCoreView_Map & GetMapInfo() { return m_map_info; }

  void SetGenotypeColor(int color_id);

  void Draw();

  int Handle(int event) {
    return 0;
  }
};

#endif

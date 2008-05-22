/*
 *  cFLTKLegend.h
 *  Avida
 *
 *  Created by Charles on 7-9-07
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

// This is a class for managing the legend of Avida grids in the viewer.

#ifndef cFLTKLegend_h
#define cFLTKLegend_h

#ifndef cFLTKBaseDraw_h
#include "cFLTKBaseDraw.h"
#endif

#ifndef cCoreView_Map_h
#include "cCoreView_Map.h"
#endif

#ifndef cFLTKGridView_h
#include "cFLTKGridView.h"
#endif

class cFLTKLegend : public cFLTKBaseDraw {
private:
  cCoreView_Info & m_info;
  cFLTKGridView & m_grid_view;

public:
  cFLTKLegend(cFLTKGridView & _gv, cGUIContainer & parent,
		int x, int y, int w, int h, const cString & name="")
    : cFLTKBaseDraw(parent, x, y, w, h, name), m_info(_gv.GetInfo()), m_grid_view(_gv) { ; }
  ~cFLTKLegend() { ; }

  void Draw();

  int Handle(int event) {
    return 0;
  }
};

#endif

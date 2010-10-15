/*
 *  cFLTKGridView.cc
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

#include "cFLTKGridView.h"

#include "cCoreView_Info.h"
#include "cPopulation.h"

#include "AvidaTools.h"

using namespace AvidaTools;


void cFLTKGridView::SetGenotypeColor(int color_id)
{
  switch (color_id) {
  case -4: SetColor(cColor::BLACK); break;
  case -3: SetColor(cColor::DARK_GRAY); break;
  case -2: SetColor(cColor::GRAY); break;
  case -1: SetColor(cColor::WHITE); break;
  case 0:  SetColor(cColor::GREEN); break;
  case 1:  SetColor(cColor::RED); break;
  case 2:  SetColor(cColor::BLUE); break;
  case 3:  SetColor(cColor::CYAN); break;
  case 4:  SetColor(cColor::YELLOW); break;
  case 5:  SetColor(cColor::MAGENTA); break;
  case 6:  SetColor(cColor::LT_GREEN); break;
  case 7:  SetColor(cColor::LT_RED); break;
  case 8:  SetColor(cColor::LT_BLUE); break;
  case 9:  SetColor(cColor::LT_CYAN); break;
  case 10: SetColor(cColor::LT_YELLOW); break;
  case 11: SetColor(cColor::LT_MAGENTA); break;
  case 12: SetColor(cColor::DARK_GREEN); break;
  case 13: SetColor(cColor::DARK_RED); break;
  case 14: SetColor(cColor::DARK_BLUE); break;
  case 15: SetColor(cColor::DARK_CYAN); break;
  case 16: SetColor(cColor::DARK_YELLOW); break;
  case 17: SetColor(cColor::DARK_MAGENTA); break;
  };
}

void cFLTKGridView::Draw()
{
  Resize(w(), h());
  cColor::Setup();

  const int world_x = m_info.GetPopulation().GetWorldX();
  const int world_y = m_info.GetPopulation().GetWorldY();

  // Determine how large each cell can be and still fit in the window.
  const int cell_side = Min((w() - 2) / world_x, (h() - 2) / world_y);
  const int grid_x = world_x * cell_side;
  const int grid_y = world_y * cell_side;

  const int grid_offset_x = (w() - grid_x) / 2 + 1;
  const int grid_offset_y = (h() - grid_y) / 2 + 1;

  SetColor(cColor::WHITE);
  DrawBox(0, 0, GetWidth(), GetHeight(), true);  // White box around map
  
  SetColor(cColor::BLACK);
  DrawBox(0, 0, GetWidth(), GetHeight(), false);   // Outline
  DrawBox(grid_offset_x - 2, grid_offset_y - 2, grid_x+2, grid_y+2, true);  // Black grid background

  m_map_info.UpdateMaps();
  const tArray<int> & color_map( m_map_info.GetColors() );
  const tArray<int> & tag_map( m_map_info.GetTags() );
  const int color_method = m_map_info.GetModeArg( m_map_info.GetColorMode() );
  
  int id = 0;
  for (int y = 0; y < world_y; y++) {
    const int cur_y = grid_offset_y + y * cell_side;
    for (int x = 0; x < world_x; x++) {
      if (color_method == cCoreView_Map::COLORS_SCALE) {
	if (color_map[id] < 0) SetColor(cColor::BLACK);
	else SetColor(cColor::SCALE_BRIGHT[color_map[id]]);
      } else {
	SetGenotypeColor(color_map[id]);
      }
      const int cur_x = grid_offset_x + x * cell_side;
      DrawBox(cur_x, cur_y, cell_side-2, cell_side-2, true);
      //      DrawCircle(x*10, y*10, 4);

      if (tag_map[id] > 0) {
	SetColor(cColor::GREEN);
	//	DrawBox(cur_x - 1, cur_y - 1, cell_side, cell_side, false);
	DrawBox(cur_x, cur_y, cell_side-2, cell_side-2, false);
      }

      id++;
    }
  }
}

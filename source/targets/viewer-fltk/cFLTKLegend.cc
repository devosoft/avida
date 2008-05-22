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

#include "cFLTKLegend.h"
#include "cFLTKGridView.h"

#include "cCoreView_Info.h"
#include "cPopulation.h"

#define FLTK_SCALE_WIDTH 50

void cFLTKLegend::Draw()
{
  Resize(w(), h());
  cColor::Setup();

  cCoreView_Map & map_info = m_grid_view.GetMapInfo();
  const cPopulation & pop = m_grid_view.GetInfo().GetPopulation();

  const int color_method = map_info.GetModeArg( map_info.GetColorMode() );
  const int bar_height = h() - 100;
  const int max_pop = pop.GetSize();
  const double max_log_pop = log(1.0 + (double) max_pop);
  const int max_hist_size = w() - FLTK_SCALE_WIDTH - 3;

  // Draw guide lines on histogram...
  for (int i = 1; i <= max_pop; i *= 10) {
    const int guide_pos = log((double) i) / max_log_pop * max_hist_size + FLTK_SCALE_WIDTH + 3;
    SetColor(cColor::GRAY);
    DrawLine(guide_pos, 0, guide_pos, bar_height);
  }

  if (color_method == cCoreView_Map::COLORS_SCALE) {
    const int num_colors = cColor::SCALE_BRIGHT.GetSize();
    const int color_height = bar_height / num_colors + 1;

    for (int i = 0; i < cColor::SCALE_BRIGHT.GetSize(); i++) {
      const int cur_y = (i) * bar_height / num_colors;
      const int count = map_info.GetColorCount(i);
      const double hist_frac = (count > 0) ? log((double) count) / max_log_pop : -0.01;
      const int hist_size = hist_frac * (double) max_hist_size;
      SetColor(cColor::SCALE_BRIGHT[i]);
      DrawBox(0, cur_y, FLTK_SCALE_WIDTH + 3 + hist_size, color_height, true);
      SetColor(cColor::BLACK);
      DrawBox(0, cur_y, FLTK_SCALE_WIDTH + 3 + hist_size, color_height+1, false);
      // DrawLine(0, cur_y, 50, cur_y);
    }

    SetColor(cColor::BLACK);
    DrawBox(0, 0, 51, bar_height);
  } else {
  }
}

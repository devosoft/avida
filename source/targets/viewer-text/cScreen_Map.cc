/*
 *  cScreen_Map.cc
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

#include "cScreen_Map.h"

#include "cPopulation.h"

cScreen_Map::cScreen_Map(cCoreView_Info & info, cTextWindow & window)
  : cTextScreen(info, window)
  , m_map_info(info)
{
}

cScreen_Map::~cScreen_Map()
{
}

void cScreen_Map::Draw()
{
  m_window.Box();
  Update();
}

void cScreen_Map::Update()
{
  m_map_info.UpdateMaps();
  const tArray<int> & color_map( m_map_info.GetColors() );

  const int world_x = m_info.GetPopulation().GetWorldX();
  const int world_y = m_info.GetPopulation().GetWorldY();
  
  int id = 0;
  for (int y = 1; y <= world_y; y++) {
    for (int x = 1; x <= world_x; x++) {
      SetGenotypeColor(color_map[id]);
      m_window.Print(y, 2*x, CHAR_BULLET);
      id++;
    }
  }
}

bool cScreen_Map::DoInput(int input)
{
  return false;
}


void cScreen_Map::SetGenotypeColor(int color_id)
{
  switch (color_id) {
  case -4: m_window.SetColor(COLOR_OFF); break;
  case -3: m_window.SetBoldColor(COLOR_OFF); break;
  case -2: m_window.SetColor(COLOR_WHITE); break;
  case -1: m_window.SetBoldColor(COLOR_WHITE); break;
  case 0:  m_window.SetBoldColor(COLOR_GREEN); break;
  case 1:  m_window.SetBoldColor(COLOR_RED); break;
  case 2:  m_window.SetBoldColor(COLOR_BLUE); break;
  case 3:  m_window.SetBoldColor(COLOR_CYAN); break;
  case 4:  m_window.SetBoldColor(COLOR_YELLOW); break;
  case 5:  m_window.SetBoldColor(COLOR_MAGENTA); break;
  case 6:  m_window.SetColor(COLOR_GREEN); break;
  case 7:  m_window.SetColor(COLOR_RED); break;
  case 8:  m_window.SetColor(COLOR_BLUE); break;
  case 9:  m_window.SetColor(COLOR_CYAN); break;
  case 10: m_window.SetColor(COLOR_YELLOW); break;
  case 11: m_window.SetColor(COLOR_MAGENTA); break;
  };
}

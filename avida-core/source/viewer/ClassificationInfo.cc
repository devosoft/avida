/*
 *  viewer/ClassificationInfo.cc
 *  Avida
 *
 *  Created by Charles on 7-9-07
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>, Charles Ofria <ofria@msu.edu>
 *
 */

#include "avida/viewer/ClassificationInfo.h"

#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Group.h"
#include "avida/systematics/Manager.h"


#include "cBitArray.h"


const Apto::String Avida::Viewer::ClassificationInfo::MapColor::ObjectKey("Avida::Viewer::ClassificationInfo::MapColor");


bool Avida::Viewer::ClassificationInfo::MapColor::Serialize(ArchivePtr) const
{
  // @TODO - map color serialize
  assert(false);
  return false;
}


Avida::Viewer::ClassificationInfo::ClassificationInfo(World* in_world, const Systematics::RoleID& role, int total_colors, int threshold_colors)
  : m_world(in_world)
  , m_role(role)
  , m_color_chart_id(total_colors)
  , m_color_chart_ptr(total_colors)
  , m_threshold_colors(threshold_colors)
  , m_next_color(0)
{
  m_color_chart_id.SetAll(-1);
  m_color_chart_ptr.SetAll(Systematics::GroupPtr(NULL));
}


void Avida::Viewer::ClassificationInfo::Update()
{
  const int num_colors = m_color_chart_id.GetSize();
  cBitArray free_color(num_colors);   // Keep track of genotypes still using their color.
  free_color.SetAll();

  // Loop through all genotypes that should be colors to mark those that we can clear out.
  Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world);
  Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole((const char*)m_role)->Begin();
  int count = 0;
  while (count < num_colors && it->Next()) {
    const int cur_color = MapColorOf(it->Get())->color;
    if (cur_color >= 0) {
      assert(m_color_chart_id[cur_color] == it->Get()->ID());     // If it has a color, the color should point back to it.
      assert(m_color_chart_ptr[cur_color] == it->Get());  // ...and so should the pointer.
      free_color[cur_color] = false;
    }
    count++;
  }

  // Clear out colors for genotypes below threshold.
  while (it->Next()) {
    if (MapColorOf(it->Get())->color >= 0) MapColorOf(it->Get())->color = -1;
  }

  // Setup genotypes above threshold.
  it = classmgr->ArbiterForRole((const char*)m_role)->Begin();
  count = 0;
  while (it->Next() && count < m_threshold_colors) {
    if (MapColorOf(it->Get())->color < 0) {
      // We start with m_next_color (so we don't keep using the same set), but loop around if we need to.
      int new_color = free_color.FindBit1(m_next_color);
      if (new_color == -1) new_color = free_color.FindBit1(0);
      assert(new_color != -1);
      m_next_color = new_color + 1;
      m_color_chart_id[new_color] = it->Get()->ID();
      m_color_chart_ptr[new_color] = it->Get();
      free_color[new_color] = false;
      MapColorOf(it->Get())->color = new_color;
    }
    count++;
  }
}


Avida::Viewer::ClassificationInfo::MapColorPtr Avida::Viewer::ClassificationInfo::MapColorOf(Systematics::GroupPtr bg)
{
  MapColorPtr mc = bg->GetData<MapColor>();
  if (!mc) {
    mc = MapColorPtr(new MapColor);
    bg->AttachData(mc);
  }
  return mc;
}

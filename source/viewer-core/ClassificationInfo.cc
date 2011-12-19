/*
 *  viewer-core/ClassificationInfo.cc
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

#include "avida/private/viewer-core/ClassificationInfo.h"

#include "cBioGroup.h"
#include "cBioGroupManager.h"
#include "cBitArray.h"
#include "cPopulation.h"
#include "cWorld.h"
#include "tAutoRelease.h"
#include "tIterator.h"


Avida::CoreView::ClassificationInfo::ClassificationInfo(cWorld* in_world, const cString& role, int total_colors)
  : m_world(in_world)
  , m_role(role)
  , m_color_chart_id(total_colors, -1)
  , m_color_chart_ptr(total_colors, NULL)
  , m_threshold_colors(total_colors * 5 / 6)
  , m_next_color(0)
{
}


void Avida::CoreView::ClassificationInfo::Update()
{
  const int num_colors = m_color_chart_id.GetSize();
  cBitArray free_color(num_colors);   // Keep track of genotypes still using their color.
  free_color.SetAll();

  // Loop through all genotypes that should be colors to mark those that we can clear out.
  tAutoRelease<tIterator<cBioGroup> > it(m_world->GetClassificationManager().GetBioGroupManager(m_role)->Iterator());
  int count = 0;
  while (count < num_colors && it->Next()) {
    const int cur_color = getMapColor(it->Get())->color;
    const int cur_id = it->Get()->GetID();
    if (cur_color >= 0) {
      assert(m_color_chart_id[cur_color] == cur_id);     // If it has a color, the color should point back to it.
      assert(m_color_chart_ptr[cur_color] == it->Get());  // ...and so should the pointer.
      free_color[cur_color] = false;
    }
    count++;
  }

  // Clear out colors for genotypes below threshold.
  while (it->Next()) {
    if (getMapColor(it->Get())->color >= 0) getMapColor(it->Get())->color = -1;
  }

  // Setup genotypes above threshold.
  it.Set(m_world->GetClassificationManager().GetBioGroupManager(m_role)->Iterator());
  count = 0;
  while (it->Next() && count < m_threshold_colors) {
    if (getMapColor(it->Get())->color < 0) {
      // We start with m_next_color (so we don't keep using the same set), but loop around if we need to.
      int new_color = free_color.FindBit1(m_next_color);
      if (new_color == -1) new_color = free_color.FindBit1(0);
      assert(new_color != -1);
      m_next_color = new_color + 1;
      m_color_chart_id[new_color] = it->Get()->GetID();
      m_color_chart_ptr[new_color] = it->Get();
      free_color[new_color] = false;
      getMapColor(it->Get())->color = new_color;
    }
    count++;
  }
}


Avida::CoreView::ClassificationInfo::MapColor* Avida::CoreView::ClassificationInfo::getMapColor(cBioGroup* bg)
{
  MapColor* mc = bg->GetData<MapColor>();
  if (!mc) {
    mc = new MapColor;
    bg->AttachData(mc);
  }
  return mc;
}

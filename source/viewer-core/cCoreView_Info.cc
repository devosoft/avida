/*
 *  cCoreView_Info.cc
 *  Avida
 *
 *  Created by Charles on 7-9-07
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#include "cCoreView_Info.h"

#include "cBioGroup.h"
#include "cBioGroupManager.h"
#include "cBitArray.h"
#include "cClassificationManager.h"
#include "cPopulation.h"
#include "cWorld.h"
#include "tAutoRelease.h"
#include "tIterator.h"

cCoreView_Info::cCoreView_Info(cWorld * in_world, int total_colors)
  : m_world(in_world)
  , m_population(in_world->GetPopulation())
  , m_color_chart_id(total_colors, -1)
  , m_color_chart_ptr(total_colors, NULL)
  , m_threshold_colors(total_colors * 5 / 6)
  , m_next_color(0)
  , m_pause_level(PAUSE_OFF)
  , m_step_organism_id(-1)
  , m_step_organism_thread(-1)
{
  // Redirect standard output...
  std::cout.rdbuf(m_cout_stream.rdbuf());
  std::cerr.rdbuf(m_cerr_stream.rdbuf());
}

cCoreView_Info::~cCoreView_Info()
{
}

///////////////////////
// Helper Methods...

void cCoreView_Info::FlushOut()
{
  m_cout_list.Load(m_cout_stream.str().c_str(), '\n');  // Load the stored stream...
  m_cout_stream.str("");  // Clear the streams.
}

void cCoreView_Info::FlushErr()
{
  m_cerr_list.Load(m_cerr_stream.str().c_str(), '\n');  // Load the stored stream...
  m_cerr_stream.str("");  // Clear the streams.
}


/////////////////////////
//  Other functions...

void cCoreView_Info::SetupUpdate()
{
  const int num_colors = m_color_chart_id.GetSize();
  cBitArray free_color(num_colors);   // Keep track of genotypes still using their color.
  free_color.SetAll();

  // Loop through all genotypes that should be colors to mark those that we can clear out.
  tAutoRelease<tIterator<cBioGroup> > it(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
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
  it.Set(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
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


void cCoreView_Info::EnterStepMode(int org_id)
{
  SetPauseLevel(PAUSE_ADVANCE_INST);
  SetStepOrganism(org_id);
}

void cCoreView_Info::ExitStepMode()
{
  SetPauseLevel(PAUSE_ON);
  SetStepOrganism(-1);
}

bool cCoreView_Info::TogglePause()
{
  // If pause is off, turn it on.
  if (m_pause_level == PAUSE_OFF) {
    SetPauseLevel(PAUSE_ON);
    return true;
  }

  // Otherwise pause is on; carefully turn it off.
  if (m_pause_level == PAUSE_ADVANCE_INST) ExitStepMode();

  // Clean up any faults we may have been tracking in step mode.
//   if (info.GetActiveCell()->IsOccupied()) {
//     info.GetActiveCell()->GetOrganism()->GetPhenotype().SetFault("");
//   }
  SetPauseLevel(PAUSE_OFF);

  return false;
}


sMapColor* cCoreView_Info::getMapColor(cBioGroup* bg)
{
  sMapColor* mc = bg->GetData<sMapColor>();
  if (!mc) {
    mc = new sMapColor;
    bg->AttachData(mc);
  }
  return mc;
}

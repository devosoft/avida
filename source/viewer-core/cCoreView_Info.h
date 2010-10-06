/*
 *  cCoreView_Info.h
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

// This is a class to manage information that will be needed throughout the interface.

#ifndef cCoreView_Info_h
#define cCoreView_Info_h

// The cCoreView_Info object is responsible for holding on to all of the general information about the state of
// a population in the view.  It does not process any of this information, but allows it to be easily shared by
// all of the sections of the viewer.
//
// We're assuming that each color has an ID.  The first four are set:
//  -4 = Black
//  -3 = Dark Gray
//  -2 = Lt. Gray
//  -1 = White
//  0+ = All other available colors roughly in a smooth order for displaying fitness, etc.
//
// When coloring genotypes, we want only the top genotypes to be given a distinct color, but we want that
// color to go away if the genotype's abundance drops to low.  Each position in the color_chart will have
// a genotype ID associated with it, or -1 if that color is unused.  Each time the map is updated, all colors
// are compared to the top of the genotype list.  Each genotype tracks the ID of its color, so it is easy to
// index into the color chart to find its entry.
//
// For color scales, we should have a large scale and only use a piece of it at a time, rotating around.  This
// will prevent an organism from having its color change during its lifetime (potentially confusing the user).
// If we end up wraping around and using a color again before the old one is gone, instead of directly reusing it
// or taking other desparate measures, we should just make the color less vibrant, and eventually become gray.
// In the case of fitness, black is dead and gray is a fitness seriously below the dominant.

#include <iostream>
#include <sstream>
#include <fstream>

#include "cStringList.h"
#include "tArray.h"
#include "tList.h"

class cBioGroup;
class cPopulation;
class cWorld;

using namespace std;

struct sMapColor
{
  char color;
  
  sMapColor() : color(-1) { ; }
};


class cCoreView_Info {
protected:
  cWorld* m_world;
  cPopulation & m_population;

  // Setup streams to capture stdin and stdout so we can control them as needed.
  std::stringstream m_cout_stream;
  std::stringstream m_cerr_stream;
  
  // And string lists to convert outputs to for easier management.
  cStringList m_cout_list;
  cStringList m_cerr_list;

  // Constant Inforation setup by specific viewer.
  tArray<int> m_color_chart_id;
  tArray<cBioGroup*> m_color_chart_ptr;
  int m_threshold_colors;
  int m_next_color;

  // Variable information, changing modes based on user input.
  int m_pause_level;
  int m_step_organism_id;
  int m_step_organism_thread;
  int m_view_mode;

  // Helper Methods
  void FlushOut();  // Move stored cout to string list 'cout_list'
  void FlushErr();  // Move stored cerr to string list 'cerr_list'
  
public:
  // Constant Information across all viewers.
  enum ePause { PAUSE_ON, PAUSE_OFF, PAUSE_ADVANCE_INST, PAUSE_ADVANCE_UPDATE, PAUSE_ADVANCE_DIVIDE };

public:
  cCoreView_Info(cWorld* in_world, int total_colors);
  ~cCoreView_Info();

  cPopulation & GetPopulation() { return m_population; }
  const cPopulation & GetPopulation() const { return m_population; }

  // Generic Functions...
  void SetupUpdate();

  // Accessors for variable information
  int GetPauseLevel() const { return m_pause_level; }
  int GetStepOrganism() const { return m_step_organism_id; }
  int GetStepThread() const { return m_step_organism_thread; }
  int GetViewMode() const { return m_view_mode; }

  void SetPauseLevel(int in_level) { m_pause_level = in_level; }
  void SetStepOrganism(int in_id) { m_step_organism_id = in_id; }
  void SetStepThread(int in_thread) { m_step_organism_thread = in_thread; }
  void SetViewMode(int in_mode) { m_view_mode = in_mode; }

  // Special accessors...
  cStringList & GetOutList() { FlushOut(); return m_cout_list; }
  cStringList & GetErrList() { FlushErr(); return m_cerr_list; }

  // Other functions...
  void EnterStepMode(int org_id);
  void ExitStepMode();
  bool TogglePause();

private:
  sMapColor* getMapColor(cBioGroup* bg);
};

#endif

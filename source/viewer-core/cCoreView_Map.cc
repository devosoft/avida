/*
 *  cCoreView_Map.cc
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

#include "cCoreView_Map.h"

#include "cBioGroup.h"
#include "cCoreView_Info.h"
#include "cEnvironment.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"

cCoreView_Map::cCoreView_Map(cCoreView_Info & info)
  : m_info(info)
  , m_color_mode(0)
  , m_symbol_mode(-1)
  , m_tag_mode(-1)
  , m_scale_max(0)
{
  // Setup the available view modes...
  AddViewMode("Genotypes",      &cCoreView_Map::SetColors_Genotype, VIEW_COLOR, COLORS_TYPES);
  AddViewMode("Fitness",        &cCoreView_Map::SetColors_Fitness,  VIEW_COLOR, COLORS_SCALE);
  AddViewMode("_Genome Length", &cCoreView_Map::SetColors_Length,   VIEW_COLOR, COLORS_SCALE);
  AddViewMode("Highlight Tags", &cCoreView_Map::SetColors_Tags,     VIEW_COLOR, COLORS_TYPES);

  AddViewMode("None",           &cCoreView_Map::TagCells_None,      VIEW_TAGS);
  AddViewMode("Parasite",       &cCoreView_Map::TagCells_Parasite,  VIEW_TAGS);

  AddViewMode("Square",         &cCoreView_Map::SetSymbol_Square,   VIEW_SYMBOLS);
  AddViewMode("Facing",         &cCoreView_Map::SetSymbol_Facing,   VIEW_SYMBOLS);

  // Load tasks...
  const cEnvironment & environment = m_info.GetPopulation().GetEnvironment();
  const int num_tasks = environment.GetNumTasks();
  for (int i = 0; i < num_tasks; i++) {
    cString mode_name = environment.GetTask(i).GetDesc();
    mode_name.Insert("Task/");
    AddViewMode(mode_name, &cCoreView_Map::TagCells_Task,  VIEW_TAGS, i);
  }
}

cCoreView_Map::~cCoreView_Map()
{
  for (int i = 0; i < m_view_modes.GetSize(); i++) {
    delete m_view_modes[i];
  }
}

void cCoreView_Map::UpdateMaps()
{
  if (m_color_mode >= 0) {
    assert(m_view_modes[m_color_mode]->GetViewType() == VIEW_COLOR);
    UpdateMap(m_color_mode);
  }

  if (m_symbol_mode >= 0) {
    assert(m_view_modes[m_symbol_mode]->GetViewType() == VIEW_SYMBOLS);
    UpdateMap(m_symbol_mode);
  }

  if (m_tag_mode >= 0) {
    assert(m_view_modes[m_tag_mode]->GetViewType() == VIEW_TAGS);
    UpdateMap(m_tag_mode);
  }
}


void cCoreView_Map::SetMode(int mode)
{
  int type = m_view_modes[mode]->GetViewType();
  if (type == VIEW_COLOR) m_color_mode = mode;
  else if (type == VIEW_SYMBOLS) m_symbol_mode = mode;
  else if (type == VIEW_TAGS) m_tag_mode = mode;
  else assert(false);
}


////////////////////////
// Protected methods

void cCoreView_Map::SetColors_Genotype(int ignore)
{
  (void) ignore;
  cPopulation & pop = m_info.GetPopulation();
  m_color_grid.Resize(pop.GetSize());
  for (int i = 0; i < pop.GetSize(); i++) {
    cOrganism * org = pop.GetCell(i).GetOrganism();
    if (org == NULL) m_color_grid[i] = -4;
    else {
      sMapColor* mapcolor = org->GetBioGroup("genotype")->GetData<sMapColor>();
      if (!mapcolor) {
        mapcolor = new sMapColor;
        org->GetBioGroup("genotype")->AttachData(mapcolor);
      }
      m_color_grid[i] = mapcolor->color;
      
    }
  }
}

void cCoreView_Map::SetColors_Fitness(int ignore)
{
  (void) ignore;
  cPopulation & pop = m_info.GetPopulation();
  m_color_grid.Resize(pop.GetSize());

  // Keep track of how many times each color was assigned.
  m_color_counts.Resize(m_scale_max);
  m_color_counts.SetAll(0);

  // Determine the max and min in the population.
  double max_fit = 3;
  double min_fit = -2;

  for (int i = 0; i < pop.GetSize(); i++) {
    cOrganism * org = pop.GetCell(i).GetOrganism();
    if (org == NULL) continue;
    double fit = org->GetPhenotype().GetFitness();
    if (fit == 0.0) continue;
    fit = log(fit);
    // if (fit < min_fit) min_fit = fit;
    if (fit > max_fit) max_fit = fit;
    if (fit < min_fit) min_fit = fit;
  }
  double fit_diff = max_fit - min_fit;
  if (fit_diff == 0.0) fit_diff = 1.0;

  // Now fill out the color grid.
  for (int i = 0; i < pop.GetSize(); i++) {
    cOrganism * org = pop.GetCell(i).GetOrganism();
    if (org == NULL) {
      m_color_grid[i] = 0;
      m_color_counts[0]++;
      continue;
    }
    double fit = org->GetPhenotype().GetFitness();
    if (fit == 0.0) {
      m_color_grid[i] = 1;
      m_color_counts[1]++;
      continue;
    }
    fit = log(fit);
    m_color_grid[i] = 1 + (int) ((m_scale_max - 2) * ((fit - min_fit) / fit_diff));
    m_color_counts[m_color_grid[i]]++;
  }
}

void cCoreView_Map::SetColors_Length(int ignore)
{
  (void) ignore;
}

void cCoreView_Map::SetColors_Tags(int ignore)
{
  (void) ignore;
}

void cCoreView_Map::TagCells_None(int ignore)
{
  cPopulation & pop = m_info.GetPopulation();
  m_tag_grid.Resize(pop.GetSize());
  m_tag_grid.SetAll(0);
}

void cCoreView_Map::TagCells_Parasite(int ignore)
{
  (void) ignore;
}

void cCoreView_Map::TagCells_Task(int task_id)
{
  cPopulation & pop = m_info.GetPopulation();
  m_tag_grid.Resize(pop.GetSize());
  for (int i = 0; i < pop.GetSize(); i++) {
    cOrganism * org = pop.GetCell(i).GetOrganism();
    if (org == NULL) m_tag_grid[i] = 0;
    else if (org->GetPhenotype().GetCurTaskCount()[task_id] > 0) m_tag_grid[i] = 1;
    else if (org->GetPhenotype().GetLastTaskCount()[task_id] > 0) m_tag_grid[i] = 2;
    else m_tag_grid[i] = 0;
  }
}

void cCoreView_Map::SetSymbol_Square(int ignore)
{
  (void) ignore;
}

void cCoreView_Map::SetSymbol_Facing(int ignore)
{
  (void) ignore;
}


int cCoreView_Map::AddViewMode(const cString & name, yMethod call, eViewType type, int arg)
{
  if (m_symbol_mode == -1 && type == VIEW_SYMBOLS) m_symbol_mode = m_view_modes.GetSize();
  if (m_tag_mode == -1 && type == VIEW_TAGS) m_tag_mode = m_view_modes.GetSize();
  
  m_view_modes.Push( new cMapViewEntry(name, call, type, arg) );
  return m_view_modes.GetSize();
}

void cCoreView_Map::UpdateMap(int map_id)
{
  yMethod call = m_view_modes[map_id]->GetCall();
  int arg = m_view_modes[map_id]->GetArg();
  (this->*call)(arg);
}

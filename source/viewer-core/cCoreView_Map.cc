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
#include "cCoreView_ClassificationInfo.h"
#include "cEnvironment.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cWorld.h"


cCoreView_Map::cCoreView_Map(cWorld* world)
  : m_width(world->GetPopulation().GetWorldX())
  , m_height(world->GetPopulation().GetWorldY())
  , m_color_mode(0)
  , m_symbol_mode(-1)
  , m_tag_mode(-1)
  , m_scale_max(0)
{
  // Setup the available view modes...
  AddViewMode("Genotypes",      &cCoreView_Map::SetColors_Genotype, VIEW_COLOR, COLORS_TYPES, new cCoreView_ClassificationInfo(world, "genotype", 10));
  AddViewMode("Fitness",        &cCoreView_Map::SetColors_Fitness,  VIEW_COLOR, COLORS_SCALE);
  AddViewMode("_Genome Length", &cCoreView_Map::SetColors_Length,   VIEW_COLOR, COLORS_SCALE);
  AddViewMode("Highlight Tags", &cCoreView_Map::SetColors_Tags,     VIEW_COLOR, COLORS_TYPES);

  AddViewMode("None",           &cCoreView_Map::TagCells_None,      VIEW_TAGS);
  AddViewMode("Parasite",       &cCoreView_Map::TagCells_Parasite,  VIEW_TAGS);

  AddViewMode("Square",         &cCoreView_Map::SetSymbol_Square,   VIEW_SYMBOLS);
  AddViewMode("Facing",         &cCoreView_Map::SetSymbol_Facing,   VIEW_SYMBOLS);

  // Load tasks...
  const cEnvironment& env = world->GetEnvironment();
  const int num_tasks = env.GetNumTasks();
  for (int i = 0; i < num_tasks; i++) {
    cString mode_name = env.GetTask(i).GetDesc();
    mode_name.Insert("Task/");
    AddViewMode(mode_name, &cCoreView_Map::TagCells_Task, VIEW_TAGS, i);
  }
}

cCoreView_Map::~cCoreView_Map()
{
  for (int i = 0; i < m_view_modes.GetSize(); i++) delete m_view_modes[i];
}


cCoreView_Map::cMapViewEntry::~cMapViewEntry()
{
  delete m_class_info;
}

inline void cCoreView_Map::cMapViewEntry::Update()
{
  if (m_class_info) m_class_info->Update();
}
              
              
void cCoreView_Map::UpdateMaps(cPopulation& pop)
{
  m_rw_lock.WriteLock();
  
  if (m_color_mode >= 0) {
    assert(m_view_modes[m_color_mode]->GetViewType() == VIEW_COLOR);
    UpdateMap(pop, m_color_mode);
  }

  if (m_symbol_mode >= 0) {
    assert(m_view_modes[m_symbol_mode]->GetViewType() == VIEW_SYMBOLS);
    UpdateMap(pop, m_symbol_mode);
  }

  if (m_tag_mode >= 0) {
    assert(m_view_modes[m_tag_mode]->GetViewType() == VIEW_TAGS);
    UpdateMap(pop, m_tag_mode);
  }
  
  m_rw_lock.WriteUnlock();
}


void cCoreView_Map::SetMode(int mode)
{
  int type = m_view_modes[mode]->GetViewType();
  if (type == VIEW_COLOR) m_color_mode = mode;
  else if (type == VIEW_SYMBOLS) m_symbol_mode = mode;
  else if (type == VIEW_TAGS) m_tag_mode = mode;
  else assert(false);
}



int cCoreView_Map::AddViewMode(const cString& name, MapUpdateMethod call, eViewType type, int arg, cCoreView_ClassificationInfo* info)
{
  if (m_symbol_mode == -1 && type == VIEW_SYMBOLS) m_symbol_mode = m_view_modes.GetSize();
  if (m_tag_mode == -1 && type == VIEW_TAGS) m_tag_mode = m_view_modes.GetSize();
  
  m_view_modes.Push( new cMapViewEntry(name, call, type, arg, info) );
  return m_view_modes.GetSize();
}

void cCoreView_Map::UpdateMap(cPopulation& pop, int map_id)
{
  m_view_modes[map_id]->Update();
  MapUpdateMethod call = m_view_modes[map_id]->GetCall();
  int arg = m_view_modes[map_id]->GetArg();
  (this->*call)(pop, arg);
}

////////////////////////
// Protected methods

void cCoreView_Map::SetColors_Genotype(cPopulation& pop, int ignore)
{
  (void) ignore;
  m_color_grid.Resize(pop.GetSize());
  for (int i = 0; i < pop.GetSize(); i++) {
    cOrganism * org = pop.GetCell(i).GetOrganism();
    if (org == NULL) m_color_grid[i] = -4;
    else {
      cCoreView_ClassificationInfo::MapColor* mapcolor = org->GetBioGroup("genotype")->GetData<cCoreView_ClassificationInfo::MapColor>();
      if (mapcolor) m_color_grid[i] = mapcolor->color;
      else m_color_grid[i] = -1;
    }
  }
}

void cCoreView_Map::SetColors_Fitness(cPopulation& pop, int ignore)
{
  (void) ignore;
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

void cCoreView_Map::SetColors_Length(cPopulation& pop, int ignore)
{
  (void) ignore;
}

void cCoreView_Map::SetColors_Tags(cPopulation& pop, int ignore)
{
  (void) ignore;
}

void cCoreView_Map::TagCells_None(cPopulation& pop, int ignore)
{
  m_tag_grid.Resize(pop.GetSize());
  m_tag_grid.SetAll(0);
}

void cCoreView_Map::TagCells_Parasite(cPopulation& pop, int ignore)
{
  (void) ignore;
}

void cCoreView_Map::TagCells_Task(cPopulation& pop, int task_id)
{
  m_tag_grid.Resize(pop.GetSize());
  for (int i = 0; i < pop.GetSize(); i++) {
    cOrganism * org = pop.GetCell(i).GetOrganism();
    if (org == NULL) m_tag_grid[i] = 0;
    else if (org->GetPhenotype().GetCurTaskCount()[task_id] > 0) m_tag_grid[i] = 1;
    else if (org->GetPhenotype().GetLastTaskCount()[task_id] > 0) m_tag_grid[i] = 2;
    else m_tag_grid[i] = 0;
  }
}

void cCoreView_Map::SetSymbol_Square(cPopulation& pop, int ignore)
{
  (void) ignore;
}

void cCoreView_Map::SetSymbol_Facing(cPopulation& pop, int ignore)
{
  (void) ignore;
}

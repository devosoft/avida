/*
 *  cMap.cc
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

#include "cMap.h"

#include "cBioGroup.h"
#include "cEnvironment.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cWorld.h"

#include "cClassificationInfo.h"


Avida::CoreView::MapMode::~MapMode() { ; }
Avida::CoreView::DiscreteScale::~DiscreteScale() { ; }


class cFitnessMapMode : public Avida::CoreView::MapMode, public Avida::CoreView::DiscreteScale
{
private:
  static const int SCALE_MAX = 10;
private:
  tArray<int> m_color_grid;
  tArray<int> m_color_count;
  tArray<DiscreteScale::Entry> m_scale_labels;
  
public:
  cFitnessMapMode(cWorld* world) { ; }
  ~cFitnessMapMode() { ; }
  
  // MapMode Interface
  const cString& GetName() const { static const cString name("Fitness"); return name; }
  const tArray<int>& GetGridValues() const { return m_color_grid; }
  const tArray<int>& GetValueCounts() const { return m_color_count; }
  
  const DiscreteScale& GetScale() const { return *this; }
  
  int GetSupportedTypes() const { return Avida::CoreView::MAP_GRID_VIEW_COLOR; }

  void Update(cPopulation& pop);
  
  
  // DiscreteScale Interface
  int GetScaleRange() const { return m_color_count.GetSize(); }
  int GetNumLabeledEntries() const { return m_scale_labels.GetSize(); }
  DiscreteScale::Entry GetEntry(int index) const { return m_scale_labels[index]; }
};

void cFitnessMapMode::Update(cPopulation& pop)
{
  m_color_grid.Resize(pop.GetSize());
  
  // Keep track of how many times each color was assigned.
  m_color_count.Resize(SCALE_MAX);
  m_color_count.SetAll(0);
  
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
    cOrganism* org = pop.GetCell(i).GetOrganism();
    if (org == NULL) {
      m_color_grid[i] = 0;
      m_color_count[0]++;
      continue;
    }
    double fit = org->GetPhenotype().GetFitness();
    if (fit == 0.0) {
      m_color_grid[i] = 1;
      m_color_count[1]++;
      continue;
    }
    fit = log(fit);
    m_color_grid[i] = 1 + (int) ((SCALE_MAX - 2) * ((fit - min_fit) / fit_diff));
    m_color_count[m_color_grid[i]]++;
  }
}


class cGenotypeMapMode : public Avida::CoreView::MapMode, public Avida::CoreView::DiscreteScale
{
private:
  static const int NUM_COLORS = 10;
private:
  Avida::CoreView::cClassificationInfo* m_info;
  tArray<int> m_color_grid;
  tArray<int> m_color_count;
  tArray<DiscreteScale::Entry> m_scale_labels;
  
public:
  cGenotypeMapMode(cWorld* world)
  : m_info(new Avida::CoreView::cClassificationInfo(world, "genotype", NUM_COLORS)), m_color_count(NUM_COLORS + 4) { ; }
  virtual ~cGenotypeMapMode() { delete m_info; }
  
  // MapMode Interface
  const cString& GetName() const { static const cString name("Genotypes"); return name; }
  const tArray<int>& GetGridValues() const { return m_color_grid; }
  const tArray<int>& GetValueCounts() const { return m_color_count; }
  
  const DiscreteScale& GetScale() const { return *this; }
  
  int GetSupportedTypes() const { return Avida::CoreView::MAP_GRID_VIEW_COLOR; }
  
  void Update(cPopulation& pop);
  
  
  // DiscreteScale Interface
  int GetScaleRange() const { return m_color_count.GetSize(); }
  int GetNumLabeledEntries() const { return m_scale_labels.GetSize(); }
  DiscreteScale::Entry GetEntry(int index) const { return m_scale_labels[index]; }  
};


void cGenotypeMapMode::Update(cPopulation& pop)
{
  m_info->Update();
  m_color_grid.Resize(pop.GetSize());
  m_color_count.SetAll(0);            // reset all color counts
  for (int i = 0; i < pop.GetSize(); i++) {
    cOrganism* org = pop.GetCell(i).GetOrganism();
    if (org == NULL) {
      m_color_grid[i] = -4;
      m_color_count[0]++;
    } else {
      Avida::CoreView::cClassificationInfo::MapColor* mapcolor =
      org->GetBioGroup("genotype")->GetData<Avida::CoreView::cClassificationInfo::MapColor>();
      if (mapcolor) {
        m_color_grid[i] = mapcolor->color;
        m_color_count[mapcolor->color + 4];
      } else {
        m_color_grid[i] = -1;
        m_color_count[3]++;
      }
    }
  }
}





Avida::CoreView::cMap::cMap(cWorld* world)
  : m_width(world->GetPopulation().GetWorldX())
  , m_height(world->GetPopulation().GetWorldY())
  , m_num_viewer_colors(-1)
  , m_color_mode(0)
  , m_symbol_mode(-1)
  , m_tag_mode(-1)
{
  // Setup the available view modes...
  m_view_modes.Resize(2);
  m_view_modes[0] = new cGenotypeMapMode(world);
  m_view_modes[1] = new cFitnessMapMode(world);

  
//  AddViewMode("Genome Length",  &cCoreView_Map::SetColors_Length,   VIEW_COLOR, COLORS_SCALE);

//  AddViewMode("None",           &cCoreView_Map::TagCells_None,      VIEW_TAGS);
//
//  AddViewMode("Square",         &cCoreView_Map::SetSymbol_Square,   VIEW_SYMBOLS);
//  AddViewMode("Facing",         &cCoreView_Map::SetSymbol_Facing,   VIEW_SYMBOLS);

  // Load tasks...
//  const cEnvironment& env = world->GetEnvironment();
//  const int num_tasks = env.GetNumTasks();
//  for (int i = 0; i < num_tasks; i++) {
//    cString mode_name = env.GetTask(i).GetDesc();
//    mode_name.Insert("Task/");
//    AddViewMode(mode_name, &cCoreView_Map::TagCells_Task, VIEW_TAGS, i);
//  }
}

Avida::CoreView::cMap::~cMap()
{
  for (int i = 0; i < m_view_modes.GetSize(); i++) delete m_view_modes[i];
}



void Avida::CoreView::cMap::UpdateMaps(cPopulation& pop)
{
  m_rw_lock.WriteLock();
  
  for (int i = 0; i < m_view_modes.GetSize(); i++) m_view_modes[i]->Update(pop);
  
  m_rw_lock.WriteUnlock();
}


void Avida::CoreView::cMap::SetMode(int mode)
{
  int type = m_view_modes[mode]->GetSupportedTypes();
  if (type == MAP_GRID_VIEW_COLOR) m_color_mode = mode;
  else if (type == MAP_GRID_VIEW_SYMBOLS) m_symbol_mode = mode;
  else if (type == MAP_GRID_VIEW_TAGS) m_tag_mode = mode;
  else assert(false);
}


//
//void cCoreView_Map::TagCells_None(cPopulation& pop, int ignore)
//{
//  m_tag_grid.Resize(pop.GetSize());
//  m_tag_grid.SetAll(0);
//}
//
//
//void cCoreView_Map::TagCells_Task(cPopulation& pop, int task_id)
//{
//  m_tag_grid.Resize(pop.GetSize());
//  for (int i = 0; i < pop.GetSize(); i++) {
//    cOrganism * org = pop.GetCell(i).GetOrganism();
//    if (org == NULL) m_tag_grid[i] = 0;
//    else if (org->GetPhenotype().GetCurTaskCount()[task_id] > 0) m_tag_grid[i] = 1;
//    else if (org->GetPhenotype().GetLastTaskCount()[task_id] > 0) m_tag_grid[i] = 2;
//    else m_tag_grid[i] = 0;
//  }
//}

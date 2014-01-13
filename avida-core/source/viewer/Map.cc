/*
 *  Map.cc
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

#include "avida/viewer/Map.h"

#include "avida/viewer/ClassificationInfo.h"

#include "cEnvironment.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStringUtil.h"
#include "cWorld.h"

#include <cmath>
#include <iostream>


Avida::Viewer::MapMode::~MapMode() { ; }
Avida::Viewer::DiscreteScale::~DiscreteScale() { ; }


class DoublePropMapMode : public Avida::Viewer::MapMode, public Avida::Viewer::DiscreteScale
{
private:
  static const int SCALE_MAX = 201;
  static const int SCALE_LABELS = 4;
  static const int RESCALE_TIME_CONSTANT = 40;
  static const double RESCALE_TOLERANCE;
  static const double MAX_RESCALE_FACTOR;
private:
  const Apto::String m_prop_id;
  Apto::String m_prop_desc;
  Apto::String m_prop_desc_rescale;
  
  Apto::Array<int> m_color_grid;
  Apto::Array<int> m_color_count;
  Apto::Array<DiscreteScale::Entry> m_scale_labels;
  
  double m_cur_min;
  double m_cur_max;
  double m_target_max;
  double m_rescale_rate_min;
  double m_rescale_rate_max;
  
public:
  DoublePropMapMode(cWorld* world, const Apto::String& prop_id, const Apto::String& prop_desc)
  : m_prop_id(prop_id), m_prop_desc(prop_desc), m_color_count(SCALE_MAX + Avida::Viewer::MAP_RESERVED_COLORS), m_scale_labels(SCALE_LABELS)
  , m_cur_min(0.0), m_cur_max(0.0), m_target_max(0.0), m_rescale_rate_min(0.0), m_rescale_rate_max(0.0)
  {
    m_color_grid.Resize(world->GetPopulation().GetSize());
    m_color_grid.SetAll(-4);
    
    m_prop_desc_rescale = m_prop_desc + " (rescaling)";
  }
  ~DoublePropMapMode() { ; }
  
  const Apto::String& PropertyID() const { return m_prop_id; }
  
  
  // MapMode Interface
  const Apto::String& GetName() const { return m_prop_desc; }
  const Apto::Array<int>& GetGridValues() const { return m_color_grid; }
  const Apto::Array<int>& GetValueCounts() const { return m_color_count; }
  
  const DiscreteScale& GetScale() const { return *this; }
  const Apto::String& GetScaleLabel() const;
  
  int GetSupportedTypes() const { return Avida::Viewer::MAP_GRID_VIEW_COLOR; }
  
  bool SetProperty(const Apto::String&, const Apto::String&) { return false; }
  Apto::String GetProperty(const Apto::String&) const { return ""; }
  
  void Update(cPopulation& pop);
  
  
  // DiscreteScale Interface
  int GetScaleRange() const { return m_color_count.GetSize() - Avida::Viewer::MAP_RESERVED_COLORS; }
  int GetNumLabeledEntries() const { return m_scale_labels.GetSize(); }
  DiscreteScale::Entry GetEntry(int index) const { return m_scale_labels[index]; }
};

const double DoublePropMapMode::RESCALE_TOLERANCE = 0.1;
const double DoublePropMapMode::MAX_RESCALE_FACTOR = 0.03;

void DoublePropMapMode::Update(cPopulation& pop)
{
  m_color_grid.Resize(pop.GetSize());
  
  // Keep track of how many times each color was assigned.
  m_color_count.SetAll(0);
  
  // Determine the max and min in the population.
  double max_fit = 0.0;
  double min_fit = 0.0;
  
  for (int i = 0; i < pop.GetSize(); i++) {
    cOrganism* org = pop.GetCell(i).GetOrganism();
    if (org == NULL) continue;
    double fit = org->Properties().Get(m_prop_id);
    if (fit == 0.0) continue;
    if (fit > max_fit) max_fit = fit;
    if (fit < min_fit) min_fit = fit;
  }
  
  if (m_cur_max == 0.0) {
    // Reset range
    m_cur_max = max_fit;
    m_target_max = max_fit;
    m_rescale_rate_min = 0.0;
    m_rescale_rate_max = 0.0;
    
    // Update scale labels
    for (int i = 0; i < m_scale_labels.GetSize(); i++) {
      m_scale_labels[i].index = (SCALE_MAX / (m_scale_labels.GetSize() - 1)) * i;
      m_scale_labels[i].label =
      static_cast<const char*>(cStringUtil::Stringf("%2.2f", ((m_cur_max - m_cur_min) / (m_scale_labels.GetSize() - 1)) * i));
    }
  } else {
    if (max_fit < (1.0 - RESCALE_TOLERANCE) * m_target_max || m_target_max < max_fit) {
      m_target_max = max_fit * (1.0 + RESCALE_TOLERANCE);
      m_rescale_rate_max = (m_target_max - m_cur_max) / RESCALE_TIME_CONSTANT;
    }
    
    if (m_rescale_rate_max != 0.0) {
      if (min_fit <= m_cur_max) {
        m_cur_max += m_rescale_rate_max;
      } else {
        double max_rate = m_cur_max * MAX_RESCALE_FACTOR;
        m_cur_max += (m_rescale_rate_max < max_rate) ? m_rescale_rate_max : max_rate;
      }
      
      if (fabs(m_target_max - m_cur_max) <= fabs(m_rescale_rate_max)) {
        m_cur_max = m_target_max;
        m_rescale_rate_max = 0.0;
      }
      
      // Update scale labels
      for (int i = 0; i < m_scale_labels.GetSize(); i++) {
        m_scale_labels[i].index = (SCALE_MAX / (m_scale_labels.GetSize() - 1)) * i;
        m_scale_labels[i].label =
        static_cast<const char*>(cStringUtil::Stringf("%2.2f", ((m_cur_max - m_cur_min) / (m_scale_labels.GetSize() - 1)) * i));
      }
    }
  }
  
  // Now fill out the color grid.
  for (int i = 0; i < pop.GetSize(); i++) {
    cOrganism* org = pop.GetCell(i).GetOrganism();
    if (org == NULL) {
      m_color_grid[i] = Avida::Viewer::MAP_RESERVED_COLOR_BLACK;
      m_color_count[Avida::Viewer::MAP_RESERVED_COLORS - Avida::Viewer::MAP_RESERVED_COLOR_BLACK]++;
      continue;
    }
    
    double fit = org->Properties().Get(m_prop_id);
    if (fit == 0.0) {
      m_color_grid[i] = Avida::Viewer::MAP_RESERVED_COLOR_DARK_GRAY;
      m_color_count[Avida::Viewer::MAP_RESERVED_COLORS - Avida::Viewer::MAP_RESERVED_COLOR_DARK_GRAY]++;
      continue;
    }
    
    //    fit = log2(fit);
    
    fit = (fit - m_cur_min) / (m_cur_max - m_cur_min);
    if (fit > 1.0) {
      m_color_grid[i] = Avida::Viewer::MAP_RESERVED_COLOR_WHITE;
      m_color_count[Avida::Viewer::MAP_RESERVED_COLORS - Avida::Viewer::MAP_RESERVED_COLOR_WHITE]++;
    } else {
      int color = fit * static_cast<double>(SCALE_MAX - 1);
      m_color_grid[i] = color;
      m_color_count[color + Avida::Viewer::MAP_RESERVED_COLORS]++;
    }
  }
}

const Apto::String& DoublePropMapMode::GetScaleLabel() const
{  
  if (m_rescale_rate_max != 0) return m_prop_desc_rescale;
  
  return m_prop_desc;
}






class ClassificationMapMode : public Avida::Viewer::MapMode, public Avida::Viewer::DiscreteScale
{
private:
  static const int NUM_COLORS = 10;
private:
  const Apto::String m_role_id;
  const Apto::String m_role_desc;
  
  Avida::Viewer::ClassificationInfo* m_info;
  Apto::Array<int> m_color_grid;
  Apto::Array<int> m_color_count;
  Apto::Array<DiscreteScale::Entry> m_scale_labels;
  
public:
  ClassificationMapMode(cWorld* world, const Apto::String& role_id, const Apto::String& role_desc);
  virtual ~ClassificationMapMode() { delete m_info; }
  
  const Apto::String& RoleID() const { return m_role_id; }
  
  // MapMode Interface
  const Apto::String& GetName() const { return m_role_desc; }
  const Apto::Array<int>& GetGridValues() const { return m_color_grid; }
  const Apto::Array<int>& GetValueCounts() const { return m_color_count; }
  
  const DiscreteScale& GetScale() const { return *this; }
  const Apto::String& GetScaleLabel() const { return m_role_desc; }
  
  int GetSupportedTypes() const { return Avida::Viewer::MAP_GRID_VIEW_COLOR; }
  
  bool SetProperty(const Apto::String&, const Apto::String&) { return false; }
  Apto::String GetProperty(const Apto::String&) const { return ""; }
  
  void Update(cPopulation& pop);
  
  
  // DiscreteScale Interface
  int GetScaleRange() const { return m_color_count.GetSize() - Avida::Viewer::MAP_RESERVED_COLORS; }
  int GetNumLabeledEntries() const { return m_scale_labels.GetSize(); }
  DiscreteScale::Entry GetEntry(int index) const { return m_scale_labels[index]; }
  bool IsCategorical() const { return true; }
};

ClassificationMapMode::ClassificationMapMode(cWorld* world, const Apto::String& role_id, const Apto::String& role_desc)
: m_role_id(role_id), m_role_desc(role_desc)
, m_info(new Avida::Viewer::ClassificationInfo(world->GetNewWorld(), role_id, NUM_COLORS, NUM_COLORS))
, m_color_count(NUM_COLORS + Avida::Viewer::MAP_RESERVED_COLORS)
, m_scale_labels(NUM_COLORS + Avida::Viewer::MAP_RESERVED_COLORS)
{
  m_scale_labels[0].index = -4;
  m_scale_labels[0].label = "Unoccupied";
  m_scale_labels[1].index = -3;
  m_scale_labels[1].label = "-";
  m_scale_labels[2].index = -2;
  m_scale_labels[2].label = "-";
  m_scale_labels[3].index = -1;
  m_scale_labels[3].label = "Unassigned";
  for (int i = 4; i < m_scale_labels.GetSize(); i++) m_scale_labels[i].index = i - 4;
  m_color_grid.Resize(world->GetPopulation().GetSize());
  m_color_grid.SetAll(-4);
}

void ClassificationMapMode::Update(cPopulation& pop)
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
      Systematics::GroupPtr bg = org->SystematicsGroup(m_role_id);
      if (bg) {
        Avida::Viewer::ClassificationInfo::MapColorPtr mapcolor = bg->GetData<Avida::Viewer::ClassificationInfo::MapColor>();
        if (mapcolor) {
          m_color_grid[i] = mapcolor->color;
          m_color_count[mapcolor->color + 4]++;
          m_scale_labels[mapcolor->color + 4].label = bg->Properties().Get("name").StringValue();
          continue;
        }
      }
      m_color_grid[i] = -1;
      m_color_count[3]++;
    }
  }
  for (int i = 0; i < m_color_count.GetSize(); i++) if (m_color_count[i] == 0) m_scale_labels[i].label = "-";
}




class EnvActionMapMode : public Avida::Viewer::MapMode, public Avida::Viewer::DiscreteScale
{
private:
  cWorld* m_world;
  Apto::Array<int> m_action_grid;
  Apto::Array<Apto::Array<int> > m_raw_action_counts;
  Apto::Array<int> m_action_counts;
  Apto::Array<Apto::String> m_action_ids;
  int m_num_enabled;
  Apto::Array<bool> m_enabled_actions;
  Apto::String m_enabled_action_string;
  DiscreteScale::Entry m_scale_label_entry;
  Apto::String m_scale_label;
  const Apto::String m_name;
  
public:
  EnvActionMapMode(cWorld* world);
  ~EnvActionMapMode() { ; }
  
  // MapMode Interface
  const Apto::String& GetName() const { return m_name; }
  const Apto::Array<int>& GetGridValues() const { return m_action_grid; }
  const Apto::Array<int>& GetValueCounts() const { return m_action_counts; }
  
  const DiscreteScale& GetScale() const { return *this; }
  const Apto::String& GetScaleLabel() const { return m_scale_label; }
  
  int GetSupportedTypes() const { return Avida::Viewer::MAP_GRID_VIEW_TAGS; }
  
  bool SetProperty(const Apto::String& property, const Apto::String& value);
  Apto::String GetProperty(const Apto::String& property) const;
  
  void Update(cPopulation& pop);
  
  
  // DiscreteScale Interface
  int GetScaleRange() const { return 0; }
  int GetNumLabeledEntries() const { return 1; }
  DiscreteScale::Entry GetEntry(int) const { return m_scale_label_entry; }
  
  
private:
  void updateTagStates();
};


EnvActionMapMode::EnvActionMapMode(cWorld* world)
 : m_world(world), m_action_counts(Avida::Viewer::MAP_RESERVED_COLORS), m_name("Actions")
{
  cEnvironment& env = m_world->GetEnvironment();
  const int num_tasks = env.GetNumTasks();
  m_action_ids.Resize(num_tasks);
  m_num_enabled = 0;
  m_enabled_actions.Resize(num_tasks);
  m_enabled_actions.SetAll(false);
  m_scale_label_entry.index = 0;
  
  for (int i = 0; i < num_tasks; i++) m_action_ids[i] = env.GetTask(i).GetName();

}

bool EnvActionMapMode::SetProperty(const Apto::String& property, const Apto::String& value)
{
  if (property == "enabled_actions") {
    Apto::String vstr(value);
    Apto::Array<bool> earr(m_action_ids.GetSize());
    earr.SetAll(false);
    int num_enabled = 0;
    while (vstr.GetSize()) {
      Apto::String act = vstr.Pop(',');
      for (int i = 0; i < m_action_ids.GetSize(); i++) {
        if (m_action_ids[i] == act) {
          earr[i] = true;
          act = "";
          num_enabled++;
          break;
        }
      }
      if (act != "") return false;
    }
    m_num_enabled = num_enabled;
    m_enabled_actions = earr;
    m_enabled_action_string = value;
    updateTagStates();
    return true;
  }
  return false;
}

Apto::String EnvActionMapMode::GetProperty(const Apto::String& property) const
{
  if (property == "actions") {
    if (m_action_ids.GetSize() == 0) return "";
    Apto::String actionstr(m_action_ids[0]);
    for (int i = 1; i < m_action_ids.GetSize(); i++) actionstr += Apto::String(",") + m_action_ids[i];
    return actionstr;
  } else if (property == "enabled_actions") {
    return m_enabled_action_string;
  }
  
  return "";
}

void EnvActionMapMode::Update(cPopulation& pop)
{
  cAvidaContext ctx(&m_world->GetDriver(), m_world->GetRandom());

  m_action_grid.Resize(pop.GetSize());
  m_raw_action_counts.Resize(pop.GetSize());
  for (int i = 0; i < m_raw_action_counts.GetSize(); i++) m_raw_action_counts[i].Resize(m_action_ids.GetSize());
  m_action_counts.SetAll(0);            // reset all color counts
  
  for (int i = 0; i < pop.GetSize(); i++) {
    cOrganism* org = pop.GetCell(i).GetOrganism();
    if (org == NULL) {
      m_raw_action_counts[i].SetAll(0);
    } else {
      for (int task_id = 0; task_id < m_action_ids.GetSize(); task_id++) {
//        if (org->GetPhenotype().GetLastTaskCount()[task_id] > 0) m_raw_action_counts[i][task_id] = 1;
//        else if (org->GetPhenotype().GetCurTaskCount()[task_id] > 0) m_raw_action_counts[i][task_id] = 2;
        Systematics::GroupPtr genotype = org->SystematicsGroup("genotype");
        Systematics::GenomeTestMetricsPtr metrics(Systematics::GenomeTestMetrics::GetMetrics(m_world, ctx, genotype));
        const Apto::Array<int>& task_counts = metrics->GetTaskCounts();
        if (task_counts[task_id] > 0) m_raw_action_counts[i][task_id] = 1;
        else m_raw_action_counts[i][task_id] = 0;
      }
    }
  }
  
  updateTagStates();
}


void EnvActionMapMode::updateTagStates()
{
  if (m_num_enabled == 0) {
    m_action_grid.SetAll(-4);
    return;
  }
  for (int i = 0; i < m_action_grid.GetSize(); i++) {
    int color = -1;
    for (int task_id = 0; task_id < m_action_ids.GetSize(); task_id++) {
      if (!m_enabled_actions[task_id]) continue;  // Task disabled, so ignore value
      
      if (m_raw_action_counts[i][task_id] == 0) {  // One of the enabled tasks is not being performed, so clear tag and exit
        color = -4;
        break;
      }
      
      if (m_raw_action_counts[i][task_id] == 2) color = -3;  // One of the enabled tasks is a current task, so dim the tag
    }
    m_action_grid[i] = color;
    m_action_counts[4 + color]++;
  }
}



Avida::Viewer::Map::Map(cWorld* world)
  : m_width(world->GetPopulation().GetWorldX())
  , m_height(world->GetPopulation().GetWorldY())
  , m_num_viewer_colors(-1)
  , m_color_mode(0)
  , m_symbol_mode(-1)
  , m_tag_mode(4)
{
  // Setup the available view modes...
  m_view_modes.Resize(5);
//  m_view_modes[0] = new cGenotypeMapMode(world);
  m_view_modes[0] = new DoublePropMapMode(world, "last_fitness", "Fitness");
  m_view_modes[1] = new DoublePropMapMode(world, "last_gestation_time", "Gestation Time");
  m_view_modes[2] = new DoublePropMapMode(world, "last_metabolic_rate", "Metabolic Rate");
  m_view_modes[3] = new ClassificationMapMode(world, "clade", "Ancestor Organism");
  m_view_modes[4] = new EnvActionMapMode(world);

  
//  AddViewMode("Genome Length",  &cViewer_Map::SetColors_Length,   VIEW_COLOR, COLORS_SCALE);

//  AddViewMode("None",           &cViewer_Map::TagCells_None,      VIEW_TAGS);
//
//  AddViewMode("Square",         &cViewer_Map::SetSymbol_Square,   VIEW_SYMBOLS);
//  AddViewMode("Facing",         &cViewer_Map::SetSymbol_Facing,   VIEW_SYMBOLS);

  // Load tasks...
//  const cEnvironment& env = world->GetEnvironment();
//  const int num_tasks = env.GetNumTasks();
//  for (int i = 0; i < num_tasks; i++) {
//    cString mode_name = env.GetTask(i).GetDesc();
//    mode_name.Insert("Task/");
//    AddViewMode(mode_name, &cViewer_Map::TagCells_Task, VIEW_TAGS, i);
//  }
}

Avida::Viewer::Map::~Map()
{
  for (int i = 0; i < m_view_modes.GetSize(); i++) delete m_view_modes[i];
}

bool Avida::Viewer::Map::SetModeProperty(int idx, const Apto::String& property, const Apto::String& value)
{
  m_rw_lock.WriteLock();
  bool rval = m_view_modes[idx]->SetProperty(property, value);
  m_rw_lock.WriteUnlock();
  return rval;
}

void Avida::Viewer::Map::UpdateMaps(cPopulation& pop)
{
  m_rw_lock.WriteLock();
  
  m_width = pop.GetWorldX();
  m_height = pop.GetWorldY();
  
  for (int i = 0; i < m_view_modes.GetSize(); i++) m_view_modes[i]->Update(pop);
  
  m_rw_lock.WriteUnlock();
}


void Avida::Viewer::Map::SetMode(int mode)
{
  int type = m_view_modes[mode]->GetSupportedTypes();
  if (type == MAP_GRID_VIEW_COLOR) m_color_mode = mode;
  else if (type == MAP_GRID_VIEW_SYMBOLS) m_symbol_mode = mode;
  else if (type == MAP_GRID_VIEW_TAGS) m_tag_mode = mode;
  else assert(false);
}


//
//void cViewer_Map::TagCells_None(cPopulation& pop, int ignore)
//{
//  m_tag_grid.Resize(pop.GetSize());
//  m_tag_grid.SetAll(0);
//}
//
//
//void cViewer_Map::TagCells_Task(cPopulation& pop, int task_id)
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

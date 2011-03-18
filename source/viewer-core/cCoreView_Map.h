/*
 *  cCoreView_Map.h
 *  Avida
 *
 *  Created by Charles on 7-9-07
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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
 */

// This class helps manage the map object in the user inteface.

#ifndef cCoreView_Map_h
#define cCoreView_Map_h

#include "cRWLock.h"
#include "cString.h"
#include "tArray.h"

class cCoreView_ClassificationInfo;
class cPopulation;
class cWorld;



class cCoreView_MapMode
{
public:
  cCoreView_MapMode() { ; }
  virtual ~cCoreView_MapMode() = 0;
  
  virtual void Update();
  virtual const tArray<int>& GetGridValues() const;
  
  
  virtual const cString& GetName() const;
};



class cCoreView_Map
{
protected:
  typedef void (cCoreView_Map::*MapUpdateMethod)(cPopulation& pop, int arg);

public:
  enum eViewType { VIEW_COLOR, VIEW_SYMBOLS, VIEW_TAGS };
  enum eColorType { COLORS_TYPES, COLORS_SCALE };
  struct ScaleEntry {
    int color_index;
    cString label;
  };

protected:
  int m_width;
  int m_height;
  int m_num_viewer_colors;
  
  class cMapViewEntry;

  tArray<cMapViewEntry*> m_view_modes;  // List of view modes...
  int m_color_mode;      // Current map color mode (index into m_view_modes, -1 = off)
  int m_symbol_mode;     // Current map symbol mode (index into m_view_modes, -1 = off)
  int m_tag_mode;        // Current map tag mode (index into m_view_modes, -1 = off)

  tArray<int> m_color_grid;   // This maintains the colors in the current grid.
  tArray<int> m_symbol_grid;  // Should we have special symbols at each cell?
  tArray<int> m_tag_grid;     // Track tagged cells.

  tArray<int> m_color_counts; // A count of how many cells are of each color.
  
  tArray<ScaleEntry> m_scale_entries;
  
  
  cRWLock m_rw_lock;
  

public:
  cCoreView_Map(cWorld* world);
  ~cCoreView_Map();
  
  
  inline int GetWidth() { return m_width; }
  inline int GetHeight() { return m_height; }

  inline int GetColorMode() const { return m_color_mode; }
  inline int GetSymbolMode() const { return m_symbol_mode; }
  inline int GetTagMode() const { return m_tag_mode; }

  inline const tArray<int>& GetColors() const { return m_color_grid; }
  inline const tArray<int>& GetSymbols() const { return m_symbol_grid; }
  inline const tArray<int>& GetTags() const { return m_tag_grid; }

  inline const tArray<int>& GetColorCounts() const { return m_color_counts; }
  inline int GetColorCount(int idx) const { return m_color_counts[idx]; }

  inline int GetNumModes() const { return m_view_modes.GetSize(); }
  inline const cString& GetModeName(int id) const { return m_view_modes[id]->GetName(); }
  inline int GetModeType(int id) const { return m_view_modes[id]->GetViewType(); }
  inline int GetModeArg(int id) const { return m_view_modes[id]->GetArg(); }

  void SetMode(int mode);
  inline void SetNumViewerColors(int num_colors) { m_num_viewer_colors = num_colors; }
  
  inline void Retain() { m_rw_lock.ReadLock(); }
  inline void Release() { m_rw_lock.ReadUnlock(); }
  

  // Core Viewer Internal Methods
  void UpdateMaps(cPopulation& pop);
  

protected:
  int AddViewMode(const cString& name, MapUpdateMethod call, eViewType type, int arg = 0,
                  cCoreView_ClassificationInfo* info = NULL);
  
  void UpdateMap(cPopulation& pop, int map_id);

  
  void SetColors_Genotype(cPopulation& pop, int ignore);
  void SetColors_Fitness(cPopulation& pop, int ignore);
  void SetColors_Length(cPopulation& pop, int ignore);
  
  void TagCells_None(cPopulation& pop, int ignore);
  void TagCells_Task(cPopulation& pop, int task_id);
  
  void SetSymbol_Square(cPopulation& pop, int ignore);
  void SetSymbol_Facing(cPopulation& pop, int ignore);

  
};

#endif

/*
 *  cCoreView_Map.h
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

// This class helps manage the map object in the user inteface.

#ifndef cCoreView_Map_h
#define cCoreView_Map_h

#ifndef cString_h
#include "cString.h"
#endif

#ifndef tArray_h
#include "tArray.h"
#endif

class cCoreView_Info;
class cPopulationCell;

class cCoreView_Map {
public:
  typedef void (cCoreView_Map::*yMethod)(int arg);
  enum eViewType { VIEW_COLOR, VIEW_SYMBOLS, VIEW_TAGS };
  enum eColorType { COLORS_TYPES, COLORS_SCALE };

protected:
  class cMapViewEntry {
  private:
    const cString m_name;
    const yMethod m_call;
    const eViewType m_type;
    const int m_arg;
  public:
    cMapViewEntry(const cString & name, yMethod call, eViewType type, int arg=0)
      : m_name(name), m_call(call), m_type(type), m_arg(arg) { ; }
    ~cMapViewEntry() { ; }
    const cString & GetName() const { return m_name; }
    yMethod GetCall() const { return m_call; }
    int GetViewType() const { return (int) m_type; }
    int GetArg() const { return m_arg; }
  };

  cCoreView_Info & m_info;

  tArray<cMapViewEntry *> m_view_modes;  // List of view modes...
  int m_color_mode;      // Current map color mode (index into m_view_modes, -1 = off)
  int m_symbol_mode;     // Current map symbol mode (index into m_view_modes, -1 = off)
  int m_tag_mode;        // Current map tag mode (index into m_view_modes, -1 = off)

  tArray<int> m_color_grid;     // This maintains the colors in the current grid.
  tArray<int> m_symbol_grid;    // Should we have special symbols at each cell?
  tArray<int> m_tag_grid;         // Track tagged cells.

  tArray<int> m_color_counts;     // A count of how many cells are of each color.
  tArray<cString> m_color_labels; // Labels for each color.

  int m_scale_max;

  void SetColors_Genotype(int ignore);
  void SetColors_Fitness(int ignore);
  void SetColors_Length(int ignore);
  void SetColors_Tags(int ignore);
  
  void TagCells_None(int ignore);
  void TagCells_Parasite(int ignore);
  void TagCells_Task(int task_id);

  void SetSymbol_Square(int ignore);
  void SetSymbol_Facing(int ignore);

  int AddViewMode(const cString & name, yMethod call, eViewType type, int arg=0);

  void UpdateMap(int map_id);

public:
  cCoreView_Map(cCoreView_Info & info);
  ~cCoreView_Map();

  int GetColorMode() const { return m_color_mode; }
  int GetSymbolMode() const { return m_symbol_mode; }
  int GetTagMode() const { return m_tag_mode; }

  const tArray<int> & GetColors() const { return m_color_grid; }
  const tArray<int> & GetSymbols() const { return m_symbol_grid; }
  const tArray<int> & GetTags() const { return m_tag_grid; }

  const tArray<int> & GetColorCounts() const { return m_color_counts; }

  int GetColorCount(int i) const { return m_color_counts[i]; }

  int GetNumModes() const { return m_view_modes.GetSize(); }
  const cString & GetModeName(int id) const { return m_view_modes[id]->GetName(); }
  int GetModeType(int id) const { return m_view_modes[id]->GetViewType(); }
  int GetModeArg(int id) const { return m_view_modes[id]->GetArg(); }

  void UpdateMaps();

  void SetMode(int mode);
  void SetScaleMax(int in_max) { m_scale_max = in_max; }
};

#endif

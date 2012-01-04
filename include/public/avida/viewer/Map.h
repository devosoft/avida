/*
 *  viewer/Map.h
 *  avida-core
 *
 *  Created by Charles on 7-9-07
 *  Copyright 2007-2011 Michigan State University. All rights reserved.
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
 */

#ifndef AvidaViewerMap_h
#define AvidaViewerMap_h

#include "apto/core.h"

class cPopulation;
class cWorld;


namespace Avida {
  namespace Viewer {
    
    // Map Mode/Type Enumerations
    // --------------------------------------------------------------------------------------------------------------  
    
    enum MapGridViewType { MAP_GRID_VIEW_COLOR = 0x1, MAP_GRID_VIEW_SYMBOLS = 0x2, MAP_GRID_VIEW_TAGS = 0x4 };
    enum MapGridViewReservedColors {
      MAP_RESERVED_COLOR_BLACK = -4,
      MAP_RESERVED_COLOR_DARK_GRAY = -3,
      MAP_RESERVED_COLOR_LIGHT_GRAY = -2,
      MAP_RESERVED_COLOR_WHITE = -1,
      MAP_RESERVED_COLORS = 4
    };

    
    // DiscreteScale Base Clase Definition
    // --------------------------------------------------------------------------------------------------------------  
    
    class DiscreteScale
    {
    public:
      struct Entry {
        int index;
        Apto::String label;
      };
      
    public:
      virtual ~DiscreteScale() = 0;
      
      virtual int GetScaleRange() const = 0;
      virtual int GetNumLabeledEntries() const = 0;
      virtual Entry GetEntry(int index) const = 0;
      virtual bool IsCategorical() const { return false; }
    };
    
    
    // MapMode Base Class Definition
    // --------------------------------------------------------------------------------------------------------------  
    
    class MapMode
    {
    public:
      virtual ~MapMode() = 0;
      
      virtual const Apto::String& GetName() const = 0;
      virtual const Apto::Array<int>& GetGridValues() const = 0;
      virtual const Apto::Array<int>& GetValueCounts() const = 0;
      
      virtual const DiscreteScale& GetScale() const = 0;
      virtual const Apto::String& GetScaleLabel() const = 0;
      
      virtual int GetSupportedTypes() const = 0;
      
      virtual bool SetProperty(const Apto::String& property, const Apto::String& value) = 0;
      virtual Apto::String GetProperty(const Apto::String& property) const = 0;
      
      virtual void Update(cPopulation& pop) = 0;
    };
    
    
    // Map Definition
    // --------------------------------------------------------------------------------------------------------------  
    
    class Map
    {
    protected:
      int m_width;
      int m_height;
      int m_num_viewer_colors;
      
      Apto::Array<MapMode*> m_view_modes;  // List of view modes...
      int m_color_mode;      // Current map color mode (index into m_view_modes, -1 = off)
      int m_symbol_mode;     // Current map symbol mode (index into m_view_modes, -1 = off)
      int m_tag_mode;        // Current map tag mode (index into m_view_modes, -1 = off)
      
      Apto::RWLock m_rw_lock;
      
      
    public:
      Map(cWorld* world);
      ~Map();
      
      
      inline int GetWidth() const { return m_width; }
      inline int GetHeight() const { return m_height; }
      
      
      inline int GetColorMode() const { return m_color_mode; }
      inline int GetSymbolMode() const { return m_symbol_mode; }
      inline int GetTagMode() const { return m_tag_mode; }
      
      
      inline const Apto::Array<int>& GetColors() const { return m_view_modes[m_color_mode]->GetGridValues(); }
      inline const Apto::Array<int>& GetSymbols() const { return m_view_modes[m_symbol_mode]->GetGridValues(); }
      inline const Apto::Array<int>& GetTags() const { return m_view_modes[m_tag_mode]->GetGridValues(); }
      
      inline const Apto::Array<int>& GetColorCounts() const { return m_view_modes[m_color_mode]->GetValueCounts(); }
      inline const Apto::Array<int>& GetSymbolCounts() const { return m_view_modes[m_symbol_mode]->GetValueCounts(); }
      inline const Apto::Array<int>& GetTagCounts() const { return m_view_modes[m_tag_mode]->GetValueCounts(); }
      
      inline const DiscreteScale& GetColorScale() const { return m_view_modes[m_color_mode]->GetScale(); }
      inline const DiscreteScale& GetSymbolScale() const { return m_view_modes[m_symbol_mode]->GetScale(); }
      inline const DiscreteScale& GetTagScale() const { return m_view_modes[m_tag_mode]->GetScale(); }
      
      inline const Apto::String& GetColorScaleLabel() const { return m_view_modes[m_color_mode]->GetScaleLabel(); }
      inline const Apto::String& GetSymbolScaleLabel() const { return m_view_modes[m_symbol_mode]->GetScaleLabel(); }
      inline const Apto::String& GetTagScaleLabel() const { return m_view_modes[m_tag_mode]->GetScaleLabel(); }
      
      inline int GetNumModes() const { return m_view_modes.GetSize(); }
      inline const Apto::String& GetModeName(int idx) const { return m_view_modes[idx]->GetName(); }
      inline int GetModeSupportedTypes(int idx) const { return m_view_modes[idx]->GetSupportedTypes(); }
      bool SetModeProperty(int idx, const Apto::String& property, const Apto::String& value);
      inline Apto::String GetModeProperty(int idx, const Apto::String& property) const { return m_view_modes[idx]->GetProperty(property); }
      
      void SetMode(int mode);
      inline void SetNumViewerColors(int num_colors) { m_num_viewer_colors = num_colors; }
      
      
      inline void Retain() { m_rw_lock.ReadLock(); }
      inline void Release() { m_rw_lock.ReadUnlock(); }
      
      
      // Core Viewer Internal Methods
      void UpdateMaps(cPopulation& pop);
      
      
    protected:
      void updateMap(cPopulation& pop, int map_id);
    };
    
  };
};




#endif

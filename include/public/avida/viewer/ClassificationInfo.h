/*
 *  viewer/ClassificationInfo.h
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
 *
 */

#ifndef AvidaViewerClassificationInfo_h
#define AvidaViewerClassificationInfo_h

// The ClassificationInfo object is responsible for holding on to all of the general information about the state of
// a specified classification role for the population in the view.  It does not process any of this information, rather
// allows it to be easily shared by multiple sections of the viewer.
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

#include "avida/systematics/Group.h"


namespace Avida {
  namespace Viewer {

    // ClassificationInfo Definition
    // --------------------------------------------------------------------------------------------------------------  
    
    class ClassificationInfo
    {
    protected:
      World* m_world;
      Systematics::RoleID m_role;
      
      // Constant Inforation setup by specific viewer.
      Apto::Array<int> m_color_chart_id;
      Apto::Array<Systematics::GroupPtr> m_color_chart_ptr;
      int m_threshold_colors;
      int m_next_color;
    
    public:
      struct MapColor : public Systematics::GroupData
      {
        static const Apto::String ObjectKey;
        
        char color;
        
        MapColor() : color(-1) { ; }
        ~MapColor() { ; }
        
        bool Serialize(ArchivePtr ar) const;
      };
      typedef Apto::SmartPtr<MapColor> MapColorPtr;
      
    public:
      LIB_EXPORT ClassificationInfo(World* in_world, const Systematics::RoleID& role, int total_colors, int threshold_colors = -1);
      LIB_EXPORT ~ClassificationInfo() { ; }
      
      LIB_EXPORT void Update();      
      
      LIB_EXPORT static MapColorPtr MapColorOf(Systematics::GroupPtr bg);
    };
    
  };
};

#endif

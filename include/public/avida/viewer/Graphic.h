/*
 *  viewer/Graphic.h
 *  Avida
 *
 *  Created by David on 3/21/12.
 *  Copyright 2012 Michigan State University. All rights reserved.
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
 *  Authors: David M. Bryson <david@programerror.com>, Charles Ofria <charles.ofria@gmail.com>
 *
 */

#ifndef AvidaViewerGraphic_h
#define AvidaViewerGraphic_h

#include "avida/viewer/Color.h"
#include "avida/viewer/Types.h"

namespace Avida {
  namespace Viewer {
    
    // GraphicObject
    // --------------------------------------------------------------------------------------------------------------  
    
    struct GraphicObject
    {
    public:
      // Shape
      GraphicShape shape;
      
      // Origin
      float x, y;           // 1.0 = approximately 1 inch, unzoomed (72 pixels); screen center = (0,0)
      
      
      union {
        // Bounding box (SHAPE_OVAL, SHAPE_RECT)
        struct {
          float width;
          float height;
        };
        // Second point (SHAPE_CURVE)
        struct {
          float x2;
          float y2;
        };
      };
  
  
      // Shape specific details
      union {
        struct {  // For SHAPE_OVAL
          float start_angle;  // In radians (0 to 2PI); both zero indicate full circle.
          float end_angle;
        };
        struct {  // For SHAPE_RECT
          float x_round;      // Radius for rounded corners.  Zero inidcated square corners.
          float y_round;
        };
        struct {  // For SHAPE_CURVE
          float ctrl_x;
          float ctrl_y;
          float ctrl_x2;
          float ctrl_y2;
        };
      };
      
      Color fill_color;
      
      // Line
      float line_width;    // 1.0 is standard 1px
      Color line_color;
      
      // Label
      Apto::String label;
      float font_size;     // Relative value; 1.0 is default.
      Color label_color;
      
      // Is this graphic object an active region?
      int active_region_id;  // -1 is not active
      
    public:
      LIB_EXPORT inline GraphicObject(float _x, float _y, float _width, float _height, GraphicShape _shape=SHAPE_NONE)
        : shape(_shape), x(_x), y(_y), width(_width), height(_height)
        , ctrl_x(0.0), ctrl_y(0.0), ctrl_x2(0.0), ctrl_y2(0.0)
        , fill_color(Color::NONE()), line_width(1.0), line_color(Color::NONE())
        , font_size(1.0), label_color(Color::BLACK()), active_region_id(-1)
      { ; }
      LIB_EXPORT inline ~GraphicObject() { ; }
    };


    // Graphic
    // --------------------------------------------------------------------------------------------------------------
    //
    // Collection of graphic objects that represent the layout of a particular graphic scene.

    class Graphic
    {
    private:
      Apto::Array<GraphicObject*, Apto::Smart> m_objects;
      
    public:
      LIB_EXPORT inline Graphic() { ; }
      LIB_EXPORT inline ~Graphic() { for (int i = 0; i < m_objects.GetSize(); i++) delete m_objects[i]; }
      
      LIB_EXPORT inline void AddObject(GraphicObject* obj) { m_objects.Push(obj); }
      
      LIB_EXPORT inline int NumObjects() const { return m_objects.GetSize(); }
      LIB_EXPORT inline const GraphicObject& Object(int idx) const { return *m_objects[idx]; }
    };


  };
};

#endif

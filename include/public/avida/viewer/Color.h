/*
 *  viewer/Color.h
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaViewerColor_h
#define AvidaViewerColor_h

namespace Avida {
  namespace Viewer {    

    // Color
    // --------------------------------------------------------------------------------------------------------------
    //
    // Basic structure for communicating sRGBa color information.
    
    struct Color
    {
      float r, g, b, a;  // Red, Green, Blue, and Alpha (opacity), all 0.0 to 1.0.
      
      Color(float _r = 0.0, float _g = 0.0, float _b = 0.0, float _a=0.0) : r(_r), g(_g), b(_b), a(_a) { ; }
      
      static inline Color NONE()     { return Color(0.0, 0.0, 0.0, 0.0); }
      static inline Color BLACK()    { return Color(0.0, 0.0, 0.0, 1.0); }
      static inline Color DARKGRAY() { return Color(1.0/3.0, 1.0/3.0, 1.0/3.0, 1.0); }
      static inline Color WHITE()    { return Color(1.0, 1.0, 1.0, 1.0); }
      static inline Color RED()      { return Color(1.0, 0.0, 0.0, 1.0); }
      static inline Color GREEN()    { return Color(0.0, 1.0, 0.0, 1.0); }
      static inline Color BLUE()     { return Color(0.0, 0.0, 1.0, 1.0); }
      static inline Color YELLOW()   { return Color(1.0, 1.0, 0.0, 1.0); }
      
      static Color WithHSV(float h, float s, float v, float a = 1.0);
    };
    
  };
};

#endif

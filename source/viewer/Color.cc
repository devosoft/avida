/*
 *  viewer/Color.cc
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

#include "avida/viewer/Color.h"

#include <cmath>


Avida::Viewer::Color Avida::Viewer::Color::WithHSV(float h, float s, float v, float a)
{
  int i;
	float f, p, q, t;
	if (s == 0) {
		// achromatic (grey)
		// r = g = b = v;
		return Color(v, v, v, a);
	}
	h /= 60;        // sector 0 to 5
	i = floor(h);
	f = h - i;      // factorial part of h
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	switch (i) {
		case 0:  return Color(v, t, p, a);
		case 1:  return Color(q, v, p, a);
		case 2:  return Color(p, v, t, a);
		case 3:  return Color(p, q, v, a);
		case 4:  return Color(t, p, v, a);
		default:	 return Color(v, p, q, a);
	}
}

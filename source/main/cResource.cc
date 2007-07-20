/*
 *  cResource.cc
 *  Avida
 *
 *  Called "resource.cc" prior to 12/5/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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
 *  along with this program; if not, write to the Free Software Foundation, 
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cResource.h"

#include "nGeometry.h"

using namespace std;

cCellResource::cCellResource()
  : cell_id(-99)
  , initial(0.0)
  , inflow(0.0)
  , outflow(0.0)
{
}

cCellResource::cCellResource(int _cell_id, double _initial, double _inflow, 
                             double _outflow)
  : cell_id(_cell_id)
  , initial(_initial)
  , inflow(_inflow)
  , outflow(_outflow)
{
}
  
cResource::cResource(const cString & _name, int _id)
  : name(_name)
  , id(_id)
  , initial(0.0)
  , inflow(0.0)
  , outflow(0.0)
  , geometry(nGeometry::GLOBAL)
  , inflowX1(-99)
  , inflowX2(-99)
  , inflowY1(-99)
  , inflowY2(-99)
  , outflowX1(-99)
  , outflowX2(-99)
  , outflowY1(-99)
  , outflowY2(-99)
  , xdiffuse(1.0)
  , xgravity(0.0)
  , ydiffuse(1.0)
  , ygravity(0.0)
  , deme_resource(false)
{
}

bool cResource::SetGeometry(cString _geometry)

/* Set the geometry for the resource */

{
     _geometry.ToLower();
     if (_geometry == "global") {
          geometry = nGeometry::GLOBAL;
          return true;
     } else if (_geometry == "grid") {
          geometry = nGeometry::GRID;
          return true;
     } else if (_geometry == "torus") {
          geometry = nGeometry::TORUS;
          return true;
     } else {
          return false;
     }
}

bool cResource::SetDemeResource(cString _deme_resource)

/* Set if the resource is going to be accessable by demes */

{
  _deme_resource.ToLower();
  if ((_deme_resource == "false") || (_deme_resource = "0")) {
    deme_resource = false;
    return(true);
  } else if ((_deme_resource == "true") || (_deme_resource = "1")) {
    deme_resource = true;
    return(true);
  } else {
    return false;
  }
}

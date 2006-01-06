/*
 *  cResource.cc
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 */

#include "cResource.h"

#include "nGeometry.h"

using namespace std;


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
{
}

bool cResource::SetGeometry(cString _geometry)
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

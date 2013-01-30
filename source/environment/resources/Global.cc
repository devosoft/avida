/*
 *  environment/resources/Global.cc
 *  avida-core
 *
 *  Created by David on 1/25/13.
 *  Copyright 2013 Michigan State University. All rights reserved.
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

#include "avida/private/environment/resources/Global.h"

#include "avida/environment/Library.h"


static const double UPDATE_STEP(1.0 / 10000.0);
static const double EPSILON(1.0e-15);
static const int PRECALC_DISTANCE(100);


Avida::Environment::Resources::Global::Global(ResourceDefinition& def) : Resource(def) { ; }

Avida::Environment::Resources::Global::~Global() { ; }


void Avida::Environment::Resources::Global::Initialize()
{
  static Util::ArgSchema schema;
  
  schema.Define("initial", 0.0);
  schema.Define("inflow", 0.0);
  
  Library::Instance().RegisterResourceType("global", schema, Create);
}


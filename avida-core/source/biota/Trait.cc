/*
 *  biota/Trait.cc
 *  avida-core
 *
 *  Created by David on 4/9/14.
 *  Copyright 2014 Michigan State University. All rights reserved.
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

#include "avida/biota/Trait.h"

Avida::Biota::Trait::~Trait() { ; }


bool Avida::Biota::Trait::ValidateReproduction(Context& ctx, const Genome& proposed_genome)
{
  (void)ctx;
  (void)proposed_genome;
  return true;
}

void Avida::Biota::Trait::SetupOffspringGenome(Genome& genome)
{
  (void)genome;
}

void Avida::Biota::Trait::NotifyOrganismEvent(OrganismEvent event_type)
{
  (void)event_type;
}

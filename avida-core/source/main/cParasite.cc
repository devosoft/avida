/*
 *  cParasite.cc
 *  Avida
 *
 *  Created by David on 12/16/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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
 */

#include "cParasite.h"

#include "avida/systematics/Group.h"

#include "cHardwareManager.h"
#include "cInstSet.h"


cParasite::cParasite(cWorld* world, const Avida::Genome& genome, int parent_generation, Systematics::Source src)
  : m_src(src), m_initial_genome(genome)
  , m_phenotype(world, parent_generation, world->GetHardwareManager().GetInstSet(genome.Properties().Get("instset").StringValue()).GetNumNops())
{
  // @TODO - properly construct cPhenotype
  // @TODO - construct parasite property map...
}

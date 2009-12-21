/*
 *  cParasite.cc
 *  Avida
 *
 *  Created by David on 12/16/09.
 *  Copyright 2009 Michigan State University. All rights reserved.
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cParasite.h"

cParasite::cParasite(cWorld* world, const cMetaGenome& genome, int parent_generation, eBioUnitSource src, const cString& src_args)
  : m_src(src), m_src_args(src_args), m_initial_genome(genome), m_phenotype(world, parent_generation)
{
  // @TODO - properly construct cPhenotype
}

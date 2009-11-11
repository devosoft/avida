/*
 *  cBGGenotype.cc
 *  Avida
 *
 *  Created by David on 11/5/09.
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

#include "cBGGenotype.h"


cBGGenotype::cBGGenotype(cBGGenotypeManager* mgr, int in_id, cBioUnit* founder, int update, tArray<cBioGroup*>* parents)
  : m_mgr(mgr)
  , m_src(founder->GetUnitSource())
  , m_genome(founder->GetMetaGenome())
  , m_name("001-no_name")
  , m_threshold(false)
  , m_active(true)
  , m_id(in_id)
  , m_update_born(update)
  , m_update_deactivated(-1)
  , m_depth(0)
  , m_active_offspring_genotypes(0)
{
  if (parents) {
    m_parents.Resize(parents->GetSize());
    for (int i = 0; i < m_parents.GetSize(); i++) {
      m_parents[i] = (*parents)[i];
      m_parents[i]->AddReference();
    }
  }
  if (m_parents.GetSize()) m_depth = m_parents[0]->GetDepth() + 1;
}

cBGGenotype::~cBGGenotype()
{
  for (int i = 0; i < m_parents.GetSize(); i++) m_parents[i]->RemoveReference();
}

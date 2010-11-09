/*
 *  cBirthDemeHandler.cc
 *  Avida
 *
 *  Created by David Bryson on 4/1/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#include "cBirthDemeHandler.h"

#include "cBirthGenomeSizeHandler.h"
#include "cBirthGlobalHandler.h"
#include "cBirthGridLocalHandler.h"
#include "cBirthMateSelectHandler.h"
#include "cBirthNeighborhoodHandler.h"
#include "cOrganism.h"
#include "cWorld.h"


cBirthDemeHandler::cBirthDemeHandler(cWorld* world, cBirthChamber* bc)
{
  int num_demes = world->GetConfig().NUM_DEMES.Get(); 
  m_deme_handlers.Resize(num_demes);
  
  const int birth_method = world->GetConfig().BIRTH_METHOD.Get();
  for (int i = 0; i < num_demes; i++) {
    if (birth_method < NUM_LOCAL_POSITION_OFFSPRING || birth_method == POSITION_OFFSPRING_PARENT_FACING) { 
      // ... else check if the birth method is one of the local ones... 
      if (world->GetConfig().LEGACY_GRID_LOCAL_SELECTION.Get()) {
        m_deme_handlers[i] = new cBirthGridLocalHandler(world, bc);
      } else {
        m_deme_handlers[i] = new cBirthNeighborhoodHandler(world, bc);
      }
    } else if (world->GetConfig().SAME_LENGTH_SEX.Get() != 0) {
      // ... else check if recombination must be with organisms of the same length
      m_deme_handlers[i] = new cBirthGenomeSizeHandler(bc);
    } else if (world->GetConfig().ALLOW_MATE_SELECTION.Get()) {
      // ... else check if we have mate selection
      m_deme_handlers[i] = new cBirthMateSelectHandler(bc);
    } else {
      // If everything failed until this point, use default global.
      m_deme_handlers[i] = new cBirthGlobalHandler(bc);
    }
  }
}

cBirthEntry* cBirthDemeHandler::SelectOffspring(cAvidaContext& ctx, const cGenome& offspring, cOrganism* parent)
{
  return m_deme_handlers[parent->GetDemeID()]->SelectOffspring(ctx, offspring, parent);
}

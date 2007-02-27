/*
 *  cSpeciesControl.cc
 *  Avida
 *
 *  Called "species_control.cc" prior to 11/30/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#include "cSpeciesControl.h"

#include "defs.h"
#include "cGenotype.h"
#include "nSpecies.h"
#include "cSpecies.h"
#include "cStats.h"
#include "cWorld.h"

#include <cassert>

void cSpeciesControl::Remove(cSpecies & in_species)
{
  switch (in_species.GetQueueType()) {
  case nSpecies::QUEUE_ACTIVE:
    active_queue.Remove(in_species);
    break;
  case nSpecies::QUEUE_INACTIVE:
    inactive_queue.Remove(in_species);
    break;
  case nSpecies::QUEUE_GARBAGE:
    garbage_queue.Remove(in_species);
    break;
  default:
    break;
  }

  in_species.SetQueueType(nSpecies::QUEUE_NONE);
}

void cSpeciesControl::Adjust(cSpecies & in_species)
{
  // Only adjust if this species is in the active queue.

  if (in_species.GetQueueType() == nSpecies::QUEUE_ACTIVE) {
    active_queue.Adjust(in_species);
  }
}

void cSpeciesControl::SetActive(cSpecies & in_species)
{
  Remove(in_species);
  active_queue.InsertRear(in_species);
  in_species.SetQueueType(nSpecies::QUEUE_ACTIVE);
}

void cSpeciesControl::SetInactive(cSpecies & in_species)
{
  Remove(in_species);
  inactive_queue.InsertRear(in_species);
  in_species.SetQueueType(nSpecies::QUEUE_INACTIVE);
}

void cSpeciesControl::SetGarbage(cSpecies & in_species)
{
  Remove(in_species);
  garbage_queue.InsertRear(in_species);
  in_species.SetQueueType(nSpecies::QUEUE_GARBAGE);
}

bool cSpeciesControl::OK()
{
  int ret_value = true;

  // Check the queues.

  assert (active_queue.OK(nSpecies::QUEUE_ACTIVE));
  assert (inactive_queue.OK(nSpecies::QUEUE_INACTIVE));
  assert (garbage_queue.OK(nSpecies::QUEUE_GARBAGE));

  return ret_value;
}

int cSpeciesControl::FindPos(cSpecies & in_species, int max_depth)
{
  cSpecies * temp_species = active_queue.GetFirst();
  if (max_depth < 0 || max_depth > active_queue.GetSize()) {
    max_depth = active_queue.GetSize();
  }

  for (int i = 0; i < max_depth; i++) {
    if (temp_species == &in_species) return i;
    temp_species = temp_species->GetNext();
  }

  return -1;
}

cSpecies * cSpeciesControl::Find(cGenotype & in_genotype, int record_level)
{
  cSpecies * found_species = NULL;
  int cur_count, best_count = MAX_CREATURE_SIZE;
  cSpecies * cur_species;
  const int species_threshold = m_world->GetConfig().SPECIES_THRESHOLD.Get();

  if (record_level == nSpecies::RECORD_FULL) {
    cur_species = active_queue.GetFirst();
    int size = active_queue.GetSize();
    for (int i = 0; i < size; i++) {
      cur_count = cur_species->Compare(in_genotype.GetGenome(), species_threshold);
      if (cur_count != -1 && cur_count <= species_threshold && cur_count < best_count) {
        found_species = cur_species;
        best_count = cur_count;
      }
      cur_species = cur_species->GetNext();
    }
  }

  if (record_level == nSpecies::RECORD_LIMITED) {
    cur_species = in_genotype.GetSpecies();

    if (cur_species) {
      int num_diff = cur_species->Compare(in_genotype.GetGenome(), species_threshold);
      if (num_diff != -1 && num_diff <= species_threshold) {
        found_species = cur_species;
      }
    }
  }

  return found_species;
}

void cSpeciesControl::Purge(cStats & stats)
{
  cSpecies * cur_species = garbage_queue.GetFirst();
  for (int i = 0; i < garbage_queue.GetSize(); i++) {
    stats.RemoveSpecies(cur_species->GetID(),
			cur_species->GetParentID(),
			cur_species->GetTotalGenotypes(),
			cur_species->GetTotalOrganisms(),
			stats.GetUpdate() - cur_species->GetUpdateBorn());
    cur_species = cur_species->GetNext();
  }
  garbage_queue.Purge();
}

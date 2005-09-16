//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef SPECIES_CONTROL_HH
#include "species_control.hh"
#endif

#ifndef CONFIG_HH
#include "cConfig.h"
#endif
#ifndef DEFS_HH
#include "defs.hh"
#endif
#ifndef GENOTYPE_HH
#include "cGenotype.h"
#endif
#ifndef GENOTYPE_MACROS_HH
#include "genotype_macros.hh"
#endif
#ifndef SPECIES_HH
#include "species.hh"
#endif
#ifndef STATS_HH
#include "stats.hh"
#endif

#include <assert.h>

/////////////////////
//  cSpeciesControl
/////////////////////

cSpeciesControl::cSpeciesControl(cGenebank & in_gb) : genebank(in_gb)
{
}

cSpeciesControl::~cSpeciesControl()
{
}

void cSpeciesControl::Remove(cSpecies & in_species)
{
  switch (in_species.GetQueueType()) {
  case SPECIES_QUEUE_ACTIVE:
    active_queue.Remove(in_species);
    break;
  case SPECIES_QUEUE_INACTIVE:
    inactive_queue.Remove(in_species);
    break;
  case SPECIES_QUEUE_GARBAGE:
    garbage_queue.Remove(in_species);
    break;
  default:
    break;
  }

  in_species.SetQueueType(SPECIES_QUEUE_NONE);
}

void cSpeciesControl::Adjust(cSpecies & in_species)
{
  // Only adjust if this species is in the active queue.

  if (in_species.GetQueueType() == SPECIES_QUEUE_ACTIVE) {
    active_queue.Adjust(in_species);
  }
}

void cSpeciesControl::SetActive(cSpecies & in_species)
{
  Remove(in_species);
  active_queue.InsertRear(in_species);
  in_species.SetQueueType(SPECIES_QUEUE_ACTIVE);
}

void cSpeciesControl::SetInactive(cSpecies & in_species)
{
  Remove(in_species);
  inactive_queue.InsertRear(in_species);
  in_species.SetQueueType(SPECIES_QUEUE_INACTIVE);
}

void cSpeciesControl::SetGarbage(cSpecies & in_species)
{
  Remove(in_species);
  garbage_queue.InsertRear(in_species);
  in_species.SetQueueType(SPECIES_QUEUE_GARBAGE);
}

bool cSpeciesControl::OK()
{
  int ret_value = true;

  // Check the queues.

  assert (active_queue.OK(SPECIES_QUEUE_ACTIVE));
  assert (inactive_queue.OK(SPECIES_QUEUE_INACTIVE));
  assert (garbage_queue.OK(SPECIES_QUEUE_GARBAGE));

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

  if (record_level == SPECIES_RECORD_FULL) {
    cur_species = active_queue.GetFirst();
    int size = active_queue.GetSize();
    for (int i = 0; i < size; i++) {
      cur_count = cur_species->Compare(in_genotype.GetGenome(),
				       cConfig::GetSpeciesThreshold());
      if (cur_count != -1 && cur_count <= cConfig::GetSpeciesThreshold() &&
	  cur_count < best_count) {
	found_species = cur_species;
	best_count = cur_count;
      }
      cur_species = cur_species->GetNext();
    }
  }

  if (record_level == SPECIES_RECORD_LIMITED) {
    cur_species = in_genotype.GetSpecies();

    if (cur_species) {
      int num_diff = cur_species->Compare(in_genotype.GetGenome(),
					  cConfig::GetSpeciesThreshold());
      if (num_diff != -1 && num_diff <= cConfig::GetSpeciesThreshold()) {
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

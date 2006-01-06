/*
 *  cInjectGenotypeControl.cc
 *  Avida
 *
 *  Created by David on 11/15/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cInjectGenotypeControl.h"

#include "defs.h"
#include "cClassificationManager.h"
#include "cInjectGenotype.h"
#include "cWorld.h"

cInjectGenotypeControl::cInjectGenotypeControl(cWorld* world) : m_world(world)
{
  size = 0;
  best = NULL;
  coalescent = NULL;
  for (int i = 0; i < nInjectGenotype::THREADS; i++) threads[i] = NULL;

  historic_list = NULL;
  historic_count = 0;
}

cInjectGenotypeControl::~cInjectGenotypeControl()
{
}

bool cInjectGenotypeControl::OK()
{
  int ret_value = true;

  // Cycle through the list, making sure all connections are proper, size
  // is correct, and all genotypes are OK().

  cInjectGenotype * cur_pos = best;
  for (int i = 0; i < size; i++) {
    if (!cur_pos->OK()) ret_value = false;
    assert (cur_pos->GetNext()->GetPrev() == cur_pos);
    cur_pos = cur_pos->GetNext();
  }

  assert (cur_pos == best);

  return ret_value;
}

void cInjectGenotypeControl::Insert(cInjectGenotype & in_inject_genotype, cInjectGenotype * prev_genotype)
{
  if (prev_genotype == NULL) {
    assert(size == 0); // Destroying a full genotype queue...

    best = &in_inject_genotype;
    best->SetNext(best);
    best->SetPrev(best);
  }
  else {
    in_inject_genotype.SetPrev(prev_genotype);
    in_inject_genotype.SetNext(prev_genotype->GetNext());
    prev_genotype->SetNext(&in_inject_genotype);
    in_inject_genotype.GetNext()->SetPrev(&in_inject_genotype);
  }

  size++;
}

void cInjectGenotypeControl::Remove(cInjectGenotype & in_inject_genotype)
{
  if (size == 1) {
    best = NULL;
  }
  if (&in_inject_genotype == best) {
    best = best->GetNext();
  }

  in_inject_genotype.GetNext()->SetPrev(in_inject_genotype.GetPrev());
  in_inject_genotype.GetPrev()->SetNext(in_inject_genotype.GetNext());
  in_inject_genotype.SetNext(NULL);
  in_inject_genotype.SetPrev(NULL);

  size--;
}

void cInjectGenotypeControl::RemoveHistoric(cInjectGenotype & in_inject_genotype)
{
  if (historic_count == 1) {
    historic_list = NULL;
  }
  if (&in_inject_genotype == historic_list) {
    historic_list = historic_list->GetNext();
  }

  in_inject_genotype.GetNext()->SetPrev(in_inject_genotype.GetPrev());
  in_inject_genotype.GetPrev()->SetNext(in_inject_genotype.GetNext());
  in_inject_genotype.SetNext(NULL);
  in_inject_genotype.SetPrev(NULL);

  historic_count--;
}

void cInjectGenotypeControl::InsertHistoric(cInjectGenotype & in_inject_genotype)
{
  if (historic_count == 0) {
    in_inject_genotype.SetNext(&in_inject_genotype);
    in_inject_genotype.SetPrev(&in_inject_genotype);
  }
  else {
    in_inject_genotype.SetPrev(historic_list->GetPrev());
    in_inject_genotype.SetNext(historic_list);
    historic_list->GetPrev()->SetNext(&in_inject_genotype);
    historic_list->SetPrev(&in_inject_genotype);
  }

  historic_list = &in_inject_genotype;
  historic_count++;
}

/*int cInjectGenotypeControl::UpdateCoalescent()
{
  // Test to see if any updating needs to be done...
  // Don't update active coalescent genotype, or if there is more than
  // one offspring.
  if (coalescent != NULL &&
      (coalescent->GetNumInjected() > 0) ||
      coalescent->GetNumOffspringGenotypes() > 1) ) {
    return coalescent->GetDepth();
  }

  // If there is no best, there is nothing to search through...
  if (best == NULL) return -1;

  // Find the new point...
  cInjectGenotype * test_gen = best;
  cInjectGenotype * found_gen = best;
  cInjectGenotype * parent_gen = best->GetParentGenotype();

  while (parent_gen != NULL) {
    // See if this genotype should be the new found genotype...
    if (test_gen->GetNumOrganisms() > 0 ||
	test_gen->GetNumOffspringGenotypes() > 1) {
      found_gen = test_gen;
    }

    // Move to the next genotype...
    test_gen = parent_gen;
    parent_gen = test_gen->GetParentGenotype();
  }

  coalescent = found_gen;

  return coalescent->GetDepth();
}*/


bool cInjectGenotypeControl::CheckPos(cInjectGenotype & in_inject_genotype)
{
  int next_OK = false;
  int prev_OK = false;

  if (in_inject_genotype.GetNumInjected() >= in_inject_genotype.GetNext()->GetNumInjected()) {
    next_OK =true;
  }
  if (in_inject_genotype.GetNumInjected() <= in_inject_genotype.GetPrev()->GetNumInjected()) {
    prev_OK =true;
  }

  if ((&in_inject_genotype == best && next_OK) ||
      (next_OK && prev_OK) ||
      (&in_inject_genotype == best->GetPrev() && prev_OK)) {
    return true;
  }

  return false;
}

void cInjectGenotypeControl::Insert(cInjectGenotype & new_genotype)
{
  // If there is nothing in the list, add this.

  if (size == 0) {
    Insert(new_genotype, NULL);
  }

  // Otherwise tack it on the end.

  else {
    Insert(new_genotype, best->GetPrev());
  }
}

bool cInjectGenotypeControl::Adjust(cInjectGenotype & in_inject_genotype)
{
  //if (in_inject_genotype.GetDeferAdjust() == true) return true;

  cInjectGenotype * cur_inject_genotype = in_inject_genotype.GetPrev();

  // Check to see if this genotype should be removed completely.

  if (in_inject_genotype.GetNumInjected() == 0) {
    m_world->GetClassificationManager().RemoveInjectGenotype(in_inject_genotype);
    return false;
  }

  // Do not adjust if this was and still is the best genotype, or is
  // otherwise in the proper spot...

  if (CheckPos(in_inject_genotype)) {
    return true;
  }

  // Otherwise, remove it from the queue for just the moment.

  Remove(in_inject_genotype);

  // Also, if this genotype is the best, put it there.

  if (in_inject_genotype.GetNumInjected() > best->GetNumInjected()) {
    Insert(in_inject_genotype, best->GetPrev());
    best = &in_inject_genotype;
    return true;
  }

  // Finally, find out where this genotype *does* go.

  while (cur_inject_genotype->GetNumInjected() >= in_inject_genotype.GetNumInjected() &&
	 cur_inject_genotype != best->GetPrev()) {
    cur_inject_genotype = cur_inject_genotype->GetNext();
  }
  while (cur_inject_genotype->GetNumInjected() < in_inject_genotype.GetNumInjected() &&
	 cur_inject_genotype != best) {
    cur_inject_genotype = cur_inject_genotype->GetPrev();
  }

  Insert(in_inject_genotype, cur_inject_genotype);

  return true;
}


cInjectGenotype * cInjectGenotypeControl::Find(const cGenome & in_genome) const
{
  int i;
  cInjectGenotype * cur_inject_genotype = best;

  for (i = 0; i < size; i++) {
    if (in_genome == cur_inject_genotype->GetGenome()) {
      return cur_inject_genotype;
    }
    cur_inject_genotype = cur_inject_genotype->GetNext();
  }

  return NULL;
}

int cInjectGenotypeControl::FindPos(cInjectGenotype & in_inject_genotype, int max_depth)
{
  cInjectGenotype * temp_genotype = best;
  if (max_depth < 0 || max_depth > size) max_depth = size;

  for (int i = 0; i < max_depth; i++) {
    if (temp_genotype == &in_inject_genotype) return i;
    temp_genotype = temp_genotype->GetNext();
  }

  return -1;
}

cInjectGenotype * cInjectGenotypeControl::Next(int thread)
{
  return threads[thread] = threads[thread]->GetNext();
}

cInjectGenotype * cInjectGenotypeControl::Prev(int thread)
{
  return threads[thread] = threads[thread]->GetPrev();
}

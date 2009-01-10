/*
 *  cGenotypeControl.cc
 *  Avida
 *
 *  Called "genotype_control.cc" prior to 11/30/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#include "cGenotypeControl.h"

#include "defs.h"
#include "cClassificationManager.h"
#include "cGenotype.h"
#include "cWorld.h"

cGenotypeControl::cGenotypeControl(cWorld* world) : m_world(world)
{
  size = 0;
  best = NULL;
  coalescent = NULL;
  for (int i = 0; i < nGenotype::THREADS; i++) threads[i] = NULL;

  historic_list = NULL;
  historic_count = 0;
}

cGenotypeControl::~cGenotypeControl()
{
}

bool cGenotypeControl::OK()
{
  int ret_value = true;

  // Cycle through the list, making sure all connections are proper, size
  // is correct, and all genotypes are OK().

  cGenotype * cur_pos = best;
  for (int i = 0; i < size; i++) {
    if (!cur_pos->OK()) ret_value = false;
    assert (cur_pos->GetNext()->GetPrev() == cur_pos);
    cur_pos = cur_pos->GetNext();
  }

  assert (cur_pos == best);

  return ret_value;
}

void cGenotypeControl::Insert(cGenotype & in_genotype, cGenotype * prev_genotype)
{
  if (prev_genotype == NULL) {
    assert(size == 0); // Destroying a full genotype queue...

    best = &in_genotype;
    best->SetNext(best);
    best->SetPrev(best);
  }
  else {
    in_genotype.SetPrev(prev_genotype);
    in_genotype.SetNext(prev_genotype->GetNext());
    prev_genotype->SetNext(&in_genotype);
    in_genotype.GetNext()->SetPrev(&in_genotype);
  }

  /*if (!CheckPos(in_genotype))
    cerr << "Genotype insertion fail! @MRR -- \n"
      << "Best Genotype:    " << best << " " << best->GetNumOrganisms() << " prev: " << best->GetPrev() << "  next: " << best->GetNext() << endl
      << "In Genotype:      " << &in_genotype << " " << in_genotype.GetNumOrganisms() << " prev: " << in_genotype.GetPrev() << "  next: " << in_genotype.GetNext() << endl
      << "Prev Genotype:    " << prev_genotype << " " << prev_genotype->GetNumOrganisms() << " prev: " << prev_genotype->GetPrev() << "  next: " << prev_genotype->GetNext() << endl;
    */
  size++;
}

void cGenotypeControl::Remove(cGenotype & in_genotype)
{
  if (size == 1) best = NULL;
  if (&in_genotype == best) best = best->GetNext();
  if (&in_genotype == coalescent) coalescent = NULL;

  in_genotype.GetNext()->SetPrev(in_genotype.GetPrev());
  in_genotype.GetPrev()->SetNext(in_genotype.GetNext());
  in_genotype.SetNext(NULL);
  in_genotype.SetPrev(NULL);

  size--;
}

void cGenotypeControl::RemoveHistoric(cGenotype & in_genotype)
{
  if (historic_count == 1) {
    historic_list = NULL;
  }
  if (&in_genotype == historic_list) {
    historic_list = historic_list->GetNext();
  }

  in_genotype.GetNext()->SetPrev(in_genotype.GetPrev());
  in_genotype.GetPrev()->SetNext(in_genotype.GetNext());
  in_genotype.SetNext(NULL);
  in_genotype.SetPrev(NULL);

  historic_count--;
}

void cGenotypeControl::InsertHistoric(cGenotype & in_genotype)
{
  if (historic_count == 0) {
    in_genotype.SetNext(&in_genotype);
    in_genotype.SetPrev(&in_genotype);
  }
  else {
    in_genotype.SetPrev(historic_list->GetPrev());
    in_genotype.SetNext(historic_list);
    historic_list->GetPrev()->SetNext(&in_genotype);
    historic_list->SetPrev(&in_genotype);
  }

  historic_list = &in_genotype;
  historic_count++;
}


int cGenotypeControl::UpdateCoalescent()
{
  // Test to see if any updating needs to be done...
  // Don't update active coalescent genotype, or if there is more than
  // one offspring.
  if (coalescent != NULL && (coalescent->GetNumOrganisms() > 0 || coalescent->GetNumOffspringGenotypes() > 1)) {
    return coalescent->GetDepth();
  }

  // If there is no best, there is nothing to search through...
  if (best == NULL) return -1;

  // Find the new point...
  cGenotype* test_gen = best;
  cGenotype* found_gen = best;
  cGenotype* parent_gen = best->GetParentGenotype();

  while (parent_gen != NULL) {
    // See if this genotype should be the new found genotype...
    if (test_gen->GetNumOrganisms() > 0 || test_gen->GetNumOffspringGenotypes() > 1) {
      found_gen = test_gen;
    }

    // Move to the next genotype...
    test_gen = parent_gen;
    parent_gen = test_gen->GetParentGenotype();
  }

  coalescent = found_gen;

  return coalescent->GetDepth();
}


bool cGenotypeControl::CheckPos(cGenotype & in_genotype)
{
  int next_OK = false;
  int prev_OK = false;

  if (in_genotype.GetNumOrganisms() >= in_genotype.GetNext()->GetNumOrganisms()) {
    next_OK =true;
  }
  if (in_genotype.GetNumOrganisms() <= in_genotype.GetPrev()->GetNumOrganisms()) {
    prev_OK =true;
  }

  if ( (next_OK && prev_OK) ||
       (&in_genotype == best && next_OK) ||
       (&in_genotype == best->GetPrev() && prev_OK)) {
    return true;
  }

  return false;
}

void cGenotypeControl::Insert(cGenotype & new_genotype)
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

bool cGenotypeControl::Adjust(cGenotype & in_genotype)
{
  cGenotype* cur_genotype = in_genotype.GetPrev();

  // Check to see if this genotype should be removed completely.
  if (in_genotype.GetNumOrganisms() == 0 && in_genotype.GetDeferAdjust() == false) {
    m_world->GetClassificationManager().RemoveGenotype(in_genotype);
    return false;
  }

  // Do not adjust the position of this genotype if it was and still is the
  // best genotype, or if it is otherwise in the proper spot...
  if (CheckPos(in_genotype)) return true;

  // Otherwise, remove it from the queue (for just the moment).
  Remove(in_genotype);

  // If this genotype is the best, put it there.
  if (in_genotype.GetNumOrganisms() > best->GetNumOrganisms()) {
    Insert(in_genotype, best->GetPrev());
    best = &in_genotype;
    return true;
  }

  // Finally, find out where this genotype *does* go.

  while (cur_genotype->GetNumOrganisms() >= in_genotype.GetNumOrganisms() &&
	 cur_genotype != best->GetPrev()) {
    cur_genotype = cur_genotype->GetNext();
  }
  while (cur_genotype->GetNumOrganisms() < in_genotype.GetNumOrganisms() &&
	 cur_genotype != best) {
    cur_genotype = cur_genotype->GetPrev();
  }

  Insert(in_genotype, cur_genotype);

  return true;
}


cGenotype * cGenotypeControl::Find(const cGenome & in_genome) const
{
  int i;
  cGenotype * cur_genotype = best;

  for (i = 0; i < size; i++) {
    if (in_genome == cur_genotype->GetGenome()) {
      return cur_genotype;
    }
    cur_genotype = cur_genotype->GetNext();
  }

  return NULL;
}

cGenotype * cGenotypeControl::Find(const int in_genotype_id) const
{
  int i;
  cGenotype * cur_genotype = best;

  for (i = 0; i < size; i++) {
    if (in_genotype_id == cur_genotype->GetID()) {
      return cur_genotype;
    }
    cur_genotype = cur_genotype->GetNext();
  }

  return NULL;
}

int cGenotypeControl::FindPos(cGenotype & in_genotype, int max_depth)
{
  cGenotype * temp_genotype = best;
  if (max_depth < 0 || max_depth > size) max_depth = size;

  for (int i = 0; i < max_depth; i++) {
    if (temp_genotype == &in_genotype) return i;
    temp_genotype = temp_genotype->GetNext();
  }

  return -1;
}

cGenotype * cGenotypeControl::Next(int thread)
{
  return threads[thread] = threads[thread]->GetNext();
}

cGenotype * cGenotypeControl::Prev(int thread)
{
  return threads[thread] = threads[thread]->GetPrev();
}

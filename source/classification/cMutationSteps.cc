/*
 *  cMutationSteps.cc
 *  Avida
 *
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

#include "cMutationSteps.h"

cMutationSteps::cMutationSteps(const cMutationSteps& in_ms) : tList<cMutationStep>()
{
  tConstListIterator<cMutationStep> mutation_it(in_ms);
  while (cMutationStep* mut = mutation_it.Next()) {
    this->PushRear(mut->copy());
  }
}

void cMutationSteps::operator=(const cMutationSteps& in_ms)
{
  tConstListIterator<cMutationStep> mutation_it(in_ms);
  while (cMutationStep* mut = mutation_it.Next()) {
    this->PushRear(mut->copy());
  }
}

cMutationSteps::~cMutationSteps()
{
  tListIterator<cMutationStep> mutation_it(*this);
  while (cMutationStep* mut = mutation_it.Next()) {
    delete mut;
  }
}

void cMutationSteps::Clear()
{
  tListIterator<cMutationStep> mutation_it(*this);
  while (cMutationStep* mut = mutation_it.Next()) {
    delete mut;
  }
  tList<cMutationStep>::Clear();
}

void cMutationSteps::AddSubstitutionMutation(int _pos, char _from, char _to)
{
  cMutationStep* mut = new cSubstitutionMutationStep(_pos, _from, _to);
  this->PushRear(mut);
}

void cMutationSteps::AddDeletionMutation(int _pos, char _from)
{
  cMutationStep* mut = new cDeletionMutationStep(_pos, _from);
  this->PushRear(mut);
}

void cMutationSteps::AddInsertionMutation(int _pos, int _to)
{
  cMutationStep* mut = new cInsertionMutationStep(_pos, _to);
  this->PushRear(mut);
}

void cMutationSteps::AddSlipMutation(int _start, int _end)
{
  cMutationStep* mut = new cSlipMutationStep(_start, _end);
  this->PushRear(mut);
}

cString cMutationSteps::AsString() const
{
  if (m_loaded_string.GetSize() > 0) return m_loaded_string;

  cString return_string;
  tConstListIterator<cMutationStep> mutation_it(*this);
  while (cMutationStep* mut = mutation_it.Next()) {
    if (return_string.GetSize() > 0) { return_string += ","; }
    return_string += mut->AsString();
  }
  return return_string;
}

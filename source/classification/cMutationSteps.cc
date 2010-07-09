/*
 *  cMutationSteps.cc
 *  Avida
 *
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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


void cMutationSteps::AddSubstitutionMutation(int _pos, char _from, char _to)
{
  int idx = m_steps.GetSize();
  m_steps.Resize(idx + 1);
  m_steps[idx].step_type = sMutStep::SUB_STEP;
  m_steps[idx].sub.pos = _pos;
  m_steps[idx].sub.from = _from;
  m_steps[idx].sub.to = _to;
}

void cMutationSteps::AddDeletionMutation(int _pos, char _from)
{
  int idx = m_steps.GetSize();
  m_steps.Resize(idx + 1);
  m_steps[idx].step_type = sMutStep::DEL_STEP;
  m_steps[idx].indel.pos = _pos;
  m_steps[idx].indel.inst = _from;
}

void cMutationSteps::AddInsertionMutation(int _pos, int _to)
{
  int idx = m_steps.GetSize();
  m_steps.Resize(idx + 1);
  m_steps[idx].step_type = sMutStep::IN_STEP;
  m_steps[idx].indel.pos = _pos;
  m_steps[idx].indel.inst = _to;
}

void cMutationSteps::AddSlipMutation(int _start, int _end)
{
  int idx = m_steps.GetSize();
  m_steps.Resize(idx + 1);
  m_steps[idx].step_type = sMutStep::SLIP_STEP;
  m_steps[idx].slip.start = _start;
  m_steps[idx].slip.end = _end;
}

cString cMutationSteps::sMutStep::AsString() const
{
  cString s;
  
  switch (step_type) {
    case SUB_STEP:
      s.Set("M%c%i%c", sub.from, sub.pos, sub.to);
      break;
    case DEL_STEP:
      s.Set("D%i%c", indel.pos, indel.inst);
      break;
    case IN_STEP:
      s.Set("I%i%c", indel.pos, indel.inst);
      break;
    case SLIP_STEP:
      s.Set("S%i-%i", slip.start, slip.end);
      break;
      
    default:
      s = "(unknown";
      break;
  }
  return s;
}


cString cMutationSteps::AsString() const
{
  if (m_loaded_string.GetSize() > 0) return m_loaded_string;

  cString return_string;
  for (int i = 0; i < m_steps.GetSize(); i++) {
    if (return_string.GetSize() > 0) return_string += ",";
    return_string += m_steps[i].AsString();
  }
  return return_string;
}



/*
 *  cReaction.cc
 *  Avida
 *
 *  Called "reaction.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#include "cReaction.h"

#include "cReactionProcess.h"
#include "cReactionRequisite.h"

using namespace std;


cReaction::cReaction(const cString & _name, int _id)
  : name(_name)
  , id(_id)
  , task(NULL)
  , active(true)
{
}

cReaction::~cReaction()
{
  while (process_list.GetSize() != 0) delete process_list.Pop();
  while (requisite_list.GetSize() != 0) delete requisite_list.Pop();
}

cReactionProcess * cReaction::AddProcess()
{
  cReactionProcess * new_process = new cReactionProcess();
  process_list.PushRear(new_process);
  return new_process;
}

cReactionRequisite * cReaction::AddRequisite()
{
  cReactionRequisite * new_requisite = new cReactionRequisite();
  requisite_list.PushRear(new_requisite);
  return new_requisite;
}

bool cReaction::ModifyValue(double new_value, int process_num) 
{
  if (process_num >= process_list.GetSize() || process_num < 0) return false;
  process_list.GetPos(process_num)->SetValue(new_value);
  return true;
}

bool cReaction::MultiplyValue(double value_mult, int process_num) 
{
  if (process_num >= process_list.GetSize() || process_num < 0) return false;
  double new_value = process_list.GetPos(process_num)->GetValue() * value_mult;
  process_list.GetPos(process_num)->SetValue(new_value);
  return true;
}

bool cReaction::ModifyInst(const cString& inst, int process_num) 
{
  if (process_num >= process_list.GetSize() || process_num < 0) return false;
  process_list.GetPos(process_num)->SetInst(inst);
  return true;
}

bool cReaction::SetMinTaskCount(int min_count, int requisite_num) 
{
  if (requisite_num >= requisite_list.GetSize() || requisite_num < 0) return false;
  requisite_list.GetPos(requisite_num)->SetMinTaskCount(min_count);
  return true;
}

bool cReaction::SetMaxTaskCount(int max_count, int requisite_num) 
{
  if (requisite_num >= requisite_list.GetSize() || requisite_num < 0) return false;
  requisite_list.GetPos(requisite_num)->SetMaxTaskCount(max_count);
  return true;
}


double cReaction::GetValue(int process_num)
{ 
  if (process_num >= process_list.GetSize() || process_num < 0) return false;
  return  process_list.GetPos(process_num)->GetValue();
}

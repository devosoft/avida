/*
 *  cReaction.cc
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

bool cReaction::ModifyInst(int inst_id, int process_num) 
{
  if (process_num >= process_list.GetSize() || process_num < 0) return false;
  process_list.GetPos(process_num)->SetInstID(inst_id);
  return true;
}


double cReaction::GetValue(int process_num)
{ 
  if (process_num >= process_list.GetSize() || process_num < 0) return false;
  return  process_list.GetPos(process_num)->GetValue();
}

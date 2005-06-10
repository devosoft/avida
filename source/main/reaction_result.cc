//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2004 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "reaction_result.hh"


cReactionResult::cReactionResult(const int num_resources,
				 const int num_tasks,
				 const int num_reactions)
  : resources_consumed(num_resources)
  , resources_produced(num_resources)
  , resources_detected(num_resources)
  , tasks_done(num_tasks)
  , receive_tasks_done(num_tasks)
  , send_tasks_done(num_tasks)
  , reactions_triggered(num_reactions)
  , bonus_add(0.0)
  , bonus_mult(1.0)
  , insts_triggered(0)
  , lethal(false)
  , active_reaction(false)
{
}

cReactionResult::~cReactionResult()
{
}


void cReactionResult::ActivateReaction()
{
  // If this reaction is already active, don't worry about it.
  if (active_reaction == true) return;

  // To activate the reaction, we must initialize all counter settings.
  resources_consumed.SetAll(0.0);
  resources_produced.SetAll(0.0);
  resources_detected.SetAll(-1.0);
  tasks_done.SetAll(false);
  receive_tasks_done.SetAll(false);
  send_tasks_done.SetAll(false);
  reactions_triggered.SetAll(false);

  // And finally note that this is indeed already active.
  active_reaction = true;
}


void cReactionResult::Consume(int id, double num)
{
  ActivateReaction();
  resources_consumed[id] += num;
}


void cReactionResult::Produce(int id, double num)
{
  ActivateReaction();
  resources_produced[id] += num;
}


void cReactionResult::Detect(int id, double num)
{
  ActivateReaction();
  resources_detected[id] += num;
}

void cReactionResult::Lethal(bool flag)
{
 ActivateReaction();
 lethal = flag;
}

void cReactionResult::MarkTask(int id)
{
  ActivateReaction();
  tasks_done[id] = true;
}


void cReactionResult::MarkReceiveTask(int id)
{
  ActivateReaction();
  receive_tasks_done[id] = true;
}


void cReactionResult::MarkSendTask(int id)
{
  ActivateReaction();
  send_tasks_done[id] = true;
}


void cReactionResult::MarkReaction(int id)
{
  ActivateReaction();
  reactions_triggered[id] = true;
}


void cReactionResult::AddBonus(double value)
{
  ActivateReaction();
  bonus_add += value;
}


void cReactionResult::MultBonus(double value)
{
  ActivateReaction();
  bonus_mult *= value;
}

void cReactionResult::AddInst(int id)
{
  insts_triggered.Push(id);
}

double cReactionResult::GetConsumed(int id)
{
  if (GetActive() == false) return 0.0;
  return resources_consumed[id];
}


double cReactionResult::GetProduced(int id)
{
  if (GetActive() == false) return 0.0;
  return resources_produced[id];
}

double cReactionResult::GetDetected(int id)
{
  if (GetActive() == false) return 0.0;
  return resources_detected[id];
}

bool cReactionResult::GetLethal()
{
  if (GetActive() == false) return false;
  return lethal;
}

bool cReactionResult::ReactionTriggered(int id)
{
  if (GetActive() == false) return false;
  return reactions_triggered[id];
}

bool cReactionResult::TaskDone(int id)
{
  if (GetActive() == false) return false;
  return tasks_done[id];
}

double cReactionResult::GetAddBonus()
{
  if (GetActive() == false) return 0.0;
  return bonus_add;
}

double cReactionResult::GetMultBonus()
{
  if (GetActive() == false) return 1.0;
  return bonus_mult;
}

tArray<int> & cReactionResult::GetInstArray()
{
  return insts_triggered;
}


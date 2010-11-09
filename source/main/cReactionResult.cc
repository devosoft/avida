/*
 *  cReactionResult.cc
 *  Avida
 *
 *  Called "reaction_result.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2004 California Institute of Technology.
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

#include "cReactionResult.h"


cReactionResult::cReactionResult(const int num_resources,
				 const int num_tasks,
				 const int num_reactions)
  : resources_consumed(num_resources)
  , resources_produced(num_resources)
  , resources_detected(num_resources)
  , internal_resources_consumed(num_resources)
  , tasks_done(num_tasks)
  , tasks_quality(num_tasks)
  , tasks_value(num_tasks)
  , reactions_triggered(num_reactions)
  , reaction_add_bonus(num_reactions)
  , active_reaction(false)
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
  internal_resources_consumed.SetAll(0.0);
  tasks_done.SetAll(false);
  tasks_quality.SetAll(0.0);
  tasks_value.SetAll(0.0);
  reactions_triggered.SetAll(false);
  reaction_add_bonus.SetAll(0.0);
  task_plasticity.SetAll(0.0);
  energy_add = 0.0;
  bonus_add = 0.0;
  bonus_mult = 1.0;
  germline_add = 0.0;
  germline_mult = 1.0;
  insts_triggered.Resize(0);
  lethal = false;
  sterilize = false;
  used_env_resource = true;
  deme_add_bonus = 0.0;
  deme_mult_bonus = 1.0;
  active_deme_reaction = false;

  // And finally note that this is indeed already active.
  active_reaction = true;
}


void cReactionResult::Consume(int id, double num, bool is_env_resource)
{
  ActivateReaction();
  if(is_env_resource) { resources_consumed[id] += num; }
  else { 
    used_env_resource = false; 
    internal_resources_consumed[id] += num;
  }
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

void cReactionResult::Sterilize(bool flag)
{
  ActivateReaction();
  sterilize = flag;
}


void cReactionResult::MarkTask(int id, const double quality, const double value)
{
  ActivateReaction();
  tasks_done[id] = true;
  tasks_quality[id] = quality;
  tasks_value[id] = value;
}


void cReactionResult::MarkReaction(int id)
{
  ActivateReaction();
  reactions_triggered[id] = true;
}

void cReactionResult::AddEnergy(double value)
{
  ActivateReaction();
  energy_add += value;
}

void cReactionResult::AddBonus(double value, int id)
{
  ActivateReaction();
  bonus_add += value;
  reaction_add_bonus[id] += value;
}


void cReactionResult::MultBonus(double value)
{
  ActivateReaction();
  bonus_mult *= value;
}

void cReactionResult::AddDemeBonus(double value)
{
  ActivateReaction();
  active_deme_reaction = true;
  deme_add_bonus += value;
}

void cReactionResult::MultDemeBonus(double value)
{
  ActivateReaction();
  active_deme_reaction = true;
  deme_mult_bonus *= value;
}

void cReactionResult::AddGermline(double value)
{
  ActivateReaction();
  germline_add += value;
}

void cReactionResult::MultGermline(double value)
{
  ActivateReaction();
  germline_mult *= value;
}


void cReactionResult::AddInst(const cString& inst)
{
  insts_triggered.Push(inst);
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

double cReactionResult::GetInternalConsumed(int id)
{
  if (GetActive() == false) return 0.0;
  return internal_resources_consumed[id];
}

bool cReactionResult::GetLethal()
{
  if (GetActive() == false) return false;
  return lethal;
}

bool cReactionResult::GetSterilize()
{
  if (GetActive() == false) return false;
  return sterilize;
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

double cReactionResult::TaskQuality(int id)
{
	if (GetActive() == false) return 0;
	return tasks_quality[id];
}

double cReactionResult::TaskValue(int id)
{
	if (GetActive() == false) return 0;
	return tasks_value[id];
}


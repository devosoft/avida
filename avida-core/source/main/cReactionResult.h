/*
 *  cReactionResult.h
 *  Avida
 *
 *  Called "reaction_result.hh" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2004 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cReactionResult_h
#define cReactionResult_h


class cReactionResult {
private:
  Apto::Array<double> resources_consumed;
  Apto::Array<double> resources_produced;
  Apto::Array<double> resources_detected;  //Initialize to -1.0
  Apto::Array<double> internal_resources_consumed;
  Apto::Array<double> internal_resources_produced;
  Apto::Array<bool> tasks_done;
  Apto::Array<double> tasks_quality;
  Apto::Array<double> tasks_value;
  Apto::Array<bool> reactions_triggered;
  Apto::Array<double> reaction_add_bonus;
  Apto::Array<double> task_plasticity;
  double bonus_add;
  double bonus_mult;
  bool lethal;
  bool sterilize;
  bool active_reaction;
  bool used_env_resource;

  inline void ActivateReaction();

public:
  cReactionResult(const int num_resources, const int num_tasks, const int num_reactions);
  ~cReactionResult() { ; }

  bool GetActive() const { return active_reaction; }
  void Invalidate() { active_reaction = false; }


  void Consume(int id, double num, bool is_env_resource);
  void Produce(int id, double num, bool is_env_resource);
  void Detect(int id, double num);
  void Lethal(bool flag);
  void Sterilize(bool flag);
  void MarkTask(int id, const double quality=1, const double value=0);

  void MarkReaction(int id);
  void AddBonus(double value, int id);
  void MultBonus(double value);

  double GetConsumed(int id);
  double GetProduced(int id);
  double GetDetected(int id);
  double GetInternalConsumed(int id);
  double GetInternalProduced(int id);
  bool GetLethal();
  bool GetSterilize();
  bool ReactionTriggered(int id);
  bool TaskDone(int id);
  double TaskQuality(int id);
  double TaskValue(int id);
  double GetAddBonus() { return bonus_add; }
  double GetReactionAddBonus(const int i) { return reaction_add_bonus[i]; }
  double GetMultBonus() { return bonus_mult; }
  bool UsedEnvResource() { return used_env_resource; }
  bool IsEnvResource() { return used_env_resource; }
};

#endif

/*
 *  cReactionResult.h
 *  Avida
 *
 *  Called "reaction_result.hh" prior to 12/5/05.
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

#ifndef cReactionResult_h
#define cReactionResult_h

#include "tArray.h"
#include "cString.h"

class cReactionResult {
private:
  tArray<double> resources_consumed;
  tArray<double> resources_produced;
  tArray<double> resources_detected;  //Initialize to -1.0
  tArray<double> internal_resources_consumed;
  tArray<bool> tasks_done;
  tArray<double> tasks_quality;
  tArray<double> tasks_value;
  tArray<bool> reactions_triggered;
  tArray<double> reaction_add_bonus; 
  tArray<double> task_plasticity;
  double energy_add;
  double bonus_add;
  double bonus_mult;
  double germline_add;
  double germline_mult;
  tArray<cString> insts_triggered;
  bool lethal;
  bool sterilize;
  bool active_reaction;
  bool used_env_resource;

  double deme_add_bonus; //!< Additive bonus applied to the deme as a result of this reaction.
  double deme_mult_bonus; //!< Multiplicative bonus applied to the deme as a result of this reaction.
  bool active_deme_reaction; //!< Whether this reaction result includes a deme merit component.
  
  inline void ActivateReaction();
  
  cReactionResult(); // @not_implemented
  cReactionResult(const cReactionResult&); // @not_implemented
  cReactionResult& operator=(const cReactionResult&); // @not_implemented

public:
  cReactionResult(const int num_resources, const int num_tasks, const int num_reactions);
  ~cReactionResult() { ; }

  bool GetActive() const { return active_reaction; }
  bool GetActiveDeme() const { return active_deme_reaction; }
  void Invalidate() { active_reaction = false; }
  

  void Consume(int id, double num, bool is_env_resource);
  void Produce(int id, double num);
  void Detect(int id, double num);
  void Lethal(bool flag);
  void Sterilize(bool flag);
  void MarkTask(int id, const double quality=1, const double value=0);

  void MarkReaction(int id);
  void AddEnergy(double value);
  void AddBonus(double value, int id);
  void MultBonus(double value);
  void AddDemeBonus(double value);
  void MultDemeBonus(double value);
  void AddGermline(double value);
  void MultGermline(double value);

  void AddInst(const cString& inst);
  
  double GetConsumed(int id);
  double GetProduced(int id);
  double GetDetected(int id);
  double GetInternalConsumed(int id);
  bool GetLethal();  
  bool GetSterilize();
  bool ReactionTriggered(int id);
  bool TaskDone(int id);
  double TaskQuality(int id);
  double TaskValue(int id);
  double GetAddEnergy() { return energy_add; }
  double GetAddBonus() { return bonus_add; }
  double GetReactionAddBonus(const int i) { return reaction_add_bonus[i]; }
  double GetMultBonus() { return bonus_mult; }
  tArray<cString>& GetInstArray() { return insts_triggered; }
  bool UsedEnvResource() { return used_env_resource; }
  bool IsEnvResource() { return used_env_resource; }
  double GetAddDemeBonus() { return deme_add_bonus; }
  double GetMultDemeBonus() { return deme_mult_bonus; }
  double GetAddGermline() { return germline_add; }
  double GetMultGermline() { return germline_mult; }
};

#endif

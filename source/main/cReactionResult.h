/*
 *  cReactionResult.h
 *  Avida
 *
 *  Called "reaction_result.hh" prior to 12/5/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef tArray_h
#include "tArray.h"
#endif

class cReactionResult {
private:
  tArray<double> resources_consumed;
  tArray<double> resources_produced;
  tArray<double> resources_detected;  //Initialize to -1.0
  tArray<bool> tasks_done;
  tArray<double> tasks_quality;
  tArray<double> tasks_value;
  tArray<bool> reactions_triggered;
  tArray<double> reaction_add_bonus; 
  double bonus_add;
  double bonus_mult;
  tArray<int> insts_triggered;
  bool lethal;
  bool active_reaction;

  inline void ActivateReaction();
  
  cReactionResult(); // @not_implemented
  cReactionResult(const cReactionResult&); // @not_implemented
  cReactionResult& operator=(const cReactionResult&); // @not_implemented

public:
  cReactionResult(const int num_resources, const int num_tasks, const int num_reactions);
  ~cReactionResult() { ; }

  bool GetActive() { return active_reaction; }

  void Consume(int id, double num);
  void Produce(int id, double num);
  void Detect(int id, double num);
  void Lethal(bool flag);
  void MarkTask(int id, const double quality=1, const double value=0);

  void MarkReaction(int id);
  void AddBonus(double value, int id);
  void MultBonus(double value);
  
  void AddInst(int id);
  
  double GetConsumed(int id);
  double GetProduced(int id);
  double GetDetected(int id);
  bool GetLethal();  
  bool ReactionTriggered(int id);
  bool TaskDone(int id);
  double TaskQuality(int id);
  double TaskValue(int id);
  double GetAddBonus() { return bonus_add; }
  double GetReactionAddBonus(const int i) { return reaction_add_bonus[i]; }
  double GetMultBonus() { return bonus_mult; }
  tArray<int>& GetInstArray() { return insts_triggered; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nReactionResult {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

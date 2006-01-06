/*
 *  cReactionResult.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2004 California Institute of Technology.
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
  tArray<bool> receive_tasks_done;
  tArray<bool> send_tasks_done;
  tArray<bool> reactions_triggered;
  double bonus_add;
  double bonus_mult;
  tArray<int> insts_triggered;
  bool lethal;
  bool active_reaction;

  inline void ActivateReaction();
public:
  cReactionResult(const int num_resources, const int num_tasks,
		  const int num_reactions);
  ~cReactionResult();

  bool GetActive() { return active_reaction; }

  void Consume(int id, double num);
  void Produce(int id, double num);
  void Detect(int id, double num);
  void Lethal(bool flag);
  void MarkTask(int id);
  void MarkReceiveTask(int id);
  void MarkSendTask(int id);
  void MarkReaction(int id);
  void AddBonus(double value);
  void MultBonus(double value);
  void AddInst(int id);

  double GetConsumed(int id);
  double GetProduced(int id);
  double GetDetected(int id);
  bool GetLethal();
  bool ReactionTriggered(int id);
  bool TaskDone(int id);
  double GetAddBonus();
  double GetMultBonus();
  tArray<int> & GetInstArray();
};

#endif

//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2004 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef REACTION_RESULT_HH
#define REACTION_RESULT_HH

#ifndef TARRAY_HH
#include "tArray.hh"
#endif

template <class T> class tArray; // aggregate

class cReactionResult {
private:
  tArray<double> resources_consumed;
  tArray<double> resources_produced;
  tArray<double> resources_detected;  //Initialize to -1.0
  tArray<bool> tasks_done;
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

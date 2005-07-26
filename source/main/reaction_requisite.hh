//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef REACTION_REQUISITE_HH
#define REACTION_REQUISITE_HH

#ifndef TLIST_HH
#include "tList.hh"
#endif

class cReaction;
class cReactionRequisite {
private:
  tList<cReaction> prior_reaction_list;
  tList<cReaction> prior_noreaction_list;
  int min_task_count;
  int max_task_count;
private:
  // disabled copy constructor.
  cReactionRequisite(const cReactionRequisite &);
public:
  cReactionRequisite();
  ~cReactionRequisite();

  const tList<cReaction> & GetReactions() const { return prior_reaction_list; }
  const tList<cReaction> & GetNoReactions() const
    { return prior_noreaction_list; }
  int GetMinTaskCount() const { return min_task_count; }
  int GetMaxTaskCount() const { return max_task_count; }

  void AddReaction(cReaction * in_reaction) {
    prior_reaction_list.PushRear(in_reaction);
  }
  void AddNoReaction(cReaction * in_reaction) {
    prior_noreaction_list.PushRear(in_reaction);
  }
  void SetMinTaskCount(int min) { min_task_count = min; }
  void SetMaxTaskCount(int max) { max_task_count = max; }

  /*
  added to satisfy Boost.Python; the semantics are fairly useless --
  equality of two references means that they refer to the same object.
  */
  bool operator==(const cReactionRequisite &in) const { return &in == this; }
};

#endif

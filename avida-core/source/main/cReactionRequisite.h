/*
 *  cReactionRequisite.h
 *  Avida
 *
 *  Called "reaction_requisite.hh" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#ifndef cReactionRequisite_h
#define cReactionRequisite_h

#include <climits>

#ifndef tList_h
#include "tList.h"
#endif

class cReaction;

class cReactionRequisite
{
private:
  tList<cReaction> prior_reaction_list;
  tList<cReaction> prior_noreaction_list;
  int min_task_count;
  int max_task_count;
  int min_reaction_count;
  int max_reaction_count;
  int min_tot_reaction_count;
  int max_tot_reaction_count;
  int divide_only;
  int parasite_only;


  cReactionRequisite(const cReactionRequisite&); // @not_implemented
  cReactionRequisite& operator=(const cReactionRequisite&);

public:
  cReactionRequisite() : min_task_count(0) , max_task_count(INT_MAX),
    min_reaction_count(0) , max_reaction_count(INT_MAX),
	  min_tot_reaction_count(0), max_tot_reaction_count(INT_MAX), divide_only(0), parasite_only(0) { ; }
  ~cReactionRequisite() { ; }

  const tList<cReaction>& GetReactions() const { return prior_reaction_list; }
  const tList<cReaction>& GetNoReactions() const { return prior_noreaction_list; }
  int GetMinTaskCount() const { return min_task_count; }
  int GetMaxTaskCount() const { return max_task_count; }
  int GetMinReactionCount() const { return min_reaction_count; }
  int GetMaxReactionCount() const { return max_reaction_count; }
  int GetDivideOnly() const { return divide_only; }
  int GetMinTotReactionCount() const { return min_tot_reaction_count; }
  int GetMaxTotReactionCount() const { return max_tot_reaction_count; }
  int GetParasiteOnly() const { return parasite_only; }

  void AddReaction(cReaction* in_reaction) {
    prior_reaction_list.PushRear(in_reaction);
  }
  void AddNoReaction(cReaction* in_reaction) {
    prior_noreaction_list.PushRear(in_reaction);
  }
  void SetMinTaskCount(int min) { min_task_count = min; }
  void SetMaxTaskCount(int max) { max_task_count = max; }
  void SetMinReactionCount(int min) { min_reaction_count = min; }
  void SetMaxReactionCount(int max) { max_reaction_count = max; }
  void SetDivideOnly(int div) { divide_only = div; }
  void SetMinTotReactionCount(int min) { min_tot_reaction_count = min; }
  void SetMaxTotReactionCount(int max) { max_tot_reaction_count = max; }
  void SetParasiteOnly(int para) { parasite_only = para; }

  /*
  added to satisfy Boost.Python; the semantics are fairly useless --
  equality of two references means that they refer to the same object.
  */
  bool operator==(const cReactionRequisite& in) const { return &in == this; }
};

#endif

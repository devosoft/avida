/*
 *  cContextReactionRequisite.h
 *  Avida
 *
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#ifndef cContextReactionRequisite_h
#define cContextReactionRequisite_h

#include <climits>

#ifndef tList_h
#include "tList.h"
#endif

class cReaction;

class cContextReactionRequisite
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


  cContextReactionRequisite(const cContextReactionRequisite&); // @not_implemented
  cContextReactionRequisite& operator=(const cContextReactionRequisite&);

public:
  cContextReactionRequisite() : min_task_count(0) , max_task_count(INT_MAX), 
    min_reaction_count(0) , max_reaction_count(INT_MAX),
	  min_tot_reaction_count(0), max_tot_reaction_count(INT_MAX), divide_only(0), parasite_only(0) { ; }
  ~cContextReactionRequisite() { ; }

  const tList<cReaction>& GetReactions() const { return prior_reaction_list; }
  const tList<cReaction>& GetNoReactions() const { return prior_noreaction_list; }
  int GetMinTaskCount() const { return min_task_count; }
  int GetMaxTaskCount() const { return max_task_count; }
  int GetMinReactionCount() const { return min_reaction_count; }
  int GetMaxReactionCount() const { return max_reaction_count; }
  int GetDivideOnly() const { return divide_only; }
  int GetParasiteOnly() const { return parasite_only; }
  int GetMinTotReactionCount() const { return min_tot_reaction_count; }
  int GetMaxTotReactionCount() const { return max_tot_reaction_count; }

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
  void SetParasiteOnly(int para) { parasite_only = para; }
  void SetMinTotReactionCount(int min) { min_tot_reaction_count = min; }
  void SetMaxTotReactionCount(int max) { max_tot_reaction_count = max; }

  /*
  added to satisfy Boost.Python; the semantics are fairly useless --
  equality of two references means that they refer to the same object.
  */
  bool operator==(const cContextReactionRequisite& in) const { return &in == this; }
};

#endif


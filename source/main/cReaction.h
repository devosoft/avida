/*
 *  cReaction.h
 *  Avida
 *
 *  Called "reaction.hh" prior to 12/5/05.
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

#ifndef cReaction_h
#define cReaction_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

class cTaskEntry;
class cReactionProcess;
class cReactionRequisite;
class cContextReactionRequisite;

class cReaction
{
private:
  cString name;
  int id;
  cTaskEntry* task;
  tList<cReactionProcess> process_list;
  tList<cReactionRequisite> requisite_list;
  tList<cContextReactionRequisite> context_requisite_list;
  bool active;
  bool internal;

  cReaction(); // @not_implemented
  cReaction(const cReaction&); // @not_implemented
  cReaction& operator=(const cReaction&); // @not_implemented

public:
  cReaction(const cString& _name, int _id);
  ~cReaction();

  const cString & GetName() const { return name; }
  int GetID() const { return id; }
  cTaskEntry* GetTask() { return task; }
  const tList<cReactionProcess>& GetProcesses() { return process_list; }
  cReactionProcess* GetProcess(int process = 0) { return process_list.GetPos(process); }
  const tList<cReactionRequisite>& GetRequisites() { return requisite_list; }
  const tList<cReactionRequisite>& GetRequisites() const { return requisite_list; }
  const tList<cContextReactionRequisite>& GetContextRequisites() { return context_requisite_list; }
  const tList<cContextReactionRequisite>& GetContextRequisites() const { return context_requisite_list; }
  bool GetActive() const { return active; }

  void SetTask(cTaskEntry* _task) { task = _task; }
  cReactionProcess* AddProcess();
  cReactionRequisite* AddRequisite();
  cContextReactionRequisite* AddContextRequisite();
  void SetActive(bool in_active = true) { active = in_active; }
  void SetInternal(bool in_internal = true) { internal = in_internal; }
  // These methods will modify the value of the process listed.
  bool ModifyValue(double new_value, int process_num = 0);
  bool MultiplyValue(double value_mult, int process_num = 0);

  // This method will modify the instruction triggered by this process
  bool ModifyInst(const cString& inst, int process_num = 0);

  // These methods will modify the min/max count of the requisite for this process
  bool SetMinTaskCount(int min_count, int requisite_num = 0);
  bool SetMaxTaskCount(int max_count, int requisite_num = 0);
  bool SetMinReactionCount(int reaction_min_count, int requisite_num = 0);
  bool SetMaxReactionCount(int reaction_max_count, int requisite_num = 0);
  
  double GetValue(int process_num = 0);
};

#endif

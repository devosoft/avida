/*
 *  cReaction.h
 *  Avida
 *
 *  Called "reaction.hh" prior to 12/5/05.
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

class cReaction
{
private:
  cString name;
  int id;
  cTaskEntry* task;
  tList<cReactionProcess> process_list;
  tList<cReactionRequisite> requisite_list;
  bool active;


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
  const tList<cReactionRequisite>& GetRequisites() { return requisite_list; }
  bool GetActive() const { return active; }

  void SetTask(cTaskEntry* _task) { task = _task; }
  cReactionProcess* AddProcess();
  cReactionRequisite* AddRequisite();
  void SetActive(bool in_active = true) { active = in_active; }

  // These methods will modify the value of the process listed.
  bool ModifyValue(double new_value, int process_num = 0);
  bool MultiplyValue(double value_mult, int process_num = 0); 

  // This method will modify the instruction triggered by this process
  bool ModifyInst(const cString& inst, int process_num = 0); 

  // These methods will modify the min/max count of the requisite for this process
  bool SetMinTaskCount(int min_count, int requisite_num = 0); 
  bool SetMaxTaskCount(int max_count, int requisite_num = 0); 

  double GetValue(int process_num = 0);
};

#endif

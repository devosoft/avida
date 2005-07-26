//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2004 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef REACTION_HH
#define REACTION_HH

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef TLIST_HH
#include "tList.hh"
#endif

class cString; // aggregate
class cTaskEntry;
template <class T> class tList; // aggregate
class cReactionProcess;
class cReactionRequisite;

class cReaction {
private:
  cString name;
  int id;
  cTaskEntry * task;
  tList<cReactionProcess> process_list;
  tList<cReactionRequisite> requisite_list;
  bool active;
private:
  // disabled copy constructor.
  cReaction(const cReaction &);
public:
  cReaction(const cString & _name, int _id);
  ~cReaction();

  const cString & GetName() const { return name; }
  int GetID() const { return id; }
  cTaskEntry * GetTask() { return task; }
  const tList<cReactionProcess> & GetProcesses() { return process_list; }
  const tList<cReactionRequisite> & GetRequisites()
    { return requisite_list; }
  bool GetActive() const { return active; }

  void SetTask(cTaskEntry * _task) { task = _task; }
  cReactionProcess * AddProcess();
  cReactionRequisite * AddRequisite();
  void SetActive(bool in_active=true) { active = in_active; }

  // These methods will modify the value of the process listed.
  bool ModifyValue(double new_value, int process_num=0);
  bool MultiplyValue(double value_mult, int process_num=0); 

  // This method will modify the instruction triggered by this process
  bool ModifyInst(int inst_id, int process_num=0); 

  double GetValue(int process_num=0);

};

#endif

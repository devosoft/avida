//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INTEGRATED_SCHEDULE_HH
#define INTEGRATED_SCHEDULE_HH

#ifndef SCHEDULE_HH
#include "schedule.hh"
#endif
#ifndef TARRAY_HH
#include "tArray.hh"
#endif

/**
 * The cIntegratedSchedule method relies on breaking up all merits into
 * sums of powers of 2 (i.e. using the binary representation of the merit).
 * All items with merits in the highest power of two will get the most
 * time, and subsequent merit components will have time divided,
 * continuing recursively.  The simplest way of doing this while maximizing
 * evenness of distribution of time slices is to simply alternate executing
 * the best, and everything else (where in everything else we again alternate
 * with the best of this sub-list recursively).
 **/

class cIntegratedScheduleNode;
class cMerit;
template <class T> class tArray; // aggregate

class cIntegratedSchedule : public cSchedule {
private:
  tArray<cIntegratedScheduleNode *> node_array;
  int num_active_nodes;
  cMerit * merit_chart;

  void InsertNode(int node_id);
  void RemoveNode(int node_id);
  void ResizeNodes(int new_size);
public:
  cIntegratedSchedule(int _item_count);
  ~cIntegratedSchedule();

  void Adjust(int item_id, const cMerit & new_merit);
  int GetNextID();
  double GetStatus(int id);

  bool OK();
};

#endif

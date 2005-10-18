//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CONST_SCHEDULE_HH
#define CONST_SCHEDULE_HH

#ifndef SCHEDULE_HH
#include "cSchedule.h"
#endif
#ifndef TARRAY_HH
#include "tArray.h"
#endif

/**
 * This class rotates between all items to schedule, giving each equal time.
 **/

class cMerit;
template <class T> class tArray; // aggregate;

class cConstSchedule : public cSchedule {
private:
  int last_id;
  tArray<bool> is_active;
public:
  cConstSchedule(int _item_count);
  ~cConstSchedule();

  bool OK();
  void Adjust(int item_id, const cMerit & merit);

  int GetNextID();
};

#endif

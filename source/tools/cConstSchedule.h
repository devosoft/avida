/*
 *  cConstSchedule.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cConstSchedule_h
#define cConstSchedule_h

#ifndef cSchedule_h
#include "cSchedule.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

/**
 * This class rotates between all items to schedule, giving each equal time.
 **/

class cMerit;

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

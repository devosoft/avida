/*
 *  cProbSchedule.cc
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cProbSchedule.h"

#include "cChangeList.h"
#include "cMerit.h"
#include "cWorld.h"


// The larger merits cause problems here; things need to be re-thought out.

cProbSchedule::cProbSchedule(cWorld* world, int _item_count)
  : cSchedule(_item_count)
  , m_world(world)
  , chart(_item_count)
{
}

cProbSchedule::~cProbSchedule()
{
}


int cProbSchedule::GetNextID()
{
  assert(chart.GetTotalWeight() > 0);
  const double position = m_world->GetRandom().GetDouble(chart.GetTotalWeight());
  return chart.FindPosition(position);
}

void cProbSchedule::Adjust(int item_id, const cMerit & item_merit)
{
  if (cChangeList *change_list = GetChangeList()) {
    change_list->MarkChange(item_id);
  }
  chart.SetWeight(item_id, item_merit.GetDouble());
}

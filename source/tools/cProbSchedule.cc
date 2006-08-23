/*
 *  cProbSchedule.cc
 *  Avida
 *
 *  Called "prob_schedule.cc" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cProbSchedule.h"

#include "cChangeList.h"
#include "cMerit.h"

// The larger merits cause problems here; things need to be re-thought out.

int cProbSchedule::GetNextID()
{
  assert(chart.GetTotalWeight() > 0);
  const double position = m_rng.GetDouble(chart.GetTotalWeight());
  return chart.FindPosition(position);
}

void cProbSchedule::Adjust(int item_id, const cMerit& item_merit)
{
  if (cChangeList *change_list = GetChangeList()) {
    change_list->MarkChange(item_id);
  }
  chart.SetWeight(item_id, item_merit.GetDouble());
}

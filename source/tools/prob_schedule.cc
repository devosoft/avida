//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef PROB_SCHEDULE_HH
#include "prob_schedule.hh"
#endif

#ifndef MERIT_HH
#include "merit.hh"
#endif
#ifndef TOOLS_HH
#include "tools.hh"
#endif

///////////////////
//  cProbSchedule
///////////////////

// The larger merits cause problems here; things need to be re-thought out.

cProbSchedule::cProbSchedule(int _item_count)
  : cSchedule(_item_count)
  , chart(_item_count)
{
}

cProbSchedule::~cProbSchedule()
{
}


int cProbSchedule::GetNextID()
{
  assert(chart.GetTotalWeight() > 0);
  const double position = g_random.GetDouble(chart.GetTotalWeight());
  return chart.FindPosition(position);
}

void cProbSchedule::Adjust(int item_id, const cMerit & item_merit)
{
  chart.SetWeight(item_id, item_merit.GetDouble());
}

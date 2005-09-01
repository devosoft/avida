//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_TRIGGERS_HH
#include "cEventTriggers.h"
#endif

#include <float.h>           // for DBL_MIN
#include <iostream>

using namespace std;

/////////////////
//  cEventTriggers
/////////////////

double const cEventTriggers::TRIGGER_BEGIN = DBL_MIN;
double const cEventTriggers::TRIGGER_END = DBL_MAX;
double const cEventTriggers::TRIGGER_ALL = 0;
double const cEventTriggers::TRIGGER_ONCE = DBL_MAX;

double cEventTriggers::GetTriggerValue( eTriggerVariable trigger ) const
{
  // Returns TRIGGER_END if invalid, TRIGGER_BEGIN for IMMEDIATE
  double t_val = TRIGGER_END;
  switch( trigger ){
  case IMMEDIATE:
    t_val = TRIGGER_BEGIN;
    break;
  case UPDATE:
    t_val = GetUpdate();
    break;
  case GENERATION:
    t_val = GetGeneration();
    break;
  default:
    cerr << "Unknown Trigger in cEventTriggers::GetTriggerValue"  << endl;
  }
  return t_val;
}

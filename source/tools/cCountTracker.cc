/*
 *  cCountTracker.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cCountTracker.h"


void cCountTracker::Inc()
{
  cur_count++;
  total_count++;
}

void cCountTracker::Dec() 
{
  cur_count--;
}

void cCountTracker::Next()
{
  last_count = cur_count;
  cur_count = 0;
}

void cCountTracker::Clear()
{
  cur_count = 0;
  last_count = 0;
  total_count = 0;
}

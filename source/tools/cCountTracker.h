/*
 *  cCountTracker.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cCountTracker_h
#define cCountTracker_h

class cCountTracker {
private:
  int cur_count;
  int last_count;
  int total_count;
public:
  cCountTracker() { Clear(); }
  ~cCountTracker() { ; }

  int GetCur() const { return cur_count; }
  int GetLast() const { return last_count; }
  int GetTotal() const { return total_count; }

  void Inc();
  void Dec();
  void Next();
  void Clear();
};

#endif

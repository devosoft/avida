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

class cCountTracker
{
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

  void Inc() { cur_count++; total_count++; }
  void Dec() { cur_count--; }
  void Next() { last_count = cur_count; cur_count = 0; }
  void Clear() { cur_count = last_count = total_count = 0; }

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

#endif

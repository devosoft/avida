//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef COUNT_TRACKER_HH
#define COUNT_TRACKER_HH

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

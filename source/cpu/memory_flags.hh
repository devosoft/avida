//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_FLAGS_HH
#define MEMORY_FLAGS_HH

class cMemoryFlags {
  friend class cCPUMemory;
protected:
  bool copied;      // Has this site been copied?
  bool mutated;     // Has this site been mutated?
  bool executed;    // Has this site been executed?
  bool breakpoint;  // Has a breakpoint been set at this position?
  bool point_mut;   // Has there been a point mutation here?
  bool copy_mut;    // Has there been a copy mutation here?
  bool injected;    // Was this instruction injected into the genome?

public:
  cMemoryFlags() { Clear(); }
  ~cMemoryFlags() { ; }

  void Clear();
};

#endif

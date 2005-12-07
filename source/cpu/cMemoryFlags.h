/*
 *  cMemoryFlags.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cMemoryFlags_h
#define cMemoryFlags_h

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

	void Clear()
	{
		copied = false;
		mutated = false;
		executed = false;
		breakpoint = false;
		point_mut = false;
		copy_mut = false;
		injected = false;
	}
};

#endif

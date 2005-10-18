//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef GENOME_UTIL_HH
#define GENOME_UTIL_HH

#ifndef GENOME_HH
#include "cGenome.h"
#endif

class cInstruction;
class cGenomeUtil {
public:
  // ========= Detection =========
  static int FindInst(const cGenome & gen, const cInstruction & inst,
		      int start_index=0);
  static bool HasInst(const cGenome & gen, const cInstruction & inst)
    { return ( FindInst(gen, inst) >= 0 ) ? true : false; }

  // === Genetic distance tools ===
  static int FindOverlap(const cGenome & gen1, const cGenome & gen2,
			 int offset = 0);
  static int FindHammingDistance(const cGenome & gen1, const cGenome & gen2,
				 int offset = 0);
  static int FindBestOffset(const cGenome & gen1, const cGenome & gen2);
  static int FindSlidingDistance(const cGenome & gen1, const cGenome & gen2);
  static int FindEditDistance(const cGenome & gen1, const cGenome & gen2);

  // ===== Construction methods =====
  static cGenome Crop(const cGenome & genome, int start, int end);
  static cGenome Cut(const cGenome & genome, int start, int end);
  static cGenome Join(const cGenome & genome1, const cGenome & genome2);
};

#endif

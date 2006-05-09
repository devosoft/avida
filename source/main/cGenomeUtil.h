/*
 *  cGenomeUtil.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cGenomeUtil_h
#define cGenomeUtil_h

#ifndef cGenome_h
#include "cGenome.h"
#endif

class cInstruction;

class cGenomeUtil
{
private:
  cGenomeUtil(); // @not_implemented
  
public:
  // ========= Detection =========
  static int FindInst(const cGenome& gen, const cInstruction& inst, int start_index = 0);
  static bool HasInst(const cGenome& gen, const cInstruction& inst)
  {
    return ( FindInst(gen, inst) >= 0 ) ? true : false;
  }

  // === Genetic distance tools ===
  static int FindOverlap(const cGenome& gen1, const cGenome& gen2, int offset = 0);
  static int FindHammingDistance(const cGenome& gen1, const cGenome& gen2, int offset = 0);
  static int FindBestOffset(const cGenome& gen1, const cGenome& gen2);
  static int FindSlidingDistance(const cGenome& gen1, const cGenome& gen2);
  static int FindEditDistance(const cGenome& gen1, const cGenome& gen2);

  // ===== Construction methods =====
  static cGenome Crop(const cGenome& genome, int start, int end);
  static cGenome Cut(const cGenome& genome, int start, int end);
  static cGenome Join(const cGenome& genome1, const cGenome& genome2);
};


#ifdef ENABLE_UNIT_TESTS
namespace nGenomeUtil {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

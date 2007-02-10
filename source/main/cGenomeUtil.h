/*
 *  cGenomeUtil.h
 *  Avida
 *
 *  Called "genome_util.hh" prior to 12/5/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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

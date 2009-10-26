/*
 *  cGenomeUtil.h
 *  Avida
 *
 *  Called "genome_util.hh" prior to 12/5/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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
#include <vector>

class cAvidaContext;
class cInstruction;
class cInstSet;

class cGenomeUtil
{
private:
  cGenomeUtil(); // @not_implemented
  
public:
  // ========= Detection =========
  static int FindInst(const cGenome& gen, const cInstruction& inst, int start_index = 0);
	static int CountInst(const cGenome& gen, const cInstruction& inst);
	static int MinDistBetween(const cGenome& gen, const cInstruction& inst);
	
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

	/*! Substring match record.
	 
	 The intent behind a substring match record is that it specifies where (position) a match
	 between two genomes was found, the length (extent) of that match in the base string,
	 and the edit distance (cost) of that match.
	 
	 The position and extent are inclusive, and describe the matching region in the
	 base string like so: [position-extent, position]
	 */
	struct substring_match {
		//! Default constructor.
		substring_match() : position(0), //extent(0),
		cost(0) { }
		//! Operator< overload.
		bool operator<(const substring_match& sm) { return cost < sm.cost; }
		int position; //!< Final position in the base string of this match.
		//int extent; //!< Length of the match in the base string.
		int cost; //!< Cost (edit distance) of this match.
	};
	
	//! Type for a list of substring matches.
	typedef std::vector<substring_match> substring_match_list_type;
	//! Return all matches of substring within base.
	static substring_match_list_type FindSubstringMatches(const cGenome& base, const cGenome& substring);
	//! Return the best match of substring within base.
	static substring_match FindBestSubstringMatch(const cGenome& base, const cGenome& substring);

  // ===== Construction methods =====
  static cGenome Crop(const cGenome& genome, int start, int end);
  static cGenome Cut(const cGenome& genome, int start, int end);
  static cGenome Join(const cGenome& genome1, const cGenome& genome2);

  // ========= Genome-File Interaction =========
  // Saving and loading of files.  These functions assume that the genome is
  // the only thing in the file unless 'Internal' is in the function name
  // (Internal genomes must begin with a number that indicates genome length)
  
  static bool LoadGenome(const cString& filename, const cInstSet& inst_set, cGenome& out_genome);
  static cGenome LoadGenome(const cString& filename, const cInstSet& inst_set);
  static cGenome LoadInternalGenome(std::istream& fp, const cInstSet& inst_set);
  static void SaveGenome(std::ostream& fp, const cInstSet& inst_set, const cGenome& gen);
  static void SaveInternalGenome(std::ostream& fp, const cInstSet& inst_set, const cGenome& gen);
  
  // ========= Genome Construction =========
  static cGenome RandomGenome(cAvidaContext& ctx, int length, const cInstSet& inst_set);
  static cGenome RandomGenomeWithoutZeroRedundantsPlusRepro(cAvidaContext& ctx, int length, const cInstSet& inst_set);
  static cGenome RandomGenomeWithoutZeroRedundantsPlusReproSex(cAvidaContext& ctx, int length, const cInstSet& inst_set);
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

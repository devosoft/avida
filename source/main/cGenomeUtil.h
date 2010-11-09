/*
 *  cGenomeUtil.h
 *  Avida
 *
 *  Called "genome_util.hh" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cSequence_h
#include "cSequence.h"
#endif
#include <vector>
#include <deque>

class cAvidaContext;
class cInstruction;
class cInstSet;

class cGenomeUtil
{
public:
	/*! Substring match record.
	 
	 The intent behind a substring match record is that it specifies where a match
	 between two genomes was found as well as the edit distance (cost) of that match.
	 
	 The begin, end members follow iterator semantics.  I.e., the matched region in
	 the base string is [begin, end).  (End points to the element immediately following
	 the match).
	 */
	//! Substring match record.
	struct substring_match {
		//! Default constructor.
		substring_match() : begin(0), end(0), cost(0), size(0) { }
		//! Initializing constructor.
		substring_match(int b, int e, int c=0, std::size_t s=0) : begin(b), end(e), cost(c), size(s) { }
		//! Convenience method to set all values of a substring match.
		void set(int b, int e, int c, std::size_t s) { begin = b; end = e; cost = c; size = s; }
		//! Operator< overload.
		bool operator<(const substring_match& that) const { return cost < that.cost; }
		//! Operator== overload (to support testing)
		bool operator==(const substring_match& that) const { 
			return (begin == that.begin) && (end == that.end) && (cost == that.cost) && (size == that.size);
		}
		//! Distance between begin and end.
		std::size_t distance();
		//! Resize to n (preserve the absolute distance between begin and end, based around begin).
		void resize(std::size_t n);
		//! Rotate around a size of n.
		void rotate(int r, std::size_t n);
		
		int begin; //!< Beginning of the substring match.
		int end; //!< Ending of the substring match.
		int cost; //!< Cost (edit distance) of this match.
		std::size_t size; //!< Size of the base string.
	};
	
	//! Find (one of) the best matches of substring in base.
	static substring_match FindSubstringMatch(const cSequence& base, const cSequence& substring);	
	//! Find (one of) the best unbiased matches of substring in base, respecting genome circularity.
	static substring_match FindUnbiasedCircularMatch(cAvidaContext& ctx, const cSequence& base, const cSequence& substring);
	typedef std::deque<cSequence> fragment_list_type; //!< Type for the list of genome fragments.
	//! Split a genome into a list of fragments, each with the given mean size and variance, and add them to the given fragment list.
	static void RandomSplit(cAvidaContext& ctx, double mean, double variance, const cSequence& genome, fragment_list_type& fragments);
	//! Randomly shuffle the instructions within genome in-place.
	static void RandomShuffle(cAvidaContext& ctx, cSequence& genome);
};

#endif

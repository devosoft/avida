/*
 *  cGenomeUtil.cc
 *  Avida
 *
 *  Called "genome_util.cc" prior to 12/5/05.
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

#include "cGenomeUtil.h"

#include "cAvidaContext.h"
#include "cSequence.h"
#include "cInitFile.h"
#include "cInstSet.h"
#include "cRandom.h"

#include "AvidaTools.h"

#include <algorithm>
#include <string.h>


using namespace std;
using namespace AvidaTools;






/*! Distance between begin and end.
 */
std::size_t cGenomeUtil::substring_match::distance() {
	std::size_t d=0;
	if(begin <= end) {
		d = end - begin;
	} else {
		d = size - begin + end;
	}
	return d;
}


/*! Resize to n (preserve the absolute distance between begin and end, based around begin).
 */
void cGenomeUtil::substring_match::resize(std::size_t n) {
	// fixup begin and end if they're negative or point beyond the current size
	if(begin < 0) { begin = size - (-begin) % size; }
	if(end < 0) { end = size - (-end) % size; }
	begin %= size;
	end %= size;
	
	// our work is done if there's no change in size...
	if(size == n) { return; }
	
	// if this is triggered, it means that the current begin index would
	// be truncated by the new, smaller string.
	assert(begin < static_cast<int>(n));
	
	// distance between begin and end:
	std::size_t d=distance();
	
	// if this is triggered, it means that the new size is smaller than the
	// region described by this match.
	assert(d < n);
	
	size = n;		
	end = (begin + d) % size;
}


/*! Rotate around a size of n.
 */
void cGenomeUtil::substring_match::rotate(int r, std::size_t n) {
	if(r < 0) {
		r = -((-r) % n);
	} else {
		r %= n;
	}
	begin += r;
	end += r;
	resize(n); // fixup the indices
}


/*! Find (one of) the best substring matches of substring in base.
 
 The algorithm here is based on the well-known dynamic programming approach to
 finding a substring match.  Here, it has been extended to track the beginning and
 ending locations of that match.  Specifically, [begin,end) of the returned substring_match
 denotes the matched region in the base string.
 */
cGenomeUtil::substring_match cGenomeUtil::FindSubstringMatch(const cSequence& base, const cSequence& substring) {
	const int rows=substring.GetSize()+1;
	const int cols=base.GetSize()+1;
	substring_match* m[2];
	m[0] = new substring_match[cols];
	m[1] = new substring_match[cols];
	substring_match* c=m[0];
	substring_match* p=m[1];
	
	for(int j=1; j<cols; ++j) {
		p[j].begin = j;
	}
	
	for(int i=1; i<rows; ++i) {
		c[0].cost = i;
		for(int j=1; j<cols; ++j) {
			substring_match l[3] = {p[j-1], p[j], c[j-1]};
			substring_match* s = &l[0]; // default match is to the upper left.
			
			if(substring[i-1] == base[j-1]) {
				// if the characters match, take the default
				c[j].cost = s->cost;
			} else {
				// otherwise, find the minimum cost, add 1.
				s = std::min_element(l,l+3);
				c[j].cost = s->cost + 1;				
			}
			
			// update the beginning and end of the match.
			c[j].begin = s->begin;
			c[j].end = j;
		}
		std::swap(c,p);
	}
	
	substring_match* min = std::min_element(p, p+cols);
	min->size = base.GetSize();
	delete [] m[0];
	delete [] m[1];
	return *min;
}


/*! Find (one of) the best unbiased matches of substring in base, respecting genome circularity.
 
 Substring matches are inherently biased towards matching near to the left-hand side of the string 
 (lower indices).  This method removes that bias by rotating the string prior to performing the
 match.
 
 Genomes in Avida are logically (not physically) circular, but substring matches in general do not 
 respect circularity.  To respect the logical circularity of genomes in Avida, we append the base
 string with substring-size instructions from the beginning of the base string.  This guarantees 
 that circular matches are detected.
 
 The return value here is de-circularfied and de-rotated such that [begin,end) are correct
 for the base string (note that, due to circularity, begin could be > end).
 */
cGenomeUtil::substring_match cGenomeUtil::FindUnbiasedCircularMatch(cAvidaContext& ctx, const cSequence& base, const cSequence& substring) {
	// create a copy of the genome:
	cSequence circ(base);
	
	// rotate it so that we remove bias for matching at the front of the genome:
	const int rotate = ctx.GetRandom().GetInt(circ.GetSize());
	circ.Rotate(rotate);
	
	// need to take circularity of the genome into account.
	// we can do this by appending the genome with a copy of its first substring-size instructions.
	cSequence head = circ.Crop(0, substring.GetSize());
	circ.Append(head);
	
	// find the location within the circular genome that best matches substring:
	cGenomeUtil::substring_match location = FindSubstringMatch(circ, substring);	
	
	// unwind the resizing & rotation:
	location.resize(base.GetSize());
	location.rotate(-rotate, base.GetSize());
	return location;
}


/*! Split a genome into a list of fragments, each with the given mean size and variance, and add them to the given fragment list.
 */
void cGenomeUtil::RandomSplit(cAvidaContext& ctx, double mean, double variance, const cSequence& genome, fragment_list_type& fragments) {	
	// rotate this genome to remove bais for the beginning and end of the genome:
	cSequence g(genome);
	g.Rotate(ctx.GetRandom().GetInt(g.GetSize()));
	
	// chop this genome up into pieces, add each to the back of the fragment list.
	int remaining_size = g.GetSize();
	int i= 0;
	do {
		int fsize=0;
		while(!fsize) {
			fsize = std::min(remaining_size, static_cast<int>(floor(fabs(ctx.GetRandom().GetRandNormal(mean, variance)))));
		}
		
		fragments.push_back(g.Crop(i, i + fsize));
		i += fsize;
		remaining_size -= fsize;
	} while (remaining_size > 0);
}


/*! Randomly shuffle the instructions within genome in-place.
 */
void cGenomeUtil::RandomShuffle(cAvidaContext& ctx, cSequence& genome) {
	std::vector<int> idx(static_cast<std::size_t>(genome.GetSize()));
	iota(idx.begin(), idx.end(), 0);
	cRandomStdAdaptor rng(ctx.GetRandom());
	std::random_shuffle(idx.begin(), idx.end(), rng);
	cSequence shuffled(genome.GetSize());
	for(int i=0; i<genome.GetSize(); ++i) {
		shuffled[i] = genome[idx[i]];
	}
	genome = shuffled;
}

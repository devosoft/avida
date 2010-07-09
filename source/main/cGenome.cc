/*
 *  cGenome.cc
 *  Avida
 *
 *  Called "genome.cc" prior to 12/2/05.
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

#include "cGenome.h"
#include <iterator>


using namespace std;


const int MEMORY_INCREASE_MINIMUM = 5;
const double MEMORY_INCREASE_FACTOR = 1.5;
const double MEMORY_SHRINK_TEST_FACTOR = 4.0;


cGenome::cGenome(int _size)
  : m_genome(_size), m_active_size(_size)
{
}

cGenome::cGenome(const cGenome & in_genome)
  : m_genome(in_genome.GetSize()), m_active_size(in_genome.GetSize()), m_mutation_steps(in_genome.GetMutationSteps())
{
  for (int i = 0; i < m_active_size; i++)  m_genome[i] = in_genome[i];
}

cGenome::cGenome(const cString & in_string)
{
  cString tmp_string(in_string);
  tmp_string.RemoveChar('_');  // Remove all blanks from alignments...

  m_active_size = tmp_string.GetSize();
  m_genome.ResizeClear(m_active_size);
  for (int i = 0; i < m_active_size; i++) {
    m_genome[i].SetSymbol(tmp_string[i]);
  }
}


/*! This constructor is used to build a new cGenome from a range of instructions.
It expects STL semantics for an iterator range.  We're avoiding templating this
(for now).  Refactor if a new range type is needed.
*/
cGenome::cGenome(const cInstruction* begin, const cInstruction* end) : m_active_size(0)
{
  m_genome.Resize(std::distance(begin,end));
  for(const cInstruction* i=begin; i!=end; i++, m_active_size++) {
		m_genome[m_active_size] = *i;
	}
}


cGenome::~cGenome()
{
}


void cGenome::adjustCapacity(int new_size)
{
  assert(new_size > 0);
  
  // Make sure we're really changing the size...
  if (new_size == m_active_size) return;
  
  const int array_size = m_genome.GetSize();
  
  // Determine if we need to adjust the allocated array sizes...
  if (new_size > array_size || new_size * MEMORY_SHRINK_TEST_FACTOR < array_size) {
    int new_array_size = (int) (new_size * MEMORY_INCREASE_FACTOR);
    const int new_array_min = new_size + MEMORY_INCREASE_MINIMUM;
		if (new_array_min > new_array_size) new_array_size = new_array_min;
    m_genome.Resize(new_array_size);
  }
  
  // And just change the m_active_size once we're sure it will be in range.
  m_active_size = new_size;
}

void cGenome::prepareInsert(int pos, int num_sites)
{
  assert(pos >= 0 && pos <= m_active_size); // Must insert at a legal position!
  assert(num_sites > 0); // Must insert positive number of lines!
  
  // Re-adjust the size...
  const int old_size = m_active_size;
  const int new_size = m_active_size + num_sites;
  adjustCapacity(new_size);
  
  // Shift any sites needed...
  for (int i = old_size - 1; i >= pos; i--) m_genome[i + num_sites] = m_genome[i];
}


void cGenome::Copy(int to, int from)
{
  assert(to   >= 0   && to   < m_active_size);
  assert(from >= 0   && from < m_active_size);
  m_genome[to] = m_genome[from];
}
 

// Return the genome as an alphabetic string

cString cGenome::AsString() const
{
  cString out_string(m_active_size);
  for (int i = 0; i < m_active_size; i++) {
    out_string[i] = m_genome[i].GetSymbol();
  }

  return out_string;
}


void cGenome::Resize(int new_size)
{
  assert(new_size >= 0);
  
  const int old_size = m_active_size;
  adjustCapacity(new_size);
  
  for (int i = old_size; i < new_size; i++) m_genome[i].SetOp(0);
}

void cGenome::Insert(int pos, const cInstruction& inst)
{
  assert(pos >= 0);
  assert(pos <= m_genome.GetSize());
  
  prepareInsert(pos, 1);
  m_genome[pos] = inst;
}

void cGenome::Insert(int pos, const cGenome& genome)
{
  assert(pos >= 0);
  assert(pos <= m_genome.GetSize());
  
  prepareInsert(pos, genome.GetSize());
  for (int i = 0; i < genome.GetSize(); i++) m_genome[i + pos] = genome[i];
}

void cGenome::Remove(int pos, int num_sites)
{
  assert(num_sites > 0);                    // Must remove something...
  assert(pos >= 0);                         // Removal must be in genome
  assert(pos + num_sites <= m_active_size); // Cannot extend past end of genome
  
  const int new_size = m_active_size - num_sites;
  for (int i = pos; i < new_size; i++) m_genome[i] = m_genome[i + num_sites];
  adjustCapacity(new_size);
}

void cGenome::Replace(int pos, int num_sites, const cGenome& genome)
{
  assert(pos >= 0);                         // Replace must be in genome
  assert(num_sites >= 0);                   // Cannot replace negative
  assert(pos + num_sites <= m_active_size); // Cannot extend past end!
  
  const int size_change = genome.GetSize() - num_sites;
  
  // First, get the size right
  if (size_change > 0) prepareInsert(pos, size_change);
  else if (size_change < 0) Remove(pos, -size_change);
  
  // Now just copy everything over!
  for (int i = 0; i < genome.GetSize(); i++) m_genome[i + pos] = genome[i];
}

/*! Replace [begin, end) instructions in this genome with g, respecting genome circularity.
 
 This method replaces [begin,end) instructions in this genome with those in g.  Begin and end
 follow STL-iterator semantics, which is to say that end "points" to **one past** the last
 instruction that will be replaced.
 
 Also, circularity of the genome is respected, which means that if end < begin 
 (the instructions that are being replaced wrap-around the end of the genome), then
 the replacement will too.
 
 Caveat: if length([begin,end)) != length(g), all size changes are made at end.
 */
void cGenome::Replace(const cGenome& g, int begin, int end) {
	if(begin == end) {
		// we're actually doing an insertion...
		Insert(begin, g);
	} else if(begin < end) {
		// no wrap-around
		Replace(begin, end-begin, g);
	} else {
		// replacement wraps around the end.  two different replacements to do now:
		// [begin, size) and [0, end).
		
		// first, replace the [begin, size) region of this genome with as much of g
		// as we can get.
		int tail_size = std::min(GetSize()-begin, g.GetSize());
		cGenome tail(&g[0], &g[0]+tail_size);
		Replace(begin, GetSize()-begin, tail);

		// now, replace the [0, end) region or remove it if the whole fragment
		// was already copied in:
		if(tail_size != g.GetSize()) {
			cGenome head(&g[0]+tail_size, &g[0]+g.GetSize());
			Replace(0, end, head);
		} else if(end > 0) {
			Remove(0, end);
		}
	}
}

/*! Rotate this genome forward n instructions.
 
 "Rotation" in this sense means to move instructions from begin->end, with instructions
 at the end wrapping around to the beginning.  Specifically, given a genome
 [0... n... m-n... m], Rotate(n) moves instructions to create [m-n... m, 0... n].
 
 Negative rotation is supported, and moves instructions from the beginning to the end.
 */
void cGenome::Rotate(int n) {
	assert(n < m_active_size);
	if(n==0) { return; }

	cInstruction* begin = &operator[](0);
	cInstruction* end = &operator[](0) + GetSize();
	
	if(n > 0) {
		// forward
		cGenome head(end-n, end);
		cGenome tail(begin, end-n);
		head.Append(tail);
		operator=(head);
	} else {
		assert(false);
		// backward
		cGenome head(begin, begin-n); // n is < 0, so this is addition.
		cGenome tail(begin-n, end);
		tail.Append(head);
		operator=(tail);
	}
}


void cGenome::operator=(const cGenome& other_genome)
{
  m_active_size = other_genome.m_active_size;
  m_genome.ResizeClear(m_active_size);
  
  // Now that both code arrays are the same size, copy the other one over
  for (int i = 0; i < m_active_size; i++) m_genome[i] = other_genome[i];
  
  m_mutation_steps = other_genome.m_mutation_steps;
}


bool cGenome::operator==(const cGenome& other_genome) const
{
  // Make sure the sizes are the same.
  if (m_active_size != other_genome.m_active_size) return false;
  
  // Then go through line by line.
  for (int i = 0; i < m_active_size; i++)
    if (m_genome[i] != other_genome[i]) return false;
  
  return true;
}



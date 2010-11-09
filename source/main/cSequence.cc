/*
 *  cSequence.cc
 *  Avida
 *
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

#include "cSequence.h"

#include "AvidaTools.h"
#include "cInstSet.h"

using namespace AvidaTools;

const int MEMORY_INCREASE_MINIMUM = 5;
const double MEMORY_INCREASE_FACTOR = 1.5;
const double MEMORY_SHRINK_TEST_FACTOR = 4.0;


cSequence::cSequence(const cSequence& seq)
  : m_seq(seq.GetSize()), m_active_size(seq.GetSize()), m_mutation_steps(seq.GetMutationSteps())
{
  for (int i = 0; i < m_active_size; i++)  m_seq[i] = seq[i];
}

cSequence::cSequence(const cString & str)
{
  cString tmp_string(str);
  tmp_string.RemoveChar('_');  // Remove all blanks from alignments...

  m_active_size = tmp_string.GetSize();
  m_seq.ResizeClear(m_active_size);
  for (int i = 0; i < m_active_size; i++) m_seq[i].SetSymbol(tmp_string[i]);
}


cSequence::~cSequence()
{
}


void cSequence::adjustCapacity(int new_size)
{
  assert(new_size > 0);
  
  // Make sure we're really changing the size...
  if (new_size == m_active_size) return;
  
  const int array_size = m_seq.GetSize();
  
  // Determine if we need to adjust the allocated array sizes...
  if (new_size > array_size || new_size * MEMORY_SHRINK_TEST_FACTOR < array_size) {
    int new_array_size = (int) (new_size * MEMORY_INCREASE_FACTOR);
    const int new_array_min = new_size + MEMORY_INCREASE_MINIMUM;
		if (new_array_min > new_array_size) new_array_size = new_array_min;
    m_seq.Resize(new_array_size);
  }
  
  // And just change the m_active_size once we're sure it will be in range.
  m_active_size = new_size;
}

void cSequence::prepareInsert(int pos, int num_sites)
{
  assert(pos >= 0 && pos <= m_active_size); // Must insert at a legal position!
  assert(num_sites > 0); // Must insert positive number of lines!
  
  // Re-adjust the size...
  const int old_size = m_active_size;
  const int new_size = m_active_size + num_sites;
  adjustCapacity(new_size);
  
  // Shift any sites needed...
  for (int i = old_size - 1; i >= pos; i--) m_seq[i + num_sites] = m_seq[i];
}


void cSequence::Copy(int to, int from)
{
  assert(to   >= 0   && to   < m_active_size);
  assert(from >= 0   && from < m_active_size);
  m_seq[to] = m_seq[from];
}
 

// Return the sequence as an alphabetic string
cString cSequence::AsString() const
{
  cString out_string(m_active_size);
  for (int i = 0; i < m_active_size; i++) out_string[i] = m_seq[i].GetSymbol();

  return out_string;
}


void cSequence::Resize(int new_size)
{
  assert(new_size >= 0);
  
  const int old_size = m_active_size;
  adjustCapacity(new_size);
  
  for (int i = old_size; i < new_size; i++) m_seq[i].SetOp(0);
}

void cSequence::Insert(int pos, const cInstruction& inst)
{
  assert(pos >= 0);
  assert(pos <= m_seq.GetSize());
  
  prepareInsert(pos, 1);
  m_seq[pos] = inst;
}

void cSequence::Insert(int pos, const cSequence& seq)
{
  assert(pos >= 0);
  assert(pos <= m_seq.GetSize());
  
  prepareInsert(pos, seq.GetSize());
  for (int i = 0; i < seq.GetSize(); i++) m_seq[i + pos] = seq[i];
}

void cSequence::Remove(int pos, int num_sites)
{
  assert(num_sites > 0);                    // Must remove something...
  assert(pos >= 0);                         // Removal must be in sequence
  assert(pos + num_sites <= m_active_size); // Cannot extend past end of sequence
  
  const int new_size = m_active_size - num_sites;
  for (int i = pos; i < new_size; i++) m_seq[i] = m_seq[i + num_sites];
  adjustCapacity(new_size);
}

void cSequence::Replace(int pos, int num_sites, const cSequence& seq)
{
  assert(pos >= 0);                         // Replace must be in sequence
  assert(num_sites >= 0);                   // Cannot replace negative
  assert(pos + num_sites <= m_active_size); // Cannot extend past end!
  
  const int size_change = seq.GetSize() - num_sites;
  
  // First, get the size right
  if (size_change > 0) prepareInsert(pos, size_change);
  else if (size_change < 0) Remove(pos, -size_change);
  
  // Now just copy everything over!
  for (int i = 0; i < seq.GetSize(); i++) m_seq[i + pos] = seq[i];
}


/*
 Replace [begin, end) instructions in this sequence with g, respecting sequence circularity.
 
 This method replaces [begin,end) instructions in this sequence with those in g.  Begin and end
 follow STL-iterator semantics, which is to say that end "points" to **one past** the last
 instruction that will be replaced.
 
 Also, circularity of the sequence is respected, which means that if end < begin 
 (the instructions that are being replaced wrap-around the end of the sequence), then
 the replacement will too.
 
 Caveat: if length([begin,end)) != length(g), all size changes are made at end.
*/
void cSequence::Replace(const cSequence& g, int begin, int end)
{
	if (begin == end) {
		// we're actually doing an insertion...
		Insert(begin, g);
	} else if (begin < end) {
		// no wrap-around
		Replace(begin, end - begin, g);
	} else {
		// replacement wraps around the end.  two different replacements to do now: [begin, size) and [0, end).
		
		// first, replace the [begin, size) region of this sequence with as much of g as we can get.
		int tail_size = Min(m_active_size - begin, g.GetSize());
		cSequence tail = g.Crop(0, tail_size);
		Replace(begin, (m_active_size - begin), tail);

		// now, replace the [0, end) region or remove it if the whole fragment was already copied in:
		if (tail_size != g.GetSize()) {
			cSequence head = g.Crop(tail_size, g.GetSize());
			Replace(0, end, head);
		} else if(end > 0) {
			Remove(0, end);
		}
	}
}


/*
 Rotate this sequence forward n instructions.
 
 "Rotation" in this sense means to move instructions from begin->end, with instructions
 at the end wrapping around to the beginning.  Specifically, given a sequence
 [0... n... m-n... m], Rotate(n) moves instructions to create [m-n... m, 0... n].
 
 Negative rotation is supported, and moves instructions from the beginning to the end.
*/
void cSequence::Rotate(int n)
{
	assert(n < m_active_size);
	if(n==0) { return; }

	if (n > 0) {
		// forward
		cSequence head = Crop(m_active_size - n, m_active_size);
		cSequence tail = Crop(0, m_active_size - n);
		head.Append(tail);
		operator=(head);
	} else {
		assert(false);
		// backward
		cSequence head = Crop(0, -n); // n is < 0, so this is addition.
		cSequence tail = Crop(-n, m_active_size);
		tail.Append(head);
		operator=(tail);
	}
}


void cSequence::operator=(const cSequence& other_seq)
{
  m_active_size = other_seq.m_active_size;
  m_seq.ResizeClear(m_active_size);
  
  // Now that both code arrays are the same size, copy the other one over
  for (int i = 0; i < m_active_size; i++) m_seq[i] = other_seq[i];
  
  m_mutation_steps = other_seq.m_mutation_steps;
}


bool cSequence::operator==(const cSequence& other_seq) const
{
  // Make sure the sizes are the same.
  if (m_active_size != other_seq.m_active_size) return false;
  
  // Then go through line by line.
  for (int i = 0; i < m_active_size; i++)
    if (m_seq[i] != other_seq[i]) return false;
  
  return true;
}


int cSequence::FindInst(const cInstruction& inst, int start_index) const
{
  assert(start_index < m_active_size);  // Starting search after sequence end.
  
  for(int i = start_index; i < m_active_size; i++) if (m_seq[i] == inst) return i;
  
  // Search failed
  return -1;  
}


int cSequence::CountInst(const cInstruction& inst) const
{
  int count = 0;
  for (int i = 0; i < m_active_size; i++) if (m_seq[i] == inst) count++;
  return count;  
}


int cSequence::MinDistBetween(const cInstruction& inst) const
{
	int firstInstance = -1;
	int secondInstance = -1;
	int startIndex = 0;
	int minDist = m_active_size;
	assert(startIndex < m_active_size);
	
	while (startIndex < m_active_size) {
		firstInstance = FindInst(inst, startIndex);
		if (firstInstance == -1 && startIndex == 0) {
			// no instance of inst
			return 0;
		} else if (firstInstance == -1) {
			// no more instances
			return minDist;
		}
		
		startIndex = firstInstance + 1;
		secondInstance = FindInst(inst, startIndex);
		
		if (secondInstance == -1) {
			// no instance between startIndex and end
			// search from begining
			secondInstance = FindInst(inst, 0);
			// worst-case this finds same instance of inst as firstInstance
		}			
    
		if (firstInstance != secondInstance) {
			minDist = Min(Min(Abs(firstInstance - secondInstance), secondInstance + m_active_size - firstInstance), minDist);
			assert(minDist > 0);
		} else { // they are equal, so there is only one instance of inst
			return 0;
		} 
	}
  
	return minDist;
}


void cSequence::SaveInstructions(std::ostream& fp, const cInstSet& inst_set) const
{
  for (int i = 0; i < m_active_size; i++) fp << inst_set.GetName(m_seq[i]) << endl;
}


cSequence cSequence::Crop(int start, int end) const
{
  assert(end > start);                // Must have a positive length clip!
  assert(m_active_size >= end);       // end must be < sequence length
  assert(start >= 0);                 // negative start illegal
  
  const int out_length = end - start;
  cSequence out_seq(out_length);
  for (int i = 0; i < out_length; i++) out_seq[i] = m_seq[i+start];
  
  return out_seq;
}


cSequence cSequence::Cut(int start, int end) const
{
  assert(end > start);                // Must have a positive size cut!
  assert(m_active_size >= end);       // end must be < sequence length
  assert(start >= 0);                 // negative start illegal
  
  const int cut_length = end - start;
  const int out_length = m_active_size - cut_length;
  
  assert(out_length > 0);             // Can't cut everything!
  
  cSequence out_seq(out_length);
  for (int i = 0; i < start; i++) out_seq[i] = m_seq[i];
  for (int i = start; i < out_length; i++) out_seq[i] = m_seq[i + cut_length];
  
  return out_seq;
}  

cSequence cSequence::Join(const cSequence& lhs, const cSequence& rhs)
{
  const int length1 = lhs.GetSize();
  const int length2 = rhs.GetSize();
  const int out_length = length1 + length2;
  
  cSequence out_seq(out_length);
  for (int i = 0; i < length1; i++) out_seq[i] = lhs[i];
  for (int i = 0; i < length2; i++) out_seq[i + length1] = rhs[i];
  
  return out_seq;
}


int cSequence::FindOverlap(const cSequence& seq1, const cSequence& seq2, int offset)
{
  assert(offset < seq1.GetSize());
  assert(-offset < seq2.GetSize());
  
  if (offset > 0) return Min(seq1.GetSize() - offset, seq2.GetSize());

  return Min(seq2.GetSize() + offset, seq1.GetSize());
}


int cSequence::FindHammingDistance(const cSequence& seq1, const cSequence& seq2, int offset)
{
  const int start1 = (offset < 0) ? 0 : offset;
  const int start2 = (offset > 0) ? 0 : -offset;
  const int overlap = FindOverlap(seq1, seq2, offset);
  
  // Initialize the hamming distance to anything protruding past the overlap.
  
  int hamming_distance = seq1.GetSize() + seq2.GetSize() - 2 * overlap;
  
  // Cycle through the overlap adding all differences to the distance.
  for (int i = 0; i < overlap; i++) {
    if (seq1[start1 + i] != seq2[start2 + i])  hamming_distance++;
  }
  
  return hamming_distance;
}


int cSequence::FindBestOffset(const cSequence& seq1, const cSequence& seq2)
{
  const int size1 = seq1.GetSize();
  const int size2 = seq2.GetSize();
  
  int best_offset = 0;
  int cur_distance = FindHammingDistance(seq1, seq2);
  int best_distance = cur_distance;
  
  // Check positive offsets...
  for (int i = 1; i < size1 || i < size2; i++) {
    if (size1 + size2 - 2 * FindOverlap(seq1, seq2, i) > best_distance) break;
    cur_distance = FindHammingDistance(seq1, seq2, i);
    if (cur_distance < best_distance) {
      best_distance = cur_distance;
      best_offset = i;
    }
  }
  
  // Check negative offsets...
  for (int i = 1; i < size1 || i < size2; i++) {
    if (size1 + size2 - 2 * FindOverlap(seq1, seq2, -i) > best_distance) break;
    cur_distance = FindHammingDistance(seq1, seq2, -i);
    if (cur_distance < best_distance) {
      best_distance = cur_distance;
      best_offset = -i;
    }
  }
  
  return best_offset;
}


int cSequence::FindSlidingDistance(const cSequence& seq1, const cSequence& seq2)
{
  const int offset = FindBestOffset(seq1, seq2);
  return FindHammingDistance(seq1, seq2, offset);
}


int cSequence::FindEditDistance(const cSequence& seq1, const cSequence& seq2)
{
  const int size1 = seq1.GetSize();
  const int size2 = seq2.GetSize();
  const int min_size = min(size1, size2);
  
  // If either size is zero, return the other one!
  if (!min_size) return max(size1, size2);
  
  // Count how many direct matches we have at the front and end.
  int match_front = 0, match_end = 0;
  while (match_front < min_size && seq1[match_front] == seq2[match_front]) match_front++;
  while (match_end < min_size && seq1[size1 - match_end - 1] == seq2[size2 - match_end - 1]) match_end++;
  
  // We can ignore the last match_end sites since we know they have distance zero.
  const int test_size1 = size1 - match_front - match_end;
  const int test_size2 = size2 - match_front - match_end;
  
  if (test_size1 <= 0 || test_size2 <=0) return abs(test_size1 - test_size2);
  
  // Now match everything else...
  int* cur_row  = new int[test_size1];  // The row we are calculating
  int* prev_row = new int[test_size1];  // The last row we calculated
  
  // Initialize the previous row to record the differece from nothing.
  for (int i = 0; i < test_size1; i++)  prev_row[i] = i + 1;
  
  // Loop through each subsequent character in the test code
  for (int i = 0; i < test_size2; i++) {
    // Initialize the first entry in cur_row.
    if (seq1[match_front] == seq2[match_front + i]) cur_row[0] = i;
    else cur_row[0] = (i < prev_row[0]) ? (i+1) : (prev_row[0] + 1);
    
    // Move down the cur_row and fill it out.
    for (int j = 1; j < test_size1; j++) {
      // If the values are equal, keep the value in the upper left.
      if (seq1[match_front + j] == seq2[match_front + i]) {
        cur_row[j] = prev_row[j-1];
      }
      
      // Otherwise, set the current position the the minimal of the three
      // numbers above (insertion), to the left (deletion), or upper left
      // (mutation) in the chart, plus one.
      else {
        cur_row[j] = (prev_row[j] < prev_row[j-1]) ? prev_row[j] : prev_row[j-1];
        if (cur_row[j-1] < cur_row[j]) cur_row[j] = cur_row[j-1];
        cur_row[j]++;
      }
    }
    
    // Swap cur_row and prev_row. (we only really need to move the cur row
    // over to prev, but this saves us from having to keep re-allocating
    // new rows.  We recycle!
    int* temp_row = cur_row;
    cur_row = prev_row;
    prev_row = temp_row;
  }
  
  // Now that we are done, return the bottom-right corner of the chart.
  
  const int value = prev_row[test_size1 - 1];
  
  delete [] cur_row;
  delete [] prev_row;
  
  return value;
}

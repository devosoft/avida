/*
 *  core/InstructionSequence.cc
 *  avida-core
 *
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *  http://avida.devosoft.org/
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/core/InstructionSequence.h"

#include "AvidaTools.h"

using namespace AvidaTools;


const int MEMORY_INCREASE_MINIMUM = 5;
const double MEMORY_INCREASE_FACTOR = 1.5;
const double MEMORY_SHRINK_TEST_FACTOR = 4.0;


Avida::InstructionSequence::InstructionSequence(const InstructionSequence& seq)
: GeneticRepresentation(seq), m_seq(seq.GetSize()), m_active_size(seq.GetSize())
{
  for (int i = 0; i < m_active_size; i++)  m_seq[i] = seq[i];
}

Avida::InstructionSequence::InstructionSequence(const Apto::String& str)
{
  m_seq.ResizeClear(str.GetSize());
  int size = 0;
  for (int i = 0; i < str.GetSize(); i++) {
    if (str[i] == '_') continue;
    switch (str[i]) {
      case '+':
      case '-':
      case '~':
      case '?':
        if (!m_seq[size].SetSymbol(str.Substring(i, 2))) continue;
        i++;
        break;
      default:
        if (!m_seq[size].SetSymbol(str.Substring(i, 1))) continue;
    }
    size++;
  }
  m_active_size = size;
  m_seq.Resize(size);
}

Avida::InstructionSequence::~InstructionSequence() { ; }


Apto::String Avida::Instruction::GetSymbol() const
{
  // aA0 +a+A+0 -a-A-0 ~a~A~0 ?a
  char symbol[3];
  symbol[1] = symbol[2] = '\0';
  
  if (m_operand == 255) {
    symbol[0] = '_';
  } else {
    int idx = 0;
    int offset = m_operand % 62;

    switch (m_operand / 62) {
      case 1:
        idx = 1;
        symbol[0] = '+';
        break;
      case 2:
        idx = 1;
        symbol[0] = '-';
        break;
      case 3:
        idx = 1;
        symbol[0] = '~';
        break;
      case 4:
        idx = 1;
        symbol[0] = '?';
        break;
    }
    
    if (offset < 26) symbol[idx] = offset + 'a';
    else if (offset < 52) symbol[idx] = offset - 26 + 'A';
    else if (offset < 62) symbol[idx] = offset - 52 + '0';
  }
  
  return  Apto::String(symbol); 
}

bool Avida::Instruction::SetSymbol(const Apto::String& symbol)
{
  assert(symbol.GetSize() > 0);
  
  char sym_char = symbol[0];
  int operand = 0;
  switch (sym_char) {
    case '+':
      if (symbol.GetSize() != 2) return false;
      operand = 62;
      sym_char = symbol[1];
      break;
    case '-':
      if (symbol.GetSize() != 2) return false;
      operand = 124;
      sym_char = symbol[1];
      break;
    case '~':
      if (symbol.GetSize() != 2) return false;
      operand = 186;
      sym_char = symbol[1];
      break;
    case '?':
      if (symbol.GetSize() != 2) return false;
      operand = 248;
      sym_char = symbol[1];
      break;
    case '_':
      m_operand = 255;
      return true;
  }
  
  if (sym_char >= 'a' && sym_char <= 'z') operand += sym_char - 'a';
  else if (sym_char >= 'A' && sym_char <= 'Z') operand += sym_char - 'A' + 26;
  else if (sym_char >= '0' && sym_char <= '9') operand += sym_char - '0' + 52;
  else return false;

  m_operand = operand;
  return true;
}




void Avida::InstructionSequence::adjustCapacity(int new_size)
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

void Avida::InstructionSequence::prepareInsert(int pos, int num_sites)
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


void Avida::InstructionSequence::Copy(int to, int from)
{
  assert(to   >= 0   && to   < m_active_size);
  assert(from >= 0   && from < m_active_size);
  m_seq[to] = m_seq[from];
}
 

// Return the sequence as an alphabetic string
Apto::String Avida::InstructionSequence::AsString() const
{
  Apto::StringBuffer out_string;
  for (int i = 0; i < m_active_size; i++) out_string += m_seq[i].GetSymbol();

  return Apto::String(out_string);
}


Avida::GeneticRepresentationPtr Avida::InstructionSequence::Clone() const
{
  return GeneticRepresentationPtr(new InstructionSequence(*this));
}

bool Avida::InstructionSequence::Serialize(ArchivePtr) const
{
  assert(false);
  // @TODO - InstructionSequence::Serialize
  return false;
}


void Avida::InstructionSequence::Resize(int new_size)
{
  assert(new_size >= 0);
  
  const int old_size = m_active_size;
  adjustCapacity(new_size);
  
  for (int i = old_size; i < new_size; i++) m_seq[i].SetOp(0);
}

void Avida::InstructionSequence::Insert(int pos, const Instruction& inst)
{
  assert(pos >= 0);
  assert(pos <= m_seq.GetSize());
  
  prepareInsert(pos, 1);
  m_seq[pos] = inst;
}

void Avida::InstructionSequence::Insert(int pos, const InstructionSequence& seq)
{
  assert(pos >= 0);
  assert(pos <= m_seq.GetSize());
  
  prepareInsert(pos, seq.GetSize());
  for (int i = 0; i < seq.GetSize(); i++) m_seq[i + pos] = seq[i];
}

void Avida::InstructionSequence::Remove(int pos, int num_sites)
{
  assert(num_sites > 0);                    // Must remove something...
  assert(pos >= 0);                         // Removal must be in sequence
  assert(pos + num_sites <= m_active_size); // Cannot extend past end of sequence
  
  const int new_size = m_active_size - num_sites;
  for (int i = pos; i < new_size; i++) m_seq[i] = m_seq[i + num_sites];
  adjustCapacity(new_size);
}

void Avida::InstructionSequence::Replace(int pos, int num_sites, const InstructionSequence& seq)
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
void Avida::InstructionSequence::Replace(const InstructionSequence& g, int begin, int end)
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
		int tail_size = Apto::Min(m_active_size - begin, g.GetSize());
		InstructionSequence tail = g.Crop(0, tail_size);
		Replace(begin, (m_active_size - begin), tail);

		// now, replace the [0, end) region or remove it if the whole fragment was already copied in:
		if (tail_size != g.GetSize()) {
			InstructionSequence head = g.Crop(tail_size, g.GetSize());
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
void Avida::InstructionSequence::Rotate(int n)
{
	assert(n < m_active_size);
	if(n==0) { return; }

	if (n > 0) {
		// forward
		InstructionSequence head = Crop(m_active_size - n, m_active_size);
		InstructionSequence tail = Crop(0, m_active_size - n);
		head.Append(tail);
		operator=(head);
	} else {
		assert(false);
		// backward
		InstructionSequence head = Crop(0, -n); // n is < 0, so this is addition.
		InstructionSequence tail = Crop(-n, m_active_size);
		tail.Append(head);
		operator=(tail);
	}
}


void Avida::InstructionSequence::operator=(const InstructionSequence& other_seq)
{
  m_active_size = other_seq.m_active_size;
  m_seq.ResizeClear(m_active_size);
  
  // Now that both code arrays are the same size, copy the other one over
  for (int i = 0; i < m_active_size; i++) m_seq[i] = other_seq[i];
}


bool Avida::InstructionSequence::operator==(const GeneticRepresentation& other_seq) const
{
  const InstructionSequence* seq = dynamic_cast<const InstructionSequence*>(&other_seq);
  if (!seq) return false;
  
  // Make sure the sizes are the same.
  if (m_active_size != seq->m_active_size) return false;
  
  // Then go through line by line.
  for (int i = 0; i < m_active_size; i++)
    if (m_seq[i] != (*seq)[i]) return false;
  
  return true;
}


int Avida::InstructionSequence::FindInst(const Instruction& inst, int start_index) const
{
  assert(start_index < m_active_size);  // Starting search after sequence end.
  
  for(int i = start_index; i < m_active_size; i++) if (m_seq[i] == inst) return i;
  
  // Search failed
  return -1;  
}


int Avida::InstructionSequence::CountInst(const Instruction& inst) const
{
  int count = 0;
  for (int i = 0; i < m_active_size; i++) if (m_seq[i] == inst) count++;
  return count;  
}


int Avida::InstructionSequence::MinDistBetween(const Instruction& inst) const
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
			minDist = Apto::Min(Apto::Min(Abs(firstInstance - secondInstance), secondInstance + m_active_size - firstInstance), minDist);
			assert(minDist > 0);
		} else { // they are equal, so there is only one instance of inst
			return 0;
		} 
	}
  
	return minDist;
}


Avida::InstructionSequence Avida::InstructionSequence::Crop(int start, int end) const
{
  assert(end > start);                // Must have a positive length clip!
  assert(m_active_size >= end);       // end must be < sequence length
  assert(start >= 0);                 // negative start illegal
  
  const int out_length = end - start;
  InstructionSequence out_seq(out_length);
  for (int i = 0; i < out_length; i++) out_seq[i] = m_seq[i+start];
  
  return out_seq;
}


Avida::InstructionSequence Avida::InstructionSequence::Cut(int start, int end) const
{
  assert(end > start);                // Must have a positive size cut!
  assert(m_active_size >= end);       // end must be < sequence length
  assert(start >= 0);                 // negative start illegal
  
  const int cut_length = end - start;
  const int out_length = m_active_size - cut_length;
  
  assert(out_length > 0);             // Can't cut everything!
  
  InstructionSequence out_seq(out_length);
  for (int i = 0; i < start; i++) out_seq[i] = m_seq[i];
  for (int i = start; i < out_length; i++) out_seq[i] = m_seq[i + cut_length];
  
  return out_seq;
}  

Avida::InstructionSequence Avida::InstructionSequence::Join(const InstructionSequence& lhs, const InstructionSequence& rhs)
{
  const int length1 = lhs.GetSize();
  const int length2 = rhs.GetSize();
  const int out_length = length1 + length2;
  
  InstructionSequence out_seq(out_length);
  for (int i = 0; i < length1; i++) out_seq[i] = lhs[i];
  for (int i = 0; i < length2; i++) out_seq[i + length1] = rhs[i];
  
  return out_seq;
}


int Avida::InstructionSequence::FindOverlap(const InstructionSequence& seq1, const InstructionSequence& seq2, int offset)
{
  assert(offset < seq1.GetSize());
  assert(-offset < seq2.GetSize());
  
  if (offset > 0) return Apto::Min(seq1.GetSize() - offset, seq2.GetSize());

  return Apto::Min(seq2.GetSize() + offset, seq1.GetSize());
}


int Avida::InstructionSequence::FindHammingDistance(const InstructionSequence& seq1, const InstructionSequence& seq2, int offset)
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


int Avida::InstructionSequence::FindBestOffset(const InstructionSequence& seq1, const InstructionSequence& seq2)
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


int Avida::InstructionSequence::FindSlidingDistance(const InstructionSequence& seq1, const InstructionSequence& seq2)
{
  const int offset = FindBestOffset(seq1, seq2);
  return FindHammingDistance(seq1, seq2, offset);
}


int Avida::InstructionSequence::FindEditDistance(const InstructionSequence& seq1, const InstructionSequence& seq2)
{
  const int size1 = seq1.GetSize();
  const int size2 = seq2.GetSize();
  const int min_size = (size1 < size2) ? size1 : size2;
  
  // If either size is zero, return the other one!
  if (!min_size) return (size1 > size2) ? size1 : size2;
  
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

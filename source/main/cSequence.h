/*
 *  cSequence.h
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

#ifndef cSequence_h
#define cSequence_h

#include <cassert>

#include "cInstruction.h"
#include "cMutationSteps.h"
#include "cString.h"
#include "tArray.h"

class cInstSet;


// This class stores the genome sequence of an Avida organism.
class cSequence
{
protected:
  tArray<cInstruction> m_seq;
  int m_active_size;
  cMutationSteps m_mutation_steps;
  
  
public:
  cSequence() { ; }
  cSequence(const cSequence& seq);
  explicit cSequence(int size) : m_seq(size), m_active_size(size) { ; }  // Builds a 'blank' sequence of the specified size
  cSequence(const cString& str);  // Builds sequence from a string
  virtual ~cSequence();
  
  inline int GetSize() const { return m_active_size; }
  cString AsString() const;
  
  inline cMutationSteps& GetMutationSteps() { return m_mutation_steps; }
  inline const cMutationSteps& GetMutationSteps() const { return m_mutation_steps; }
  
  inline cInstruction& operator[](int idx) { assert(idx >= 0 && idx < m_active_size);  return m_seq[idx]; }
  inline const cInstruction& operator[](int idx) const { assert(idx >= 0 && idx < m_active_size);  return m_seq[idx]; }
  
  virtual void Resize(int new_size);
  virtual void Copy(int to, int from);
  virtual void Insert(int pos, const cInstruction& inst);
  virtual void Insert(int pos, const cSequence& genome);
  virtual void Remove(int pos, int num_sites = 1);
  virtual void Replace(int pos, int num_sites, const cSequence& seq);
	virtual void Replace(const cSequence& g, int begin, int end); // Replace [begin, end) instructions in this sequence with g, respecting sequence circularity
	virtual void Rotate(int n); // Rotate this genome forward n instructions.
  
  inline void Append(const cInstruction& inst) { Insert(GetSize(), inst); }
  inline void Append(const cSequence& seq) { Insert(GetSize(), seq); }
  
  virtual void operator=(const cSequence& other_seq);
  virtual bool operator==(const cSequence& other_seq) const;
  virtual bool operator!=(const cSequence& other_seq) const { return !(this->operator==(other_seq)); }
  virtual bool operator<(const cSequence& other_seq) const { return AsString() < other_seq.AsString(); }
  
  int FindInst(const cInstruction& inst, int start_index = 0) const;
  int CountInst(const cInstruction& inst) const;
  int MinDistBetween(const cInstruction& inst) const;
  inline bool HasInst(const cInstruction& inst) const { return (FindInst(inst) >= 0); }
  
  void SaveInstructions(std::ostream& fp, const cInstSet& inst_set) const;  
  

  // Sequence Creation Methods
  cSequence Crop(int start, int end) const;
  cSequence Cut(int start, int end) const;
  
  static cSequence Join(const cSequence& lhs, const cSequence& rhs);

  
  // Genetic Distance Methods
  static int FindOverlap(const cSequence& seq1, const cSequence& seq2, int offset = 0);
  static int FindHammingDistance(const cSequence& seq1, const cSequence& seq2, int offset = 0);
  static int FindBestOffset(const cSequence& seq1, const cSequence& seq2);
  static int FindSlidingDistance(const cSequence& seq1, const cSequence& seq2);
  static int FindEditDistance(const cSequence& seq1, const cSequence& seq2);
  
protected:
  virtual void adjustCapacity(int new_size);
  virtual void prepareInsert(int pos, int num_sites);
};

inline cSequence operator+(const cSequence& lhs, const cSequence& rhs) { return cSequence::Join(lhs, rhs); }

#endif

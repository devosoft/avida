/*
 *  core/Sequence.h
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

#ifndef Sequence_h
#define Sequence_h

#include <cassert>

#include "cInstruction.h"
#include "cMutationSteps.h"
#include "cString.h"
#include "tArray.h"

class cInstSet;


namespace Avida {
  
  // Sequence - a series of bytes containing a base level genetic sequence
  // --------------------------------------------------------------------------------------------------------------

  class Sequence
  {
  protected:
    tArray<cInstruction> m_seq;
    int m_active_size;
    cMutationSteps m_mutation_steps;
    
    
  public:
    Sequence() { ; }
    Sequence(const Sequence& seq);
    explicit Sequence(int size) : m_seq(size), m_active_size(size) { ; }
    Sequence(const cString& str);
    virtual ~Sequence();
    

    // Accessor Methods
    // --------------------------------------------------------------------------------------------------------------
    
    inline int GetSize() const { return m_active_size; }
    cString AsString() const;
    
    inline cMutationSteps& GetMutationSteps() { return m_mutation_steps; }
    inline const cMutationSteps& GetMutationSteps() const { return m_mutation_steps; }
    
    inline cInstruction& operator[](int idx) { assert(idx >= 0 && idx < m_active_size);  return m_seq[idx]; }
    inline const cInstruction& operator[](int idx) const { assert(idx >= 0 && idx < m_active_size);  return m_seq[idx]; }
    

    // Subclassable Methods
    // --------------------------------------------------------------------------------------------------------------

    virtual void Resize(int new_size);
    virtual void Copy(int to, int from);
    virtual void Insert(int pos, const cInstruction& inst);
    virtual void Insert(int pos, const Sequence& genome);
    virtual void Remove(int pos, int num_sites = 1);
    virtual void Replace(int pos, int num_sites, const Sequence& seq);
    virtual void Replace(const Sequence& g, int begin, int end);
    virtual void Rotate(int n);
    
    virtual void operator=(const Sequence& other_seq);
    virtual bool operator==(const Sequence& other_seq) const;
    virtual bool operator!=(const Sequence& other_seq) const { return !(this->operator==(other_seq)); }
    virtual bool operator<(const Sequence& other_seq) const { return AsString() < other_seq.AsString(); }
    

    
    // Utility Methods
    // --------------------------------------------------------------------------------------------------------------
    
    inline void Append(const cInstruction& inst) { Insert(GetSize(), inst); }
    inline void Append(const Sequence& seq) { Insert(GetSize(), seq); }
    
    int FindInst(const cInstruction& inst, int start_index = 0) const;
    int CountInst(const cInstruction& inst) const;
    int MinDistBetween(const cInstruction& inst) const;
    inline bool HasInst(const cInstruction& inst) const { return (FindInst(inst) >= 0); }
    
    void SaveInstructions(std::ostream& fp, const cInstSet& inst_set) const;  
    

    // Sequence Creation Methods
    // --------------------------------------------------------------------------------------------------------------
    
    Sequence Crop(int start, int end) const;
    Sequence Cut(int start, int end) const;
    
    static Sequence Join(const Sequence& lhs, const Sequence& rhs);

    
    // Genetic Distance Methods
    // --------------------------------------------------------------------------------------------------------------
    
    static int FindOverlap(const Sequence& seq1, const Sequence& seq2, int offset = 0);
    static int FindHammingDistance(const Sequence& seq1, const Sequence& seq2, int offset = 0);
    static int FindBestOffset(const Sequence& seq1, const Sequence& seq2);
    static int FindSlidingDistance(const Sequence& seq1, const Sequence& seq2);
    static int FindEditDistance(const Sequence& seq1, const Sequence& seq2);
    
    
  protected:
    virtual void adjustCapacity(int new_size);
    virtual void prepareInsert(int pos, int num_sites);
  };


  // Sequence Helper Methods
  // --------------------------------------------------------------------------------------------------------------
  
  inline Sequence operator+(const Sequence& lhs, const Sequence& rhs) { return Sequence::Join(lhs, rhs); }
};

#endif

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

#ifndef InstructionSequence_h
#define InstructionSequence_h

#include <cassert>

#include "apto/platform.h"
#include "avida/core/GeneticRepresentation.h"


namespace Avida {
  
  // InstructionSequence - a series of bytes containing a base level genetic sequence
  // --------------------------------------------------------------------------------------------------------------
  
  class Instruction
  {
  private:
    unsigned char m_operand;
    
  public:
    LIB_EXPORT inline Instruction() : m_operand(0) { ; }
    LIB_EXPORT inline Instruction(const Instruction& inst) { m_operand = inst.m_operand; }
    LIB_EXPORT inline explicit Instruction(int in_op) { SetOp(in_op); }
    LIB_EXPORT inline explicit Instruction(const Apto::String& symbol) { SetSymbol(symbol); }
    
    LIB_EXPORT inline int GetOp() const { return static_cast<int>(m_operand); }
    LIB_EXPORT inline void SetOp(int in_op) { assert(in_op < 256); m_operand = in_op; }
    
    LIB_EXPORT inline void operator=(const Instruction& inst) { m_operand = inst.m_operand; }
    LIB_EXPORT bool operator==(const Instruction& inst) const { return (m_operand == inst.m_operand); }
    LIB_EXPORT inline bool operator!=(const Instruction& inst) const { return (m_operand != inst.m_operand); }
    
    LIB_EXPORT Apto::String GetSymbol() const;
    LIB_EXPORT bool SetSymbol(const Apto::String& symbol);    
    
    LIB_EXPORT inline Apto::String AsString() const { return GetSymbol(); }
  };

  
  
  // InstructionSequence - a series of bytes containing a base level genetic sequence
  // --------------------------------------------------------------------------------------------------------------

  class InstructionSequence : public GeneticRepresentation
  {
  protected:
    Apto::Array<Instruction> m_seq;
    int m_active_size;
    
  public:
    LIB_EXPORT inline InstructionSequence() : m_active_size(0) { ; }
    LIB_EXPORT InstructionSequence(const InstructionSequence& seq);
    LIB_EXPORT inline explicit InstructionSequence(int size) : m_seq(size), m_active_size(size) { ; }
    LIB_EXPORT explicit InstructionSequence(const Apto::String& str);
    LIB_EXPORT virtual ~InstructionSequence();
    

    // Accessors
    LIB_EXPORT inline int GetSize() const { return m_active_size; }
    
    LIB_EXPORT inline Instruction& operator[](int idx) { assert(idx >= 0 && idx < m_active_size);  return m_seq[idx]; }
    LIB_EXPORT inline const Instruction& operator[](int idx) const { assert(idx >= 0 && idx < m_active_size);  return m_seq[idx]; }


    // GeneticRepresentation Interface
    LIB_EXPORT Apto::String AsString() const;

    LIB_EXPORT GeneticRepresentationPtr Clone() const;
    
    LIB_EXPORT bool Serialize(ArchivePtr ar) const;


    // Manipulation
    LIB_EXPORT virtual void Resize(int new_size);
    LIB_EXPORT virtual void Copy(int to, int from);
    LIB_EXPORT virtual void Insert(int pos, const Instruction& inst);
    LIB_EXPORT virtual void Insert(int pos, const InstructionSequence& genome);
    LIB_EXPORT virtual void Remove(int pos, int num_sites = 1);
    LIB_EXPORT virtual void Replace(int pos, int num_sites, const InstructionSequence& seq);
    LIB_EXPORT virtual void Replace(const InstructionSequence& g, int begin, int end);
    LIB_EXPORT virtual void Rotate(int n);
    

    // GeneticRepresentation Operators
    LIB_EXPORT bool operator==(const GeneticRepresentation& other_seq) const;

    // Operators
    LIB_EXPORT virtual void operator=(const InstructionSequence& other_seq);
    LIB_EXPORT virtual bool operator<(const InstructionSequence& other_seq) const { return AsString() < other_seq.AsString(); }

    
    // Utility Methods
    LIB_EXPORT inline void Append(const Instruction& inst) { Insert(GetSize(), inst); }
    LIB_EXPORT inline void Append(const InstructionSequence& seq) { Insert(GetSize(), seq); }
    
    LIB_EXPORT int FindInst(const Instruction& inst, int start_index = 0) const;
    LIB_EXPORT int CountInst(const Instruction& inst) const;
    LIB_EXPORT int MinDistBetween(const Instruction& inst) const;
    LIB_EXPORT inline bool HasInst(const Instruction& inst) const { return (FindInst(inst) >= 0); }
        

    // InstructionSequence Creation Methods
    LIB_EXPORT InstructionSequence Crop(int start, int end) const;
    LIB_EXPORT InstructionSequence Cut(int start, int end) const;
    
    LIB_EXPORT static InstructionSequence Join(const InstructionSequence& lhs, const InstructionSequence& rhs);

    
    // Genetic Distance Methods
    static int FindOverlap(const InstructionSequence& seq1, const InstructionSequence& seq2, int offset = 0);
    static int FindHammingDistance(const InstructionSequence& seq1, const InstructionSequence& seq2, int offset = 0);
    static int FindBestOffset(const InstructionSequence& seq1, const InstructionSequence& seq2);
    static int FindSlidingDistance(const InstructionSequence& seq1, const InstructionSequence& seq2);
    static int FindEditDistance(const InstructionSequence& seq1, const InstructionSequence& seq2);
    
    
  protected:
    LIB_EXPORT virtual void adjustCapacity(int new_size);
    LIB_EXPORT virtual void prepareInsert(int pos, int num_sites);
  };


  // InstructionSequence Helper Methods
  // --------------------------------------------------------------------------------------------------------------
  
  LIB_EXPORT inline InstructionSequence operator+(const InstructionSequence& lhs, const InstructionSequence& rhs)
  {
    return InstructionSequence::Join(lhs, rhs);
  }
  
};

#endif

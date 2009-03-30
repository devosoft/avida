/*
 *  cGenome.h
 *  Avida
 *
 *  Called "genome.hh" prior to 12/2/05.
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

#ifndef cGenome_h
#define cGenome_h

#include <cassert>

#ifndef cInstruction_h
#include "cInstruction.h"
#endif
#ifndef cMutationSteps_h
#include "cMutationSteps.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

/**
 * This class stores the genome of an Avida organism.  Note that once created,
 * a genome should not be modified; only the corresponding memory should be,
 * before creating the genome.  Keeping genome light-weight...
 **/
class cGenome
{
protected:
  tArray<cInstruction> m_genome;
  int m_active_size;
  cMutationSteps m_mutation_steps;
  
  
  virtual void adjustCapacity(int new_size);
  virtual void prepareInsert(int pos, int num_sites);
  

public:
  cGenome() { ; }                                   //! Default constructor
  explicit cGenome(int _size);                      //! Constructor that builds a 'blank' cGenome of the specified size
  cGenome(const cGenome& in_genome);                //! Copy constructor
  cGenome(const cString& in_string);                //! Constructor that builds genome from a string
  cGenome(cInstruction* begin, cInstruction* end);  //! Constructor that builds genome from a range of instructions  
  virtual ~cGenome();                               //! Virtual destructor; there are subclasses.

  inline int GetSize() const { return m_active_size; }
  cString AsString() const;
  
  inline cMutationSteps& GetMutationSteps() { return m_mutation_steps; }
  inline const cMutationSteps& GetMutationSteps() const { return m_mutation_steps; }
  
  inline cInstruction& operator[](int idx) { assert(idx >= 0 && idx < m_active_size);  return m_genome[idx]; }
  inline const cInstruction& operator[](int idx) const { assert(idx >= 0 && idx < m_active_size);  return m_genome[idx]; }

  virtual void Resize(int new_size);
  virtual void Copy(int to, int from);
  virtual void Insert(int pos, const cInstruction& inst);
  virtual void Insert(int pos, const cGenome& genome);
  virtual void Remove(int pos, int num_sites = 1);
  virtual void Replace(int pos, int num_sites, const cGenome& genome);

  inline void Append(const cInstruction& in_inst) { Insert(GetSize(), in_inst); }
  inline void Append(const cGenome& in_genome) { Insert(GetSize(), in_genome); }

  virtual void operator=(const cGenome& other_genome);
  virtual bool operator==(const cGenome& other_genome) const;
  virtual bool operator!=(const cGenome& other_genome) const { return !(this->operator==(other_genome)); }
  virtual bool operator<(const cGenome& other_genome) const { return AsString() < other_genome.AsString(); }
  
};

#endif

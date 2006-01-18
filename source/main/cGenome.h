/*
 *  cGenome.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cGenome_h
#define cGenome_h

#include <assert.h>

#ifndef cInstruction_h
#include "cInstruction.h"
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
  tArray<cInstruction> genome;
  int active_size;

public:
  cGenome() { ; }
  explicit cGenome(int _size);
  cGenome(const cGenome& in_genome);
  cGenome(const cString& in_string);
  virtual ~cGenome();

  virtual void operator=(const cGenome & other_genome);
  virtual bool operator==(const cGenome & other_genome) const;
  virtual bool operator!=(const cGenome & other_genome) const { return !(this->operator==(other_genome)); }
  virtual bool operator<(const cGenome & other_genome) const { return AsString() < other_genome.AsString(); }

  cInstruction & operator[](int index) { assert(index >= 0 && index < active_size);  return genome[index]; }
  const cInstruction & operator[](int index) const { assert(index >= 0 && index < active_size);  return genome[index]; }

  virtual void Copy(int to, int from);

  bool OK() const;
   
  int GetSize() const { return active_size; }
  cString AsString() const;
};

#endif

//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "genome.hh"


using namespace std;


/////////////
//  cGenome
/////////////

cGenome::cGenome(int _size)
  : genome(_size), active_size(_size)
{
}

cGenome::cGenome(const cGenome & in_genome)
  : genome(in_genome.GetSize()), active_size(in_genome.GetSize())
{
  for (int i = 0; i < active_size; i++)  genome[i] = in_genome[i];
}

cGenome::cGenome(const cString & in_string)
{
  cString tmp_string(in_string);
  tmp_string.RemoveChar('_');  // Remove all blanks from alignments...

  active_size = tmp_string.GetSize();
  genome.ResizeClear(active_size);
  for (int i = 0; i < active_size; i++) {
    genome[i].SetSymbol(tmp_string[i]);
  }
}

cGenome::~cGenome()
{
}


void cGenome::operator=(const cGenome & other_genome)
{
  // If we need to resize, do so...
  active_size = other_genome.GetSize();
  genome.ResizeClear(active_size);

  // Now that both code arrays are the same size, copy the other one over.

  for (int i = 0; i < active_size; i++) {
    genome[i] = other_genome[i];
  }
}


bool cGenome::operator==(const cGenome & other_genome) const
{
  // Make sure the sizes are the same.
  if (active_size != other_genome.active_size) return false;

  // Then go through line by line.
  for (int i = 0; i < active_size; i++)
    if (genome[i] != other_genome[i]) return false;

  return true;
}


void cGenome::Copy(int to, int from)
{
  assert(to   >= 0   && to   < active_size);
  assert(from >= 0   && from < active_size);
  genome[to] = genome[from];
}

bool cGenome::OK() const
{
  assert (active_size <= genome.GetSize()); // active_size too large!

  return true;
}
 

// Return the genome as an alphabetic string

cString cGenome::AsString() const
{
  cString out_string(active_size);
  for (int i = 0; i < active_size; i++) {
    out_string[i] = genome[i].GetSymbol();
  }

  return out_string;
}

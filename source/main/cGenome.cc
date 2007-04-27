/*
 *  cGenome.cc
 *  Avida
 *
 *  Called "genome.cc" prior to 12/2/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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


using namespace std;


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


/*! This constructor is used to build a new cGenome from a range of instructions.
It expects STL semantics for an iterator range.  We're avoiding templating this
(for now).  Refactor if a new range type is needed.

\todo Just as an aside, it looks like Push continually reallocs memory in tArray.
*/
cGenome::cGenome(cInstruction* begin, cInstruction* end)
: active_size(0)
{
  for(cInstruction* i=begin; i!=end; ++i,++active_size) {
    genome.Push(*i);
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

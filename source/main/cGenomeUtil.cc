/*
 *  cGenomeUtil.cc
 *  Avida
 *
 *  Called "genome_util.cc" prior to 12/5/05.
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

#include "cGenomeUtil.h"

#include "cAvidaContext.h"
#include "cGenome.h"
#include "cInitFile.h"
#include "cInstSet.h"
#include "functions.h"


using namespace std;


int cGenomeUtil::FindInst(const cGenome & gen, const cInstruction & inst,
			  int start_index)
{
  assert(start_index < gen.GetSize());  // Starting search after genome end.

  for(int i = start_index; i < gen.GetSize(); i++) {
    if (gen[i] == inst) return i;
  }

  // Search failed
  return -1;
}

int cGenomeUtil::CountInst(const cGenome & gen, const cInstruction & inst)
{
  int count = 0;
  for(int i = 0; i < gen.GetSize(); i++) {
    if (gen[i] == inst) count++;
  }
	
  return count;
}

int cGenomeUtil::FindOverlap(const cGenome & gen1, const cGenome & gen2,
			     int offset)
{
  assert(offset < gen1.GetSize());
  assert(-offset < gen2.GetSize());

  if (offset > 0) return Min(gen1.GetSize() - offset, gen2.GetSize());
  // else
  return Min(gen2.GetSize() + offset, gen1.GetSize());
}


int cGenomeUtil::FindHammingDistance(const cGenome &gen1, const cGenome &gen2,
				     int offset)
{
  const int start1 = (offset < 0) ? 0 : offset;
  const int start2 = (offset > 0) ? 0 : -offset;
  const int overlap = FindOverlap(gen1, gen2, offset);

  // Initialize the hamming distance to anything protruding past the overlap.

  int hamming_distance = gen1.GetSize() + gen2.GetSize() - 2 * overlap;

  // Cycle through the overlap adding all differences to the distance.
  for (int i = 0; i < overlap; i++) {
    if (gen1[start1 + i] != gen2[start2 + i])  hamming_distance++;
  }

  return hamming_distance;
}


int cGenomeUtil::FindBestOffset(const cGenome & gen1, const cGenome & gen2)
{
  const int size1 = gen1.GetSize();
  const int size2 = gen2.GetSize();

  int best_offset = 0;
  int cur_distance = FindHammingDistance(gen1, gen2);
  int best_distance = cur_distance;

  // Check positive offsets...
  for (int i = 1; i < size1 || i < size2; i++) {
    if (size1 + size2 - 2 * FindOverlap(gen1, gen2, i) > best_distance) break;
    cur_distance = FindHammingDistance(gen1, gen2, i);
    if (cur_distance < best_distance) {
      best_distance = cur_distance;
      best_offset = i;
    }
  }

  // Check negative offsets...
  for (int i = 1; i < size1 || i < size2; i++) {
    if (size1 + size2 - 2 * FindOverlap(gen1, gen2, -i) > best_distance) break;
    cur_distance = FindHammingDistance(gen1, gen2, -i);
    if (cur_distance < best_distance) {
      best_distance = cur_distance;
      best_offset = -i;
    }
  }

  return best_offset;
}


int cGenomeUtil::FindSlidingDistance(const cGenome &gen1, const cGenome &gen2)
{
  const int offset = FindBestOffset(gen1, gen2);
  return FindHammingDistance(gen1, gen2, offset);
}


int cGenomeUtil::FindEditDistance(const cGenome & gen1, const cGenome & gen2)
{
  const int size1 = gen1.GetSize();
  const int size2 = gen2.GetSize();

  if (!size1) return size2;
  if (!size2) return size1;

  int * cur_row  = new int[size1];  // The row we are calculating
  int * prev_row = new int[size1];  // The last row we calculater

  // Initialize the previous row to record the differece from nothing.
  for (int i = 0; i < size1; i++)  prev_row[i] = i + 1;

  // Loop through each subsequent character in the test code
  for (int i = 0; i < size2; i++) {
    // Initialize the first entry in cur_row.
    if (gen1[0] == gen2[i]) cur_row[0] = i;
    else cur_row[0] = (i < prev_row[0]) ? (i+1) : (prev_row[0] + 1);

    // Move down the cur_row and fill it out.
    for (int j = 1; j < size1; j++) {
      // If the values are equal, keep the value in the upper left.
      if (gen1[j] == gen2[i]) {
	cur_row[j] = prev_row[j-1];
      }

      // Otherwise, set the current position the the minimal of the three
      // numbers above (insertion), to the left (deletion), or upper left
      // (mutation) in the chart, plus one.
      else {
	cur_row[j] =
	  (prev_row[j] < prev_row[j-1]) ? prev_row[j] : prev_row[j-1];
	if (cur_row[j-1] < cur_row[j]) cur_row[j] = cur_row[j-1];
	cur_row[j]++;
      }
    }

    // Swap cur_row and prev_row. (we only really need to move the cur row
    // over to prev, but this saves us from having to keep re-allocating
    // new rows.  We recycle!
    int * temp_row = cur_row;
    cur_row = prev_row;
    prev_row = temp_row;
  }

  // Now that we are done, return the bottom-right corner of the chart.

  const int value = prev_row[size1 - 1];

  delete [] cur_row;
  delete [] prev_row;

  return value;
}


cGenome cGenomeUtil::Crop(const cGenome & in_genome, int start, int end)
{
  assert(end > start);                // Must have a positive length clip!
  assert(in_genome.GetSize() >= end); // end must be < genome length
  assert(start >= 0);                 // negative start illegal

  const int out_length = end - start;
  cGenome out_genome(out_length);
  for (int i = 0; i < out_length; i++) {
    out_genome[i] = in_genome[i+start];
  }

  return out_genome;
}


cGenome cGenomeUtil::Cut(const cGenome & in_genome, int start, int end)
{
  assert(end > start);                // Must have a positive size cut!
  assert(in_genome.GetSize() >= end); // end must be < genome length
  assert(start >= 0);                 // negative start illegal

  const int cut_length = end - start;
  const int out_length = in_genome.GetSize() - cut_length;

  assert(out_length > 0);             // Can't cut everything!

  cGenome out_genome(out_length);
  for (int i = 0; i < start; i++) {
    out_genome[i] = in_genome[i];
  }
  for (int i = start; i < out_length; i++) {
    out_genome[i] = in_genome[i+cut_length];
  }

  return out_genome;
}


cGenome cGenomeUtil::Join(const cGenome & genome1, const cGenome & genome2)
{
  const int length1 = genome1.GetSize();
  const int length2 = genome2.GetSize();
  const int out_length = length1 + length2;

  cGenome out_genome(out_length);
  for (int i = 0; i < length1; i++) {
    out_genome[i] = genome1[i];
  }
  for (int i = 0; i < length2; i++) {
    out_genome[i+length1] = genome2[i];
  }

  return out_genome;
}

cGenome cGenomeUtil::LoadGenome(const cString& filename, const cInstSet& inst_set)
{
  cGenome new_genome(0);
  if (!LoadGenome(filename, inst_set, new_genome)) {
    cerr << "Error: Unable to load genome" << endl;
    exit(1);
  }
  return new_genome;
}

bool cGenomeUtil::LoadGenome(const cString& filename, const cInstSet& inst_set, cGenome& out_genome)
{
  cInitFile input_file(filename);
  bool success = true;
  
  if (!input_file.WasOpened()) return false;
  
  // Setup the code array...
  cGenome new_genome(input_file.GetNumLines());
  
  for (int line_num = 0; line_num < new_genome.GetSize(); line_num++) {
    cString cur_line = input_file.GetLine(line_num);
    new_genome[line_num] = inst_set.GetInst(cur_line);
    
    if (new_genome[line_num] == inst_set.GetInstError()) {
      // You're using the wrong instruction set!  YOU FOOL!
      if (success) {
        cerr << "Error: Cannot load organism '" << filename << "'" << endl;
        success = false;
      }
      cerr << "       Unknown line: " << cur_line << " (best match is '" << inst_set.FindBestMatch(cur_line) << "')" << endl;
    }
  }
  
  if (new_genome.GetSize() == 0) cerr << "Warning: Genome size is 0!" << endl;
  if (success) out_genome = new_genome;
  return success;
}

cGenome cGenomeUtil::LoadInternalGenome(istream& fp, const cInstSet& inst_set)
{
  assert(fp.good()); // Invalid stream to load genome from!
  
  int num_lines = -1;
  fp >> num_lines;
  
  if (num_lines <= 0) { return cGenome(1); }
  
  // Setup the code array...
  cGenome new_genome(num_lines);
  cString cur_line;
  
  for (int line_num = 0; line_num < new_genome.GetSize(); line_num++) {
    fp >> cur_line;
    new_genome[line_num] = inst_set.GetInst(cur_line);
    
    if (new_genome[line_num] == inst_set.GetInstError()) {
      // You're using the wrong instruction set!  YOU FOOL!
      cerr << "Cannot load creature from stream:" << endl
      << "       Unknown line: " << cur_line << endl;
    }
  }
  return new_genome;
}

void cGenomeUtil::SaveGenome(ostream& fp, const cInstSet& inst_set,
                           const cGenome & gen)
{
  for (int i = 0; i < gen.GetSize(); i++) {
    fp << inst_set.GetName(gen[i]) << endl;
  }
}

void cGenomeUtil::SaveInternalGenome(ostream& fp, const cInstSet& inst_set,
                                   const cGenome & gen)
{
  fp << gen.GetSize() << endl;
  SaveGenome(fp, inst_set, gen);
}


cGenome cGenomeUtil::RandomGenome(cAvidaContext& ctx, int length, const cInstSet& inst_set)
{
  cGenome genome(length);
  for (int i = 0; i < length; i++) {
    genome[i] = inst_set.GetRandomInst(ctx);
  }
  return genome;
}

cGenome cGenomeUtil::RandomGenomeWithoutZeroRedundantsPlusRepro(cAvidaContext& ctx, int length, const cInstSet& inst_set)
{
  cGenome genome(length+1);
  for (int i = 0; i < length; i++) {
	  cInstruction inst = inst_set.GetRandomInst(ctx);
	  while (inst_set.GetRedundancy(inst)==0)
		  inst = inst_set.GetRandomInst(ctx);
    genome[i] = inst;
  }
  genome[length] = inst_set.GetInst("repro");
  return genome;
}

cGenome cGenomeUtil::RandomGenomeWithoutZeroRedundantsPlusReproSex(cAvidaContext& ctx, int length, const cInstSet& inst_set)
{
  cGenome genome(length+1);
  for (int i = 0; i < length; i++) {
	  cInstruction inst = inst_set.GetRandomInst(ctx);
	  while (inst_set.GetRedundancy(inst)==0)
		  inst = inst_set.GetRandomInst(ctx);
    genome[i] = inst;
  }
  genome[length] = inst_set.GetInst("repro-sex");
  return genome;
}


//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2000 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef BLOCK_STRUCT_HH
#include "block_struct.hh"
#endif

#include <iostream>

using namespace std;

////////////////////
//  cBlockStruct
////////////////////

cBlockStruct::cBlockStruct(int in_num_scaled, int in_num_fixed)
{
  num_fixed = in_num_fixed;
  num_scaled = in_num_scaled;

  fixed_array = new cFixedBlock[num_fixed];
  scaled_array = new cScaledBlock[num_scaled];

  fixed_used(0, 0);
  scaled_used = 0;

  fixed_size = 0;
}

cBlockStruct::~cBlockStruct()
{
  delete [] scaled_array;
  delete [] fixed_array;
}

int cBlockStruct::AddData(void * in_data, int in_size)
{
  if (in_size <= 0) return -1;    // Only allow positive sized data.

  scaled_array[scaled_used].SetData(in_data);
  scaled_array[scaled_used].SetSize(in_size);
  
  return scaled_used++;
}

int cBlockStruct::AddDataTo(void * in_data, int in_index)
{
  scaled_array[in_index].SetData(in_data);

  return scaled_array[in_index].GetSize();
}

void cBlockStruct::Clear()
{
  fixed_used(0, 0);
  scaled_used = 0;
}

int cBlockStruct::Process()
{
  if (!fixed_size) {
    cerr << "Error: No fixed size!" << endl;
    return -1;
  }

  int cur_scaled = 0;
  int cur_fixed = 0;
  int size_left = scaled_array[0].GetSize();

  // Set up the internal data for the first scaled block.

  scaled_array[0].SetStart(0, 0);

  // Move through the fixed blocks assigning each to the corresponding scaled
  // block.  This is done by keeping track of the current scaled block for
  // a particular location, and keeping track the "size" until the next block.
  
  for(cur_fixed = 0; cur_fixed < num_fixed; cur_fixed++) {
    // Set the current fixed block to point to whatever scaled block we are
    // currently in.

    fixed_array[cur_fixed].SetStart(cur_scaled);

    // If the current scaled block covers the entire fixed block, then just
    // adjust the size_left and move on.

    if (size_left > fixed_size) {
      size_left -= fixed_size;
      continue;
    }

    // Otherwise, start moving through the scaled blocks until the size can
    // be covered.

    while (size_left <= fixed_size) {
      // move onto the next scaled block...
      cur_scaled++;

      //  Check to see if we are finished...
      if (cur_scaled == scaled_used) {
	fixed_used(cur_fixed, size_left);  // Store max coord
	return cur_fixed + 1;
      }

      // Setup the next cScaledBlock...

      scaled_array[cur_scaled].SetStart(cur_fixed, size_left);
      if (size_left == fixed_size) {
	scaled_array[cur_scaled].SetStart(cur_fixed + 1, 0);
      }

      // Add the size of the new cScaledBlock onto the size_left.

      size_left += scaled_array[cur_scaled].GetSize();
    }

    // Move onto the next fixed block.
    size_left -= fixed_size;
  }

  cout << "END Process() : Ran out of fixed_blocks..." << endl;
  
  return -1;
}

int cBlockStruct::Update()
{
  // @CAO make sure a process has been done at some point in the past?

  // Start out where the last Process() finished.

  int cur_scaled = fixed_array[fixed_used.GetBlockNum()].GetStart();
  int cur_fixed = scaled_array[cur_scaled].GetStart().GetBlockNum();
  int cur_offset = scaled_array[cur_scaled].GetStart().GetOffset();
  int size_left = scaled_array[cur_scaled].GetSize();

  // Re-adjust to make sure we are starting at the begining of a fixed block.

  if (cur_offset) {
    size_left += cur_offset - fixed_size;
    cur_fixed++;
  }

  // Move through the fixed blocks assigning each to the corresponding scaled
  // block.  This is done by keeping track of the current scaled block for
  // a particular location, and keeping track the "size" until the next block.
  
  for(; cur_fixed < num_fixed; cur_fixed++) {
    // Set the current fixed block to point to whatever scaled block we are
    // currently in.

    fixed_array[cur_fixed].SetStart(cur_scaled);

    // If the current scaled block covers the entire fixed block, then just
    // adjust the size_left and move on.

    if (size_left > fixed_size) {
      size_left -= fixed_size;
      continue;
    }

    // Otherwise, start moving through the scaled blocks until the size can
    // be covered.

    while (size_left <= fixed_size) {
      // move onto the next scaled block...
      cur_scaled++;

      //  Check to see if we are finished...
      if (cur_scaled == scaled_used) {
	fixed_used(cur_fixed, size_left);  // Store max coord
	return cur_fixed + 1;
      }

      // Setup the next cScaledBlock...

      scaled_array[cur_scaled].SetStart(cur_fixed, size_left);
      if (size_left == fixed_size) {
	scaled_array[cur_scaled].SetStart(cur_fixed + 1, 0);
      }

      // Add the size of the new cScaledBlock onto the size_left.

      size_left += scaled_array[cur_scaled].GetSize();
    }

    // Move onto the next fixed block.
    size_left -= fixed_size;
  }

  cout << "END Update() : Ran out of fixed_blocks..." << endl;
  
  return -1;
}


int cBlockStruct::Compress()
{
  int scan_loc;
  int write_loc = 0;

  for (scan_loc = 0; scan_loc < num_scaled; scan_loc++) {
    if (scaled_array[scan_loc].GetData()) {
      if (scan_loc != write_loc) {
	scaled_array[write_loc] = scaled_array[scan_loc];
	scaled_array[scan_loc].SetData(NULL);
	scaled_array[scan_loc].SetSize(0);
      }
      write_loc++;
    }
  }

  num_scaled = write_loc;

  return num_scaled;
}

void * cBlockStruct::Remove(int out_block)
{
  void * temp = scaled_array[out_block].GetData();
  scaled_array[out_block].SetData(NULL);
  return temp;
}

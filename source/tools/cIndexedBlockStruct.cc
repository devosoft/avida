/*
 *  cIndexedBlockStruct.cc
 *  Avida
 *
 *  Called "indexed_block_struct.cc" prior to 12/7/05.
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

#include "cIndexedBlockStruct.h"

#include <iostream>

using namespace std;


cIndexedBlockStruct::cIndexedBlockStruct(int in_num_scaled, int in_num_fixed,
					 int in_num_ref)
{
  num_fixed = in_num_fixed;
  num_scaled = in_num_scaled;
  num_ref = in_num_ref;
  num_data = 0;

  fixed_array = new cFixedBlock[num_fixed];
  scaled_array = new cRefBlock[num_scaled];
  ref_array = new int[num_ref];

  int i;
  for (i = 0; i < num_ref; i++) {
    ref_array[i] = -1;
  }

  fixed_used(0, 0);
  scaled_used = 0;

  total_size = 0;
  fixed_size = 0;
}

cIndexedBlockStruct::~cIndexedBlockStruct()
{
  delete [] scaled_array;
  delete [] fixed_array;
  delete [] ref_array;
}

int cIndexedBlockStruct::AddData(int in_ref, int in_size)
{
  // Only allow positive sized data, and only if there is room.
  if (in_size <= 0 || scaled_used >= num_scaled) return -1;    

  // If this reference is already represented, combine them.
  if (ref_array[in_ref] >= 0) {
    in_size += scaled_array[ref_array[in_ref]].GetSize();
    RemoveRef(in_ref);
  }

  scaled_array[scaled_used].SetRef(in_ref);
  scaled_array[scaled_used].SetSize(in_size);
  ref_array[in_ref] = scaled_used;
  
  total_size += in_size;
  num_data++;

  return scaled_used++;
}

int cIndexedBlockStruct::AddDataTo(int in_ref, int in_index)
{
  if (ref_array[in_ref] < 0) num_data++;

  scaled_array[in_index].SetRef(in_ref);
  ref_array[in_ref] = in_index;

  return scaled_array[in_index].GetSize();
}

void cIndexedBlockStruct::Clear()
{
  for (int i = 0; i < num_ref; i++) {
    ref_array[i] = -1;
  }

  fixed_used(0,0);
  scaled_used = 0;

  total_size = 0;
  num_data = 0;
}

int cIndexedBlockStruct::Process()
{
  if (!fixed_size) {
    CalcFixedSize();
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

      // Setup the next cRefBlock...

      scaled_array[cur_scaled].SetStart(cur_fixed, size_left);
      if (size_left == fixed_size) {
	scaled_array[cur_scaled].SetStart(cur_fixed + 1, 0);
      }

      // Add the size of the new cRefBlock onto the size_left.

      size_left += scaled_array[cur_scaled].GetSize();
    }

    // Move onto the next fixed block.
    size_left -= fixed_size;
  }

  return -1;
}

int cIndexedBlockStruct::Update()
{
  // @CAO make sure a process has been done at some point in the past?

  // If the data to NULL ratio is too low, have the update fail.
  // @CAO make this configurable!

  if (num_data * 2 + 2 < scaled_used) {
    return -1;
  }

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

  return -1;
}

int cIndexedBlockStruct::Compress()
{
  int scan_loc;
  int write_loc = 0;

  total_size = 0;
  for (scan_loc = 0; scan_loc < scaled_used; scan_loc++) {
    if (scaled_array[scan_loc].GetRef() >= 0) {
      if (scan_loc != write_loc) {
	scaled_array[write_loc] = scaled_array[scan_loc];
	scaled_array[scan_loc].SetRef(-1);
	scaled_array[scan_loc].SetSize(0);
	ref_array[scaled_array[write_loc].GetRef()] = write_loc;
      }
      total_size += scaled_array[write_loc].GetSize();
      write_loc++;
    }
  }

  scaled_used = write_loc;

  if (scaled_used != num_data)
    cout << "Warning: after Compress() scaled_used = " << scaled_used << ", while num_data = " << num_data << endl;

  return scaled_used;
}

void cIndexedBlockStruct::RemoveRef(int in_ref)
{
  if (ref_array[in_ref] >= 0) num_data--;
  scaled_array[ref_array[in_ref]].SetRef(-1);
  ref_array[in_ref] = -1;
}

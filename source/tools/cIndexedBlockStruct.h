//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

// See block_struct_proto.hh for a good description of this structure.

// This file is a variation of the cBlockStruct data structure.  It uses
// reference numbers (which it takes in and passed out) rather than void
// pointers.  Additionally, it has a third array in which index i contains
// the location in the scaled array for reference number i, so that 
// reference numbers are not lost with compression.  The structure must be
// initialized with the maximum reference number which will be passed into
// it (so as to create the third array of that size.

#ifndef INDEXED_BLOCK_STRUCT_HH
#define INDEXED_BLOCK_STRUCT_HH

#ifndef FIXED_BLOCK_HH
#include "cFixedBlock.h"
#endif
#ifndef FIXED_COORDS_HH
#include "cFixedCoords.h"
#endif
#ifndef REF_BLOCK_HH
#include "cRefBlock.h"
#endif
#ifndef UINT_HH
#include "cUInt.h"
#endif

class cFixedBlock; // accessed
class cRefBlock; // accessed
class cFixedCoords; // aggregate
class cUInt; // aggregate

class cIndexedBlockStruct {
private:
  cFixedBlock * fixed_array;
  cRefBlock * scaled_array;
  int * ref_array;

  int num_fixed;
  int num_scaled;
  int num_ref;
  int num_data;

  cFixedCoords fixed_used;
  int scaled_used;

  cUInt total_size;
  int fixed_size;
public:
  cIndexedBlockStruct(int in_num_scaled = 1, int in_num_fixed = 1,
		      int in_num_ref = 1);
  ~cIndexedBlockStruct();

  // The following functions add a single piece of data to the scaled array,
  // but do not update the fixed array yet.  In the first, the data and its
  // size are entered, and an index into the scaled block array returned, or
  // else -1 if the array is full.  In the second, the data and the index
  // into the array are entered, and the size at that location returned.
  int AddData(int in_ref, int in_size);
  int AddDataTo(int in_ref, int in_index);

  // This function removes all of the data from the structure.
  void Clear();

  // This function takes the scaled array, and from it fully constructs the
  // proper fixed array.  Any information  which was in the fixed array is
  // completely erased and re-constructed.  The number of cFixedBlock's used
  // is returned, or -1 if it overflows.
  int Process();

  // The next function looks at data which has been taken in since the last
  // time the fixed array was built or updated, and the new info at the end.
  // The total number of cFixedBlock's used is returned, or -1 if it overflows.
  int Update();

  // This function takes the scaled array, and removes any elements from it
  // which no longer point to data, and compresses everything to the begining.
  // This should always be followed shortly by adding any new data, and then
  // a Process().  The new number of cRefBlock's used is returned.
  int Compress();

  // Two functions to retrieve data from the structure.
  inline int Find(cFixedCoords & search_coords) const;
  inline int Find(int in_block, int in_offset) const;
  inline int Find(cUInt in_position) const;

  // A function to remove data from the structure...
  void RemoveRef(int in_ref);

  // Finally, internal variable control...
  inline int GetFixedSize() const { return fixed_size; }
  inline int GetBlocksUsed() const { return fixed_used.GetBlockNum() + 1; }
  inline void SetFixedSize(int in_fs) {
    fixed_size = in_fs;
  }
  inline void CalcFixedSize() {
    if (num_data && total_size.AsInt()) {
      fixed_size = (total_size / num_data).AsInt();
    }
    else fixed_size = 1;
  }
};

inline int cIndexedBlockStruct::Find(cFixedCoords & search_coords) const
{
  // Make sure the find is within range.
  if (search_coords >= fixed_used) {
    return -1;
  }

  // Find the starting point in the scaled array for the search.
  int scaled_loc = fixed_array[search_coords.GetBlockNum()].GetStart();
  cFixedCoords cur_coords = scaled_array[scaled_loc].GetStart();

  // Starting at the starting point, move through the array until the entry
  // which covers this area is found.
  cur_coords.Add(0, scaled_array[scaled_loc].GetSize(), fixed_size);

  while (cur_coords <= search_coords) {
    scaled_loc++;
    cur_coords.Add(0, scaled_array[scaled_loc].GetSize(), fixed_size);
  }

  // Return the data in the found entry.
  return scaled_array[scaled_loc].GetRef();
}

inline int cIndexedBlockStruct::Find(int in_block, int in_offset) const
{
  cFixedCoords search_coords(in_block, in_offset);
  return Find(search_coords);
}

inline int cIndexedBlockStruct::Find(cUInt in_position) const
{
  cFixedCoords search_coords((in_position /= fixed_size).AsInt(),
			     (in_position %= fixed_size).AsInt());
  return Find(search_coords);
}

#endif

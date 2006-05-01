/*
 *  cBlockStruct.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cBlockStruct_H
#define cBlockStruct_h

#ifndef cFixedBlock_h
#include "cFixedBlock.h"
#endif
#ifndef cFixedCoords_h
#include "cFixedCoords.h"
#endif
#ifndef cScaledBlock_h
#include "cScaledBlock.h"
#endif
#ifndef cUInt_h
#include "cUInt.h"
#endif

#define BLOCK_FLAG_FILL_IN       0
#define BLOCK_FLAG_AUTO_COMPRESS 1
#define BLOCK_FLAG_FRAGMENT_OK   2
#define BLOCK_FLAG_AUTOCALC_SIZE 3

/**
 * A data structure which allows indexing into an array
 * where each cell of the array is *not* a fixed size. This would be useful
 * in situations such as when working with probabilities (construct an array
 * with each piece of data having a size propotional to its probability, and
 * then index randomly into the array) or mapping the contents of a one-
 * dimensional space (such as computer memory) allowing random access to what
 * is at any location with maximal precision, high speed, and reasonably low
 * memory costs.
 *
 * The structure works by first creating an array where each block contains
 * the data it points to, and a number representing its size. This is then
 * translated onto a second array where all of the blocks represent a fixed
 * size, and simply point back to their respective location in the original
 * array. The structure can therefore find the approximate location in the
 * original array, and do a serial search from there.
 *
 * Example: We have a time line of 100 years which there were 5 distinct
 *   periods of time (labeled A-E for convienence), A = 15 yrs, B = 20 yrs,
 *   C = 10 yrs, D = 5 yrs, and E = 50 yrs. We can represent each of these
 *   as a block with a size equal to its duration. Then, we might translate
 *   it to 5 fixed blocks, each of size 20 years, where the first points to
 *   block A, the second to B, the third to C, and the last two to E. Thus,
 *   if you want to know what period any specific year was in, you only have
 *   to indexed into the fixed-size array, and you immediately have a much
 *   more acurate starting point for your search in the scaled array. In
 *   this case it's not a big deal since there only five blocks to consider,
 *   but when you start having huge quantities of data to consider, this
 *   technique is much more useful.
 *
 * In avida this is used in two parts. The first is in the probabilistic
 * time slicing scheme where we want all of the creatures to have a chance
 * of being executed proportional to their merit. The second is in the
 * cosmic ray mutations, where each creature's chance of being struck by one
 * is scaled to their size.
 *
 * In creating a cBlockStruct, the variables which need to be set are the
 * number of blocks in each of the arrays. In the first array, this simply
 * determines how many of pieces of data can be entered into the structure.
 * In the second (fixed-size blocked) array, this will determine the
 * resolution of the initial search. A higher resolution allows for a much
 * faster search, while a lower resultion means less memory is used. By the
 * diminishing returns of larger arrays, memory and speed maximize when both
 * arrays are made of the same size.
 *
 * The only other variable which needs to be considered is the actual size
 * of each block in the fixed-size array. This can either be entered by the
 * programer, or else optimally determined from the data which has thus far
 * been entered by running the CalcFixedSize() method. It is only recomened
 * the you calculate the value outside of the object if you are planing to
 * still insert more data to be added into the structure after it is initially
 * constructed.
 *
 * To add data to the object, call AddData(void *, int),
 * where the first input
 * is a pointer to the data, and the second is its size. Once all the data
 * has been added, a call to Process() will build up the object so it can
 * be used (remember, the fixed_size must be set before Process() 
 * can run).
 * If more data is added to the structure after Process() is run, a second
 * function, Update(), will incorporate all of the new information without
 * having to take the time to rebuild the whole thing.
 *
 * When AddData(void *, int) is called, it will return a int value 
 * which is
 * the location in the propotional sized array that that piece of data has.
 * If you every want to remove the data from the structure, you must call
 * Remove(int) giving it back this int tag. 
 * Remove will simply change the
 * pointer to the data at this location to a NULL pointer. If you want to
 * put new data in this location, call AddDataTo(void *, int) where the
 * first argument is the data, and the second is the location to place it in.
 * It will have the same size as the original piece of data in this location.
 * Finally, if you wish to simply get rid of the NULL data locations, a
 * call to the method Compress() will do this in O(N) time. 
 * Note that once
 * the data is compressed, all of the tags will change, and the ones sent
 * back by AddData() will no longer be valid.
 *
 * Extra options (in the form of flags) are going to be added. These are:
 *   BLOCK_FLAG_FILL_IN : This causes all new entries added with AddData()
 *     to be placed in the first cell pointing to NULL data in which it fits,
 *     rather then at the end of the currently used portion of the array.
 *     This is useful to conserve space in the array, but insertions take
 *     slightly longer.
 *   BLOCK_FLAG_AUTO_COMPRESS : This causes the array to automatically be
 *     compressed to remove the space left by removed data, and the runs
 *     process again. This keeps clean arrays with no holes in them, but
 *     insertions take *very* long.
 *   BLOCK_FLAG_FRAGMENT_OK : This causes new data to be placed in the blocks
 *     of old data, even if it means spliting up a single piece of data over
 *     a number of blocks. This minimizes the amount of wasted space in the
 *     array without taking the time to compress the entire thing, but can
 *     lead to a huge number of cells in the array being used due to
 *     fragmentation.
 *   BLOCK_FLAG_AUTOCALC_SIZE : This option causes Process() to 
 *     automatically
 *     recalculate the fixed_size every time it is run.
 **/

class cBlockStruct {
private:
  cFixedBlock * fixed_array;
  cScaledBlock * scaled_array;

  int num_fixed;
  int num_scaled;

  cFixedCoords fixed_used;
  int scaled_used;

  int fixed_size;
public:
  cBlockStruct(int in_num_scaled = 1, int in_num_fixed = 1);
  ~cBlockStruct();

/**
 * This function adds a single piece of data to the scaled array
 * but does not update the fixed array yet. 
 * 
 * @see cBlockStruct::AddDataTo
 * @return An index into the scaled block array.
 * @param in_data A pointer to the data.
 * @param in_size The size of the data.
 **/
  int AddData(void * in_data, int in_size);
  
    
/**
 * This function adds a single piece of data to the scaled array
 * but does not update the fixed array yet. In comparison to
 * @ref cBlockStruct::AddData, this function adds the data at the given
 * index in_index in the scaled block array, and returns the size of the
 * data at that index position.
 * 
 * @see cBlockStruct::AddData
 * @return The size of the data at position in_index.
 * @param in_data A pointer to the data.
 * @param in_index The index position at which the data should be put.
 **/
  int AddDataTo(void * in_data, int in_index);

/**
 * This function removes all of the data from the structure.
 **/
  void Clear();

/**
 * This function takes the scaled array and constructs from it the
 * proper fixed array.  Any information which previously was in the fixed 
 * array is  
 * completely erased and re-constructed.  
 *
 * @return The number of @ref cFixedBlock's, or -1 if that number overflows.
 **/
  int Process();   

/**
 * This function looks at data which has been taken in since the last
 * time the fixed array was built or updated, and the new info at the end.
 *
 * @return The number of @ref cFixedBlock's, or -1 if that number overflows.
 **/
  int Update();

/**
 * This function takes the scaled array, and removes any elements from it
 * which no longer point to data, and compresses everything to the begining.
 * This should always be followed shortly by adding any new data, and then
 * a Process(). 
 *
 * @return The new number of @ref cScaledBlock's used.
 **/
    
  int Compress();


/**
 * Function to retrieve data from the structure.
 **/
  inline void * Find(const cFixedCoords & search_coords) const;
/**
 * Function to retrieve data from the structure.
 **/    
  inline void * Find(int in_block, int in_offset) const;
/**
 * Function to retrieve data from the structure.
 **/    
  inline void * Find(cUInt in_position) const;

/**
 *  Function to remove data from the structure.
 **/
  void * Remove(int out_block);

  // Finally, internal variable control...
  inline int GetFixedSize() const { return fixed_size; }
  inline int GetBlocksUsed() const { return fixed_used.GetBlockNum() + 1; }
  inline void SetFixedSize(int in_fs) { fixed_size = in_fs; }

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

inline void * cBlockStruct::Find(const cFixedCoords & search_coords) const
{
  // Make sure the find is within range.
  if (search_coords >= fixed_used) {
    return NULL;
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
  return scaled_array[scaled_loc].GetData();
}

inline void * cBlockStruct::Find(int in_block, int in_offset) const
{
  cFixedCoords search_coords(in_block, in_offset);
  return Find(search_coords);
}

inline void * cBlockStruct::Find(cUInt in_position) const
{
  cFixedCoords search_coords((in_position /= fixed_size).AsInt(),
			     (in_position %= fixed_size).AsInt());
  return Find(search_coords);
}

#endif

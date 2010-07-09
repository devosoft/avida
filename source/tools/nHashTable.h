/*
 *  nHashTable.h
 *  Avida
 *
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

#ifndef nHashTable_h
#define nHashTable_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef BIT_ARRAY_H
#include "cBitArray.h"
#endif

#include <cstdlib>


#define HASH_TABLE_SIZE_DEFAULT 23
#define HASH_TABLE_SIZE_MEDIUM  331
#define HASH_TABLE_SIZE_LARGE   2311


namespace nHashTable {
  
  // HASH_TYPE = basic object
  // Casts the pointer to an int, shift right last two bit positions, mod by
  // the size of the hash table and hope for the best.  The shift is to account
  // for typical 4-byte alignment of pointer values.  Depending on architecture
  // this may not be true and could result in suboptimal hashing at higher
  // order alignments.
  template<typename HASH_TYPE> inline int HashKey(const HASH_TYPE& key, int table_size)
  {
    // Cast/Dereference of key as an int* tells the compiler that we really want
    // to truncate the value to an integer, even if a pointer is larger.
    return abs((*((int*)&key) >> 2) % table_size);    
  }
  
  // HASH_TYPE = int
  // Simply mod the into by the size of the hash table and hope for the best
  template<> inline int HashKey<int>(const int& key, int table_size)
  {
    return abs(key % table_size);
  }
  
  // HASH_TYPE = cString
  // We hash a string simply by adding up the individual character values in
  // that string and modding by the hash size.  For most applications this
  // will work fine (and reasonably fast!) but some patterns will cause all
  // strings to go into the same cell.  For example, "ABC"=="CBA"=="BBB".
  template<> inline int HashKey<cString>(const cString& key, int table_size)
  {
    unsigned int out_hash = 0;
    for (int i = 0; i < key.GetSize(); i++)
      out_hash += (unsigned int) key[i];
    return out_hash % table_size;
  }
  
  // HASH_TYPE = cBitArray
  // We hash a bit array by calculating the sum of the squared values of the
  // positions where bits are on, then modding this number by the size of 
  // the hash table
  template<> inline int HashKey<cBitArray>(const cBitArray& key, int table_size)
  {
    unsigned int out_hash = 0;
    for (int i = 0; i < key.GetSize(); i++) {
      if (key.Get(i)) { out_hash += i*i; }
    }
    return out_hash % table_size;
  }
  
}

#endif

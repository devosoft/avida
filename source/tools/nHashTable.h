/*
 *  nHashTable.h
 *  Avida
 *
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
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
  template<typename HASH_TYPE> inline int HashKey(const HASH_TYPE& key, int table_size)
  {
    // tHashMap is deprecated and exists only to support tDictionary for now.
    assert(false);
    return 0;
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
  
}

#endif

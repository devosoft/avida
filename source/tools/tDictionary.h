/*
 *  tDictionary.h
 *  Avida2
 *
 *  Created by David on 10/11/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 *
 * This template is used to look up objects of the desired type by name.
 * It is essentially a wrapper around tHashTable<cString, DATA_TYPE>, with
 * the addition of NearMatch().
 *
 * For details about the encapsulated methods, see tHashTable.
 */

#ifndef TDICTIONARY_HH
#define TDICTIONARY_HH

#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef STRING_UTIL_HH
#include "cStringUtil.h"
#endif
#ifndef THASH_TABLE_HH
#include "tHashTable.h"
#endif

template <class T> class tDictionary {
private:
  tHashTable<cString, T> m_hash;

  // disabled copy constructor.
  tDictionary(const tDictionary &);
public:
  tDictionary() {}
  tDictionary(int in_hash_size) : m_hash(in_hash_size) {}

  // The following methods just call the encapsulated tHashTable
  bool OK() { return m_hash.OK(); }
  int GetSize() { return m_hash.GetSize(); }
  void Add(const cString & name, T data) { m_hash.Add(name, data); }
  void SetValue(const cString & name, T data) { m_hash.SetValue(name, data); }
  bool HasEntry(const cString & name) { return m_hash.HasEntry(name); }
  bool Find(const cString & name, T & out_data) { return m_hash.Find(name, out_data); }
  T Remove(const cString & name) { return m_hash.Remove(name); }
  void SetHash(int _hash) { m_hash.SetTableSize(_hash); }
  void AsLists(tList<cString> & name_list, tList<T> & value_list)
  {
    m_hash.AsLists(name_list, value_list);
  }
  
  // This function has no direct implementation in tHashTable
  // Grabs key/value lists, and processes the keys.
  cString NearMatch(const cString name) {
    tList<cString> keys;
    tList<T> values;
    m_hash.AsLists(keys, values);

    cString best_match("");
    int best_dist = name.GetSize();
    tListIterator<cString> list_it(keys);
    while (list_it.Next() != NULL) {
      int dist = cStringUtil::EditDistance(name, *list_it.Get());
      if (dist < best_dist) {
        best_dist = dist;
        best_match = *list_it.Get();
      }
    }
    return best_match;
  }
  
};

#endif

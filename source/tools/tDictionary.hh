//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef TDICTIONARY_HH
#define TDICTIONARY_HH

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef STRING_UTIL_HH
#include "string_util.hh"
#endif
#ifndef TLIST_HH
#include "tList.hh"
#endif

#define DICTIONARY_HASH_DEFAULT 23
#define DICTIONARY_HASH_MEDIUM  331
#define DICTIONARY_HASH_LARGE   2311

class cString; // aggregate
struct cStringUtil; // access
template <class T> class tList; // access
template <class T> class tListIterator; // aggregate

template <class T> class tDictionary {

  template <class U> struct tDictEntry {
    cString name;
    U data;
  };

private:
  int size;
  int hash_size;
  tList< tDictEntry<T> > * hash_table;
  
  int HashString(const cString & key) const {
    unsigned int out_hash = 0;
    for (int i = 0; i < key.GetSize(); i++)
      out_hash += (unsigned int) key[i];
    return out_hash % hash_size;
  }
  tDictEntry<T> * FindEntry(const cString & name) {
    const int bin = HashString(name);
    tListIterator< tDictEntry<T> > list_it(hash_table[bin]);
    while (list_it.Next() != NULL) {
      if (list_it.Get()->name == name) return list_it.Get();
    }
    return NULL;
  }
public:
  tDictionary() : size(0), hash_size(DICTIONARY_HASH_DEFAULT) {
    hash_table = new tList< tDictEntry<T> >[hash_size];
  }
  ~tDictionary() {
    for (int i = 0; i < hash_size; i++)
      while (hash_table[i].GetSize()) delete hash_table[i].Pop();
    delete [] hash_table;
  }

  int GetSize() { return size; }
  
  void Add(const cString & name, T data) {
    tDictEntry<T> * new_entry = new tDictEntry<T>;
    new_entry->name = name;
    new_entry->data = data;
    const int bin = HashString(name);
    hash_table[bin].Push(new_entry);
    size++;
  }
  
  bool HasEntry(const cString & name) {
    return FindEntry(name) != NULL;
  }

  bool Find(const cString & name, T & out_data) {
    tDictEntry<T> * found_entry = FindEntry(name);
    if (found_entry != NULL) {
      out_data = found_entry->data;
      return true;
    }
    return false;
  }

  T Remove(const cString & name) {
    const int bin = HashString(name);
    T out_data = NULL;
    tListIterator< tDictEntry<T> > list_it(hash_table[bin]);
    while (list_it.Next() != NULL) {
      if (list_it.Get()->name == name) {
	out_data = list_it.Get()->data;
	list_it.Remove();
	size--;
	break;
      }
    }
    return out_data;
  }

  cString NearMatch(const cString name) {
    cString best_match("");
    int best_dist = name.GetSize();
    for (int i = 0; i < hash_size; i++) {
      tListIterator< tDictEntry<T> > list_it(hash_table[i]);
      while (list_it.Next() != NULL) {
	int dist = cStringUtil::EditDistance(name, list_it.Get()->name);
	if (dist < best_dist) {
	  best_dist = dist;
	  best_match = list_it.Get()->name;
	}
      }
    }
    return best_match;
  }

  void SetHash(int _hash) {
    const int old_hash_size = hash_size;

    // Create the new table...
    hash_size = _hash;
    tList< tDictEntry<T> > * new_hash_table =
      new tList< tDictEntry<T> >[hash_size];

    // Move everything over...
    for (int i = 0; i < old_hash_size; i++) {
      while (hash_table[i].GetSize() > 0) {
	tDictEntry<T> * cur_entry = hash_table[i].Pop();
	const int bin = HashString(cur_entry->name);
	new_hash_table[bin].Push(cur_entry);
      }
    }

    // Cleanup...
    delete [] hash_table;
    hash_table = new_hash_table;
  }
};

#endif

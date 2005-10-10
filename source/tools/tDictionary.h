//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2005 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

/*
 * This template is used to look up objects of the desired type by name.
 * I is implemented through use of a linked list and a hash table.  The linked
 * list contains all of the individual entries stored in the dictionary (in an
                                                                         * arbitrary order).  The hash table points to the first entry in the list
 * that fits in its cell.  If there are no entries that fit in the cell, the
 * has table contains a NULL pointer at that location.
 *
 * INTERFACE:
 *    tDictionary(int in_hash_size=DICTIONARY_HASH_DEFAULT)  // Constructor
 *    ~tDictionary()                                // Destructor
 *    int GetSize()                                 // Get dictionary size
 *    void Add(const cString & name, T data)        // Add new entry
 *    bool HasEntry(const cString & name)           // Test if key exists
 *    bool Find(const cString & name, T & out_data) // Find entry for key
 *    T Remove(const cString & name)                // Remove entry
 *    cString NearMatch(const cString name)         // Find closest key
 *    void SetHash(int _hash)                       // Change hash table size
 *
 *
 * IMPLEMENTATION NOTES:
 *
 * On INSERT: If a cell already has at least one entry in it, the new entry
 * gets inserted into the linked list before the existing entry.  If the cell
 * is currently empty, the new entry gets placed at the end of the linked
 * list.  In either case, the cell is updated to point at the new entry.
 *
 * On DELETE: Start looking at the position in the list where the cell is
 * pointing and continue until the entry-to-be-deleted is found.  If the
 * entry to be deleted is the one being pointed at, be sure to update the
 * cell.
 *
 * On LOOKUP: If the cell has a NULL pointer, lookup fails.  Otherwise search
 * through list until either correct entry is found (lookup succeeds) or else
 * the lookup finds an entry not in the current cell (lookup fails).
 */

#ifndef TDICTIONARY_HH
#define TDICTIONARY_HH

#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef STRING_UTIL_HH
#include "cStringUtil.h"
#endif
#ifndef TARRAY_HH
#include "tArray.h"
#endif
#ifndef TLIST_HH
#include "tList.h"
#endif

#define DICTIONARY_HASH_DEFAULT 23
#define DICTIONARY_HASH_MEDIUM  331
#define DICTIONARY_HASH_LARGE   2311

class cString; // aggregate
struct cStringUtil; // access
template <class T> class tList; // access
template <class T> class tListIterator; // aggregate

template <class T> class tDictionary {
  
  // We create a structure with full information about each entry stored in
  // this dictionary.
  template <class U> struct tDictEntry {
    cString name;
    int id;
    U data;
  };
  
private:
  int dict_size;  // How many entries are we storing?
  int hash_size;  // What size hash table are we using?
  
  tList< tDictEntry<T> > entry_list;      // A linked list of ALL entries
  tArray< tListNode< tDictEntry<T> > * > cell_array;       // Pointers to the entry list.
  tListIterator< tDictEntry<T> > list_it; // Iterator for entry_list
  
  // Currently, we hash a string simply by adding up the individual character
  // values in that string and modding by the hash size.  For most applications
  // this will work fine (and it is fast!) but some patters will cause all
  // strings to go into the same cell.  For example, "ABC"=="CBA"=="BBB".
  int HashString(const cString & key) const {
    unsigned int out_hash = 0;
    for (int i = 0; i < key.GetSize(); i++)
      out_hash += (unsigned int) key[i];
    return out_hash % hash_size;
  }
  
  // Function to find the appropriate tDictEntry for a string that is passed
  // in and return it.
  tDictEntry<T> * FindEntry(const cString & name) {
    const int bin = HashString(name);
    if (cell_array[bin] == NULL) return NULL;
    
    // Set the list iterator to the first entry of this bin.
    list_it.Set(cell_array[bin]);
    
    // Loop through all entries in this bin to see if any are a perfect match.
    while (list_it.Get() != NULL && list_it.Get()->id == bin) {
      if (list_it.Get()->name == name) return list_it.Get();
      list_it.Next();
    }
    
    // No matches found.
    return NULL;
  }
private:
    // disabled copy constructor.
    tDictionary(const tDictionary &);
public:
    tDictionary(int in_hash_size=DICTIONARY_HASH_DEFAULT)
    : dict_size(0)
    , hash_size(in_hash_size)
    , cell_array(in_hash_size)
    , list_it(entry_list)
  {
      cell_array.SetAll(NULL);
  }
  
  ~tDictionary() {
    while (entry_list.GetSize()) delete entry_list.Pop();
  }
  
  
  bool OK() {
    std::cout << "DICT_SIZE = " << dict_size << std::endl;
    std::cout << "HASH_SIZE = " << hash_size << std::endl;
    int count = 0;
    std::cout << "LIST ELEMENTS:" << std::endl;
    list_it.Reset();
    while (list_it.Next() != NULL) {
      tDictEntry<T> * cur_entry = list_it.Get();
      std::cout << "  " << count << " : "
      << cur_entry->id << " "
      << cur_entry->name << " "
      << cur_entry->data << " "
      << std::endl;
    }
    std::cout << std::endl;
    std::cout << "ARRAY CELLS: "
      << cell_array.GetSize()
      << std::endl;
    for (int i = 0; i < hash_size; i++) {
      tListNode< tDictEntry<T> > * cur_list_node = cell_array[i];
      if (cur_list_node == NULL) {
        std::cout << "  NULL" << std::endl;
      } else {
        std::cout << "  " << cur_list_node->data->id
        << " " << cur_list_node->data->name
        << std::endl;
      }
    }
    
    return true;
  }
  
  int GetSize() { return dict_size; }
  
  // This function is used to add a new entry...
  void Add(const cString & name, T data) {
    // Build the new entry...
    tDictEntry<T> * new_entry = new tDictEntry<T>;
    new_entry->name = name;
    new_entry->data = data;
    const int bin = HashString(name);
    new_entry->id = bin;
    
    
    // Determine where this new entry should go; either at the end of
    // the list (if there are no others in the bin) or following another
    // entry in the bin.
    if (cell_array[bin] == NULL) { list_it.Reset(); } // Reset to list start
    else { list_it.Set(cell_array[bin]); }            // Else find insert point
    
    entry_list.Insert(list_it, new_entry); // Place new entry in the list
    list_it.Prev();                        // Back up to new entry
    cell_array[bin] = list_it.GetPos();    // Record position
    
    // Update our entry count...
    dict_size++;
  }
  
  
  // This function will change the value of an entry that exists, or add it
  // if it doesn't exist.
  void SetValue(const cString & name, T data) {
    tDictEntry<T> * cur_entry = FindEntry(name);
    if (cur_entry == NULL) {
      Add(name, data);
      return;
    }
    cur_entry->data = data;
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
    // Determine the bin that we are going to be using.
    const int bin = HashString(name);
    
    T out_data;
    assert(cell_array[bin] != NULL);
    list_it.Set(cell_array[bin]);
    
    // If we are deleting the first entry in this bin we must clean up...
    if (list_it.Get()->name == name) {
      out_data = list_it.Get()->data;
      delete list_it.Remove();
      list_it.Next();
      dict_size--;
      // See if the next entry is still part of this cell.
      if (list_it.AtRoot() == false && list_it.Get()->id == bin) {
        cell_array[bin] = list_it.GetPos();
      } else {
        cell_array[bin] = NULL;
      }
    }
    
    // If it was not the first entry in this cell, keep looking!
    else {
      while (list_it.Next() != NULL && list_it.Get()->id == bin) {
        if (list_it.Get()->name == name) {
          out_data = list_it.Get()->data;
          delete list_it.Remove();
          dict_size--;
          break;
        }
      }
    }
    
    return out_data;
  }
  
  cString NearMatch(const cString name) {
    cString best_match("");
    int best_dist = name.GetSize();
    list_it.Reset();
    while (list_it.Next() != NULL) {
      int dist = cStringUtil::EditDistance(name, list_it.Get()->name);
      if (dist < best_dist) {
        best_dist = dist;
        best_match = list_it.Get()->name;
      }
    }
    return best_match;
  }
  
  void SetHash(int _hash) {
    // Create the new table...
    hash_size = _hash;
    cell_array.ResizeClear(hash_size);
    cell_array.SetAll(NULL);
    
    // Backup all of the entries in the list and re-insert them one-by-one.
    tList< tDictEntry<T> > backup_list;
    backup_list.Transfer(entry_list);
    
    while (backup_list.GetSize() > 0) {
      tDictEntry<T> * cur_entry = backup_list.Pop();
      
      // determine the new bin for this entry.
      int bin = HashString(cur_entry->name);
      cur_entry->id = bin;
      
      if (cell_array[bin] == NULL) { list_it.Reset(); } // Reset to list start
      else { list_it.Set(cell_array[bin]); }            // Else find insert point
      
      entry_list.Insert(list_it, cur_entry); // Place new entry in the list
      list_it.Prev();                        // Back up to new entry
      cell_array[bin] = list_it.GetPos();    // Record position
    }
  }
  
  // The following method allows the user to convert the dictionary contents
  // into lists.  Empty lists show be passed in as arguments and the method
  // will fill in their contents.
  void AsLists(tList<cString> & name_list, tList<T> & value_list) {
    // Setup the lists to fill in.
    assert(name_list.GetSize() == 0);
    assert(value_list.GetSize() == 0);
    tListIterator<cString> name_it(name_list);
    tListIterator<T> value_it(value_list);
    
    // Loop through the current entries and included them into the output
    // list one at a time.
    list_it.Reset();
    while (list_it.Next() != NULL) {
      // Grab the info about the current entry.
      cString & cur_name = list_it.Get()->name;
      T & cur_value = list_it.Get()->data;
      
      // Find the position to place this in the lists.
      name_it.Reset();
      value_it.Reset();
      value_it.Next();
      while (name_it.Next() != NULL && cur_name > *(name_it.Get())) {
        value_it.Next();
      }
      name_list.Insert(name_it, &cur_name);
      value_list.Insert(value_it, &cur_value);
    }
  }
};

#endif

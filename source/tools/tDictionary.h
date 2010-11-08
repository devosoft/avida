/*
 *  tDictionary.h
 *  Avida
 *
 *  Called "tDictionary.hh" prior to 10/11/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef tDictionary_h
#define tDictionary_h

/*
 * This template is used to look up objects of the desired type by name.
 * It is essentially a wrapper around tHashMap<cString, DATA_TYPE>, with
 * the addition of NearMatch().
 *
 * For details about the encapsulated methods, see tHashMap.
 */

#ifndef cString_h
#include "cString.h"
#endif
#ifndef cStringUtil_h
#include "cStringUtil.h"
#endif
#ifndef tHashMap_h
#include "tHashMap.h"
#endif


template <class T> class tDictionary
{
private:
  tHashMap<cString, T> m_hash;
  T m_default;

  tDictionary(const tDictionary&); // @not_implemented


public:
  inline tDictionary() { ; }
  inline tDictionary(int in_hash_size) : m_hash(in_hash_size) { ; }
  
  inline void SetDefault(const T& def) { m_default = def; }
  
  // The following methods just call the encapsulated tHashMap
  inline bool OK() const { return m_hash.OK(); }
  inline int GetSize() const { return m_hash.GetSize(); }
  inline void Set(const cString& name, T data) { m_hash.Set(name, data); }
  inline bool HasEntry(const cString& name) const { return m_hash.HasEntry(name); }
  inline bool Find(const cString& name, T& out_data) const { return m_hash.Find(name, out_data); }
  inline T Get(const cString& name) const { T rval = m_default; m_hash.Find(name, rval); return rval; }
  inline T GetWithDefault(const cString& name, const T& def) const;
  inline void Remove(const cString& name) { m_hash.Remove(name); }
  inline bool Remove(const cString& name, T& data) { return m_hash.Remove(name, data); }
  inline void Clear() { m_hash.ClearAll(); }
  inline void SetHash(int _hash) { m_hash.SetTableSize(_hash); }
  inline void AsLists(tList<cString>& name_list, tList<T>& value_list) const {
    m_hash.AsLists(name_list, value_list);
  }
  inline void GetKeys(tList<cString>& names) const { m_hash.GetKeys(names); }
  inline void GetKeys(tArray<cString>& names) const { m_hash.GetKeys(names); }
  inline void GetValues(tList<T>& values) const { m_hash.GetValues(values); }
  inline void GetValues(tArray<T>& values) const { m_hash.GetValues(values); }
  
  // This function will take an input string and load its value into the
  // dictionary; it will only work for types that cStringUtil can convert to.
  void Load(cString load_string, char assign='=') {
    // Break the string into two based on the assignment character.
    cString key(load_string.Pop(assign));
 
    // Convert the value to the correct type.
    T value;
    value = cStringUtil::Convert(load_string, value);

    SetValue(key, value);
  }
  
  // This function has no direct implementation in tHashMap
  // Grabs key/value lists, and processes the keys.
  cString NearMatch(const cString name) const {
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

template<class T> inline T tDictionary<T>::GetWithDefault(const cString& name, const T& def) const
{
  T rval;
  if (m_hash.Find(name, rval)) return rval;
  return def;
}



template <class T> class tDictionaryNoCase
{
private:
  tHashMap<cString, T> m_hash;
  
  // disabled copy constructor.
  tDictionaryNoCase(const tDictionaryNoCase &);

public:
  inline tDictionaryNoCase() { ; }
  inline tDictionaryNoCase(int in_hash_size) : m_hash(in_hash_size) { ; }
  
  // The following methods just call the encapsulated tHashMap
  inline bool OK() { return m_hash.OK(); }
  inline int GetSize() { return m_hash.GetSize(); }
  inline void SetHash(int _hash) { m_hash.SetTableSize(_hash); }
  inline void AsLists(tList<cString>& name_list, tList<T>& value_list) const {
    m_hash.AsLists(name_list, value_list);
  }


  // Encapsulated tHashMap methods with No Case functionality
  inline void Set(const cString& name, T data) { cString uname(name); uname.ToUpper(); m_hash.Set(uname, data); }
  inline bool HasEntry(const cString& name) const { cString uname(name); uname.ToUpper(); return m_hash.HasEntry(uname); }
  inline bool Find(const cString& name, T& out_data) const {
    cString uname(name); uname.ToUpper(); return m_hash.Find(uname, out_data);
  }
  inline void Remove(const cString& name) { cString uname(name); uname.ToUpper(); m_hash.Remove(uname); }
  inline bool Remove(const cString& name, T& data) { cString uname(name); uname.ToUpper(); return m_hash.Remove(uname, data); }
  
  inline void Clear() { m_hash.ClearAll(); }

  // Fast Accessor Methods - Calling method assumes responsibility for UCasing the key
  inline bool HasEntryFast(const cString& name) const { return m_hash.HasEntry(name); }
  inline bool FindFast(const cString& name, T& out_data) const { return m_hash.Find(name, out_data); }
  
  // This function will take an input string and load its value into the
  // dictionary; it will only work for types that cStringUtil can convert to.
  void Load(cString load_string, char assign='=') {
    // Break the string into two based on the assignment character.
    cString key(load_string.Pop(assign));
    
    // Convert the value to the correct type.
    T value;
    value = cStringUtil::Convert(load_string, value);
    
    SetValue(key, value);
  }
  
  // This function has no direct implementation in tHashMap
  // Grabs key/value lists, and processes the keys.
  cString NearMatch(const cString name) const {
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

/*
 *  tArrayMap.h
 *  Avida
 *
 *  Created by David on 1/11/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#ifndef tArrayMap_h
#define tArrayMap_h

#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tKVPair_h
#include "tKVPair.h"
#endif


template<class KeyType, class ValueType> class tArrayMap
{
private:
  tArray<tKVPair<KeyType, ValueType> > m_map;
  
public:
  tArrayMap() { ; }
  tArrayMap(const tArrayMap& am) : m_map(am.m_map) { ; }
  
  int GetSize() const { return m_map.GetSize(); }
  
  void Set(const KeyType& key, const ValueType& value)
  {
    for (int i = 0; i < m_map.GetSize(); i++) {
      if (m_map[i].Key() == key) {
        m_map[i].Value() = value;
        return;
      }
    }
    m_map.Push(tKVPair<KeyType, ValueType>(key, value));
  }
  
  bool Get(const KeyType& key, ValueType& out_value) const
  {
    for (int i = 0; i < m_map.GetSize(); i++) {
      if (m_map[i].Key() == key) {
        out_value = m_map[i].Value();
        return true;
      }
    }
    return false;
  }
  
  const ValueType& GetWithDefault(const KeyType& key, const ValueType& default_value)
  {
    for (int i = 0; i < m_map.GetSize(); i++) {
      if (m_map[i].Key() == key) return m_map[i].Value();
    }
    m_map.Push(tKVPair<KeyType, ValueType>(key, default_value));
    return m_map[m_map.GetSize() - 1].Value();
  }
  
  ValueType& ValueFor(const KeyType& key)
  {
    for (int i = 0; i < m_map.GetSize(); i++) {
      if (m_map[i].Key() == key) {
        return m_map[i].Value();
      }
    }
    m_map.Push(tKVPair<KeyType, ValueType>(key));
    return m_map[m_map.GetSize() - 1].Value();
  }

  const ValueType& ValueFor(const KeyType& key) const
  {
    for (int i = 0; i < m_map.GetSize(); i++) {
      if (m_map[i].Key() == key) {
        return m_map[i].Value();
      }
    }
    m_map.Push(tKVPair<KeyType, ValueType>(key));
    return m_map[m_map.GetSize() - 1].Value();
  }
  
  ValueType& operator[](const KeyType& key) { return ValueFor(key); }
  const ValueType& operator[](const KeyType& key) const { return ValueFor(key); }
  
  tArray<KeyType> GetKeys() const
  {
    tArray<KeyType> keys(m_map.GetSize());
    for (int i = 0; i < m_map.GetSize(); i++) keys[i] = m_map[i].Key();
    return keys;
  }  
  
  void Remove(const KeyType& key)
  {
    for (int i = 0; i < m_map.GetSize(); i++) {
      if (m_map[i].Key() == key) {
        int lastkv = m_map.GetSize() - 1;
        if (i != lastkv) m_map[i] = m_map[lastkv];
        m_map.Resize(lastkv);
      }
    }
  }

  bool Remove(const KeyType& key, ValueType& out_value)
  {
    for (int i = 0; i < m_map.GetSize(); i++) {
      if (m_map[i].Key() == key) {
        int lastkv = m_map.GetSize() - 1;
        out_value = m_map[i].Value();
        if (i != lastkv) m_map[i] = m_map[lastkv];
        m_map.Resize(lastkv);
        return true;
      }
    }
    return false;
  }
  
  
  typedef typename tArray<tKVPair<KeyType, ValueType> >::iterator iterator;
  typedef typename tArray<tKVPair<KeyType, ValueType> >::const_iterator const_iterator;
  
  inline iterator begin() { return m_map.begin(); }
  inline iterator end() { return m_map.end(); }
  inline const_iterator begin() const { return m_map.begin(); }
  inline const_iterator end() const { return m_map.end(); }
};

#endif

/*
 *  tKVPair.h
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

#ifndef tKVPair_h
#define tKVPair_h

template<class K, class V> class tKVPair
{
private:
  K m_key;
  V m_value;
  
public:
  inline tKVPair() { ; }
  inline tKVPair(const K& key) : m_key(key) { ; }
  inline tKVPair(const K& key, const V& value) : m_key(key), m_value(value) { ; }
  inline tKVPair(const tKVPair& p) : m_key(p.m_key), m_value(p.m_value) { ; }
  
  inline tKVPair& operator=(const tKVPair& rhs) { m_key = rhs.m_key; m_value = rhs.m_value; return *this; }
  
  inline K& Key() { return m_key; }
  inline const K& Key() const { return m_key; }
  
  inline V& Value() { return m_value; }
  inline const V& Value() const { return m_value; }
  
  inline void Set(const K& key, const V& value) { m_key = key; m_value = value; } 
};


#endif

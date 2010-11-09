/*
 *  cMetaGenome.h
 *  Avida
 *
 *  Created by David Bryson on 3/29/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#ifndef cMetaGenome_h
#define cMetaGenome_h

#include "cSequence.h"
#include "cString.h"

class cDataFile;
class cHardwareManager;
template <typename T> class tDictionary;
template <typename T> class tList;


class cMetaGenome
{
private:
  int m_hw_type;
  cString m_inst_set;
  cSequence m_seq;
 
  
public:
  cMetaGenome() : m_hw_type(-1), m_inst_set("(default)") { ; }
  cMetaGenome(int hw, const cString& is, const cSequence& seq) : m_hw_type(hw), m_inst_set(is), m_seq(seq) { ; }
  explicit cMetaGenome(const cString& seq_str);
  cMetaGenome(const cMetaGenome& mg) : m_hw_type(mg.m_hw_type), m_inst_set(mg.m_inst_set), m_seq(mg.m_seq) { ; }
  
  inline int GetHardwareType() const { return m_hw_type; }
  inline const cString& GetInstSet() const { return m_inst_set; }
  inline const cSequence& GetSequence() const { return m_seq; }
  inline cSequence& GetSequence() { return m_seq; }
  
  inline int GetSize() const { return m_seq.GetSize(); }
  
  inline void SetHardwareType(int type) { m_hw_type = type; }
  inline void SetInstSet(const cString& is) { m_inst_set = is; }
  inline void SetSequence(const cSequence& seq) { m_seq = seq; }
  
  cString AsString() const;
  
  bool operator==(const cMetaGenome& mg) const
    { return (m_hw_type == mg.m_hw_type && m_inst_set == mg.m_inst_set && m_seq == mg.m_seq); }
  cMetaGenome& operator=(const cMetaGenome& mg)
    { m_hw_type = mg.m_hw_type; m_inst_set = mg.m_inst_set; m_seq = mg.m_seq; return *this; }

  void Load(const tDictionary<cString>& props, cHardwareManager& hwm);
  void Save(cDataFile& df);
  
  bool LoadFromDetailFile(const cString& fname, const cString& wdir, cHardwareManager& hwm, tList<cString>* errors = NULL);
  void SaveAsDetailFile(cDataFile& df, cHardwareManager& hwm);
};


#endif

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

#include "cGenome.h"
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
  cGenome m_genome;
 
  
public:
  cMetaGenome() : m_hw_type(-1), m_inst_set("(default)") { ; }
  cMetaGenome(int hw, const cString& is, const cGenome& gen) : m_hw_type(hw), m_inst_set(is), m_genome(gen) { ; }
  explicit cMetaGenome(const cString& gen_str);
  cMetaGenome(const cMetaGenome& mg) : m_hw_type(mg.m_hw_type), m_inst_set(mg.m_inst_set), m_genome(mg.m_genome) { ; }
  
  inline int GetHardwareType() const { return m_hw_type; }
  inline const cString& GetInstSet() const { return m_inst_set; }
  inline const cGenome& GetGenome() const { return m_genome; }
  inline cGenome& GetGenome() { return m_genome; }
  
  inline int GetSize() const { return m_genome.GetSize(); }
  
  inline void SetHardwareType(int type) { m_hw_type = type; }
  inline void SetInstSet(const cString& is) { m_inst_set = is; }
  inline void SetGenome(const cGenome& gen) { m_genome = gen; }
  
  cString AsString() const;
  
  bool operator==(const cMetaGenome& mg) const
    { return (m_hw_type == mg.m_hw_type && m_inst_set == mg.m_inst_set && m_genome == mg.m_genome); }
  cMetaGenome& operator=(const cMetaGenome& mg)
    { m_hw_type = mg.m_hw_type; m_inst_set = mg.m_inst_set; m_genome = mg.m_genome; return *this; }

  void Load(const tDictionary<cString>& props, cHardwareManager& hwm);
  void Save(cDataFile& df);
  
  bool LoadFromDetailFile(const cString& fname, const cString& wdir, cHardwareManager& hwm, tList<cString>* errors = NULL);
  void SaveAsDetailFile(cDataFile& df, cHardwareManager& hwm);
};


#endif

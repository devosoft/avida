/*
 *  cMetaGenome.h
 *  Avida
 *
 *  Created by David Bryson on 3/29/09.
 *  Copyright 2009 Michigan State University. All rights reserved.
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

class cDataFile;


class cMetaGenome
{
private:
  int m_hw_type;
  int m_inst_set_id;
  cGenome m_genome;
 
  
public:
  cMetaGenome() : m_hw_type(-1), m_inst_set_id(-1) { ; }
  cMetaGenome(int hw, int is, const cGenome& gen) : m_hw_type(hw), m_inst_set_id(is), m_genome(gen) { ; }
  cMetaGenome(const cMetaGenome& mg) : m_hw_type(mg.m_hw_type), m_inst_set_id(mg.m_inst_set_id), m_genome(mg.m_genome) { ; }
  
  inline int GetHardwareType() const { return m_hw_type; }
  inline int GetInstSetID() const { return m_inst_set_id; }
  inline const cGenome& GetGenome() const { return m_genome; }
  inline cGenome& GetGenome() { return m_genome; }
  
  inline int GetSize() const { return m_genome.GetSize(); }
  
  inline void SetHardwareType(int type) { m_hw_type = type; }
  inline void SetInstSetID(int is) { m_inst_set_id = is; }
  inline void SetGenome(const cGenome& gen) { m_genome = gen; }
  
  bool operator==(const cMetaGenome& mg) const
    { return (m_hw_type == mg.m_hw_type && m_inst_set_id == mg.m_inst_set_id && m_genome == mg.m_genome); }
  cMetaGenome& operator=(const cMetaGenome& mg)
    { m_hw_type = mg.m_hw_type; m_inst_set_id = mg.m_inst_set_id; m_genome = mg.m_genome; return *this; }
  
  void Save(cDataFile& df);
};


#endif

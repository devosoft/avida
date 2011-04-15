/*
 *  core/cGenome.h
 *  avida-core
 *
 *  Created by David Bryson on 3/29/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaCoreGenome_h
#define AvidaCoreGenome_h

#include "avida/core/cSequence.h"

#include "cString.h"

class cDataFile;
class cHardwareManager;
class cUserFeedback;
template <typename T> class tDictionary;
template <typename T> class tList;

namespace Avida {
  
  // cGenome - genetic and epi-genetic heritable information
  // --------------------------------------------------------------------------------------------------------------
  
  class cGenome
  {
  private:
    int m_hw_type;
    cString m_inst_set;
    cSequence m_seq;
   
    
  public:
    cGenome() : m_hw_type(-1), m_inst_set("(default)") { ; }
    cGenome(int hw, const cString& is, const cSequence& seq) : m_hw_type(hw), m_inst_set(is), m_seq(seq) { ; }
    explicit cGenome(const cString& seq_str);
    cGenome(const cGenome& gen) : m_hw_type(gen.m_hw_type), m_inst_set(gen.m_inst_set), m_seq(gen.m_seq) { ; }
    
    
    inline int GetHardwareType() const { return m_hw_type; }
    inline const cString& GetInstSet() const { return m_inst_set; }
    inline const cSequence& GetSequence() const { return m_seq; }
    inline cSequence& GetSequence() { return m_seq; }
    
    inline int GetSize() const { return m_seq.GetSize(); }
    
    
    inline void SetHardwareType(int type) { m_hw_type = type; }
    inline void SetInstSet(const cString& is) { m_inst_set = is; }
    inline void SetSequence(const cSequence& seq) { m_seq = seq; }
    
    
    cString AsString() const;
    
    
    bool operator==(const cGenome& gen) const
      { return (m_hw_type == gen.m_hw_type && m_inst_set == gen.m_inst_set && m_seq == gen.m_seq); }
    cGenome& operator=(const cGenome& gen)
      { m_hw_type = gen.m_hw_type; m_inst_set = gen.m_inst_set; m_seq = gen.m_seq; return *this; }

    
    void Load(const tDictionary<cString>& props, cHardwareManager& hwm);
    void Save(cDataFile& df);
    
    bool LoadFromDetailFile(const cString& fname, const cString& wdir, cHardwareManager& hwm, cUserFeedback* errors = NULL);
    void SaveAsDetailFile(cDataFile& df, cHardwareManager& hwm);
  };  
};

#endif

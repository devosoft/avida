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

#include "apto/platform.h"
#include "avida/core/cSequence.h"

#include "cString.h"

class cHardwareManager;

class cDataFile;
template <typename T> class tDictionary;


namespace Avida {  
  
  // Class Declarations
  // --------------------------------------------------------------------------------------------------------------
  
  class Feedback;
  
  
  // cGenome - genetic and epi-genetic heritable information
  // --------------------------------------------------------------------------------------------------------------
  
  class cGenome
  {
  private:
    int m_hw_type;
    cString m_inst_set;
    cSequence m_seq;
   
    
  public:
    LIB_EXPORT cGenome() : m_hw_type(-1), m_inst_set("(default)") { ; }
    LIB_EXPORT cGenome(int hw, const cString& is, const cSequence& seq) : m_hw_type(hw), m_inst_set(is), m_seq(seq) { ; }
    explicit LIB_EXPORT cGenome(const cString& seq_str);
    LIB_EXPORT cGenome(const cGenome& gen) : m_hw_type(gen.m_hw_type), m_inst_set(gen.m_inst_set), m_seq(gen.m_seq) { ; }
    
    
    inline LIB_EXPORT int GetHardwareType() const { return m_hw_type; }
    inline LIB_EXPORT const cString& GetInstSet() const { return m_inst_set; }
    inline LIB_EXPORT const cSequence& GetSequence() const { return m_seq; }
    inline LIB_EXPORT cSequence& GetSequence() { return m_seq; }
    
    inline LIB_EXPORT int GetSize() const { return m_seq.GetSize(); }
    
    
    inline LIB_EXPORT void SetHardwareType(int type) { m_hw_type = type; }
    inline LIB_EXPORT void SetInstSet(const cString& is) { m_inst_set = is; }
    inline LIB_EXPORT void SetSequence(const cSequence& seq) { m_seq = seq; }
    
    
    cString LIB_EXPORT AsString() const;
    
    
    LIB_EXPORT bool operator==(const cGenome& gen) const
      { return (m_hw_type == gen.m_hw_type && m_inst_set == gen.m_inst_set && m_seq == gen.m_seq); }
    LIB_EXPORT cGenome& operator=(const cGenome& gen)
      { m_hw_type = gen.m_hw_type; m_inst_set = gen.m_inst_set; m_seq = gen.m_seq; return *this; }

    
    LIB_EXPORT void Load(const tDictionary<cString>& props, cHardwareManager& hwm);
    LIB_EXPORT void Save(cDataFile& df);
    
    LIB_EXPORT bool LoadFromDetailFile(const cString& fname, const cString& wdir, cHardwareManager& hwm, Feedback& feedback);
    LIB_EXPORT void SaveAsDetailFile(cDataFile& df, cHardwareManager& hwm);
  };  
};

#endif

/*
 *  hardware/InstSet.h
 *  avida-core
 *
 *  Created by David on 2/1/13.
 *  Copyright 2013 Michigan State University. All rights reserved.
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

#ifndef AvidaHardwareInstSet_h
#define AvidaHardwareInstSet_h

#include "avida/hardware/Config.h"
#include "avida/hardware/InstLib.h"
#include "avida/util/ArgParser.h"


namespace Avida {
  namespace Hardware {
    
    // Hardware::InstSet
    // --------------------------------------------------------------------------------------------------------------
    
    class InstSet : public Config
    {
    private:
      struct Entry;
      
    private:
      Apto::String m_name;
      Apto::String m_hw_type;
      InstLib* m_inst_lib;
      
      Apto::Array<Entry> m_entries;
      Apto::Array<int> m_lib_nopmod_map;
      Apto::Array<Apto::String> m_features;
      
      
    public:
      LIB_EXPORT InstSet* LoadInstSet(const Apto::String& name, const Apto::String& hardware_type, Util::Args* args,
                                      const Apto::String& inst_set_str, Feedback& feedback);
      LIB_EXPORT ~InstSet();
      
      // Instruction Set Details
      LIB_EXPORT const Apto::String& Name() const { return m_name; }
      LIB_EXPORT const Apto::String& HardwareType() const { return m_hw_type; }
      
      LIB_EXPORT inline InstLib* InstructionLibrary() { return m_inst_lib; }
      LIB_EXPORT inline const InstLib* InstructionLibrary() const { return m_inst_lib; }
      
      LIB_EXPORT inline int NumInsts() const { return m_entries.GetSize(); }
      LIB_EXPORT inline int NumNops() const { return m_lib_nopmod_map.GetSize(); }
      
      LIB_EXPORT inline const Apto::Array<Apto::String>& RequiredFeatures() const { return m_features; }
      
      
      // Instruction Entry Details
      LIB_EXPORT inline const Apto::String& NameOf(const Instruction& inst) const { return m_inst_lib->NameOf(m_entries[inst.GetOp()].lib_id); }
      LIB_EXPORT inline int LibIDOf(const Instruction& inst) const { return m_entries[inst.GetOp()].lib_id; }
      LIB_EXPORT inline const Util::Args& ArgsOf(const Instruction& inst) const { return *m_entries[inst.GetOp()].args; }
      
      LIB_EXPORT inline int NopModOf(const Instruction& inst) const { return m_inst_lib->NopModOf(m_lib_nopmod_map[inst.GetOp()]); }

      LIB_EXPORT inline int IsNop(const Instruction& inst) const { return (inst.GetOp() < m_lib_nopmod_map.GetSize()); }
      LIB_EXPORT inline bool IsLabel(const Instruction& inst) const { return m_inst_lib->EntryAt(LibIDOf(inst)).IsLabel(); }
      LIB_EXPORT inline bool IsPromoter(const Instruction& inst) const { return m_inst_lib->EntryAt(LibIDOf(inst)).IsPromoter(); }
      LIB_EXPORT inline bool IsTerminator(const Instruction& inst) const { return m_inst_lib->EntryAt(LibIDOf(inst)).IsTerminator(); }
      LIB_EXPORT inline bool ShouldStall(const Instruction& inst) const { return m_inst_lib->EntryAt(LibIDOf(inst)).ShouldStall(); }
      LIB_EXPORT inline bool IsImmediateValue(const Instruction& inst) const { return m_inst_lib->EntryAt(LibIDOf(inst)).IsImmediateValue(); }
      
      LIB_EXPORT inline unsigned int FlagsOf(const Instruction& inst) const { return m_inst_lib->EntryAt(LibIDOf(inst)).Flags(); }

      
      // Special purpose instructions
      LIB_EXPORT inline Instruction InstDefault() const { return m_inst_lib->InstDefault(); }
      LIB_EXPORT inline Instruction InstError() const { return Instruction(255); }
      
      
      // Helper methods
      LIB_EXPORT inline bool InstInSet(const Apto::String& name) const { return (InstWithName(name) != InstError()); }
      LIB_EXPORT Instruction InstWithName(const Apto::String& name) const;
      LIB_EXPORT Apto::String FindBestMatch(const Apto::String& name) const;
      
      
    private:
      LIB_LOCAL InstSet(const Apto::String& name, const Apto::String& hardware_type, InstLib* inst_lib, Util::Args* args)
        : Config(args), m_name(name), m_hw_type(hardware_type), m_inst_lib(inst_lib) { ; }
      
      
    private:
      struct Entry
      {
        int lib_id;
        Util::Args* args;
        
        LIB_EXPORT inline Entry() : lib_id(-1), args(NULL) { ; }
        LIB_LOCAL inline Entry(int in_lib_id, Util::Args* in_args) : lib_id(in_lib_id), args(in_args) { ; }
        LIB_EXPORT inline ~Entry() { delete args; }
      };
      
    };
    
  };
};

#endif

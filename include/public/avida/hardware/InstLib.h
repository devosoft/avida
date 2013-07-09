/*
 *  hardware/InstLib.h
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

#ifndef AvidaHardwareInstLib_h
#define AvidaHardwareInstLib_h

#include "avida/core/InstructionSequence.h"
#include "avida/hardware/Types.h"


namespace Avida {
  namespace Hardware {
    
    // Hardware::InstructionFlags
    // --------------------------------------------------------------------------------------------------------------

    namespace InstructionFlags {
      const unsigned int DEFAULT = 0x1;
      const unsigned int NOP = 0x2;
      const unsigned int LABEL = 0x4;
      const unsigned int STALL = 0x8;
      const unsigned int SLEEP = 0x10;
      const unsigned int PROMOTER = 0x20;
      const unsigned int TERMINATOR = 0x40;
      const unsigned int IMMEDIATE_VALUE = 0x80;
      const unsigned int RESERVED_1 = 0x100;
      const unsigned int RESERVED_2 = 0x200;
      const unsigned int RESERVED_3 = 0x400;
      const unsigned int RESERVED_4 = 0x800;
      const unsigned int RESERVED_5 = 0x1000;
      const unsigned int RESERVED_6 = 0x2000;
      const unsigned int RESERVED_7 = 0x4000;
      const unsigned int RESERVED_8 = 0x8000;
      const unsigned int USER_1 = 0x10000;
      const unsigned int USER_2 = 0x20000;
      const unsigned int USER_3 = 0x40000;
      const unsigned int USER_4 = 0x80000;
      const unsigned int USER_5 = 0x100000;
      const unsigned int USER_6 = 0x200000;
      const unsigned int USER_7 = 0x400000;
      const unsigned int USER_8 = 0x800000;
      const unsigned int USER_9 = 0x1000000;
      const unsigned int USER_10 = 0x2000000;
      const unsigned int USER_11 = 0x4000000;
      const unsigned int USER_12 = 0x8000000;
      const unsigned int USER_13 = 0x10000000;
      const unsigned int USER_14 = 0x20000000;
      const unsigned int USER_15 = 0x40000000;
      const unsigned int USER_16 = 0x80000000;
    };

    
    // Hardware::InstructionFlags
    // --------------------------------------------------------------------------------------------------------------
    
    enum InstructionClass {
      INST_CLASS_NOP = 0,
      INST_CLASS_FLOW_CONTROL,
      INST_CLASS_CONDITIONAL,
      INST_CLASS_ARITHMETIC_LOGIC,
      INST_CLASS_DATA,
      INST_CLASS_ENVIRONMENT,
      INST_CLASS_LIFECYCLE,
      INST_CLASS_OTHER,
      
      NUM_INST_CLASSES
    };

    
    // Hardware::InstLib
    // --------------------------------------------------------------------------------------------------------------
    
    class InstLib
    {
    public:
      class Entry
      {
      protected:
        const Apto::String m_name;
        const InstructionClass m_class;
        const unsigned int m_flags;
        const Apto::String m_desc;
        
      public:
        LIB_EXPORT inline Entry(const Apto::String& name, InstructionClass iclass, unsigned int flags, const Apto::String& desc)
          : m_name(name), m_class(iclass), m_flags(flags), m_desc(desc) { ; }
        LIB_EXPORT virtual ~Entry();
        
        LIB_EXPORT inline const Apto::String& Name() const { return m_name; }
        LIB_EXPORT inline InstructionClass InstClass() const { return m_class; }
        LIB_EXPORT inline const Apto::String& Description() const { return m_desc; }
        
        LIB_EXPORT inline unsigned int Flags() const { return m_flags; }
        LIB_EXPORT inline bool IsDefault() const { return (m_flags & InstructionFlags::DEFAULT) != 0; }
        LIB_EXPORT inline bool IsNop() const { return (m_flags & InstructionFlags::NOP) != 0; }
        LIB_EXPORT inline bool IsLabel() const { return (m_flags & InstructionFlags::LABEL) != 0; }
        LIB_EXPORT inline bool IsPromoter() const { return (m_flags & InstructionFlags::PROMOTER) != 0; }
        LIB_EXPORT inline bool IsTerminator() const { return (m_flags & InstructionFlags::TERMINATOR) != 0; }
        LIB_EXPORT inline bool ShouldStall() const { return (m_flags & InstructionFlags::STALL) != 0; }
        LIB_EXPORT inline bool ShouldSleep() const { return (m_flags & InstructionFlags::SLEEP) != 0; }
        LIB_EXPORT inline bool IsImmediateValue() const { return (m_flags & InstructionFlags::IMMEDIATE_VALUE) != 0; }
      };
      
      
    protected:
      const int m_size;
      Apto::Map<Apto::String, int> m_namemap;
      
      int m_inst_default;
      int m_inst_null;
      
      
    public:
      LIB_EXPORT inline InstLib(int size, int inst_default, int inst_null)
        : m_size(size), m_inst_default(inst_default), m_inst_null(inst_null) { ; }
      LIB_EXPORT virtual ~InstLib() = 0;
      
      LIB_EXPORT inline int Size() const { return m_size; }
      LIB_EXPORT inline int InstDefault() const { return m_inst_default; }
      LIB_EXPORT inline int InstNull() const { return m_inst_null; }
      

      // Subclass Methods
      LIB_EXPORT virtual const Entry& EntryAt(int i) const = 0;
      LIB_EXPORT virtual int NopModOf(unsigned int idx) const = 0;


      // Convenience Accessors
      LIB_EXPORT inline const Entry& operator[](int i) const { return EntryAt(i); }
      LIB_EXPORT inline const Apto::String& NameOf(int entry) const { return EntryAt(entry).Name(); }
      LIB_EXPORT inline int NopModOf(const Instruction& inst) const { return NopModOf(inst.GetOp()); }
      

      // Lookup Methods
      LIB_EXPORT inline int IndexOf(const Apto::String& name) const;
      LIB_EXPORT inline Apto::String NearMatch(const Apto::String& name) const { return Apto::NearMatch(name, m_namemap.Keys()); }
    };
    
    
    inline int InstLib::IndexOf(const Apto::String& name) const
    {
      int idx = -1;
      if (m_namemap.Get(name, idx)) return idx;
      return -1;
    }


    // Hardware::StaticTableInstLib
    // --------------------------------------------------------------------------------------------------------------
    
    template <class MethodType> class StaticTableInstLib : public InstLib
    {
    public:
      class MethodEntry : public Entry
      {
      private:
        const MethodType m_function;
        
      public:
        MethodEntry(const Apto::String& name, MethodType function, InstructionClass _class = INST_CLASS_OTHER,
                          unsigned int flags = 0, const Apto::String& desc = "")
        : Entry(name, _class, flags, desc), m_function(function) { ; }
        
        const MethodType Function() const { return m_function; }
      };

    private:
      const MethodEntry* m_entries;
      const int* m_nopmods;
      const MethodType* m_functions;
      
    public:
      StaticTableInstLib(int size, const MethodEntry* entries, const int* nopmods, const MethodType* functions,
                         int default_inst, int null_inst)
        : InstLib(size, default_inst, null_inst), m_entries(entries), m_nopmods(nopmods), m_functions(functions)
      {
        // Fill out InstLib::m_namemap dictionary with instruction name to entry index mappings
        for (int i = 0; i < m_size; i++) m_namemap.Set(m_entries[i].Name(), i);
      }
      
      const MethodType* Functions() const { return m_functions; }
      
      const Entry& EntryAt(int i) const { assert(i < m_size); return m_entries[i]; }
      int NopModOf(unsigned int idx) const { return m_nopmods[idx]; }
    };
  };
};

#endif

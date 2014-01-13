/*
 *  viewer/Freezer.h
 *  avida-core
 *
 *  Created by David on 10/28/10.
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
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

#ifndef AvidaViewerFreezer_h
#define AvidaViewerFreezer_h

#include "avida/core/Types.h"
#include "avida/viewer/Types.h"

class cWorld;


namespace Avida {
  namespace Viewer {
    
    // Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    enum FreezerObjectType {
      CONFIG = 0,
      GENOME,
      WORLD
    };
    
    
    // FreezerID
    // --------------------------------------------------------------------------------------------------------------
    
    struct FreezerID
    {
    public:
      FreezerObjectType type:2;
      int identifier:30;
      
      LIB_EXPORT inline FreezerID() : type(CONFIG), identifier(-1) { ; }
      LIB_EXPORT inline FreezerID(FreezerObjectType in_type, int in_id) : type(in_type), identifier(in_id) { ; }
    };
    
    
    // Freezer
    // --------------------------------------------------------------------------------------------------------------

    class Freezer
    {
    public:
      class Iterator;
      
    private:
      Apto::String m_dir;
      bool m_opened;
      
      struct Entry {
        Apto::String name;
        Apto::String path;
        
        bool active;
        
        LIB_EXPORT inline Entry() : active(false) { ; }
        LIB_EXPORT inline Entry(const Apto::String& in_name, const Apto::String& in_path) : name(in_name), path(in_path), active(true) { ; }
      };
      Apto::Array<Entry> m_entries[3];
      int m_next_id[3];
      
      
    public:
      LIB_EXPORT static FreezerPtr LoadWithPath(const Apto::String& dir);
      LIB_EXPORT static FreezerPtr CreateWithPath(const Apto::String& dir);
      
      LIB_EXPORT ~Freezer();
      
      LIB_EXPORT int NumEntriesOfType(FreezerObjectType type) const { return m_entries[type].GetSize(); }
      
      LIB_EXPORT inline Iterator EntriesOfType(FreezerObjectType type) const { return Iterator(type, m_entries[type]); }
      
      LIB_EXPORT FreezerID SaveConfig(cWorld* world, const Apto::String& name);
      LIB_EXPORT FreezerID SaveGenome(GenomePtr genome, const Apto::String& name);
      LIB_EXPORT FreezerID SaveWorld(cWorld* world, const Apto::String& name);
      
      LIB_EXPORT bool InstantiateWorkingDir(FreezerID entry_id, const Apto::String& working_directory) const;
      LIB_EXPORT GenomePtr InstantiateGenome(FreezerID entry_id) const;
      
      LIB_EXPORT Apto::String PathOf(FreezerID entry_id) const;
      LIB_EXPORT inline const Apto::String& NameOf(FreezerID eid) const { return m_entries[eid.type][eid.identifier].name; }
      LIB_EXPORT inline bool IsActive(FreezerID eid) const { return m_entries[eid.type][eid.identifier].active; }
      
      LIB_EXPORT bool SaveAttachment(FreezerID entry_id, const Apto::String& name, const Apto::String& value);
      LIB_EXPORT Apto::String LoadAttachment(FreezerID entry_id, const Apto::String& name);
      
      
      LIB_EXPORT bool Rename(FreezerID entry_id, const Apto::String& name);
      LIB_EXPORT Apto::String NewUniqueNameForType(FreezerObjectType type, const Apto::String& name = "Untitled");
      
      
      LIB_EXPORT inline void Remove(FreezerID eid) { m_entries[eid.type][eid.identifier].active = false; }
      LIB_EXPORT inline void Restore(FreezerID eid) { m_entries[eid.type][eid.identifier].active = true; }
      
      
      LIB_EXPORT inline bool IsValid(FreezerID eid) const
      {
        return (eid.identifier >= 0 && eid.identifier < m_entries[eid.type].GetSize());
      }
      
      LIB_EXPORT void DuplicateFreezerAt(Apto::String destination);
      LIB_EXPORT void ExportItem(FreezerID entry_id, Apto::String destination);
      LIB_EXPORT FreezerID ImportItem(Apto::String src);
      
    public:
      class Iterator
      {
        friend class Freezer;
        
      private:
        const Apto::Array<Entry>& m_entries;
        FreezerID m_id;
        
        LIB_EXPORT inline Iterator(FreezerObjectType type, const Apto::Array<Entry>& entries)
          : m_entries(entries), m_id(type, -1) { ; }
        
      public:
        LIB_EXPORT inline const FreezerID* Get() const
        {
          return (m_id.identifier < 0 || m_id.identifier >= m_entries.GetSize()) ? NULL : &m_id;
        }
        LIB_EXPORT inline const FreezerID* Next() { ++m_id.identifier; return Get(); }
      };
      
      
    private:
      LIB_LOCAL Freezer(const Apto::String& dir);
    };
    
  };
};

#endif

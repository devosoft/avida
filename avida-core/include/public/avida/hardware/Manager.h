/*
 *  hardware/Manager.h
 *  avida-core
 *
 *  Created by David on 1/31/13.
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

#ifndef AvidaHardwareManager_h
#define AvidaHardwareManager_h

#include "avida/core/Universe.h"
#include "avida/hardware/Types.h"
#include "avida/util/ArgParser.h"
#include "avida/util/ConfigFile.h"


namespace Avida {
  namespace Hardware {
    
    // Hardware::Manager
    // --------------------------------------------------------------------------------------------------------------
    
    class Manager : public UniverseFacet
    {
    private:
      struct HTInfo {
        const InstLib* inst_lib;
        Util::ArgSchema* schema;
        
        LIB_EXPORT inline HTInfo() : inst_lib(NULL), schema(NULL) { ; }
        LIB_EXPORT ~HTInfo();
      };
      
      static Apto::Map<Apto::String, HTInfo> s_hardware_types;
      
    private:
      Universe* m_universe;

      Apto::Map<Apto::String, InstSet*> m_inst_sets;

      
    public:
      LIB_EXPORT static ManagerPtr CreateFor(Universe* universe);
      LIB_EXPORT ~Manager();
      
      LIB_EXPORT InstSet* InstSetWithName(const Apto::String& set_name);
      
      LIB_EXPORT bool LoadInstSet(const Util::ConfigFile& fileref);
      LIB_EXPORT bool LoadInstSet(const Apto::String& inst_set_str);
      
      
      LIB_EXPORT static bool RegisterHardwareType(const Apto::String& type_name, const InstLib* inst_lib, Util::ArgSchema* schema);
      
      LIB_EXPORT static ManagerPtr Of(Universe* universe);
      
    public:
      LIB_EXPORT bool Serialize(ArchivePtr ar) const;
      
    public:
      LIB_LOCAL UniverseFacetID UpdateBefore() const;
      LIB_LOCAL UniverseFacetID UpdateAfter() const;

    private:
      LIB_EXPORT Manager(Universe* universe);
    };
    
  };
};

#endif

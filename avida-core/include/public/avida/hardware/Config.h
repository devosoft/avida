/*
 *  hardware/Config.h
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

#ifndef AvidaHardwareConfig_h
#define AvidaHardwareConfig_h

#include "avida/hardware/Types.h"


namespace Avida {
  namespace Hardware {
    
    // Hardware::Config
    // --------------------------------------------------------------------------------------------------------------
    
    class Config : Apto::RefCountObject<Apto::ThreadSafe>
    {
    private:
      Util::Args* m_args;
      
    public:
      LIB_EXPORT inline Config(Util::Args* args) : m_args(args) { ; }
      LIB_EXPORT virtual ~Config() = 0;

      LIB_EXPORT inline Util::Args& Arguments() { return *m_args; }
      LIB_EXPORT inline const Util::Args& Arguments() const { return *m_args; }
    };
    
  };
};

#endif

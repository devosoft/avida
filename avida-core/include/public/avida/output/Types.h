/*
 *  output/Types.h
 *  avida-core
 *
 *  Created by David on 1/16/13.
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

#ifndef AvidaOutputTypes_h
#define AvidaOutputTypes_h

#include "avida/core/Types.h"

namespace Avida {
  namespace Output {
    
    // Class Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    class File;
    class Manager;
    class Socket;
    
    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    typedef Apto::String OutputID;
    typedef Socket* SocketWeakRef;
    typedef Apto::SmartPtr<File, Apto::InternalRCObject> FilePtr;
    typedef Apto::SmartPtr<Manager, Apto::InternalRCObject> ManagerPtr;
    typedef Apto::SmartPtr<Socket, Apto::InternalRCObject> SocketPtr;
  };
};

#endif

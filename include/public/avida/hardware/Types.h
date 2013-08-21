/*
 *  hardware/Types.h
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

#ifndef AvidaHardwareTypes_h
#define AvidaHardwareTypes_h

#include "avida/core/Types.h"


namespace Avida {
  namespace Hardware {
    
    // Class Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    class Base;
    class Config;
    class CycleListener;
    class Feature;
    class InstArchCPU;
    class InstLib;
    class InstMemSpace;
    class InstSet;
    class Library;
    class Manager;

    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    typedef Apto::SmartPtr<Config, Apto::InternalRCObject> ConfigPtr;
    typedef Apto::SmartPtr<Base, Apto::InternalRCObject> InstancePtr;
    typedef Apto::SmartPtr<Manager, Apto::InternalRCObject> ManagerPtr;
    
    typedef Apto::Functor<InstancePtr, Apto::TL::Create<> > HardwareCreateFunctor;
    
    
    // Constant Declarations
    // --------------------------------------------------------------------------------------------------------------

    static const unsigned int MASK_SIGNBIT = 0x7FFFFFFF;

    static const unsigned int MASK_LOW4    = 0x0000000F;
    static const unsigned int MASK_LOW8    = 0x000000FF;
    static const unsigned int MASK_LOW12   = 0x00000FFF;
    static const unsigned int MASK_LOW16   = 0x0000FFFF;
    static const unsigned int MASK_LOW20   = 0x000FFFFF;
    static const unsigned int MASK_LOW24   = 0x00FFFFFF;
    static const unsigned int MASK_LOW28   = 0x0FFFFFFF;
    
    static const unsigned int MASK_UPPER4  = 0xF0000000;
    static const unsigned int MASK_UPPER8  = 0xFF000000;
    static const unsigned int MASK_UPPER12 = 0xFFF00000;
    static const unsigned int MASK_UPPER16 = 0xFFFF0000;
    static const unsigned int MASK_UPPER17 = 0xFFFF8000;
    static const unsigned int MASK_UPPER18 = 0xFFFFC000;
    static const unsigned int MASK_UPPER19 = 0xFFFFE000;
    static const unsigned int MASK_UPPER20 = 0xFFFFF000;
    static const unsigned int MASK_UPPER24 = 0xFFFFFF00;
    static const unsigned int MASK_UPPER28 = 0xFFFFFFF0;

  };
};

#endif

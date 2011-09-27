/*
 *  data/Types.h
 *  avida-core
 *
 *  Created by David on 5/18/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
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

#ifndef AvidaDataTypes_h
#define AvidaDataTypes_h

#include "apto/core.h"
#include "avida/core/Types.h"

namespace Avida {
  namespace Data {
    
    // Class Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    class Manager;
    class Package;
    class Provider;    
    class Recorder;

    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------

    typedef Apto::String DataID;
    typedef Apto::SmartPtr<Provider, Apto::ThreadSafeRefCount> ProviderPtr;
    typedef Apto::Functor<ProviderPtr, Apto::TL::Create<World*, Apto::String> > ProviderActivateFunctor;
    
    typedef Apto::SmartPtr<Recorder, Apto::ThreadSafeRefCount> RecorderPtr;
    
    typedef Apto::Set<DataID> DataSet;
    typedef Apto::SmartPtr<DataSet, Apto::ThreadSafeRefCount> DataSetPtr;
    typedef Apto::SmartPtr<const DataSet, Apto::ThreadSafeRefCount> ConstDataSetPtr;
    typedef Apto::Set<Apto::String>::ConstIterator ConstDataSetIterator;
    
    typedef Apto::SmartPtr<Package, Apto::ThreadSafeRefCount> PackagePtr;
    
    typedef Apto::Functor<PackagePtr, Apto::TL::Create<const DataID&> > DataRetrievalFunctor;
    
    typedef Apto::SmartPtr<Manager, Apto::InternalRCObject> ManagerPtr;
  };
};

#endif

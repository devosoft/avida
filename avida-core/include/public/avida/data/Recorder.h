/*
 *  data/Recorder.h
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

#ifndef AvidaDataRecorder_h
#define AvidaDataRecorder_h

#include "apto/platform.h"
#include "avida/core/Types.h"
#include "avida/data/Types.h"


namespace Avida {
  namespace Data {
    
    // Data::Recorder - Protocol defining interface for data recorders that can attach to the data manager
    // --------------------------------------------------------------------------------------------------------------
    
    class Recorder : public virtual Apto::RefCountObject<Apto::ThreadSafe>
    {
    public:
      LIB_EXPORT virtual ~Recorder() = 0;
      
      LIB_EXPORT virtual ConstDataSetPtr RequestedData() const = 0;
      
      LIB_EXPORT virtual void NotifyData(Update current_update, DataRetrievalFunctor retrieve_data) = 0; 
    };
    
  };
};

#endif

/*
 *  data/Provider.h
 *  avida-core
 *
 *  Created by David on 5/16/11.
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

#ifndef AvidaDataProvider_h
#define AvidaDataProvider_h

#include "apto/platform.h"
#include "avida/core/Types.h"
#include "avida/data/Types.h"


namespace Avida {
  namespace Data {
    
    // Data::Provider - Data Provider Protocol Definition
    // --------------------------------------------------------------------------------------------------------------
    
    class Provider : virtual public Apto::RefCountObject<Apto::ThreadSafe>
    {
    public:
      LIB_EXPORT virtual ~Provider() { ; }
      
      LIB_EXPORT virtual ConstDataSetPtr Provides() const = 0;
      LIB_EXPORT virtual void UpdateProvidedValues(Update current_update) = 0;
      
      LIB_EXPORT virtual PackagePtr GetProvidedValue(const DataID& data_id) const = 0;
      LIB_EXPORT virtual Apto::String DescribeProvidedValue(const DataID& data_id) const = 0;
      
      LIB_EXPORT virtual bool SupportsConcurrentUpdate() const;
    };
    
    
    class ArgumentedProvider : public Provider
    {
    public:
      LIB_EXPORT virtual void SetActiveArguments(const DataID& data_id, ConstArgumentSetPtr args) = 0;
      LIB_EXPORT virtual ConstArgumentSetPtr GetValidArguments(const DataID& data_id) const = 0;
      LIB_EXPORT virtual bool IsValidArgument(const DataID& data_id, Argument arg) const = 0;
      
      LIB_EXPORT virtual PackagePtr GetProvidedValueForArgument(const DataID& data_id, const Argument& arg) const = 0;
      LIB_EXPORT virtual PackagePtr GetProvidedValuesForArguments(const DataID& data_id, ConstArgumentSetPtr args) const;
      
      LIB_EXPORT virtual PackagePtr GetProvidedValue(const DataID& data_id) const;
    };
    
  };
};

#endif

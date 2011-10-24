/*
 *  data/Package.h
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

#ifndef AvidaDataPackage_h
#define AvidaDataPackage_h

#include "apto/core/String.h"
#include "apto/core/StringUtils.h"
#include "apto/platform.h"
#include "avida/data/Types.h"


namespace Avida {
  namespace Data {
    
    // Data::Package - base class for data provider value packages
    // --------------------------------------------------------------------------------------------------------------
    
    class Package
    {
    public:
      LIB_EXPORT virtual ~Package() { ; }
      
      LIB_EXPORT virtual bool BoolValue() const = 0;
      LIB_EXPORT virtual int IntValue() const = 0;
      LIB_EXPORT virtual double DoubleValue() const = 0;
      LIB_EXPORT virtual Apto::String StringValue() const = 0;

      LIB_EXPORT virtual bool IsAggregate() const;
      LIB_EXPORT virtual Apto::String GetAggregateDescriptor() const;
      LIB_EXPORT virtual PackagePtr GetComponent(int index);
      LIB_EXPORT virtual ConstPackagePtr GetComponent(int index) const;
    };
    
    
    // Data::Wrap - basic type wrappers
    // --------------------------------------------------------------------------------------------------------------
    
    template <class T> class Wrap : public Package
    {
    private:
      T m_value;
      
    public:
      Wrap(T value) : m_value(value) { ; }
      
      bool BoolValue() const { return m_value; }
      int IntValue() const { return m_value; }
      double DoubleValue() const { return m_value; }
      Apto::String StringValue() const { return Apto::String(Apto::AsStr(m_value)); }
    };
    
    template <> class Wrap<Apto::String> : public Package
    {
    private:
      Apto::String m_value;
      
    public:
      Wrap(Apto::String value) : m_value(value) { ; }
      
      bool BoolValue() const { return Apto::StrAs(m_value); }
      int IntValue() const { return Apto::StrAs(m_value); }
      double DoubleValue() const { return Apto::StrAs(m_value); }
      Apto::String StringValue() const { return m_value; }
    };
        
    
    // Data::ArrayPackage - basic array aggregate
    // --------------------------------------------------------------------------------------------------------------
    
    class ArrayPackage : public Package
    {
    private:
      Apto::Array<PackagePtr> m_entries;
      
    public:
      inline ArrayPackage() { ; }
      
      bool BoolValue() const;
      int IntValue() const;
      double DoubleValue() const;
      Apto::String StringValue() const;
      
      bool IsAggregate() const;
      Apto::String GetAggregateDescriptor() const;
      PackagePtr GetComponent(int index);
      ConstPackagePtr GetComponent(int index) const;
      
      inline void AddComponent(PackagePtr comp) { m_entries.Push(comp); }
    };
  };
};

#endif

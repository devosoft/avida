/*
 *  data/cPackage.h
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


namespace Avida {
  namespace Data {
    
    class cPackage
    {
    public:
      LIB_EXPORT virtual ~cPackage() { ; }
      
//      LIB_EXPORT virtual bool IsAggregate() const = 0;
//      LIB_EXPORT virtual Apto::String GetAggregateDescriptor() const = 0;
      
      LIB_EXPORT virtual bool BoolValue() const = 0;
      LIB_EXPORT virtual int IntValue() const = 0;
      LIB_EXPORT virtual double DoubleValue() const = 0;
      LIB_EXPORT virtual Apto::String StringValue() const = 0;
    };
    
    
    template <class T> class tPackage : public cPackage
    {
    private:
      T m_value;
      
    public:
      tPackage(T value) : m_value(value) { ; }
      
      bool BoolValue() const { return m_value; }
      int IntValue() const { return m_value; }
      double DoubleValue() const { return m_value; }
      Apto::String StringValue() const { return Apto::String(Apto::AsStr(m_value)); }
    };
    
    template <> class tPackage<Apto::String> : public cPackage
    {
    private:
      Apto::String m_value;
      
    public:
      tPackage(Apto::String value) : m_value(value) { ; }
      
      bool BoolValue() const { return Apto::StrAs(m_value); }
      int IntValue() const { return Apto::StrAs(m_value); }
      double DoubleValue() const { return Apto::StrAs(m_value); }
      Apto::String StringValue() const { return m_value; }
    };
  };
};

#endif

/*
 *  data/TimeSeriesRecorder.cc
 *  avida-core
 *
 *  Created by David on 5/20/11.
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

#include "avida/data/TimeSeriesRecorder.h"

#include "avida/data/Package.h"


namespace Avida {
  namespace Data {
    
    template <>
    void TimeSeriesRecorder<PackagePtr>::NotifyData(Update, DataRetrievalFunctor retrieve_data)
    {
      m_data.Push(retrieve_data(m_data_id));
    }
    
    
    template <>
    void TimeSeriesRecorder<bool>::NotifyData(Update, DataRetrievalFunctor retrieve_data)
    {
      m_data.Push(retrieve_data(m_data_id)->BoolValue());
    }
    
    template <>
    void TimeSeriesRecorder<int>::NotifyData(Update, DataRetrievalFunctor retrieve_data)
    {
      m_data.Push(retrieve_data(m_data_id)->IntValue());
    }
    
    template <>
    void TimeSeriesRecorder<double>::NotifyData(Update, DataRetrievalFunctor retrieve_data)
    {
      m_data.Push(retrieve_data(m_data_id)->DoubleValue());
    }

    template <>
    void TimeSeriesRecorder<Apto::String>::NotifyData(Update, DataRetrievalFunctor retrieve_data)
    {
      m_data.Push(retrieve_data(m_data_id)->StringValue());
    }
    
  };
};


// Explicitly instantiate classes
template class Avida::Data::TimeSeriesRecorder<Avida::Data::PackagePtr>;
template class Avida::Data::TimeSeriesRecorder<bool>;
template class Avida::Data::TimeSeriesRecorder<int>;
template class Avida::Data::TimeSeriesRecorder<double>;
template class Avida::Data::TimeSeriesRecorder<Apto::String>;

// Explicitly instantiate member functions   @DMB disabled, c++0x extension warning under Apple LLVM 3.0
//template void Avida::Data::TimeSeriesRecorder<Avida::Data::PackagePtr>::NotifyData(Update, DataRetrievalFunctor);
//template void Avida::Data::TimeSeriesRecorder<bool>::NotifyData(Update, DataRetrievalFunctor);
//template void Avida::Data::TimeSeriesRecorder<int>::NotifyData(Update, DataRetrievalFunctor);
//template void Avida::Data::TimeSeriesRecorder<double>::NotifyData(Update, DataRetrievalFunctor);
//template void Avida::Data::TimeSeriesRecorder<Apto::String>::NotifyData(Update, DataRetrievalFunctor);

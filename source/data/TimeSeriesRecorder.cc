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
    TimeSeriesRecorder<PackagePtr>::TimeSeriesRecorder(const DataID& data_id) : m_data_id(data_id)
    {
      DataSetPtr ds(new DataSet);
      ds->Insert(m_data_id);
      m_requested = ds;
    }

    template <>
    TimeSeriesRecorder<bool>::TimeSeriesRecorder(const DataID& data_id) : m_data_id(data_id)
    {
      DataSetPtr ds(new DataSet);
      ds->Insert(m_data_id);
      m_requested = ds;
    }
    
    template <>
    TimeSeriesRecorder<int>::TimeSeriesRecorder(const DataID& data_id) : m_data_id(data_id)
    {
      DataSetPtr ds(new DataSet);
      ds->Insert(m_data_id);
      m_requested = ds;
    }

    template <>
    TimeSeriesRecorder<double>::TimeSeriesRecorder(const DataID& data_id) : m_data_id(data_id)
    {
      DataSetPtr ds(new DataSet);
      ds->Insert(m_data_id);
      m_requested = ds;
    }

    template <>
    TimeSeriesRecorder<Apto::String>::TimeSeriesRecorder(const DataID& data_id) : m_data_id(data_id)
    {
      DataSetPtr ds(new DataSet);
      ds->Insert(m_data_id);
      m_requested = ds;
    }
    
    
    template <>
    TimeSeriesRecorder<PackagePtr>::TimeSeriesRecorder(const DataID& data_id, Apto::String str) : m_data_id(data_id)
    {
      DataSetPtr ds(new DataSet);
      ds->Insert(m_data_id);
      m_requested = ds;
      
      while (str.GetSize()) {
        Apto::String entry_str = str.Pop(',');
        Update update = Apto::StrAs(entry_str.Pop(':'));
        PackagePtr package(new Wrap<Apto::String>(entry_str));
        m_data.Push(DataEntry(update, package));
      }
    }
    
    template <>
    TimeSeriesRecorder<bool>::TimeSeriesRecorder(const DataID& data_id, Apto::String str) : m_data_id(data_id)
    {
      DataSetPtr ds(new DataSet);
      ds->Insert(m_data_id);
      m_requested = ds;

      while (str.GetSize()) {
        Apto::String entry_str = str.Pop(',');
        Update update = Apto::StrAs(entry_str.Pop(':'));
        bool value = Apto::StrAs(entry_str);
        m_data.Push(DataEntry(update, value));
      }
    }
    
    template <>
    TimeSeriesRecorder<int>::TimeSeriesRecorder(const DataID& data_id, Apto::String str) : m_data_id(data_id)
    {
      DataSetPtr ds(new DataSet);
      ds->Insert(m_data_id);
      m_requested = ds;
      
      while (str.GetSize()) {
        Apto::String entry_str = str.Pop(',');
        Update update = Apto::StrAs(entry_str.Pop(':'));
        int value = Apto::StrAs(entry_str);
        m_data.Push(DataEntry(update, value));
      }
    }
    
    template <>
    TimeSeriesRecorder<double>::TimeSeriesRecorder(const DataID& data_id, Apto::String str) : m_data_id(data_id)
    {
      DataSetPtr ds(new DataSet);
      ds->Insert(m_data_id);
      m_requested = ds;
      
      while (str.GetSize()) {
        Apto::String entry_str = str.Pop(',');
        Update update = Apto::StrAs(entry_str.Pop(':'));
        double value = Apto::StrAs(entry_str);
        m_data.Push(DataEntry(update, value));
      }
    }
    
    template <>
    TimeSeriesRecorder<Apto::String>::TimeSeriesRecorder(const DataID& data_id, Apto::String str) : m_data_id(data_id)
    {
      DataSetPtr ds(new DataSet);
      ds->Insert(m_data_id);
      m_requested = ds;
      
      while (str.GetSize()) {
        Apto::String entry_str = str.Pop(',');
        Update update = Apto::StrAs(entry_str.Pop(':'));
        m_data.Push(DataEntry(update, entry_str));
      }
    }
    

    
    template <>
    void TimeSeriesRecorder<PackagePtr>::NotifyData(Update update, DataRetrievalFunctor retrieve_data)
    {
      if (shouldRecordValue(update)) {
        m_data.Push(DataEntry(update, retrieve_data(m_data_id)));
        didRecordValue();
      }
    }
    
    
    template <>
    void TimeSeriesRecorder<bool>::NotifyData(Update update, DataRetrievalFunctor retrieve_data)
    {
      if (shouldRecordValue(update)) {
        m_data.Push(DataEntry(update, retrieve_data(m_data_id)->BoolValue()));
        didRecordValue();
      }
    }
    
    template <>
    void TimeSeriesRecorder<int>::NotifyData(Update update, DataRetrievalFunctor retrieve_data)
    {
      if (shouldRecordValue(update)) {
        m_data.Push(DataEntry(update, retrieve_data(m_data_id)->IntValue()));
        didRecordValue();
      }
    }
    
    template <>
    void TimeSeriesRecorder<double>::NotifyData(Update update, DataRetrievalFunctor retrieve_data)
    {
      if (shouldRecordValue(update)) {
        m_data.Push(DataEntry(update, retrieve_data(m_data_id)->DoubleValue()));
        didRecordValue();
      }
    }

    template <>
    void TimeSeriesRecorder<Apto::String>::NotifyData(Update update, DataRetrievalFunctor retrieve_data)
    {
      if (shouldRecordValue(update)) {
        m_data.Push(DataEntry(update, retrieve_data(m_data_id)->StringValue()));
        didRecordValue();
      }
    }
    
    
    template <>
    Apto::String TimeSeriesRecorder<PackagePtr>::AsString() const
    {
      if (m_data.GetSize() == 0) return "";
      
      Apto::String rtn = Apto::FormatStr("%d:%s", m_data[0].update, (const char*)m_data[0].data->StringValue());
      for (int i = 1; i < m_data.GetSize(); i++) {
        rtn += Apto::FormatStr(",%d:%s", m_data[i].update, (const char*)m_data[i].data->StringValue());
      }
      return rtn;
    }

    template <>
    Apto::String TimeSeriesRecorder<bool>::AsString() const
    {
      if (m_data.GetSize() == 0) return "";
      
      Apto::String rtn = Apto::FormatStr("%d:%d", m_data[0].update, m_data[0].data);
      for (int i = 1; i < m_data.GetSize(); i++) {
        rtn += Apto::FormatStr(",%d:%d", m_data[i].update, m_data[i].data);
      }
      return rtn;
    }

    template <>
    Apto::String TimeSeriesRecorder<int>::AsString() const
    {
      if (m_data.GetSize() == 0) return "";
      
      Apto::String rtn = Apto::FormatStr("%d:%d", m_data[0].update, m_data[0].data);
      for (int i = 1; i < m_data.GetSize(); i++) {
        rtn += Apto::FormatStr(",%d:%d", m_data[i].update, m_data[i].data);
      }
      return rtn;
    }

    template <>
    Apto::String TimeSeriesRecorder<double>::AsString() const
    {
      if (m_data.GetSize() == 0) return "";
      
      Apto::String rtn = Apto::FormatStr("%d:%f", m_data[0].update, m_data[0].data);
      for (int i = 1; i < m_data.GetSize(); i++) {
        rtn += Apto::FormatStr(",%d:%f", m_data[i].update, m_data[i].data);
      }
      return rtn;
    }
  
    template <>
    Apto::String TimeSeriesRecorder<Apto::String>::AsString() const
    {
      if (m_data.GetSize() == 0) return "";
      
      Apto::String rtn = Apto::FormatStr("%d:%s", m_data[0].update, (const char*)m_data[0].data);
      for (int i = 1; i < m_data.GetSize(); i++) {
        rtn += Apto::FormatStr(",%d:%s", m_data[i].update, (const char*)m_data[i].data);
      }
      return rtn;
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

/*
 *  data/TimeSeriesRecorder.h
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

#ifndef AvidaDataTimeSeriesRecorder_h
#define AvidaDataTimeSeriesRecorder_h

#include "apto/core/Array.h"
#include "avida/core/Types.h"
#include "avida/data/Recorder.h"


namespace Avida {
  namespace Data {
    
    // Data::TimeSeriesRecorder
    // --------------------------------------------------------------------------------------------------------------
    
    template <class T> class TimeSeriesRecorder : public Recorder
    {
    private:
      DataID m_data_id;
      ConstDataSetPtr m_requested;
      
      struct DataEntry;
      Apto::Array<DataEntry, Apto::Smart> m_data;
      
    public:
      LIB_EXPORT TimeSeriesRecorder(const DataID& data_id);
      LIB_EXPORT TimeSeriesRecorder(const DataID& data_id, Apto::String str);
      
      // Data::Recorder Interface
      LIB_EXPORT inline ConstDataSetPtr RequestedData() const { return m_requested; }
      LIB_EXPORT void NotifyData(Update current_update, DataRetrievalFunctor retrieve_data);
      
      // Value Access
      LIB_EXPORT inline const DataID& RecordedDataID() const { return m_data_id; }
      
      LIB_EXPORT inline int NumPoints() const { return m_data.GetSize(); }
      LIB_EXPORT inline T DataPoint(int idx) const { return m_data[idx].data; }
      LIB_EXPORT inline Update DataTime(int idx) const { return m_data[idx].update; }
      
      LIB_EXPORT Apto::String AsString() const;
      
    protected:
      LIB_EXPORT virtual bool shouldRecordValue(Update update) = 0;
      LIB_EXPORT virtual void didRecordValue() { ; }
      
      
    private:
      struct DataEntry
      {
        T data;
        Update update;
        
        LIB_LOCAL inline DataEntry() : update(-1) { ; }
        LIB_LOCAL inline DataEntry(Update in_update, const T& in_data) : data(in_data), update(in_update) { ; }
      };
    };
    
  };
};

#endif

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
      Apto::Array<T, Apto::Smart> m_data;
      
    public:
      LIB_EXPORT TimeSeriesRecorder(const DataID& data_id);
      
      // Data::Recorder Interface
      LIB_EXPORT ConstDataSetPtr GetRequested() const { return m_requested; }
      LIB_EXPORT void NotifyData(Update current_update, DataRetrievalFunctor retrieve_data);
      
      // Value Access
      LIB_EXPORT inline int GetNumPoints() const { return m_data.GetSize(); }
      LIB_EXPORT inline T GetDataPoint(int idx) const { return m_data[idx]; }
      LIB_EXPORT inline const Apto::Array<T, Apto::Smart>& GetData() const { return m_data; }
    };
    
  };
};

#endif

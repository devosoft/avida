/*
 *  viewer/GraphicsContext.h
 *  Avida
 *
 *  Created by David on 3/21/12.
 *  Copyright 2012 Michigan State University. All rights reserved.
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

#ifndef AvidaViewerGraphicsContext_h
#define AvidaViewerGraphicsContext_h

#include "avida/viewer/Types.h"


namespace Avida {
  namespace Viewer {

    // GroupData
    // --------------------------------------------------------------------------------------------------------------
    
    class GraphicsContextData
    {
    public:
      LIB_EXPORT virtual ~GraphicsContextData() = 0;
    };
    
    
    // GraphicsContext
    // --------------------------------------------------------------------------------------------------------------  
    
    class GraphicsContext
    {
    private:
      Apto::Map<Apto::String, Apto::SmartPtr<GraphicsContextData> > m_data;

    public:
      LIB_EXPORT inline GraphicsContext() { ; }
      LIB_EXPORT ~GraphicsContext();
  
      
      // Graphics Context Data
      LIB_EXPORT bool AttachDataWithID(Apto::SmartPtr<GraphicsContextData> obj, Apto::String data_id)
      {
        assert(obj);
        if (m_data.Get(data_id)) return false;
        m_data.Set(data_id, obj);
        return true;
      }
      
      template <typename T> LIB_EXPORT Apto::SmartPtr<T> GetDataForID(Apto::String data_id)
      {
        Apto::SmartPtr<T> rtn;
        rtn.DynamicCastFrom(m_data.Get(data_id));
        return rtn;
      }
    };
    
  };
};

#endif

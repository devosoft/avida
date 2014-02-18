/*
 *  environment/NumericIO.h
 *  avida-core
 *
 *  Created by David on 2/13/14.
 *  Copyright 2014 Michigan State University. All rights reserved.
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

#ifndef AvidaEnvironmentNumericIO_h
#define AvidaEnvironmentNumericIO_h

#include "avida/hardware/Feature.h"


namespace Avida {
  namespace Environment {
    
    // Environment::NumericIO - holds numeric inputs and outputs that may be used to perform environment actions
    // --------------------------------------------------------------------------------------------------------------
    
    class NumericIO : public Hardware::Feature
    {
    private:
      Apto::Array<int> m_input_buffer;
      Apto::Array<int> m_output_buffer;
      int m_input_pointer;
      int m_output_offset;
      
      
    public:
      
      void AddInput(const T& in_value)
      {
        data[offset] = in_value;
        total++;
        offset++;
        offset %= data.GetSize();
      }
      
      LIB_EXPORT inline void PopInput()
      {
        m_buffer_offset--;
        if (m_buffer_offset < 0) m_buffer_offset += m_input_buffer.GetSize();
      }
      
      LIB_EXPORT inline int InputAt(Apto::SizeType idx) const
      {
        int index = m_input_pointer - idx - 1;
        if (index < 0)  index += m_input_buffer.GetSize();
        assert(index >= 0 && index < m_input_buffer.GetSize());
        return m_input_buffer[index];
      }

      LIB_EXPORT inline int OutputAt(Apto::SizeType idx) const
      {
        int index = m_output_buffer_offset - idx - 1;
        if (index < 0)  index += m_output_buffer.GetSize();
        assert(index >= 0 && index < m_output_buffer.GetSize());
        return m_output_buffer[index];
      }
    };

  };
};

#endif

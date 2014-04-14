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
      Apto::Array<int> m_env_inputs;
      int m_env_input_pointer;

      Apto::Array<int> m_input_buffer;
      int m_input_offset;
      int m_input_total;

      Apto::Array<int> m_output_buffer;
      int m_output_offset;
      int m_output_total;
      
      bool m_logic_valid;
      int m_logic_id;
      
      
    public:
      NumericIO(Hardware::Base* hw) : Hardware::Feature(hw) { ; }
      
      
      
      // ---------- Input Buffer ----------
      LIB_EXPORT inline int NextInput()
      {
        // Increment the input pointer
        m_env_input_pointer++;

        // if we reach the end of the buffer, wrap to index 0
        if (m_env_input_pointer == m_env_inputs.GetSize()) m_env_input_pointer = 0;

        int value = m_env_inputs[m_env_input_pointer];
        
        // Store in the input buffer
        m_input_buffer[m_input_offset] = value;
        m_input_total++;
        m_input_offset++;

        // if we reach the end of the buffer, wrap to index 0
        if (m_input_offset == m_input_buffer.GetSize()) m_input_offset = 0;

        return value;
      }
      
      LIB_EXPORT inline Apto::Array<int>& InputBuffer() { return m_input_buffer; }
      
      
      LIB_EXPORT inline int InputAt(Apto::SizeType idx) const
      {
        int index = m_input_offset - idx - 1;
        if (index < 0)  index += m_input_buffer.GetSize();
        assert(index >= 0 && index < m_input_buffer.GetSize());
        return m_input_buffer[index];
      }
      
      LIB_EXPORT inline int InputCapacity() const { return m_input_buffer.GetSize(); }
      LIB_EXPORT inline int InputTotal() const { return m_input_total; }
      LIB_EXPORT inline int InputsStored() const
      {
        return (m_input_total <= m_input_buffer.GetSize()) ? m_input_total : m_input_buffer.GetSize();
      }
      
      LIB_EXPORT inline void ClearInputBuffer() { m_input_offset = 0; m_input_total = 0; }

      
      // ---------- Output Buffer ----------
      LIB_EXPORT inline void AddOutput(int value)
      {
        m_output_buffer[m_output_offset] = value;
        m_output_total++;
        m_output_offset++;
        
        // if we reach the end of the buffer, wrap to index 0
        if (m_output_offset == m_output_buffer.GetSize()) m_output_offset = 0;
        
        // Outputs should invalidate calculation of logic actions
        m_logic_valid = false;
      }
      
      LIB_EXPORT inline int OutputAt(Apto::SizeType idx) const
      {
        int index = m_output_offset - idx - 1;
        if (index < 0)  index += m_output_buffer.GetSize();
        assert(index >= 0 && index < m_output_buffer.GetSize());
        return m_output_buffer[index];
      }
      
      LIB_EXPORT inline int OutputCapacity() const { return m_output_buffer.GetSize(); }
      LIB_EXPORT inline int OutputTotal() const { return m_output_total; }
      LIB_EXPORT inline int OutputsStored() const
      {
        return (m_output_total <= m_output_buffer.GetSize()) ? m_output_total : m_output_buffer.GetSize();
      }

      LIB_EXPORT inline void ClearOutputBuffer() { m_output_offset = 0; m_output_total = 0; }

      
      // ---------- Logic Action Handling ----------
      LIB_EXPORT inline int LogicActionID() { if (!m_logic_valid) setupLogicID(); return m_logic_id; }
      

    private:
      void setupLogicID();
    };

  };
};

#endif

/*
 *  cTaskContext.h
 *  Avida
 *
 *  Created by David on 3/29/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cTaskContext_h
#define cTaskContext_h

#ifndef cOrgInterface_h
#include "cOrgInterface.h"
#endif
#ifndef tBuffer_h
#include "tBuffer.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif
#ifndef tHashMap_h
#include "tHashMap.h"
#endif
#ifndef tSmartArray_h
#include "tSmartArray.h"
#endif

class cTaskEntry;
class cTaskState;


class cTaskContext
{
private:
  cOrganism* m_organism;
  const tBuffer<int>& m_input_buffer;
  const tBuffer<int>& m_output_buffer;
  const tList<tBuffer<int> >& m_other_input_buffers;
  const tList<tBuffer<int> >& m_other_output_buffers;
  const tSmartArray<int>& m_ext_mem;
  tBuffer<int>* m_received_messages;
  int m_logic_id;
  bool m_on_divide;

  // for optimize tasks actual value of function org is outputting, for all others nothing
  // implemented for now...
  double m_task_value;	

  cTaskEntry* m_task_entry;
  tHashMap<void*, cTaskState*>* m_task_states;
  
  
public:
  cTaskContext(cOrganism* organism, const tBuffer<int>& inputs, const tBuffer<int>& outputs,
               const tList<tBuffer<int> >& other_inputs, const tList<tBuffer<int> >& other_outputs,
               const tSmartArray<int>& ext_mem, bool in_on_divide = false,
               tBuffer<int>* in_received_messages = NULL)
    : m_organism(organism)
    , m_input_buffer(inputs)
    , m_output_buffer(outputs)
    , m_other_input_buffers(other_inputs)
    , m_other_output_buffers(other_outputs)
    , m_ext_mem(ext_mem)
    , m_received_messages(in_received_messages)
    , m_logic_id(0)
    , m_on_divide(in_on_divide)
    , m_task_entry(NULL)
    , m_task_states(NULL)
  {
	  m_task_value = 0;
  }
  
  inline cOrganism* GetOrganism() { return m_organism; }
  inline const tBuffer<int>& GetInputBuffer() { return m_input_buffer; }
  inline const tBuffer<int>& GetOutputBuffer() { return m_output_buffer; }
  inline const tList<tBuffer<int> >& GetNeighborhoodInputBuffers() { return m_other_input_buffers; }
  inline const tList<tBuffer<int> >& GetNeighborhoodOutputBuffers() { return m_other_output_buffers; }
  inline const tSmartArray<int>& GetExtendedMemory() const { return m_ext_mem; }
  inline tBuffer<int>* GetReceivedMessages() { return m_received_messages; }
  inline int GetLogicId() const { return m_logic_id; }
  inline void SetLogicId(int v) { m_logic_id = v; }
  inline bool GetOnDivide() const { return m_on_divide; }
  inline void SetTaskValue(double v) { m_task_value = v; }
  inline double GetTaskValue() { return m_task_value; }
  
  inline void SetTaskEntry(cTaskEntry* in_entry) { m_task_entry = in_entry; }
  inline cTaskEntry* GetTaskEntry() { return m_task_entry; }
    
  inline void SetTaskStates(tHashMap<void*, cTaskState*>* states) { m_task_states = states; }
  
  inline cTaskState* GetTaskState()
  {
    cTaskState* ret = NULL;
    m_task_states->Find(m_task_entry, ret);
    return ret;
  }
  inline void AddTaskState(cTaskState* value) { m_task_states->Set(m_task_entry, value); }
};

#endif

/*
 *  cTaskContext.h
 *  Avida
 *
 *  Created by David on 3/29/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
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

class cTaskEntry;


class cTaskContext
{
private:
  cOrgInterface* m_interface;
  const tBuffer<int>& input_buffer;
  const tBuffer<int>& output_buffer;
  const tList<tBuffer<int> >& other_input_buffers;
  const tList<tBuffer<int> >& other_output_buffers;
  bool net_valid;
  int net_completed;
  tBuffer<int>* received_messages;
  int logic_id;
  bool on_divide;
  
  cTaskEntry* task_entry;

public:
  cTaskContext(cOrgInterface* interface, const tBuffer<int>& inputs, const tBuffer<int>& outputs,
               const tList<tBuffer<int> >& other_inputs, const tList<tBuffer<int> >& other_outputs,
               bool in_net_valid, int in_net_completed, bool in_on_divide = false,
               tBuffer<int>* in_received_messages = NULL)
    : m_interface(interface)
    , input_buffer(inputs)
    , output_buffer(outputs)
    , other_input_buffers(other_inputs)
    , other_output_buffers(other_outputs)
    , net_valid(in_net_valid)
    , net_completed(in_net_completed)
    , received_messages(in_received_messages)
    , logic_id(0)
    , on_divide(in_on_divide)
    , task_entry(NULL)
  {
  }
  
  inline int GetInputAt(int index) { return m_interface->GetInputAt(index); }
  inline const tBuffer<int>& GetInputBuffer() { return input_buffer; }
  inline const tBuffer<int>& GetOutputBuffer() { return output_buffer; }
  inline const tList<tBuffer<int> >& GetNeighborhoodInputBuffers() { return other_input_buffers; }
  inline const tList<tBuffer<int> >& GetNeighborhoodOutputBuffers() { return other_output_buffers; }
  inline bool NetIsValid() { return net_valid; }
  inline int GetNetCompleted() { return net_completed; }
  inline tBuffer<int>* GetReceivedMessages() { return received_messages; }
  inline int GetLogicId() { return logic_id; }
  inline void SetLogicId(int v) { logic_id = v; }
  inline bool GetOnDivide() { return on_divide; }
  
  void SetTaskEntry(cTaskEntry* in_entry) { task_entry = in_entry; }
  cTaskEntry* GetTaskEntry() { return task_entry; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nTaskContext {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

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

#ifndef tBuffer_h
#include "tBuffer.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif


class cTaskContext
{
  friend class cTaskLib;
private:
  const tBuffer<int>& input_buffer;
  const tBuffer<int>& output_buffer;
  const tList<tBuffer<int> >& other_input_buffers;
  const tList<tBuffer<int> >& other_output_buffers;
  bool net_valid;
  int net_completed;
  int logic_id;

public:
  cTaskContext(const tBuffer<int>& inputs, const tBuffer<int>& outputs, const tList<tBuffer<int> >& other_inputs,
               const tList<tBuffer<int> >& other_outputs, bool in_net_valid, int in_net_completed)
    : input_buffer(inputs), output_buffer(outputs), other_input_buffers(other_inputs),
    other_output_buffers(other_outputs), net_valid(in_net_valid), net_completed(in_net_completed), logic_id(0)
  {
  }


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif

/*
 *  cTaskLib.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cTaskLib_h
#define cTaskLib_h

#ifndef cTaskEntry_h
#include "cTaskEntry.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tBuffer_h
#include "tBuffer.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif


class cString;

class cTaskContext
{
public:
  tBuffer<int> input_buffer;
  tBuffer<int> output_buffer;
  tList<tBuffer<int> > other_input_buffers;
  tList<tBuffer<int> > other_output_buffers;
  int logic_id;
  
  cTaskContext(const tBuffer<int>& inputs, const tBuffer<int>& outputs,
               const tList<tBuffer<int> >& other_inputs,
               const tList<tBuffer<int> >& other_outputs, int in_logic_id)
  : input_buffer(3), output_buffer(3), logic_id(in_logic_id)
  {
    input_buffer = inputs;
    output_buffer = outputs;
    other_input_buffers.Copy(other_inputs);
    other_output_buffers.Copy(other_outputs);
  }
};


class cTaskLib
{
private:
  tArray<cTaskEntry*> task_array;

  // What extra information should be sent along when we are evaluating
  // which tasks have been performed?
  bool use_neighbor_input;
  bool use_neighbor_output;

  // Active task information...

  enum req_list
  {
    REQ_NEIGHBOR_INPUT=1,
    REQ_NEIGHBOR_OUTPUT=2, 
    UNUSED_REQ_C=4,
    UNUSED_REQ_D=8
  };
  

  cTaskLib(const cTaskLib &); // @not_implemented
  cTaskLib& operator=(const cTaskLib&); // @not_implemented

public:
  cTaskLib();
  ~cTaskLib();

  int GetSize() const { return task_array.GetSize(); }

  cTaskEntry* AddTask(const cString& name);
  const cTaskEntry& GetTask(int id) const;
  
  cTaskContext* SetupTests(const tBuffer<int>& inputs, const tBuffer<int>& outputs,
                          const tList<tBuffer<int> >& other_inputs,
                          const tList<tBuffer<int> >& other_outputs) const
  {
    return new cTaskContext(inputs, outputs, other_inputs, other_outputs, SetupLogicTests(inputs, outputs));
  }
  inline double TestOutput(const cTaskEntry& task, cTaskContext* ctx) const;

  bool UseNeighborInput() const { return use_neighbor_input; }
  bool UseNeighborOutput() const { return use_neighbor_output; }
  
private:  // Direct task related methods
  void NewTask(const cString& name, const cString& desc, tTaskTest task_fun, int reqs = 0);
  int SetupLogicTests(const tBuffer<int>& inputs, const tBuffer<int>& outputs) const;

  inline double FractionalReward(unsigned int supplied, unsigned int correct);  

  double Task_Echo(cTaskContext* ctx) const;
  double Task_Add(cTaskContext* ctx) const;
  double Task_Sub(cTaskContext* ctx) const;

  // 1- and 2-Input Logic Tasks
  double Task_Not(cTaskContext* ctx) const;
  double Task_Nand(cTaskContext* ctx) const;
  double Task_And(cTaskContext* ctx) const;
  double Task_OrNot(cTaskContext* ctx) const;
  double Task_Or(cTaskContext* ctx) const;
  double Task_AndNot(cTaskContext* ctx) const;
  double Task_Nor(cTaskContext* ctx) const;
  double Task_Xor(cTaskContext* ctx) const;
  double Task_Equ(cTaskContext* ctx) const;

  // 3-Input Logic Tasks
  double Task_Logic3in_AA(cTaskContext* ctx) const;
  double Task_Logic3in_AB(cTaskContext* ctx) const;
  double Task_Logic3in_AC(cTaskContext* ctx) const;
  double Task_Logic3in_AD(cTaskContext* ctx) const;
  double Task_Logic3in_AE(cTaskContext* ctx) const;
  double Task_Logic3in_AF(cTaskContext* ctx) const;
  double Task_Logic3in_AG(cTaskContext* ctx) const;
  double Task_Logic3in_AH(cTaskContext* ctx) const;
  double Task_Logic3in_AI(cTaskContext* ctx) const;
  double Task_Logic3in_AJ(cTaskContext* ctx) const;
  double Task_Logic3in_AK(cTaskContext* ctx) const;
  double Task_Logic3in_AL(cTaskContext* ctx) const;
  double Task_Logic3in_AM(cTaskContext* ctx) const;
  double Task_Logic3in_AN(cTaskContext* ctx) const;
  double Task_Logic3in_AO(cTaskContext* ctx) const;
  double Task_Logic3in_AP(cTaskContext* ctx) const;
  double Task_Logic3in_AQ(cTaskContext* ctx) const;
  double Task_Logic3in_AR(cTaskContext* ctx) const;
  double Task_Logic3in_AS(cTaskContext* ctx) const;
  double Task_Logic3in_AT(cTaskContext* ctx) const;
  double Task_Logic3in_AU(cTaskContext* ctx) const;
  double Task_Logic3in_AV(cTaskContext* ctx) const;
  double Task_Logic3in_AW(cTaskContext* ctx) const;
  double Task_Logic3in_AX(cTaskContext* ctx) const;
  double Task_Logic3in_AY(cTaskContext* ctx) const;
  double Task_Logic3in_AZ(cTaskContext* ctx) const;
  double Task_Logic3in_BA(cTaskContext* ctx) const;
  double Task_Logic3in_BB(cTaskContext* ctx) const;
  double Task_Logic3in_BC(cTaskContext* ctx) const;
  double Task_Logic3in_BD(cTaskContext* ctx) const;
  double Task_Logic3in_BE(cTaskContext* ctx) const;
  double Task_Logic3in_BF(cTaskContext* ctx) const;
  double Task_Logic3in_BG(cTaskContext* ctx) const;
  double Task_Logic3in_BH(cTaskContext* ctx) const;
  double Task_Logic3in_BI(cTaskContext* ctx) const;
  double Task_Logic3in_BJ(cTaskContext* ctx) const;
  double Task_Logic3in_BK(cTaskContext* ctx) const;
  double Task_Logic3in_BL(cTaskContext* ctx) const;
  double Task_Logic3in_BM(cTaskContext* ctx) const;
  double Task_Logic3in_BN(cTaskContext* ctx) const;
  double Task_Logic3in_BO(cTaskContext* ctx) const;
  double Task_Logic3in_BP(cTaskContext* ctx) const;
  double Task_Logic3in_BQ(cTaskContext* ctx) const;
  double Task_Logic3in_BR(cTaskContext* ctx) const;
  double Task_Logic3in_BS(cTaskContext* ctx) const;
  double Task_Logic3in_BT(cTaskContext* ctx) const;
  double Task_Logic3in_BU(cTaskContext* ctx) const;
  double Task_Logic3in_BV(cTaskContext* ctx) const;
  double Task_Logic3in_BW(cTaskContext* ctx) const;
  double Task_Logic3in_BX(cTaskContext* ctx) const;
  double Task_Logic3in_BY(cTaskContext* ctx) const;
  double Task_Logic3in_BZ(cTaskContext* ctx) const;
  double Task_Logic3in_CA(cTaskContext* ctx) const;
  double Task_Logic3in_CB(cTaskContext* ctx) const;
  double Task_Logic3in_CC(cTaskContext* ctx) const;
  double Task_Logic3in_CD(cTaskContext* ctx) const;
  double Task_Logic3in_CE(cTaskContext* ctx) const;
  double Task_Logic3in_CF(cTaskContext* ctx) const;
  double Task_Logic3in_CG(cTaskContext* ctx) const;
  double Task_Logic3in_CH(cTaskContext* ctx) const;
  double Task_Logic3in_CI(cTaskContext* ctx) const;
  double Task_Logic3in_CJ(cTaskContext* ctx) const;
  double Task_Logic3in_CK(cTaskContext* ctx) const;
  double Task_Logic3in_CL(cTaskContext* ctx) const;
  double Task_Logic3in_CM(cTaskContext* ctx) const;
  double Task_Logic3in_CN(cTaskContext* ctx) const;
  double Task_Logic3in_CO(cTaskContext* ctx) const;
  double Task_Logic3in_CP(cTaskContext* ctx) const;

  // 1-Input math tasks...
  double Task_Math1in_AA(cTaskContext* ctx) const;
  double Task_Math1in_AB(cTaskContext* ctx) const;
  double Task_Math1in_AC(cTaskContext* ctx) const;
  double Task_Math1in_AD(cTaskContext* ctx) const;
  double Task_Math1in_AE(cTaskContext* ctx) const;
  double Task_Math1in_AF(cTaskContext* ctx) const;
  double Task_Math1in_AG(cTaskContext* ctx) const;
  double Task_Math1in_AH(cTaskContext* ctx) const;
  double Task_Math1in_AI(cTaskContext* ctx) const;
  double Task_Math1in_AJ(cTaskContext* ctx) const;
  double Task_Math1in_AK(cTaskContext* ctx) const;
  double Task_Math1in_AL(cTaskContext* ctx) const;
  double Task_Math1in_AM(cTaskContext* ctx) const;
  double Task_Math1in_AN(cTaskContext* ctx) const;
  double Task_Math1in_AO(cTaskContext* ctx) const;
  double Task_Math1in_AP(cTaskContext* ctx) const;

  // 2-Input math tasks...
  double Task_Math2in_AA(cTaskContext* ctx) const;
  double Task_Math2in_AB(cTaskContext* ctx) const;
  double Task_Math2in_AC(cTaskContext* ctx) const;
  double Task_Math2in_AD(cTaskContext* ctx) const;
  double Task_Math2in_AE(cTaskContext* ctx) const;
  double Task_Math2in_AF(cTaskContext* ctx) const;
  double Task_Math2in_AG(cTaskContext* ctx) const;
  double Task_Math2in_AH(cTaskContext* ctx) const;
  double Task_Math2in_AI(cTaskContext* ctx) const;
  double Task_Math2in_AJ(cTaskContext* ctx) const;
  double Task_Math2in_AK(cTaskContext* ctx) const;
  double Task_Math2in_AL(cTaskContext* ctx) const;
  double Task_Math2in_AM(cTaskContext* ctx) const;
  double Task_Math2in_AN(cTaskContext* ctx) const;
  double Task_Math2in_AO(cTaskContext* ctx) const;
  double Task_Math2in_AP(cTaskContext* ctx) const;
  double Task_Math2in_AQ(cTaskContext* ctx) const;
  double Task_Math2in_AR(cTaskContext* ctx) const;
  double Task_Math2in_AS(cTaskContext* ctx) const;
  double Task_Math2in_AT(cTaskContext* ctx) const;
  double Task_Math2in_AU(cTaskContext* ctx) const;
  double Task_Math2in_AV(cTaskContext* ctx) const;

  double Task_Math3in_AA(cTaskContext* ctx) const;
  double Task_Math3in_AB(cTaskContext* ctx) const;
  double Task_Math3in_AC(cTaskContext* ctx) const;
  double Task_Math3in_AD(cTaskContext* ctx) const;
  double Task_Math3in_AE(cTaskContext* ctx) const;
  double Task_Math3in_AF(cTaskContext* ctx) const;
  double Task_Math3in_AG(cTaskContext* ctx) const;
  double Task_Math3in_AH(cTaskContext* ctx) const;
  double Task_Math3in_AI(cTaskContext* ctx) const;
  double Task_Math3in_AJ(cTaskContext* ctx) const;
  double Task_Math3in_AK(cTaskContext* ctx) const;
  double Task_Math3in_AL(cTaskContext* ctx) const;
  double Task_Math3in_AM(cTaskContext* ctx) const;
  
  // Communication Tasks...
  double Task_CommEcho(cTaskContext* ctx) const;
  double Task_CommNot(cTaskContext* ctx) const;
};


inline double cTaskLib::TestOutput(const cTaskEntry& task, cTaskContext* ctx) const
{
  tTaskTest test_fun = task.GetTestFun();
  return (this->*test_fun)(ctx);
}

#endif

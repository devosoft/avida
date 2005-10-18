//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef TASK_LIB_HH
#define TASK_LIB_HH

#ifndef TARRAY_HH
#include "tArray.h"
#endif
#ifndef TBUFFER_HH
#include "tBuffer.h"
#endif
#ifndef TLIST_HH
#include "tList.h"
#endif

class cTaskLib;
typedef double (cTaskLib::*tTaskTest)() const;

class cString;
class cTaskEntry;
class cTaskLib {
private:
  tArray<cTaskEntry *> task_array;

  // What extra information should be sent along when we are evaluating
  // which tasks have been performed?
  bool use_neighbor_input;
  bool use_neighbor_output;

  // Active task information...
  mutable tBuffer<int> input_buffer;
  mutable tBuffer<int> output_buffer;
  mutable tList<tBuffer<int> > other_input_buffers;
  mutable tList<tBuffer<int> > other_output_buffers;
  mutable int logic_id;

  enum req_list { REQ_NEIGHBOR_INPUT=1,
		  REQ_NEIGHBOR_OUTPUT=2, 
		  UNUSED_REQ_C=4,
		  UNUSED_REQ_D=8 };
private:
  // disabled copy constructor.
  cTaskLib(const cTaskLib &);
public:
  cTaskLib();
  ~cTaskLib();

  int GetSize() const { return task_array.GetSize(); }

  cTaskEntry * AddTask(const cString & name);
  const cTaskEntry & GetTask(int id) const;
  
  void SetupTests(const tBuffer<int> & inputs,
		  const tBuffer<int> & outputs,
		  const tList<tBuffer<int> > & other_inputs,
		  const tList<tBuffer<int> > & other_outputs) const;
  double TestOutput(const cTaskEntry & task) const;

  bool UseNeighborInput() const { return use_neighbor_input; }
  bool UseNeighborOutput() const { return use_neighbor_output; }

private:  // Direct task related methods
  void NewTask(const cString & name, const cString & desc,
	       tTaskTest task_fun, int reqs=0);
  void SetupLogicTests(const tBuffer<int> & inputs,
		       const tBuffer<int> & outputs) const;

  double Task_Echo() const;
  double Task_Add()  const;
  double Task_Sub()  const;

  // 1- and 2-Input Logic Tasks
  double Task_Not()    const;
  double Task_Nand()   const;
  double Task_And()    const;
  double Task_OrNot()  const;
  double Task_Or()     const;
  double Task_AndNot() const;
  double Task_Nor()    const;
  double Task_Xor()    const;
  double Task_Equ()    const;

  // 3-Input Logic Tasks
  double Task_Logic3in_AA() const;
  double Task_Logic3in_AB() const;
  double Task_Logic3in_AC() const;
  double Task_Logic3in_AD() const;
  double Task_Logic3in_AE() const;
  double Task_Logic3in_AF() const;
  double Task_Logic3in_AG() const;
  double Task_Logic3in_AH() const;
  double Task_Logic3in_AI() const;
  double Task_Logic3in_AJ() const;
  double Task_Logic3in_AK() const;
  double Task_Logic3in_AL() const;
  double Task_Logic3in_AM() const;
  double Task_Logic3in_AN() const;
  double Task_Logic3in_AO() const;
  double Task_Logic3in_AP() const;
  double Task_Logic3in_AQ() const;
  double Task_Logic3in_AR() const;
  double Task_Logic3in_AS() const;
  double Task_Logic3in_AT() const;
  double Task_Logic3in_AU() const;
  double Task_Logic3in_AV() const;
  double Task_Logic3in_AW() const;
  double Task_Logic3in_AX() const;
  double Task_Logic3in_AY() const;
  double Task_Logic3in_AZ() const;
  double Task_Logic3in_BA() const;
  double Task_Logic3in_BB() const;
  double Task_Logic3in_BC() const;
  double Task_Logic3in_BD() const;
  double Task_Logic3in_BE() const;
  double Task_Logic3in_BF() const;
  double Task_Logic3in_BG() const;
  double Task_Logic3in_BH() const;
  double Task_Logic3in_BI() const;
  double Task_Logic3in_BJ() const;
  double Task_Logic3in_BK() const;
  double Task_Logic3in_BL() const;
  double Task_Logic3in_BM() const;
  double Task_Logic3in_BN() const;
  double Task_Logic3in_BO() const;
  double Task_Logic3in_BP() const;
  double Task_Logic3in_BQ() const;
  double Task_Logic3in_BR() const;
  double Task_Logic3in_BS() const;
  double Task_Logic3in_BT() const;
  double Task_Logic3in_BU() const;
  double Task_Logic3in_BV() const;
  double Task_Logic3in_BW() const;
  double Task_Logic3in_BX() const;
  double Task_Logic3in_BY() const;
  double Task_Logic3in_BZ() const;
  double Task_Logic3in_CA() const;
  double Task_Logic3in_CB() const;
  double Task_Logic3in_CC() const;
  double Task_Logic3in_CD() const;
  double Task_Logic3in_CE() const;
  double Task_Logic3in_CF() const;
  double Task_Logic3in_CG() const;
  double Task_Logic3in_CH() const;
  double Task_Logic3in_CI() const;
  double Task_Logic3in_CJ() const;
  double Task_Logic3in_CK() const;
  double Task_Logic3in_CL() const;
  double Task_Logic3in_CM() const;
  double Task_Logic3in_CN() const;
  double Task_Logic3in_CO() const;
  double Task_Logic3in_CP() const;

  // 1-Input math tasks...
  double Task_Math1in_AA() const;
  double Task_Math1in_AB() const;
  double Task_Math1in_AC() const;
  double Task_Math1in_AD() const;
  double Task_Math1in_AE() const;
  double Task_Math1in_AF() const;
  double Task_Math1in_AG() const;
  double Task_Math1in_AH() const;
  double Task_Math1in_AI() const;
  double Task_Math1in_AJ() const;
  double Task_Math1in_AK() const;
  double Task_Math1in_AL() const;
  double Task_Math1in_AM() const;
  double Task_Math1in_AN() const;
  double Task_Math1in_AO() const;
  double Task_Math1in_AP() const;

  // 2-Input math tasks...
  double Task_Math2in_AA() const;
  double Task_Math2in_AB() const;
  double Task_Math2in_AC() const;
  double Task_Math2in_AD() const;
  double Task_Math2in_AE() const;
  double Task_Math2in_AF() const;
  double Task_Math2in_AG() const;
  double Task_Math2in_AH() const;
  double Task_Math2in_AI() const;
  double Task_Math2in_AJ() const;
  double Task_Math2in_AK() const;
  double Task_Math2in_AL() const;
  double Task_Math2in_AM() const;
  double Task_Math2in_AN() const;
  double Task_Math2in_AO() const;
  double Task_Math2in_AP() const;
  double Task_Math2in_AQ() const;
  double Task_Math2in_AR() const;
  double Task_Math2in_AS() const;
  double Task_Math2in_AT() const;
  double Task_Math2in_AU() const;
  double Task_Math2in_AV() const;

  double Task_Math3in_AA() const;
  double Task_Math3in_AB() const;
  double Task_Math3in_AC() const;
  double Task_Math3in_AD() const;
  double Task_Math3in_AE() const;
  double Task_Math3in_AF() const;
  double Task_Math3in_AG() const;
  double Task_Math3in_AH() const;
  double Task_Math3in_AI() const;
  double Task_Math3in_AJ() const;
  double Task_Math3in_AK() const;
  double Task_Math3in_AL() const;
  double Task_Math3in_AM() const;
  
  // Communication Tasks...
  double Task_CommEcho() const;
  double Task_CommNot() const;
};

#endif

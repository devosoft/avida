/*
 *  cTaskLib.cc
 *  Avida
 *
 *  Called "task_lib.cc" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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
 */

#include "cTaskLib.h"

#include "apto/platform.h"
#include "avida/core/Feedback.h"

#include "cEnvironment.h"
#include "cEnvReqs.h"
#include "cTaskState.h"
#include "cPopulation.h"
#include "cPopulationCell.h"

#include <cstdlib>
#include <cmath>
#include <climits>
#include <iomanip>

// Various workarounds for Visual Studio shortcomings
#if APTO_PLATFORM(WINDOWS)
# define llabs(x) _abs64(x)
# define log2(x) (log(x)/log(2.0))
#endif

// Various workarounds for FreeBSD
#if APTO_PLATFORM(FREEBSD)
# define log2(x) (log(x)/log(2.0))
#endif

static const double dCastPrecision = 100000.0;


cTaskLib::~cTaskLib()
{
  for (int i = 0; i < task_array.GetSize(); i++) delete task_array[i];
}

inline double cTaskLib::FractionalReward(unsigned int supplied, unsigned int correct)
{
  const unsigned int variance = supplied ^ correct;
  const unsigned int w = variance - ((variance >> 1) & 0x55555555);
  const unsigned int x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
  const unsigned int bit_diff = ((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
  return static_cast<double>(32 - bit_diff) / 32.0; 
}

cTaskEntry* cTaskLib::AddTask(const cString& name, const cString& info, cEnvReqs& envreqs, Feedback& feedback)
{
  // Determine if this task is already in the active library.
  for (int i = 0; i < task_array.GetSize(); i++) {
    assert(task_array[i] != NULL);
    if (task_array[i]->GetName() == name && !task_array[i]->HasArguments()) return task_array[i];
  }
  
  // Match up this name to its corresponding task
  const int start_size = task_array.GetSize();
  
  // The following if blocks are grouped based on class of task.  Chaining too
  // many if block causes problems block nesting depth in Visual Studio.net 2003.
  
  if (name == "echo")      NewTask(name, "Echo", &cTaskLib::Task_Echo);
  else if (name == "add")  NewTask(name, "Add",  &cTaskLib::Task_Add);
  else if (name == "sub")  NewTask(name, "Sub",  &cTaskLib::Task_Sub);
  else if (name == "dontcare")  NewTask(name, "DontCare", &cTaskLib::Task_DontCare);
  
  // All 1- and 2-Input Logic Functions
  if (name == "not") NewTask(name, "Not", &cTaskLib::Task_Not);
  else if (name == "nand") NewTask(name, "Nand", &cTaskLib::Task_Nand);
  else if (name == "and") NewTask(name, "And", &cTaskLib::Task_And);
  else if (name == "orn") NewTask(name, "OrNot", &cTaskLib::Task_OrNot);
  else if (name == "or") NewTask(name, "Or", &cTaskLib::Task_Or);
  else if (name == "andn") NewTask(name, "AndNot", &cTaskLib::Task_AndNot);
  else if (name == "nor") NewTask(name, "Nor", &cTaskLib::Task_Nor);
  else if (name == "xor") NewTask(name, "Xor", &cTaskLib::Task_Xor);
  else if (name == "equ") NewTask(name, "Equals", &cTaskLib::Task_Equ);
  
  else if (name == "xor-max") NewTask(name, "Xor-max", &cTaskLib::Task_XorMax);
	
  // All 3-Input Logic Functions
  if (name == "logic_3AA")      NewTask(name, "Logic 3AA (A+B+C == 0)", &cTaskLib::Task_Logic3in_AA);
  else if (name == "logic_3AB") NewTask(name, "Logic 3AB (A+B+C == 1)", &cTaskLib::Task_Logic3in_AB);
  else if (name == "logic_3AC") NewTask(name, "Logic 3AC (A+B+C <= 1)", &cTaskLib::Task_Logic3in_AC);
  else if (name == "logic_3AD") NewTask(name, "Logic 3AD (A+B+C == 2)", &cTaskLib::Task_Logic3in_AD);
  else if (name == "logic_3AE") NewTask(name, "Logic 3AE (A+B+C == 0,2)", &cTaskLib::Task_Logic3in_AE);
  else if (name == "logic_3AF") NewTask(name, "Logic 3AF (A+B+C == 1,2)", &cTaskLib::Task_Logic3in_AF);
  else if (name == "logic_3AG") NewTask(name, "Logic 3AG (A+B+C <= 2)", &cTaskLib::Task_Logic3in_AG);
  else if (name == "logic_3AH") NewTask(name, "Logic 3AH (A+B+C == 3)", &cTaskLib::Task_Logic3in_AH);
  else if (name == "logic_3AI") NewTask(name, "Logic 3AI (A+B+C == 0,3)", &cTaskLib::Task_Logic3in_AI);
  else if (name == "logic_3AJ") NewTask(name, "Logic 3AJ (A+B+C == 1,3) XOR", &cTaskLib::Task_Logic3in_AJ);
  else if (name == "logic_3AK") NewTask(name, "Logic 3AK (A+B+C != 2)", &cTaskLib::Task_Logic3in_AK);
  else if (name == "logic_3AL") NewTask(name, "Logic 3AL (A+B+C >= 2)", &cTaskLib::Task_Logic3in_AL);
  else if (name == "logic_3AM") NewTask(name, "Logic 3AM (A+B+C != 1)", &cTaskLib::Task_Logic3in_AM);
  else if (name == "logic_3AN") NewTask(name, "Logic 3AN (A+B+C != 0)", &cTaskLib::Task_Logic3in_AN);
  else if (name == "logic_3AO") NewTask(name, "Logic 3AO (A & ~B & ~C) [3]", &cTaskLib::Task_Logic3in_AO);
  else if (name == "logic_3AP") NewTask(name, "Logic 3AP (A^B & ~C)  [3]", &cTaskLib::Task_Logic3in_AP);
  else if (name == "logic_3AQ") NewTask(name, "Logic 3AQ (A==B & ~C) [3]", &cTaskLib::Task_Logic3in_AQ);
  else if (name == "logic_3AR") NewTask(name, "Logic 3AR (A & B & ~C) [3]", &cTaskLib::Task_Logic3in_AR);
  else if (name == "logic_3AS") NewTask(name, "Logic 3AS", &cTaskLib::Task_Logic3in_AS);
  else if (name == "logic_3AT") NewTask(name, "Logic 3AT", &cTaskLib::Task_Logic3in_AT);
  else if (name == "logic_3AU") NewTask(name, "Logic 3AU", &cTaskLib::Task_Logic3in_AU);
  else if (name == "logic_3AV") NewTask(name, "Logic 3AV", &cTaskLib::Task_Logic3in_AV);
  else if (name == "logic_3AW") NewTask(name, "Logic 3AW", &cTaskLib::Task_Logic3in_AW);
  else if (name == "logic_3AX") NewTask(name, "Logic 3AX", &cTaskLib::Task_Logic3in_AX);
  else if (name == "logic_3AY") NewTask(name, "Logic 3AY", &cTaskLib::Task_Logic3in_AY);
  else if (name == "logic_3AZ") NewTask(name, "Logic 3AZ", &cTaskLib::Task_Logic3in_AZ);
  else if (name == "logic_3BA") NewTask(name, "Logic 3BA", &cTaskLib::Task_Logic3in_BA);
  else if (name == "logic_3BB") NewTask(name, "Logic 3BB", &cTaskLib::Task_Logic3in_BB);
  else if (name == "logic_3BC") NewTask(name, "Logic 3BC", &cTaskLib::Task_Logic3in_BC);
  else if (name == "logic_3BD") NewTask(name, "Logic 3BD", &cTaskLib::Task_Logic3in_BD);
  else if (name == "logic_3BE") NewTask(name, "Logic 3BE", &cTaskLib::Task_Logic3in_BE);
  else if (name == "logic_3BF") NewTask(name, "Logic 3BF", &cTaskLib::Task_Logic3in_BF);
  else if (name == "logic_3BG") NewTask(name, "Logic 3BG", &cTaskLib::Task_Logic3in_BG);
  else if (name == "logic_3BH") NewTask(name, "Logic 3BH", &cTaskLib::Task_Logic3in_BH);
  else if (name == "logic_3BI") NewTask(name, "Logic 3BI", &cTaskLib::Task_Logic3in_BI);
  else if (name == "logic_3BJ") NewTask(name, "Logic 3BJ", &cTaskLib::Task_Logic3in_BJ);
  else if (name == "logic_3BK") NewTask(name, "Logic 3BK", &cTaskLib::Task_Logic3in_BK);
  else if (name == "logic_3BL") NewTask(name, "Logic 3BL", &cTaskLib::Task_Logic3in_BL);
  else if (name == "logic_3BM") NewTask(name, "Logic 3BM", &cTaskLib::Task_Logic3in_BM);
  else if (name == "logic_3BN") NewTask(name, "Logic 3BN", &cTaskLib::Task_Logic3in_BN);
  else if (name == "logic_3BO") NewTask(name, "Logic 3BO", &cTaskLib::Task_Logic3in_BO);
  else if (name == "logic_3BP") NewTask(name, "Logic 3BP", &cTaskLib::Task_Logic3in_BP);
  else if (name == "logic_3BQ") NewTask(name, "Logic 3BQ", &cTaskLib::Task_Logic3in_BQ);
  else if (name == "logic_3BR") NewTask(name, "Logic 3BR", &cTaskLib::Task_Logic3in_BR);
  else if (name == "logic_3BS") NewTask(name, "Logic 3BS", &cTaskLib::Task_Logic3in_BS);
  else if (name == "logic_3BT") NewTask(name, "Logic 3BT", &cTaskLib::Task_Logic3in_BT);
  else if (name == "logic_3BU") NewTask(name, "Logic 3BU", &cTaskLib::Task_Logic3in_BU);
  else if (name == "logic_3BV") NewTask(name, "Logic 3BV", &cTaskLib::Task_Logic3in_BV);
  else if (name == "logic_3BW") NewTask(name, "Logic 3BW", &cTaskLib::Task_Logic3in_BW);
  else if (name == "logic_3BX") NewTask(name, "Logic 3BX", &cTaskLib::Task_Logic3in_BX);
  else if (name == "logic_3BY") NewTask(name, "Logic 3BY", &cTaskLib::Task_Logic3in_BY);
  else if (name == "logic_3BZ") NewTask(name, "Logic 3BZ", &cTaskLib::Task_Logic3in_BZ);
  else if (name == "logic_3CA") NewTask(name, "Logic 3CA", &cTaskLib::Task_Logic3in_CA);
  else if (name == "logic_3CB") NewTask(name, "Logic 3CB", &cTaskLib::Task_Logic3in_CB);
  else if (name == "logic_3CC") NewTask(name, "Logic 3CC", &cTaskLib::Task_Logic3in_CC);
  else if (name == "logic_3CD") NewTask(name, "Logic 3CD", &cTaskLib::Task_Logic3in_CD);
  else if (name == "logic_3CE") NewTask(name, "Logic 3CE", &cTaskLib::Task_Logic3in_CE);
  else if (name == "logic_3CF") NewTask(name, "Logic 3CF", &cTaskLib::Task_Logic3in_CF);
  else if (name == "logic_3CG") NewTask(name, "Logic 3CG", &cTaskLib::Task_Logic3in_CG);
  else if (name == "logic_3CH") NewTask(name, "Logic 3CH", &cTaskLib::Task_Logic3in_CH);
  else if (name == "logic_3CI") NewTask(name, "Logic 3CI", &cTaskLib::Task_Logic3in_CI);
  else if (name == "logic_3CJ") NewTask(name, "Logic 3CJ", &cTaskLib::Task_Logic3in_CJ);
  else if (name == "logic_3CK") NewTask(name, "Logic 3CK", &cTaskLib::Task_Logic3in_CK);
  else if (name == "logic_3CL") NewTask(name, "Logic 3CL", &cTaskLib::Task_Logic3in_CL);
  else if (name == "logic_3CM") NewTask(name, "Logic 3CM", &cTaskLib::Task_Logic3in_CM);
  else if (name == "logic_3CN") NewTask(name, "Logic 3CN", &cTaskLib::Task_Logic3in_CN);
  else if (name == "logic_3CO") NewTask(name, "Logic 3CO", &cTaskLib::Task_Logic3in_CO);
  else if (name == "logic_3CP") NewTask(name, "Logic 3CP", &cTaskLib::Task_Logic3in_CP);
  

  // Sequence Tasks
  if (name == "sort_inputs") Load_SortInputs(name, info, envreqs, feedback);
  else if (name == "fibonacci_seq") Load_FibonacciSequence(name, info, envreqs, feedback);
  
  // Feed Specific Tasks
  if (name == "eat-target") Load_ConsumeTarget(name, info, envreqs, feedback);
  else if (name == "eat-target-echo") Load_ConsumeTargetEcho(name, info, envreqs, feedback);
  else if (name == "eat-target-nand") Load_ConsumeTargetNand(name, info, envreqs, feedback);
  else if (name == "eat-target-and") Load_ConsumeTargetAnd(name, info, envreqs, feedback);
  else if (name == "eat-target-orn") Load_ConsumeTargetOrn(name, info, envreqs, feedback);
  else if (name == "eat-target-or") Load_ConsumeTargetOr(name, info, envreqs, feedback);
  else if (name == "eat-target-andn") Load_ConsumeTargetAndn(name, info, envreqs, feedback);
  else if (name == "eat-target-nor") Load_ConsumeTargetNor(name, info, envreqs, feedback);
  else if (name == "eat-target-xor") Load_ConsumeTargetXor(name, info, envreqs, feedback);
  else if (name == "eat-target-equ") Load_ConsumeTargetEqu(name, info, envreqs, feedback);
  
  // Make sure we have actually found a task
  if (task_array.GetSize() == start_size) {
    feedback.Error("unknown/unprocessed task entry '%s'", (const char*)name);
    return NULL;
  }
  
  // And return the found task.
  return task_array[start_size];
}

void cTaskLib::NewTask(const cString& name, const cString& desc, tTaskTest task_fun, int reqs, Util::Args* args)
{
  if (reqs & REQ_NEIGHBOR_INPUT) use_neighbor_input = true;
  if (reqs & REQ_NEIGHBOR_OUTPUT) use_neighbor_output = true;
  
  const int id = task_array.GetSize();
  task_array.Resize(id + 1);
  task_array[id] = new cTaskEntry(name, desc, id, task_fun, args);
}


void cTaskLib::SetupTests(cTaskContext& ctx) const
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  // Collect the inputs in a useful form.
  const int num_inputs = input_buffer.GetNumStored();
  int test_inputs[3];
  for (int i = 0; i < 3; i++) {
    test_inputs[i] = (num_inputs > i) ? input_buffer[i] : 0;
  }

  int test_output = 0;
  if (ctx.GetOutputBuffer().GetNumStored()) test_output = ctx.GetOutputBuffer()[0];
  
  
  // Setup logic_out to test the output for each logical combination...
  // Assuming each bit in logic out to be based on the inputs:
  //
  //  Logic ID Bit: 7 6 5 4 3 2 1 0
  //       Input C: 1 1 1 1 0 0 0 0
  //       Input B: 1 1 0 0 1 1 0 0
  //       Input A: 1 0 1 0 1 0 1 0
  
  int logic_out[8];
  for (int i = 0; i < 8; i++) logic_out[i] = -1;
  
  // Test all input combos!
  bool func_OK = true;  // Have all outputs been consistant?
  for (int test_pos = 0; test_pos < 32; test_pos++) {
    int logic_pos = 0;
    for (int i = 0; i < 3; i++)  logic_pos += (test_inputs[i] & 1) << i;
    
    if ( logic_out[logic_pos] != -1 &&
        logic_out[logic_pos] != (test_output & 1) ) {
      func_OK = false;
      break;
    }
    else {
      logic_out[logic_pos] = test_output & 1;
    }
    
    test_output >>= 1;
    for (int i = 0; i < 3; i++) test_inputs[i] >>= 1;
  }
  
  // If there were any inconsistancies, deal with them.
  if (func_OK == false) {
    ctx.SetLogicId(-1);
    return;
  }
  
  // Determine the logic ID number of this task.
  if (num_inputs < 1) {  // 000 -> 001
    logic_out[1] = logic_out[0];
  }
  if (num_inputs < 2) { // 000 -> 010; 001 -> 011
    logic_out[2] = logic_out[0];
    logic_out[3] = logic_out[1];
  }
  if (num_inputs < 3) { // 000->100;  001->101;  010->110;  011->111
    logic_out[4] = logic_out[0];
    logic_out[5] = logic_out[1];
    logic_out[6] = logic_out[2];
    logic_out[7] = logic_out[3];
  }
  
  // Lets just make sure we've gotten this correct...
  assert(logic_out[0] >= 0 && logic_out[0] <= 1);
  assert(logic_out[1] >= 0 && logic_out[1] <= 1);
  assert(logic_out[2] >= 0 && logic_out[2] <= 1);
  assert(logic_out[3] >= 0 && logic_out[3] <= 1);
  assert(logic_out[4] >= 0 && logic_out[4] <= 1);
  assert(logic_out[5] >= 0 && logic_out[5] <= 1);
  assert(logic_out[6] >= 0 && logic_out[6] <= 1);
  assert(logic_out[7] >= 0 && logic_out[7] <= 1);
  
  int logicid = 0;
  for (int i = 0; i < 8; i++) logicid += logic_out[i] << i;
  
  ctx.SetLogicId(logicid);
}


double cTaskLib::Task_Echo(cTaskContext& ctx) const
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  for (int i = 0; i < input_buffer.GetNumStored(); i++) {
    if (input_buffer[i] == test_output) {
      assert(ctx.GetLogicId() == 170 || ctx.GetLogicId() == 204 || ctx.GetLogicId() == 240);
      return 1.0;
    }
  }
  return 0.0;
}


double cTaskLib::Task_Add(cTaskContext& ctx) const
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  for (int i = 0; i < input_buffer.GetNumStored(); i++) {
    for (int j = 0; j < i; j++) {
      if (test_output == input_buffer[i] + input_buffer[j]) return 1.0;
    }
  }
  return 0.0;
}


double cTaskLib::Task_Sub(cTaskContext& ctx) const
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = ctx.GetInputBuffer().GetNumStored();
  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == input_buffer[i] - input_buffer[j]) return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_DontCare(cTaskContext&) const
{
  return 1.0;
}

double cTaskLib::Task_Not(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 15 || logic_id == 51 || logic_id == 85) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Nand(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 63 || logic_id == 95 || logic_id == 119) return 1.0;
  return 0.0;
}

double cTaskLib::Task_And(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 136 || logic_id == 160 || logic_id == 192) return 1.0;
  return 0.0;
}

double cTaskLib::Task_OrNot(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 175 || logic_id == 187 || logic_id == 207 ||
      logic_id == 221 || logic_id == 243 || logic_id == 245) return 1.0;
  
  return 0.0;
}

double cTaskLib::Task_Or(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 238 || logic_id == 250 || logic_id == 252) return 1.0;
  return 0.0;
}

double cTaskLib::Task_AndNot(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 10 || logic_id == 12 || logic_id == 34 ||
      logic_id == 48 || logic_id == 68 || logic_id == 80) return 1.0;
  
  return 0.0;
}

double cTaskLib::Task_Nor(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 3 || logic_id == 5 || logic_id == 17) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Xor(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 60 || logic_id == 90 || logic_id == 102) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Equ(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 153 || logic_id == 165 || logic_id == 195) return 1.0;
  return 0.0;
}


double cTaskLib::Task_Logic3in_AA(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 1) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AB(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 22) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AC(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 23) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AD(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 104) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AE(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 105) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AF(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 126) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AG(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 127) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AH(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 128) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AI(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 129) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AJ(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 150) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AK(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 151) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AL(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 232) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AM(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 233) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AN(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 254) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AO(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 2 || logic_id == 4 || logic_id == 16) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AP(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 6 || logic_id == 18 || logic_id == 20) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AQ(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 7 || logic_id == 19 || logic_id == 21) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AR(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 8 || logic_id == 32 || logic_id == 64) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AS(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 9 || logic_id == 33 || logic_id == 65) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AT(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 14 || logic_id == 50 || logic_id == 84) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AU(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 24 || logic_id == 36 || logic_id == 66) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AV(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 25 || logic_id == 37 || logic_id == 67) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AW(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 30 || logic_id == 54 || logic_id == 86) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AX(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 31 || logic_id == 55 || logic_id == 87) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AY(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 40 || logic_id == 72 || logic_id == 96) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_AZ(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 41 || logic_id == 73 || logic_id == 97) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BA(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 42 || logic_id == 76 || logic_id == 112) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BB(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 43 || logic_id == 77 || logic_id == 113) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BC(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 61 || logic_id == 91 || logic_id == 103) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BD(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 62 || logic_id == 94 || logic_id == 118) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BE(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 106 || logic_id == 108 || logic_id == 120) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BF(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 107 || logic_id == 109 || logic_id == 121) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BG(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 110 || logic_id == 122 || logic_id == 124) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BH(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 111 || logic_id == 123 || logic_id == 125) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BI(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 130 || logic_id == 132 || logic_id == 144) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BJ(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 131 || logic_id == 133 || logic_id == 145) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BK(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 134 || logic_id == 146 || logic_id == 148) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BL(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 135 || logic_id == 147 || logic_id == 149) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BM(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 137 || logic_id == 161 || logic_id == 193) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BN(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 142 || logic_id == 178 || logic_id == 212) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BO(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 143 || logic_id == 179 || logic_id == 213) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BP(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 152 || logic_id == 164 || logic_id == 194) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BQ(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 158 || logic_id == 182 || logic_id == 214) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BR(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 159 || logic_id == 183 || logic_id == 215) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BS(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 168 || logic_id == 200 || logic_id == 224) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BT(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 169 || logic_id == 201 || logic_id == 225) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BU(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 171 || logic_id == 205 || logic_id == 241) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BV(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 188 || logic_id == 218 || logic_id == 230) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BW(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 189 || logic_id == 219 || logic_id == 231) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BX(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 190 || logic_id == 222 || logic_id == 246) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BY(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 191 || logic_id == 223 || logic_id == 247) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_BZ(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 234 || logic_id == 236 || logic_id == 248) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CA(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 235 || logic_id == 237 || logic_id == 249) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CB(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 239 || logic_id == 251 || logic_id == 253) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CC(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 11 || logic_id == 13 || logic_id == 35 ||
      logic_id == 49 || logic_id == 69 || logic_id == 81) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CD(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 26 || logic_id == 28 || logic_id == 38 ||
      logic_id == 52 || logic_id == 70 || logic_id == 82) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CE(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 27 || logic_id == 29 || logic_id == 39 ||
      logic_id == 53 || logic_id == 71 || logic_id == 83) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CF(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 44 || logic_id == 56 || logic_id == 74 ||
      logic_id == 88 || logic_id == 98 || logic_id == 100) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CG(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 45 || logic_id == 57 || logic_id == 75 ||
      logic_id == 89 || logic_id == 99 || logic_id == 101) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CH(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 46 || logic_id == 58 || logic_id == 78 ||
      logic_id == 92 || logic_id == 114 || logic_id == 116) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CI(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 47 || logic_id == 59 || logic_id == 79 ||
      logic_id == 93 || logic_id == 115 || logic_id == 117) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CJ(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 138 || logic_id == 140 || logic_id == 162 ||
      logic_id == 176 || logic_id == 196 || logic_id == 208) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CK(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 139 || logic_id == 141 || logic_id == 163 ||
      logic_id == 177 || logic_id == 197 || logic_id == 209) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CL(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 154 || logic_id == 156 || logic_id == 166 ||
      logic_id == 180 || logic_id == 198 || logic_id == 210) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CM(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 155 || logic_id == 157 || logic_id == 167 ||
      logic_id == 181 || logic_id == 199 || logic_id == 211) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CN(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 172 || logic_id == 184 || logic_id == 202 ||
      logic_id == 216 || logic_id == 226 || logic_id == 228) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CO(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 173 || logic_id == 185 || logic_id == 203 ||
      logic_id == 217 || logic_id == 227 || logic_id == 229) return 1.0;
  return 0.0;
}

double cTaskLib::Task_Logic3in_CP(cTaskContext& ctx) const
{
  const int logic_id = ctx.GetLogicId();
  if (logic_id == 174 || logic_id == 186 || logic_id == 206 ||
      logic_id == 220 || logic_id == 242 || logic_id == 244) return 1.0;
  return 0.0;
}



void cTaskLib::Load_MatchNumber(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  // Integer Arguments
  schema.Define("target", Util::INT);
  schema.Define("threshold", -1);
  // Double Arguments
  schema.Define("halflife", Util::DOUBLE);
  
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) NewTask(name, "Match Number", &cTaskLib::Task_MatchNumber, 0, args);
}

double cTaskLib::Task_MatchNumber(cTaskContext& ctx) const
{
  double quality = 0.0;
  const Util::Args& args = ctx.GetTaskEntry()->GetArguments();
  
  long long diff = ::llabs((long long)args.Int(0) - ctx.GetOutputBuffer()[0]);
  int threshold = args.Int(1);
  
  if (threshold < 0 || diff <= threshold) { // Negative threshold == infinite
    // If within threshold range, quality decays based on absolute difference
    double halflife = -1.0 * fabs(args.Double(0));
    quality = pow(2.0, static_cast<double>(diff) / halflife);
  }
  
  return quality;
}


void cTaskLib::Load_SortInputs(const cString& name, const cString& argstr, cEnvReqs& envreqs, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  // Integer Arguments
  schema.Define("size", Util::INT); // Number of items to sort
  schema.Define("direction", 0); // < 0 = Descending, Otherwise = Ascending
  schema.Define("contiguous", 1); // 0 = No, Otherwise = Yes
  // Double Arguments
  schema.Define("halflife", Util::DOUBLE);
  
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) {
    envreqs.SetMinInputs(args->Int(0));
    envreqs.SetMinOutputs(args->Int(0) * 2);
    envreqs.SetTrueRandInputs();
    NewTask(name, "Sort Inputs", &cTaskLib::Task_SortInputs, 0, args);
  }
}

double cTaskLib::Task_SortInputs(cTaskContext& ctx) const
{
  const Util::Args& args = ctx.GetTaskEntry()->GetArguments();
  const tBuffer<int>& output = ctx.GetOutputBuffer();
  const int size = args.Int(0);
  const int stored = output.GetNumStored();
  
  // if less than half, can't possibly reach threshold
  if (stored <= (size / 2)) return 0.0;
  
  Apto::Map<int, int> valmap;
  int score = 0;
  int maxscore = 0;
  
  // add all valid inputs into the value map
  for (int i = 0; i < size; i++) valmap.Set(ctx.GetOrganism()->GetInputAt(i), -1);
  
  int span_start = -1;
  int span_end = stored;
  
  if (args.Int(2)) { // Contiguous
    // scan for the largest contiguous span
    // - in the event of a tie, keep the first discovered
    for (int i = 0; i < stored; i++) {
      if (valmap.Has(output[i])) {
        int t_start = i;
        while (++i < stored && valmap.Has(output[i])) ;
        if (span_start == -1 || (i - t_start) > (span_end - span_start)) {
          span_start = t_start;
          span_end = i;
        }
      }
    }
    
    // no span was found
    if (span_start == -1) return 0.0;    
  } else { // Scattered
    // search for first valid entry
    while (++span_start < stored && valmap.Has(output[span_start])) ;
    
    // scanned past the end of the output, nothing to validate
    if (span_start >= stored) return 0.0;
  }
  
  // again, if span is less than half the size can't possibly reach threshold
  if ((span_end - span_start) <= (size / 2)) return 0.0;
  
  // insertion sort span
  // - count number of actual entries
  // - count moves required
  // - update valmap, tracking observed inputs
  Apto::Array<int> sorted(size);
  const bool ascending = (args.Int(1) >= 0);
  int count = 1;
  
  // store first value
  valmap.Set(output[span_start], span_start);
  sorted[0] = output[span_start];
  
  // iterate over the remaining span (discovered for contiguous, full output for scattered)
  for (int i = span_start + 1; i < span_end; i++) {      
    int value = output[i];
    
    // check for a dup or invalid output, skip it if so
    int idx;
    if (!valmap.Get(value, idx) || idx != -1) continue;
    
    maxscore += count; // count the maximum moves possible
    count++; // iterate the observed count
    valmap.Set(value,i); // save position, so that missing values can be determined later
    
    // sort value based on ascending for descending, counting moves
    int j = count - 2;
    while (j >= 0 && ((ascending && sorted[j] > value) || (!ascending && sorted[j] < value))) {
      sorted[j + 1] = sorted[j];
      j--;
      score++;
    }
    sorted[j + 1] = value;
  }
  
  // if not all of the inputs were observed
  if (count < size) {
    // iterate over all inputs
    for (int i = 0; i < size; i++) {
      int idx;
      // if input was not observed
      if (valmap.Get(ctx.GetOrganism()->GetInputAt(i), idx) && idx == -1) {
        maxscore += count; // add to the maximum move count
        score += count; // missing values, scored as maximally out of order
        count++; // increment observed count
      }
    }
  }
  
  double quality = 0.0;
  
  // score of 50% expected with random output
  // - only grant quality when less than 50% maximum moves are required
  if (static_cast<double>(score) / static_cast<double>(maxscore) < 0.5) {
    double halflife = -1.0 * fabs(args.Double(0));
    quality = pow(2.0, static_cast<double>(score) / halflife);
  }
  
  return quality;
}




class cFibSeqState : public cTaskState {
public:
  int seq[2];
  int count;
  
  cFibSeqState() : count(0) { seq[0] = 1; seq[1] = 0; }
};


void cTaskLib::Load_FibonacciSequence(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  // Integer Arguments
  schema.Define("target", Util::INT);
  // Double Arguments
  schema.Define("penalty", 0.0);
  
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  
  if (args) NewTask(name, "Fibonacci Sequence", &cTaskLib::Task_FibonacciSequence, 0, args);
}


double cTaskLib::Task_FibonacciSequence(cTaskContext& ctx) const
{
  const Util::Args& args = ctx.GetTaskEntry()->GetArguments();
  cFibSeqState* state = static_cast<cFibSeqState*>(ctx.GetTaskState());
  if (state == NULL) {
    state = new cFibSeqState();
    ctx.AddTaskState(state);
  }
  
  const int next = state->seq[0] + state->seq[1];
  
  // If output matches next in sequence
  if (ctx.GetOutputBuffer()[0] == next) {
    // Increment count and store next value
    state->count++;
    state->seq[state->count % 2] = next;
    
    // If past target sequence ending point, return the penalty setting
    if (state->count > args.Int(0)) return args.Double(0);
    
    return 1.0;
  }
  
  return 0.0;
}






/* Reward organisms for having found a targeted resource*/
void cTaskLib::Load_ConsumeTarget(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  schema.Define("target_id", 1);
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) NewTask(name, "ConsumeTarget", &cTaskLib::Task_ConsumeTarget, 0, args);

  // Add this target id to the list in the instructions file. 
  m_world->GetEnvironment().AddTargetID(args->Int(0));
}

/* Reward organisms for having found a targeted resource + performing logic*/
void cTaskLib::Load_ConsumeTargetEcho(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  schema.Define("target_id", 1);
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) NewTask(name, "ConsumeTargetEcho", &cTaskLib::Task_ConsumeTargetEcho, 0, args);

  // Add this target id to the list in the instructions file. 
  m_world->GetEnvironment().AddTargetID(args->Int(0));
}

/* Reward organisms for having found a targeted resource + performing logic*/
void cTaskLib::Load_ConsumeTargetNand(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  schema.Define("target_id", 1);
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) NewTask(name, "ConsumeTargetNand", &cTaskLib::Task_ConsumeTargetNand, 0, args);

  // Add this target id to the list in the instructions file. 
  m_world->GetEnvironment().AddTargetID(args->Int(0));
}

/* Reward organisms for having found a targeted resource + performing logic*/
void cTaskLib::Load_ConsumeTargetAnd(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  schema.Define("target_id", 1);
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) NewTask(name, "ConsumeTargetAnd", &cTaskLib::Task_ConsumeTargetAnd, 0, args);

  // Add this target id to the list in the instructions file. 
  m_world->GetEnvironment().AddTargetID(args->Int(0));
}

/* Reward organisms for having found a targeted resource + performing logic*/
void cTaskLib::Load_ConsumeTargetOrn(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  schema.Define("target_id", 1);
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) NewTask(name, "ConsumeTargetOrn", &cTaskLib::Task_ConsumeTargetOrn, 0, args);

  // Add this target id to the list in the instructions file. 
  m_world->GetEnvironment().AddTargetID(args->Int(0));
}

/* Reward organisms for having found a targeted resource + performing logic*/
void cTaskLib::Load_ConsumeTargetOr(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  schema.Define("target_id", 1);
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) NewTask(name, "ConsumeTargetOr", &cTaskLib::Task_ConsumeTargetOr, 0, args);

  // Add this target id to the list in the instructions file. 
  m_world->GetEnvironment().AddTargetID(args->Int(0));
}

/* Reward organisms for having found a targeted resource + performing logic*/
void cTaskLib::Load_ConsumeTargetAndn(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  schema.Define("target_id", 1);
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) NewTask(name, "ConsumeTargetAndn", &cTaskLib::Task_ConsumeTargetAndn, 0, args);

  // Add this target id to the list in the instructions file. 
  m_world->GetEnvironment().AddTargetID(args->Int(0));
}

/* Reward organisms for having found a targeted resource + performing logic*/
void cTaskLib::Load_ConsumeTargetNor(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  schema.Define("target_id", 1);
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) NewTask(name, "ConsumeTargetNor", &cTaskLib::Task_ConsumeTargetNor, 0, args);

  // Add this target id to the list in the instructions file. 
  m_world->GetEnvironment().AddTargetID(args->Int(0));
}

/* Reward organisms for having found a targeted resource + performing logic*/
void cTaskLib::Load_ConsumeTargetXor(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  schema.Define("target_id", 1);
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) NewTask(name, "ConsumeTargetXor", &cTaskLib::Task_ConsumeTargetXor, 0, args);

  // Add this target id to the list in the instructions file. 
  m_world->GetEnvironment().AddTargetID(args->Int(0));
}

/* Reward organisms for having found a targeted resource + performing logic*/
void cTaskLib::Load_ConsumeTargetEqu(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  schema.Define("target_id", 1);
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) NewTask(name, "ConsumeTargetEqu", &cTaskLib::Task_ConsumeTargetEqu, 0, args);

  // Add this target id to the list in the instructions file. 
  m_world->GetEnvironment().AddTargetID(args->Int(0));
}

double cTaskLib::Task_ConsumeTarget(cTaskContext& ctx) const
{
  int des_target = ctx.GetTaskEntry()->GetArguments().Int(0);
  
  double reward = 0.0;
  int target_res = ctx.GetOrganism()->GetForageTarget();
  
  // If the organism is on the right resource...
  if (target_res == des_target) {
    reward = 1;
  }
  return reward;
}

double cTaskLib::Task_ConsumeTargetEcho(cTaskContext& ctx) const
{
  int des_target = ctx.GetTaskEntry()->GetArguments().Int(0);
  
  double reward = 0.0;
  int target_res = ctx.GetOrganism()->GetForageTarget();
  
  // If the organism is on the right resource...
  if (target_res == des_target) {
    reward = Task_Echo(ctx);
  }
  return reward;
}

double cTaskLib::Task_ConsumeTargetNand(cTaskContext& ctx) const
{
  int des_target = ctx.GetTaskEntry()->GetArguments().Int(0);
  
  double reward = 0.0;
  int target_res = ctx.GetOrganism()->GetForageTarget();
  
  // If the organism is on the right resource...
  if (target_res == des_target) {
    reward = Task_Nand(ctx);
  }
  return reward;
}

double cTaskLib::Task_ConsumeTargetAnd(cTaskContext& ctx) const
{
  int des_target = ctx.GetTaskEntry()->GetArguments().Int(0);
  
  double reward = 0.0;
  int target_res = ctx.GetOrganism()->GetForageTarget();
  
  // If the organism is on the right resource...
  if (target_res == des_target) {
    reward = Task_And(ctx);
  }
  return reward;
}

double cTaskLib::Task_ConsumeTargetOrn(cTaskContext& ctx) const
{
  int des_target = ctx.GetTaskEntry()->GetArguments().Int(0);
  
  double reward = 0.0;
  int target_res = ctx.GetOrganism()->GetForageTarget();
  
  // If the organism is on the right resource...
  if (target_res == des_target) {
    reward = Task_OrNot(ctx);
  }
  return reward;
}

double cTaskLib::Task_ConsumeTargetOr(cTaskContext& ctx) const
{
  int des_target = ctx.GetTaskEntry()->GetArguments().Int(0);
  
  double reward = 0.0;
  int target_res = ctx.GetOrganism()->GetForageTarget();
  
  // If the organism is on the right resource...
  if (target_res == des_target) {
    reward = Task_Or(ctx);
  }
  return reward;
}

double cTaskLib::Task_ConsumeTargetAndn(cTaskContext& ctx) const
{
  int des_target = ctx.GetTaskEntry()->GetArguments().Int(0);
  
  double reward = 0.0;
  int target_res = ctx.GetOrganism()->GetForageTarget();
  
  // If the organism is on the right resource...
  if (target_res == des_target) {
    reward = Task_AndNot(ctx);
  }
  return reward;
}

double cTaskLib::Task_ConsumeTargetNor(cTaskContext& ctx) const
{
  int des_target = ctx.GetTaskEntry()->GetArguments().Int(0);
  
  double reward = 0.0;
  int target_res = ctx.GetOrganism()->GetForageTarget();
  
  // If the organism is on the right resource...
  if (target_res == des_target) {
    reward = Task_Nor(ctx);
  }
  return reward;
}

double cTaskLib::Task_ConsumeTargetXor(cTaskContext& ctx) const
{
  int des_target = ctx.GetTaskEntry()->GetArguments().Int(0);
  
  double reward = 0.0;
  int target_res = ctx.GetOrganism()->GetForageTarget();
  
  // If the organism is on the right resource...
  if (target_res == des_target) {
    reward = Task_Xor(ctx);
  }
  return reward;
}

double cTaskLib::Task_ConsumeTargetEqu(cTaskContext& ctx) const
{
  int des_target = ctx.GetTaskEntry()->GetArguments().Int(0);
  
  double reward = 0.0;
  int target_res = ctx.GetOrganism()->GetForageTarget();
  
  // If the organism is on the right resource...
  if (target_res == des_target) {
    reward = Task_Equ(ctx);
  }
  return reward;
}


double cTaskLib::Task_XorMax(cTaskContext& ctx) const
{
  Apto::Array<double> cell_res;
  if (!m_world->GetConfig().USE_AVATARS.Get()) cell_res = ctx.GetOrganism()->GetOrgInterface().GetResources(m_world->GetDefaultContext());
  else if (m_world->GetConfig().USE_AVATARS.Get()) cell_res = ctx.GetOrganism()->GetOrgInterface().GetAVResources(m_world->GetDefaultContext());
  
  double max_amount = 0.0;
  int max_res = 0;
  // if more than one resource is available, set the reaction to use the resource with the most available in this spot (note that, with global resources, the GLOBAL total will evaluated)
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if (cell_res[i] > max_amount) {
      max_amount = cell_res[i];
      max_res = i;
    }
  }    
  cReaction* found_reaction = m_world->GetEnvironment().GetReactionLib().GetReaction(ctx.GetTaskEntry()->GetID());
  if (found_reaction == NULL) return false;
  m_world->GetEnvironment().ChangeResource(found_reaction, m_world->GetEnvironment().GetResourceLib().GetResource(max_res)->GetName());
  return Task_Xor(ctx);
}


	

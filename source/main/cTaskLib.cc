
/*
 *  cTaskLib.cc
 *  Avida
 *
 *  Called "task_lib.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#include "cTaskLib.h"

#include "cArgSchema.h"
#include "cDeme.h"
#include "cEnvironment.h"
#include "cEnvReqs.h"
#include "cTaskState.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cOrgMessagePredicate.h"
#include "cOrgMovementPredicate.h"
#include "cStateGrid.h"
#include "cUserFeedback.h"
#include "tArrayUtils.h"
#include "tHashMap.h"

#include "Platform.h"

#include <cstdlib>
#include <cmath>
#include <climits>
#include <iomanip>

// Various workarounds for Visual Studio shortcomings
#if AVIDA_PLATFORM(WINDOWS)
# define llabs(x) _abs64(x)
# define log2(x) (log(x)/log(2.0))
#endif

// Various workarounds for FreeBSD
#if AVIDA_PLATFORM(FREEBSD)
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

cTaskEntry* cTaskLib::AddTask(const cString& name, const cString& info, cEnvReqs& envreqs, cUserFeedback* feedback)
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
  else if (name == "echo_dup")  NewTask(name, "Echo_dup",  &cTaskLib::Task_Echo);
  else if (name == "add")  NewTask(name, "Add",  &cTaskLib::Task_Add);
  else if (name == "add3")  NewTask(name, "Add3",  &cTaskLib::Task_Add3);  
  else if (name == "sub")  NewTask(name, "Sub",  &cTaskLib::Task_Sub);
  // @WRE DontCare task always succeeds.
  else if (name == "dontcare")  NewTask(name, "DontCare", &cTaskLib::Task_DontCare);
  
  // All 1- and 2-Input Logic Functions
  if (name == "not") NewTask(name, "Not", &cTaskLib::Task_Not);
  else if (name == "not_dup") NewTask(name, "Not_dup", &cTaskLib::Task_Not);
  else if (name == "nand") NewTask(name, "Nand", &cTaskLib::Task_Nand);
  else if (name == "nand_dup") NewTask(name, "Nand_dup", &cTaskLib::Task_Nand);
  else if (name == "and") NewTask(name, "And", &cTaskLib::Task_And);
  else if (name == "and_dup") NewTask(name, "And_dup", &cTaskLib::Task_And);
  else if (name == "orn") NewTask(name, "OrNot", &cTaskLib::Task_OrNot);
  else if (name == "orn_dup") NewTask(name, "OrNot_dup", &cTaskLib::Task_OrNot);
  else if (name == "or") NewTask(name, "Or", &cTaskLib::Task_Or);
  else if (name == "or_dup") NewTask(name, "Or_dup", &cTaskLib::Task_Or);
  else if (name == "andn") NewTask(name, "AndNot", &cTaskLib::Task_AndNot);
  else if (name == "andn_dup") NewTask(name, "AndNot_dup", &cTaskLib::Task_AndNot);
  else if (name == "nor") NewTask(name, "Nor", &cTaskLib::Task_Nor);
  else if (name == "nor_dup") NewTask(name, "Nor_dup", &cTaskLib::Task_Nor);
  else if (name == "xor") NewTask(name, "Xor", &cTaskLib::Task_Xor);
  else if (name == "xor_dup") NewTask(name, "Xor_dup", &cTaskLib::Task_Xor);
  else if (name == "equ") NewTask(name, "Equals", &cTaskLib::Task_Equ);
  else if (name == "equ_dup") NewTask(name, "Equals_dup", &cTaskLib::Task_Equ);
  
	// resoruce dependent version
  else if (name == "nand-resourceDependent") NewTask(name, "Nand-resourceDependent", &cTaskLib::Task_Nand_ResourceDependent);
  else if (name == "nor-resourceDependent") NewTask(name, "Nor-resourceDependent", &cTaskLib::Task_Nor_ResourceDependent);
	
  // All 3-Input Logic Functions
  if (name == "logic_3AA")
    NewTask(name, "Logic 3AA (A+B+C == 0)", &cTaskLib::Task_Logic3in_AA);
  else if (name == "logic_3AB")
    NewTask(name, "Logic 3AB (A+B+C == 1)", &cTaskLib::Task_Logic3in_AB);
  else if (name == "logic_3AC")
    NewTask(name, "Logic 3AC (A+B+C <= 1)", &cTaskLib::Task_Logic3in_AC);
  else if (name == "logic_3AD")
    NewTask(name, "Logic 3AD (A+B+C == 2)", &cTaskLib::Task_Logic3in_AD);
  else if (name == "logic_3AE")
    NewTask(name, "Logic 3AE (A+B+C == 0,2)", &cTaskLib::Task_Logic3in_AE);
  else if (name == "logic_3AF")
    NewTask(name, "Logic 3AF (A+B+C == 1,2)", &cTaskLib::Task_Logic3in_AF);
  else if (name == "logic_3AG")
    NewTask(name, "Logic 3AG (A+B+C <= 2)", &cTaskLib::Task_Logic3in_AG);
  else if (name == "logic_3AH")
    NewTask(name, "Logic 3AH (A+B+C == 3)", &cTaskLib::Task_Logic3in_AH);
  else if (name == "logic_3AI")
    NewTask(name, "Logic 3AI (A+B+C == 0,3)", &cTaskLib::Task_Logic3in_AI);
  else if (name == "logic_3AJ")
    NewTask(name, "Logic 3AJ (A+B+C == 1,3) XOR", &cTaskLib::Task_Logic3in_AJ);
  else if (name == "logic_3AK")
    NewTask(name, "Logic 3AK (A+B+C != 2)", &cTaskLib::Task_Logic3in_AK);
  else if (name == "logic_3AL")
    NewTask(name, "Logic 3AL (A+B+C >= 2)", &cTaskLib::Task_Logic3in_AL);
  else if (name == "logic_3AM")
    NewTask(name, "Logic 3AM (A+B+C != 1)", &cTaskLib::Task_Logic3in_AM);
  else if (name == "logic_3AN")
    NewTask(name, "Logic 3AN (A+B+C != 0)", &cTaskLib::Task_Logic3in_AN);
  else if (name == "logic_3AO")
    NewTask(name, "Logic 3AO (A & ~B & ~C) [3]", &cTaskLib::Task_Logic3in_AO);
  else if (name == "logic_3AP")
    NewTask(name, "Logic 3AP (A^B & ~C)  [3]", &cTaskLib::Task_Logic3in_AP);
  else if (name == "logic_3AQ")
    NewTask(name, "Logic 3AQ (A==B & ~C) [3]", &cTaskLib::Task_Logic3in_AQ);
  else if (name == "logic_3AR")
    NewTask(name, "Logic 3AR (A & B & ~C) [3]", &cTaskLib::Task_Logic3in_AR);
  else if (name == "logic_3AS")
    NewTask(name, "Logic 3AS", &cTaskLib::Task_Logic3in_AS);
  else if (name == "logic_3AT")
    NewTask(name, "Logic 3AT", &cTaskLib::Task_Logic3in_AT);
  else if (name == "logic_3AU")
    NewTask(name, "Logic 3AU", &cTaskLib::Task_Logic3in_AU);
  else if (name == "logic_3AV")
    NewTask(name, "Logic 3AV", &cTaskLib::Task_Logic3in_AV);
  else if (name == "logic_3AW")
    NewTask(name, "Logic 3AW", &cTaskLib::Task_Logic3in_AW);
  else if (name == "logic_3AX")
    NewTask(name, "Logic 3AX", &cTaskLib::Task_Logic3in_AX);
  else if (name == "logic_3AY")
    NewTask(name, "Logic 3AY", &cTaskLib::Task_Logic3in_AY);
  else if (name == "logic_3AZ")
    NewTask(name, "Logic 3AZ", &cTaskLib::Task_Logic3in_AZ);
  else if (name == "logic_3BA")
    NewTask(name, "Logic 3BA", &cTaskLib::Task_Logic3in_BA);
  else if (name == "logic_3BB")
    NewTask(name, "Logic 3BB", &cTaskLib::Task_Logic3in_BB);
  else if (name == "logic_3BC")
    NewTask(name, "Logic 3BC", &cTaskLib::Task_Logic3in_BC);
  else if (name == "logic_3BD")
    NewTask(name, "Logic 3BD", &cTaskLib::Task_Logic3in_BD);
  else if (name == "logic_3BE")
    NewTask(name, "Logic 3BE", &cTaskLib::Task_Logic3in_BE);
  else if (name == "logic_3BF")
    NewTask(name, "Logic 3BF", &cTaskLib::Task_Logic3in_BF);
  else if (name == "logic_3BG")
    NewTask(name, "Logic 3BG", &cTaskLib::Task_Logic3in_BG);
  else if (name == "logic_3BH")
    NewTask(name, "Logic 3BH", &cTaskLib::Task_Logic3in_BH);
  else if (name == "logic_3BI")
    NewTask(name, "Logic 3BI", &cTaskLib::Task_Logic3in_BI);
  else if (name == "logic_3BJ")
    NewTask(name, "Logic 3BJ", &cTaskLib::Task_Logic3in_BJ);
  else if (name == "logic_3BK")
    NewTask(name, "Logic 3BK", &cTaskLib::Task_Logic3in_BK);
  else if (name == "logic_3BL")
    NewTask(name, "Logic 3BL", &cTaskLib::Task_Logic3in_BL);
  else if (name == "logic_3BM")
    NewTask(name, "Logic 3BM", &cTaskLib::Task_Logic3in_BM);
  else if (name == "logic_3BN")
    NewTask(name, "Logic 3BN", &cTaskLib::Task_Logic3in_BN);
  else if (name == "logic_3BO")
    NewTask(name, "Logic 3BO", &cTaskLib::Task_Logic3in_BO);
  else if (name == "logic_3BP")
    NewTask(name, "Logic 3BP", &cTaskLib::Task_Logic3in_BP);
  else if (name == "logic_3BQ")
    NewTask(name, "Logic 3BQ", &cTaskLib::Task_Logic3in_BQ);
  else if (name == "logic_3BR")
    NewTask(name, "Logic 3BR", &cTaskLib::Task_Logic3in_BR);
  else if (name == "logic_3BS")
    NewTask(name, "Logic 3BS", &cTaskLib::Task_Logic3in_BS);
  else if (name == "logic_3BT")
    NewTask(name, "Logic 3BT", &cTaskLib::Task_Logic3in_BT);
  else if (name == "logic_3BU")
    NewTask(name, "Logic 3BU", &cTaskLib::Task_Logic3in_BU);
  else if (name == "logic_3BV")
    NewTask(name, "Logic 3BV", &cTaskLib::Task_Logic3in_BV);
  else if (name == "logic_3BW")
    NewTask(name, "Logic 3BW", &cTaskLib::Task_Logic3in_BW);
  else if (name == "logic_3BX")
    NewTask(name, "Logic 3BX", &cTaskLib::Task_Logic3in_BX);
  else if (name == "logic_3BY")
    NewTask(name, "Logic 3BY", &cTaskLib::Task_Logic3in_BY);
  else if (name == "logic_3BZ")
    NewTask(name, "Logic 3BZ", &cTaskLib::Task_Logic3in_BZ);
  else if (name == "logic_3CA")
    NewTask(name, "Logic 3CA", &cTaskLib::Task_Logic3in_CA);
  else if (name == "logic_3CB")
    NewTask(name, "Logic 3CB", &cTaskLib::Task_Logic3in_CB);
  else if (name == "logic_3CC")
    NewTask(name, "Logic 3CC", &cTaskLib::Task_Logic3in_CC);
  else if (name == "logic_3CD")
    NewTask(name, "Logic 3CD", &cTaskLib::Task_Logic3in_CD);
  else if (name == "logic_3CE")
    NewTask(name, "Logic 3CE", &cTaskLib::Task_Logic3in_CE);
  else if (name == "logic_3CF")
    NewTask(name, "Logic 3CF", &cTaskLib::Task_Logic3in_CF);
  else if (name == "logic_3CG")
    NewTask(name, "Logic 3CG", &cTaskLib::Task_Logic3in_CG);
  else if (name == "logic_3CH")
    NewTask(name, "Logic 3CH", &cTaskLib::Task_Logic3in_CH);
  else if (name == "logic_3CI")
    NewTask(name, "Logic 3CI", &cTaskLib::Task_Logic3in_CI);
  else if (name == "logic_3CJ")
    NewTask(name, "Logic 3CJ", &cTaskLib::Task_Logic3in_CJ);
  else if (name == "logic_3CK")
    NewTask(name, "Logic 3CK", &cTaskLib::Task_Logic3in_CK);
  else if (name == "logic_3CL")
    NewTask(name, "Logic 3CL", &cTaskLib::Task_Logic3in_CL);
  else if (name == "logic_3CM")
    NewTask(name, "Logic 3CM", &cTaskLib::Task_Logic3in_CM);
  else if (name == "logic_3CN")
    NewTask(name, "Logic 3CN", &cTaskLib::Task_Logic3in_CN);
  else if (name == "logic_3CO")
    NewTask(name, "Logic 3CO", &cTaskLib::Task_Logic3in_CO);
  else if (name == "logic_3CP")
    NewTask(name, "Logic 3CP", &cTaskLib::Task_Logic3in_CP);
  
  // Arbitrary 1-Input Math Tasks
  else if (name == "math_1AA")
    NewTask(name, "Math 1AA (2X)", &cTaskLib::Task_Math1in_AA);
  else if (name == "math_1AB")
    NewTask(name, "Math 1AB (2X/3)", &cTaskLib::Task_Math1in_AB);  
  else if (name == "math_1AC")
    NewTask(name, "Math 1AC (5X/4)", &cTaskLib::Task_Math1in_AC);  
  else if (name == "math_1AD")
    NewTask(name, "Math 1AD (X^2)", &cTaskLib::Task_Math1in_AD);  
  else if (name == "math_1AE")
    NewTask(name, "Math 1AE (X^3)", &cTaskLib::Task_Math1in_AE);  
  else if (name == "math_1AF")
    NewTask(name, "Math 1AF (sqrt(X))", &cTaskLib::Task_Math1in_AF);  
  else if (name == "math_1AG")
    NewTask(name, "Math 1AG (log(X))", &cTaskLib::Task_Math1in_AG);  
  else if (name == "math_1AH")
    NewTask(name, "Math 1AH (X^2+X^3)", &cTaskLib::Task_Math1in_AH);  
  else if (name == "math_1AI")
    NewTask(name, "Math 1AI (X^2+sqrt(X))", &cTaskLib::Task_Math1in_AI);  
  else if (name == "math_1AJ")
    NewTask(name, "Math 1AJ (abs(X))", &cTaskLib::Task_Math1in_AJ);  
  else if (name == "math_1AK")
    NewTask(name, "Math 1AK (X-5)", &cTaskLib::Task_Math1in_AK);  
  else if (name == "math_1AL")
    NewTask(name, "Math 1AL (-X)", &cTaskLib::Task_Math1in_AL);  
  else if (name == "math_1AM")
    NewTask(name, "Math 1AM (5X)", &cTaskLib::Task_Math1in_AM);  
  else if (name == "math_1AN")
    NewTask(name, "Math 1AN (X/4)", &cTaskLib::Task_Math1in_AN);  
  else if (name == "math_1AO")
    NewTask(name, "Math 1AO (X-6)", &cTaskLib::Task_Math1in_AO);  
  else if (name == "math_1AP")
    NewTask(name, "Math 1AP (X-7)", &cTaskLib::Task_Math1in_AP);
  
  // Arbitrary 2-Input Math Tasks
  if (name == "math_2AA")
    NewTask(name, "Math 2AA (sqrt(X+Y))", &cTaskLib::Task_Math2in_AA);  
  else if (name == "math_2AB")
    NewTask(name, "Math 2AB ((X+Y)^2)", &cTaskLib::Task_Math2in_AB);  
  else if (name == "math_2AC")
    NewTask(name, "Math 2AC (X%Y)", &cTaskLib::Task_Math2in_AC);  
  else if (name == "math_2AD")
    NewTask(name, "Math 2AD (3X/2+5Y/4)", &cTaskLib::Task_Math2in_AD);  
  else if (name == "math_2AE")
    NewTask(name, "Math 2AE (abs(X-5)+abs(Y-6))", &cTaskLib::Task_Math2in_AE);  
  else if (name == "math_2AF")
    NewTask(name, "Math 2AF (XY-X/Y)", &cTaskLib::Task_Math2in_AF);  
  else if (name == "math_2AG")
    NewTask(name, "Math 2AG ((X-Y)^2)", &cTaskLib::Task_Math2in_AG);  
  else if (name == "math_2AH")
    NewTask(name, "Math 2AH (X^2+Y^2)", &cTaskLib::Task_Math2in_AH);  
  else if (name == "math_2AI")
    NewTask(name, "Math 2AI (X^2+Y^3)", &cTaskLib::Task_Math2in_AI);
  else if (name == "math_2AJ")
    NewTask(name, "Math 2AJ ((sqrt(X)+Y)/(X-7))", &cTaskLib::Task_Math2in_AJ);
  else if (name == "math_2AK")
    NewTask(name, "Math 2AK (log(|X/Y|))", &cTaskLib::Task_Math2in_AK);
  else if (name == "math_2AL")
    NewTask(name, "Math 2AL (log(|X|)/Y)", &cTaskLib::Task_Math2in_AL);
  else if (name == "math_2AM")
    NewTask(name, "Math 2AM (X/log(|Y|))", &cTaskLib::Task_Math2in_AM);
  else if (name == "math_2AN")
    NewTask(name, "Math 2AN (X+Y)", &cTaskLib::Task_Math2in_AN);
  else if (name == "math_2AO")
    NewTask(name, "Math 2AO (X-Y)", &cTaskLib::Task_Math2in_AO);
  else if (name == "math_2AP")
    NewTask(name, "Math 2AP (X/Y)", &cTaskLib::Task_Math2in_AP);
  else if (name == "math_2AQ")
    NewTask(name, "Math 2AQ (XY)", &cTaskLib::Task_Math2in_AQ);
  else if (name == "math_2AR")
    NewTask(name, "Math 2AR (sqrt(X)+sqrt(Y))", &cTaskLib::Task_Math2in_AR);
  else if (name == "math_2AS")
    NewTask(name, "Math 2AS (X+2Y)", &cTaskLib::Task_Math2in_AS);
  else if (name == "math_2AT")
    NewTask(name, "Math 2AT (X+3Y)", &cTaskLib::Task_Math2in_AT);
  else if (name == "math_2AU")
    NewTask(name, "Math 2AU (2X+3Y)", &cTaskLib::Task_Math2in_AU);
  else if (name == "math_2AV")
    NewTask(name, "Math 2AV (XY^2)", &cTaskLib::Task_Math2in_AV);
  
  // Arbitrary 3-Input Math Tasks
  if (name == "math_3AA")
    NewTask(name, "Math 3AA (X^2+Y^2+Z^2)", &cTaskLib::Task_Math3in_AA);  
  else if (name == "math_3AB")
    NewTask(name, "Math 3AB (sqrt(X)+sqrt(Y)+sqrt(Z))", &cTaskLib::Task_Math3in_AB);  
  else if (name == "math_3AC")
    NewTask(name, "Math 3AC (X+2Y+3Z)", &cTaskLib::Task_Math3in_AC);  
  else if (name == "math_3AD")
    NewTask(name, "Math 3AD (XY^2+Z^3)", &cTaskLib::Task_Math3in_AD);  
  else if (name == "math_3AE")
    NewTask(name, "Math 3AE ((X%Y)*Z)", &cTaskLib::Task_Math3in_AE);  
  else if (name == "math_3AF")
    NewTask(name, "Math 3AF ((X+Y)^2+sqrt(Y+Z))", &cTaskLib::Task_Math3in_AF);
  else if (name == "math_3AG")
    NewTask(name, "Math 3AG ((XY)%(YZ))", &cTaskLib::Task_Math3in_AG);  
  else if (name == "math_3AH")
    NewTask(name, "Math 3AH (X+Y+Z)", &cTaskLib::Task_Math3in_AH);  
  else if (name == "math_3AI")
    NewTask(name, "Math 3AI (-X-Y-Z)", &cTaskLib::Task_Math3in_AI);  
  else if (name == "math_3AJ")
    NewTask(name, "Math 3AJ ((X-Y)^2+(Y-Z)^2+(Z-X)^2)", &cTaskLib::Task_Math3in_AJ);  
  else if (name == "math_3AK")
    NewTask(name, "Math 3AK ((X+Y)^2+(Y+Z)^2+(Z+X)^2)", &cTaskLib::Task_Math3in_AK);  
  else if (name == "math_3AL")
    NewTask(name, "Math 3AL ((X-Y)^2+(X-Z)^2)", &cTaskLib::Task_Math3in_AL);  
  else if (name == "math_3AM")
    NewTask(name, "Math 3AM ((X+Y)^2+(Y+Z)^2)", &cTaskLib::Task_Math3in_AM);  
  
  // Matching Tasks
  if (name == "matchstr") 
    Load_MatchStr(name, info, envreqs, feedback);
  else if (name == "match_number")
    Load_MatchNumber(name, info, envreqs, feedback);
	else if (name == "matchprodstr") 
    Load_MatchProdStr(name, info, envreqs, feedback);

  
  // Sequence Tasks
  if (name == "sort_inputs")
    Load_SortInputs(name, info, envreqs, feedback);
  else if (name == "fibonacci_seq")
    Load_FibonacciSequence(name, info, envreqs, feedback);
  
  // Math Tasks
  if (name == "mult")
    Load_Mult(name, info, envreqs, feedback);
  else if (name == "div")
    Load_Div(name, info, envreqs, feedback);
  else if (name == "log")
    Load_Log(name, info, envreqs, feedback);
  else if (name == "log2")
    Load_Log2(name, info, envreqs, feedback);
  else if (name == "log10")
    Load_Log10(name, info, envreqs, feedback);
  else if (name == "sqrt")
    Load_Sqrt(name, info, envreqs, feedback);
  else if (name == "sine")
    Load_Sine(name, info, envreqs, feedback);
  else if (name == "cosine")
    Load_Cosine(name, info, envreqs, feedback);
  
  
  // Optimization Tasks
  if (name == "optimize")
    Load_Optimize(name, info, envreqs, feedback);
  
  // Communication Tasks
  if (name == "comm_echo")
    NewTask(name, "Echo of Neighbor's Input", &cTaskLib::Task_CommEcho, REQ_NEIGHBOR_INPUT);
  else if (name == "comm_not")
    NewTask(name, "Not of Neighbor's Input", &cTaskLib::Task_CommNot, REQ_NEIGHBOR_INPUT);
  
  // Network Tasks
  if (name == "net_send")
    NewTask(name, "Successfully Sent Network Message", &cTaskLib::Task_NetSend);
  else if (name == "net_receive")
    NewTask(name, "Successfully Received Network Message", &cTaskLib::Task_NetReceive);
  
  // Movement Tasks
  if (name == "move_up_gradient")
    NewTask(name, "Move up gradient", &cTaskLib::Task_MoveUpGradient);
  else if (name == "move_neutral_gradient")
    NewTask(name, "Move neutral gradient", &cTaskLib::Task_MoveNeutralGradient);
  else if (name == "move_down_gradient")
    NewTask(name, "Move down gradient", &cTaskLib::Task_MoveDownGradient);
  else if (name == "move_not_up_gradient")
    NewTask(name, "Move not up gradient", &cTaskLib::Task_MoveNotUpGradient);
  else if (name == "move_to_right_side")
    NewTask(name, "Move to right side", &cTaskLib::Task_MoveToRightSide);
  else if (name == "move_to_left_side")
    NewTask(name, "Move to left side", &cTaskLib::Task_MoveToLeftSide);
  // BDC Movement Tasks
  else if (name == "move")
    NewTask(name, "Successfully Moved", &cTaskLib::Task_Move);
  else if (name == "movetotarget")
    NewTask(name, "Move to a target area", &cTaskLib::Task_MoveToTarget);
  else if (name == "movetoevent")
    NewTask(name, "Move to a target area", &cTaskLib::Task_MoveToMovementEvent);
  else if (name == "movebetweenevent")
    NewTask(name, "Move to a target area", &cTaskLib::Task_MoveBetweenMovementEvent); 
	
  // reputation based tasks
  else if(name == "perfect_strings") 
    NewTask(name, "Produce and store perfect strings", &cTaskLib::Task_CreatePerfectStrings);		

  // event tasks
  if(name == "move_to_event")
    NewTask(name, "Moved into cell containing event", &cTaskLib::Task_MoveToEvent);
  else if(name == "event_killed")
    NewTask(name, "Killed event", &cTaskLib::Task_EventKilled);
  
  // Optimization Tasks
  if (name == "sg_path_traversal")
    Load_SGPathTraversal(name, info, envreqs, feedback);  
  
  if (name == "form-group")
    Load_FormSpatialGroup(name, info, envreqs, feedback);
	
  if (name == "form-group-id")
    Load_FormSpatialGroupWithID(name, info, envreqs, feedback);
	
  if (name == "live-on-patch-id")
    Load_LiveOnPatchRes(name, info, envreqs, feedback);
	
  // String matching
  if(name == "all-ones")
    Load_AllOnes(name, info, envreqs, feedback);
  else if (name == "royal-road")
    Load_RoyalRoad(name, info, envreqs, feedback);
  else if (name == "royal-road-wd")
    Load_RoyalRoadWithDitches(name, info, envreqs, feedback);
  
  // Division of labor
  if(name == "opinion_is") {
    Load_OpinionIs(name, info, envreqs, feedback);
  }
  
  // Make sure we have actually found a task  
  if (task_array.GetSize() == start_size) {
    if (feedback && feedback->GetNumErrors() == 0) feedback->Error("unknown task entry '%s'", (const char*)name);
    return NULL;
  }
  
  // And return the found task.
  return task_array[start_size];
}

void cTaskLib::NewTask(const cString& name, const cString& desc, tTaskTest task_fun, int reqs, cArgContainer* args)
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


double cTaskLib::Task_Add3(cTaskContext& ctx) const
{
  const tBuffer<int>& input = ctx.GetInputBuffer();
  const int output = ctx.GetOutputBuffer()[0];
  for(int i=0; i<(input.GetNumStored()-2); ++i) {
    if(output == (input[i] + input[i+1] + input[i+2])) {
      return 1.0;
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

// @WRE DontCare task always succeeds.
double cTaskLib::Task_DontCare(cTaskContext& ctx) const
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


double cTaskLib::Task_Nand_ResourceDependent(cTaskContext& ctx) const
{
  const double resCrossoverLevel = 100;

  const int logic_id = ctx.GetLogicId();
  if (!(logic_id == 63 || logic_id == 95 || logic_id == 119)) return 0.0;
		
  const cResourceLib& resLib = m_world->GetEnvironment().GetResourceLib();
  const tArray<double>& resource_count_array = ctx.GetOrganism()->GetOrgInterface().GetResources();
  const cResourceCount& resource_count = m_world->GetPopulation().GetResourceCount();
	
  if (resource_count.GetSize() == 0) assert(false); // change to: return false;
	
  double pher_amount = 0;
  cResource* res = resLib.GetResource("pheromone");
	
  if(strncmp(resource_count.GetResName(res->GetID()), "pheromone", 9) == 0) {
    pher_amount += resource_count_array[res->GetID()];
  }
  
  if (pher_amount < resCrossoverLevel) return 1.0;
  return 0.0;	
}


double cTaskLib::Task_Nor_ResourceDependent(cTaskContext& ctx) const
{
  const double resCrossoverLevel = 100;

  const int logic_id = ctx.GetLogicId();
  if (!(logic_id == 3 || logic_id == 5 || logic_id == 17))  return 0.0;
	
  const cResourceLib& resLib = m_world->GetEnvironment().GetResourceLib();
  const tArray<double>& resource_count_array = ctx.GetOrganism()->GetOrgInterface().GetResources();
  const cResourceCount& resource_count = m_world->GetPopulation().GetResourceCount();
  
  //if (resource_count.GetSize() == 0) assert(false); // change to: return false;
  assert(resource_count.GetSize() != 0);
	
  double pher_amount = 0;
  cResource* res = resLib.GetResource("pheromone");
  
  if (strncmp(resource_count.GetResName(res->GetID()), "pheromone", 9) == 0) {
    pher_amount += resource_count_array[res->GetID()];
  }
	
  if (pher_amount > resCrossoverLevel) return 1.0;
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

double cTaskLib::Task_Math1in_AA(cTaskContext& ctx) const //(2X)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    if (test_output == 2 * input_buffer[i]) return 1.0;
  }
  return 0.0; 
}

double cTaskLib::Task_Math1in_AB(cTaskContext& ctx) const //(2X/3)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == 2 * input_buffer[i] / 3) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AC(cTaskContext& ctx) const //(5X/4)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == 5 * input_buffer[i] / 4) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AD(cTaskContext& ctx) const //(X^2)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  for (int i = 0; i < input_size; i ++) {
    if (test_output == input_buffer[i] * input_buffer[i]) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AE(cTaskContext& ctx) const //(X^3)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == input_buffer[i] * input_buffer[i] * input_buffer[i])
      return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AF(cTaskContext& ctx) const //(sqrt(X)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  for (int i = 0; i < input_size; i ++) {
    if (test_output == (int) sqrt((double) abs(input_buffer[i]))) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AG(cTaskContext& ctx) const //(log(X))
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (input_buffer[i] <= 0) continue;
    if (test_output == (int) log((double) input_buffer[i])) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AH(cTaskContext& ctx) const //(X^2+X^3)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == input_buffer[i] * input_buffer[i] + input_buffer[i] * input_buffer[i] * input_buffer[i])
      return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AI(cTaskContext& ctx) const // (X^2 + sqrt(X))
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == input_buffer[i] * input_buffer[i] + (int) sqrt((double) abs(input_buffer[i]))) 
      return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AJ(cTaskContext& ctx) const // abs(X)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == abs(input_buffer[i])) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AK(cTaskContext& ctx) const //(X-5)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == input_buffer[i] - 5) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AL(cTaskContext& ctx) const //(-X)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == 0 - input_buffer[i]) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AM(cTaskContext& ctx) const //(5X)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == 5 * input_buffer[i]) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AN(cTaskContext& ctx) const //(X/4)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == input_buffer[i] / 4) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AO(cTaskContext& ctx) const //(X-6)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == input_buffer[i] - 6) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math1in_AP(cTaskContext& ctx) const //(X-7)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    if (test_output == input_buffer[i] - 7) return 1.0;
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AA(cTaskContext& ctx) const //(sqrt(X+Y))
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == (int) sqrt((double) abs(input_buffer[i] + input_buffer[j])))
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AB(cTaskContext& ctx) const  //((X+Y)^2)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == (input_buffer[i] + input_buffer[j]) * 
          (input_buffer[i] + input_buffer[j])) return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AC(cTaskContext& ctx) const //(X%Y)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (input_buffer[j] == 0) continue; // mod by zero
      if (test_output == input_buffer[i] % input_buffer[j]) return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AD(cTaskContext& ctx) const //(3X/2+5Y/4)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == 3 * input_buffer[i] / 2 + 5 * input_buffer[j] / 4)
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AE(cTaskContext& ctx) const //(abs(X-5)+abs(Y-6))
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == abs(input_buffer[i] - 5) + abs(input_buffer[j] - 6))
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AF(cTaskContext& ctx) const //(XY-X/Y)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (input_buffer[j] == 0) continue;
      if (0-INT_MAX > input_buffer[i] && input_buffer[j] == -1) continue;
      if (test_output == input_buffer[i] * input_buffer[j] - 
          input_buffer[i] / input_buffer[j]) return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AG(cTaskContext& ctx) const //((X-Y)^2)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == (input_buffer[i] - input_buffer[j]) *
          (input_buffer[i] - input_buffer[j])) return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AH(cTaskContext& ctx) const //(X^2+Y^2)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == input_buffer[i] * input_buffer[i] +
          input_buffer[j] * input_buffer[j]) return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AI(cTaskContext& ctx) const //(X^2+Y^3)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == input_buffer[i] * input_buffer[i] + input_buffer[j] * input_buffer[j] * input_buffer[j]) 
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AJ(cTaskContext& ctx) const //((sqrt(X)+Y)/(X-7))
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (input_buffer[i] - 7 == 0) continue;
      if (test_output == ((int) sqrt((double) abs(input_buffer[i])) + input_buffer[j]) / (input_buffer[i] - 7)) return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AK(cTaskContext& ctx) const //(log(|X/Y|))
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j || input_buffer[j] == 0 ) continue;
      if (0-INT_MAX > input_buffer[i] && input_buffer[j] == -1) continue;
      if (input_buffer[i] / input_buffer[j] == 0) continue;
      if (test_output == (int) log((double) abs(input_buffer[i] / input_buffer[j])))
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AL(cTaskContext& ctx) const //(log(|X|)/Y)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j || input_buffer[j] == 0) continue;
      if (test_output == (int) log((double) abs(input_buffer[i])) / input_buffer[j])
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AM(cTaskContext& ctx) const //(X/log(|Y|))
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j || log((double) abs(input_buffer[j])) == 0) continue;
      if (0-INT_MAX > input_buffer[i] && log((double) abs(input_buffer[j])) == -1) continue;
      if (test_output == input_buffer[i] / (int) log((double) abs(input_buffer[j])))
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AN(cTaskContext& ctx) const //(X+Y)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == input_buffer[i] + input_buffer[j])
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AO(cTaskContext& ctx) const //(X-Y)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == input_buffer[i] - input_buffer[j])
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AP(cTaskContext& ctx) const //(X/Y)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j || input_buffer[j] == 0) continue;
      if (0 - INT_MAX > input_buffer[i] && input_buffer[j] == -1) continue;
      if (test_output == input_buffer[i] / input_buffer[j])
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AQ(cTaskContext& ctx) const //(XY)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == input_buffer[i] * input_buffer[j])
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AR(cTaskContext& ctx) const //(sqrt(X)+sqrt(Y))
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == (int) sqrt((double) abs(input_buffer[i])) + (int) sqrt((double) abs(input_buffer[j])))
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AS(cTaskContext& ctx) const //(X+2Y)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == input_buffer[i] + 2 * input_buffer[j])
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AT(cTaskContext& ctx) const //(X+3Y)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == input_buffer[i] + 3 * input_buffer[j])
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AU(cTaskContext& ctx) const //(2X+3Y)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == 2 * input_buffer[i] + 3 * input_buffer[j])
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math2in_AV(cTaskContext& ctx) const //(XY^2)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < input_size; j++) {
      if (i == j) continue;
      if (test_output == input_buffer[i] * input_buffer[j] * input_buffer[j])
        return 1.0;
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AA(cTaskContext& ctx) const //(X^2+Y^2+Z^2)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;
        if (test_output == input_buffer[i] * input_buffer[i] + 
            input_buffer[j] * input_buffer[j] + 
            input_buffer[k] * input_buffer[k]) return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AB(cTaskContext& ctx) const //(sqrt(X)+sqrt(Y)+sqrt(Z))
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;
        if (test_output == (int) sqrt((double) abs(input_buffer[i])) +
            (int) sqrt((double) abs(input_buffer[j])) + (int) sqrt((double) abs(input_buffer[k])))
          return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AC(cTaskContext& ctx) const //(X+2Y+3Z)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;
        if (test_output == input_buffer[i] + 2 * input_buffer[j] +
            3 * input_buffer[k]) return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AD(cTaskContext& ctx) const //(XY^2+Z^3)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;
        if (test_output == input_buffer[i] * input_buffer[j] * input_buffer[j] + input_buffer[k] * input_buffer[k] * input_buffer[k])
          return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AE(cTaskContext& ctx) const //((X%Y)*Z)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;
        if (input_buffer[j] == 0) continue; // mod by zero
        if (test_output == input_buffer[i] % input_buffer[j] * input_buffer[k])
          return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AF(cTaskContext& ctx) const //((X+Y)^2+sqrt(Y+Z))
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;
        if (test_output == (input_buffer[i] + input_buffer[j]) *
            (input_buffer[i] + input_buffer[j]) +
            (int) sqrt((double) abs(input_buffer[j] + input_buffer[k])))
          return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AG(cTaskContext& ctx) const //((XY)%(YZ))
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;
        int mod_base = input_buffer[j] * input_buffer[k];
        if (mod_base == 0) continue;
        if (test_output == (input_buffer[i] * input_buffer[j]) %
            mod_base) return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AH(cTaskContext& ctx) const //(X+Y+Z)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;
        if (test_output == input_buffer[i] + input_buffer[j] + input_buffer[k])
          return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AI(cTaskContext& ctx) const //(-X-Y-Z)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;
        if (test_output == 0 - input_buffer[i] - input_buffer[j] - input_buffer[k])
          return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AJ(cTaskContext& ctx) const //((X-Y)^2+(Y-Z)^2+(Z-X)^2)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;
        if (test_output == (input_buffer[i] - input_buffer[j]) * (input_buffer[i] - input_buffer[j]) + (input_buffer[j] - input_buffer[k]) * (input_buffer[j] - input_buffer[k]) + (input_buffer[k] - input_buffer[i]) * (input_buffer[k] - input_buffer[i]))
          return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AK(cTaskContext& ctx) const //((X+Y)^2+(Y+Z)^2+(Z+X)^2)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();

  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;
        if (test_output == (input_buffer[i] + input_buffer[j]) * (input_buffer[i] + input_buffer[j]) + (input_buffer[j] + input_buffer[k]) * (input_buffer[j] + input_buffer[k]) + (input_buffer[k] + input_buffer[i]) * (input_buffer[k] + input_buffer[i])) 
          return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AL(cTaskContext& ctx) const //((X-Y)^2+(X-Z)^2)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;  
        if (test_output == (input_buffer[i] - input_buffer[j]) * (input_buffer[i] - input_buffer[j]) + (input_buffer[i] - input_buffer[k]) * (input_buffer[i] - input_buffer[k]))
          return 1.0;
      }
    }
  }
  return 0.0;
}

double cTaskLib::Task_Math3in_AM(cTaskContext& ctx) const //((X+Y)^2+(Y+Z)^2)
{
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const int test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      for (int k = 0; k < input_size; k ++) {
        if (i == j || j == k || i == k) continue;  
        if (test_output == (input_buffer[i] + input_buffer[j]) * (input_buffer[i] + input_buffer[j]) + (input_buffer[i] + input_buffer[k]) * (input_buffer[i] + input_buffer[k]))
          return 1.0;
      }
    }
  }
  return 0.0;
}


void cTaskLib::Load_MatchStr(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  schema.AddEntry("string", 0, cArgSchema::SCHEMA_STRING);
  schema.AddEntry("partial",0, 0);
  schema.AddEntry("binary",1,1);
  schema.AddEntry("pow",0,2.0);
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  envreqs.SetMinOutputs(args->GetString(0).GetSize());
  if (args) NewTask(name, "MatchStr", &cTaskLib::Task_MatchStr, 0, args);
}

double cTaskLib::Task_MatchStr(cTaskContext& ctx) const
{
  tBuffer<int> temp_buf(ctx.GetOutputBuffer());
  //  if (temp_buf[0] != 357913941) return 0;
  
  //  temp_buf.Pop(); // pop the signal value off of the buffer
  
  const cString& string_to_match = ctx.GetTaskEntry()->GetArguments().GetString(0);
  int partial = ctx.GetTaskEntry()->GetArguments().GetInt(0);
  int binary = ctx.GetTaskEntry()->GetArguments().GetInt(1);
//  double mypow = ctx.GetTaskEntry()->GetArguments().GetDouble(0);
  int string_index;
  int num_matched = 0;
  int test_output;
  int max_num_matched = 0;
  int num_real=0;

  if (!binary) {
    if (temp_buf.GetNumStored() > 0) {
      test_output = temp_buf[0];
      
      for (int j = 0; j < string_to_match.GetSize(); j++) {  
	string_index = string_to_match.GetSize() - j - 1; // start with last char in string
	int k = 1 << j;
	if ((string_to_match[string_index] == '0' && !(test_output & k)) ||
	    (string_to_match[string_index] == '1' && (test_output & k))) num_matched++;
      }
      max_num_matched = num_matched;
    }
  }
  else {
    for (int j = 0; j < string_to_match.GetSize(); j++) {
      if (string_to_match[j] != '9') {
	num_real++;
      }
      if (string_to_match[j] == '0' && temp_buf[j]==0 ||
	  string_to_match[j] == '1' && temp_buf[j]==1) {
          num_matched++;
      }
    }
    max_num_matched = num_matched;
  }

  bool used_received = false;
  if (ctx.GetReceivedMessages()) {
    tBuffer<int> received(*(ctx.GetReceivedMessages()));
    for (int i = 0; i < received.GetNumStored(); i++) {
      test_output = received[i];
      num_matched = 0;
      
      for (int j = 0; j < string_to_match.GetSize(); j++) {
        string_index = string_to_match.GetSize() - j - 1; // start with last char in string
        int k = 1 << j;
        if ((string_to_match[string_index]=='0' && !(test_output & k)) ||
            (string_to_match[string_index]=='1' && (test_output & k))) {
	  num_matched++;
	}
      }
      
      if (num_matched > max_num_matched) {
        max_num_matched = num_matched;
        used_received = true;
      }
    }
  }
  
  double bonus = 0.0;
  // return value between 0 & 1 representing the percentage of string that was matched
  double base_bonus = static_cast<double>(max_num_matched) * 2.0 / static_cast<double>(string_to_match.GetSize()) - 1;

  if (partial) {
    base_bonus=double(max_num_matched)*2/double(num_real) -1;
  }

  if (base_bonus > 0.0) {
    bonus = pow(base_bonus, 2);
    if (used_received) m_world->GetStats().AddMarketItemUsed();
    else m_world->GetStats().AddMarketOwnItemUsed();
  }
  return bonus;
}

vector<cString> cTaskLib::GetMatchStrings()
{
  return m_strings;
}

cString cTaskLib::GetMatchString(int x)
{ 
  cString s; 
  if(x >= 0 && x < (int)m_strings.size()){
    s = m_strings[x]; 
  } else { 
    s = cString("");
  }
  
  return s;
}


void cTaskLib::Load_MatchProdStr(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  schema.AddEntry("string", 0, cArgSchema::SCHEMA_STRING);		
  schema.AddEntry("partial",0, 0);
  schema.AddEntry("binary",1,1);
  schema.AddEntry("pow",0,2.0);
	schema.AddEntry("tag",2,-1);
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);	
  envreqs.SetMinOutputs(args->GetString(0).GetSize());
	m_strings.push_back(args->GetString(0));
  if (args) NewTask(name, "MatchProdStr", &cTaskLib::Task_MatchStr, 0, args);
}


double cTaskLib::Task_MatchProdStr(cTaskContext& ctx) const
{
  // These even out the stats tracking.
  m_world->GetStats().AddTag(ctx.GetTaskEntry()->GetArguments().GetInt(2), 0);
  m_world->GetStats().AddTag(-1, 0);
	
  tBuffer<int> temp_buf(ctx.GetOutputBuffer());
  
  const cString& string_to_match = ctx.GetTaskEntry()->GetArguments().GetString(0);
  int partial = ctx.GetTaskEntry()->GetArguments().GetInt(0);
  int binary = ctx.GetTaskEntry()->GetArguments().GetInt(1);
  double mypow = ctx.GetTaskEntry()->GetArguments().GetDouble(0);
  int string_index;
  int num_matched = 0;
  int test_output;
  int max_num_matched = 0;
  int num_real=0;
	
  if (!binary) {
    if (temp_buf.GetNumStored() > 0) {
      test_output = temp_buf[0];
			
      for (int j = 0; j < string_to_match.GetSize(); j++) {  
	string_index = string_to_match.GetSize() - j - 1; // start with last char in string
	int k = 1 << j;
	if ((string_to_match[string_index] == '0' && !(test_output & k)) ||
	    (string_to_match[string_index] == '1' && (test_output & k))) num_matched++;
      }
      max_num_matched = num_matched;
    }
  }
  else {
    for (int j = 0; j < string_to_match.GetSize(); j++) {
      if (string_to_match[j]!='9') num_real++;
      if (string_to_match[j]=='0' && temp_buf[j]==0 ||
	  string_to_match[j]=='1' && temp_buf[j]==1)
	num_matched++;
    }
    max_num_matched = num_matched;
  }
  
  // Check if the organism already produced this string. 
  // If so, it receives a perfect score for this task.
  int tag = ctx.GetTaskEntry()->GetArguments().GetInt(2);
	
  if (m_world->GetConfig().MATCH_ALREADY_PRODUCED.Get()) {
    int prod = ctx.GetOrganism()->GetNumberStringsProduced(tag); 
    if (prod) max_num_matched = string_to_match.GetSize();
  }
	
  
  // Update the organism's tag. 
  ctx.GetOrganism()->UpdateTag(tag, max_num_matched);
  if (ctx.GetOrganism()->GetTagLabel() == tag) {
    ctx.GetOrganism()->SetLineageLabel(ctx.GetTaskEntry()->GetArguments().GetInt(2));
  } 
	
	
  // Update stats
  cString name;
  name = "[produced"; 
  name += string_to_match;
  name += "]";
  m_world->GetStats().AddStringBitsMatchedValue(name, max_num_matched);
  
  // if the organism hasn't donated, then zero out its reputation. 
  if ((ctx.GetOrganism()->GetReputation() > 0) && 
      (ctx.GetOrganism()->GetNumberOfDonations() == 0)) {
    ctx.GetOrganism()->SetReputation(0);
  }
	
  double bonus = 0.0;
  double base_bonus = 0.0; 
  
  base_bonus = static_cast<double>(max_num_matched) * 2.0 / static_cast<double>(string_to_match.GetSize()) - 1;
  
  if (partial) {
    base_bonus=double(max_num_matched)*2/double(num_real) -1;
  }
  
  if (base_bonus > 0.0) {
    bonus = pow(base_bonus,mypow);
  }
  return bonus;
	
}


void cTaskLib::Load_MatchNumber(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("target", 0, cArgSchema::SCHEMA_INT);
  schema.AddEntry("threshold", 1, -1);
  // Double Arguments
  schema.AddEntry("halflife", 0, cArgSchema::SCHEMA_DOUBLE);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "Match Number", &cTaskLib::Task_MatchNumber, 0, args);
}

double cTaskLib::Task_MatchNumber(cTaskContext& ctx) const
{
  double quality = 0.0;
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  
  long long diff = ::llabs((long long)args.GetInt(0) - ctx.GetOutputBuffer()[0]);
  int threshold = args.GetInt(1);
  
  if (threshold < 0 || diff <= threshold) { // Negative threshold == infinite
    // If within threshold range, quality decays based on absolute difference
    double halflife = -1.0 * fabs(args.GetDouble(0));
    quality = pow(2.0, static_cast<double>(diff) / halflife);
  }
  
  return quality;
}


void cTaskLib::Load_SortInputs(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("size", 0, cArgSchema::SCHEMA_INT); // Number of items to sort
  schema.AddEntry("direction", 1, 0); // < 0 = Descending, Otherwise = Ascending
  schema.AddEntry("contiguous", 2, 1); // 0 = No, Otherwise = Yes
  // Double Arguments
  schema.AddEntry("halflife", 0, cArgSchema::SCHEMA_DOUBLE);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) {
    envreqs.SetMinInputs(args->GetInt(0));
    envreqs.SetMinOutputs(args->GetInt(0) * 2);
    envreqs.SetTrueRandInputs();
    NewTask(name, "Sort Inputs", &cTaskLib::Task_SortInputs, 0, args);
  }
}

double cTaskLib::Task_SortInputs(cTaskContext& ctx) const
{
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  const tBuffer<int>& output = ctx.GetOutputBuffer();
  const int size = args.GetInt(0);
  const int stored = output.GetNumStored();
  
  // if less than half, can't possibly reach threshold
  if (stored <= (size / 2)) return 0.0;
  
  tHashMap<int, int> valmap;
  int score = 0;
  int maxscore = 0;
  
  // add all valid inputs into the value map
  for (int i = 0; i < size; i++) valmap.Set(ctx.GetOrganism()->GetInputAt(i), -1);
  
  int span_start = -1;
  int span_end = stored;
  
  if (args.GetInt(2)) { // Contiguous
    // scan for the largest contiguous span
    // - in the event of a tie, keep the first discovered
    for (int i = 0; i < stored; i++) {
      if (valmap.HasEntry(output[i])) {
        int t_start = i;
        while (++i < stored && valmap.HasEntry(output[i])) ;
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
    while (++span_start < stored && valmap.HasEntry(output[span_start])) ;
    
    // scanned past the end of the output, nothing to validate
    if (span_start >= stored) return 0.0;
  }
  
  // again, if span is less than half the size can't possibly reach threshold
  if ((span_end - span_start) <= (size / 2)) return 0.0;
  
  // insertion sort span
  // - count number of actual entries
  // - count moves required
  // - update valmap, tracking observed inputs
  tArray<int> sorted(size);
  const bool ascending = (args.GetInt(1) >= 0);
  int count = 1;
  
  // store first value
  valmap.Set(output[span_start], span_start);
  sorted[0] = output[span_start];
  
  // iterate over the remaining span (discovered for contiguous, full output for scattered)
  for (int i = span_start + 1; i < span_end; i++) {      
    int value = output[i];
    
    // check for a dup or invalid output, skip it if so
    int idx;
    if (!valmap.Find(value, idx) || idx != -1) continue;
    
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
      if (valmap.Find(ctx.GetOrganism()->GetInputAt(i), idx) && idx == -1) {
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
    double halflife = -1.0 * fabs(args.GetDouble(0));
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


void cTaskLib::Load_FibonacciSequence(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("target", 0, cArgSchema::SCHEMA_INT);
  // Double Arguments
  schema.AddEntry("penalty", 0, 0.0);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  
  if (args) NewTask(name, "Fibonacci Sequence", &cTaskLib::Task_FibonacciSequence, 0, args);
}


double cTaskLib::Task_FibonacciSequence(cTaskContext& ctx) const
{
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
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
    if (state->count > args.GetInt(0)) return args.GetDouble(0);
    
    return 1.0;
  }
  
  return 0.0;
}


void cTaskLib::Load_Optimize(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("function", 0, cArgSchema::SCHEMA_INT);
  schema.AddEntry("binary", 1, 0);
  schema.AddEntry("varlength", 2, 8);
  schema.AddEntry("numvars", 3, 2);
  // Double Arguments
  schema.AddEntry("basepow", 0, 2.0);
  schema.AddEntry("maxFx", 1, 1.0);
  schema.AddEntry("minFx", 2, 0.0);
  schema.AddEntry("thresh", 3, -1.0);
  schema.AddEntry("threshMax", 4, -1.0);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) {
    if (args->GetInt(1)) {
      envreqs.SetMinOutputs(args->GetInt(2)*args->GetInt(3));
    }
    else {
      // once have ability to change args should in each of these cases change the max/min
      // to the appropriate defaults for this function
      switch (args->GetInt(0)) {
      case 1:
	envreqs.SetMinOutputs(1);
	break;
      case 2:
	envreqs.SetMinOutputs(2);
	break;
      case 3:
	envreqs.SetMinOutputs(2);
	break;
      default:
	envreqs.SetMinOutputs(2);
      };
    }
    
    NewTask(name, "Optimize", &cTaskLib::Task_Optimize, 0, args);
  }
}

double cTaskLib::Task_Optimize(cTaskContext& ctx) const
{
  // if the org hasn't output yet enough numbers, just return without completing any tasks
  if (ctx.GetOutputBuffer().GetNumStored() < ctx.GetOutputBuffer().GetCapacity()) return 0;
  
  double quality = 0.0;
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  
  // which function are we currently checking?
  const int function = args.GetInt(0);
  
  // get however many variables need, turn them into doubles between 0 and 1
  tArray<double> vars;
  vars.Resize(args.GetInt(3));
  
  double Fx = 0.0;
  
  // some of the problems don't need double variables but use the bit string as a bit string
  // string match, Fx=length - num_matched (0 best, length worst)
  if (function == 20) {
    const cString& string_to_match = args.GetString(0);
    int matched=0;
    for (int i=0; i<args.GetInt(2); i++) {
      if ((string_to_match[i] == '0' && ctx.GetOutputBuffer()[i]==0)  ||
	  (string_to_match[i] == '1' && ctx.GetOutputBuffer()[i]==1))
	matched++;
    }
    Fx=args.GetInt(2) - matched;
  }

  // string with all 1's at beginning until pattern 0101, reward for # 1's
  else if (function == 21) {
    int numOnes=0;
    int patFound=0;
    for (int i=0; i<args.GetInt(2)-3; i++) {
      if (ctx.GetOutputBuffer()[i]==1) numOnes++;
      else {
	if (ctx.GetOutputBuffer()[i+1] == 1 &&
	    ctx.GetOutputBuffer()[i+2]==0 && ctx.GetOutputBuffer()[i+3]==1) {
	  patFound=1;
	}
	break;
      }
    }
    if (patFound) Fx=args.GetInt(2)-4-numOnes;
    else Fx=args.GetInt(2);
  }

  // simply rewared for number of 1's at beginning of string, maxFx=length of string
  else if (function == 22) {
    int numOnes=0;
    for (int i=0; i<args.GetInt(2); i++) {
      if (ctx.GetOutputBuffer()[i]==1) numOnes++;
      else break;
    }
    Fx=args.GetInt(2)-numOnes;
  }

  // simply reward for number 0's in string
  else if (function == 23) {
    int numZeros=0;
    for (int i=0; i<args.GetInt(2); i++) {
      if (ctx.GetOutputBuffer()[i]==0) numZeros++;
      else break;
    }
    Fx=args.GetInt(2)-numZeros;
  }

  else if (function == 18) {
    int tot=0;
    for (int i=0; i<30; i++) {
      tot+= ctx.GetOutputBuffer()[i];
    }
    Fx = 1+tot;
  }

  else if (function == 19) {
    tArray<double> tempVars;
    tempVars.Resize(args.GetInt(3));
    for (int i=0; i<args.GetInt(3); i++) {
      tempVars[i]=0;
    }
    
    for (int i=0; i<30; i++) {
      tempVars[0]+= ctx.GetOutputBuffer()[i];
    }
    
    int len = args.GetInt(2);
    for (int i = len - 1; i >= 0; i--) {
      for (int j=1; j<args.GetInt(3); j++) {
        tempVars[j-1] += ctx.GetOutputBuffer()[30+i + len*(args.GetInt(3)-j-1)];
      }
    }
    
    int Gx = 0;
    for (int i = 1; i < args.GetInt(3); i++) {
      if (tempVars[i] == 5) Gx += 1;
      else Gx += int(tempVars[i]) + 2;
    }
    Fx = Gx * (1 / (1 + tempVars[0]));
  }

  else {
    if (args.GetInt(1)) {
      int len = args.GetInt(2);
      double base_pow = args.GetDouble(0);
      
      tArray<double> tempVars;
      tempVars.Resize(args.GetInt(3));
      for (int i=0; i<args.GetInt(3); i++) tempVars[i] = 0;
      
      double tot = 0;
      for (int i = len - 1; i >= 0; i--) {
        for (int j=0; j<args.GetInt(3); j++) {
          tempVars[j] += ctx.GetOutputBuffer()[i + len*(args.GetInt(3)-j-1)] * pow(base_pow, (len - 1) - i);
        }
        tot += pow(base_pow, double(i));
      }
      for (int i=0; i<args.GetInt(3); i++) {
        vars[i] = tempVars[i] / tot;
      }
      //	cout << "x: " << vars[0] << " ";
    } 
    else {
      for (int j=0; j<args.GetInt(3); j++) {
        vars[j] = double(ctx.GetOutputBuffer()[j]) / 0xffffffff;
      }
    }
    
    for (int j=0; j<args.GetInt(3); j++) {
      if (vars[j] < 0) vars[j] = 0;
      else if (vars[j] > 1) vars[j] = 1;
    }
    
    switch(function) {
    case 1:
      Fx = vars[0];		// F1
      //	  cout << "Fx1: " << Fx << " ";
      break;
        
    case 2:
      Fx = (1.0 + vars[1]) * (1.0 - sqrt(vars[0] / (1.0 + vars[1])));   // F2
      break;
        
    case 3:
      Fx = (1.0 + vars[1]) * (1.0 - pow(vars[0] / (1.0 + vars[1]), 2.0));  // F3
      break;
        
    case 4:
      Fx = (1.0 + vars[1]) * (1.0 - sqrt(vars[0] / (1.0 + vars[1])) - (vars[0] / (1.0 + vars[1])) * sin(3.14159 * vars[0] * 10.0));
      break;
        
    case 5:
      vars[0] = vars[0] * -2.0;
      Fx = vars[0]*vars[0] + vars[1]*vars[1];
      break;
        
    case 6:
      vars[0] = vars[0] * -2.0;
      Fx = (vars[0] + 2.0)*(vars[0] + 2.0) + vars[1]*vars[1];
      break;
        
    case 7:
      vars[0] = vars[0] * 4.0;
      Fx = sqrt(vars[0]) + vars[1];
      break;
        
    case 8:
      vars[0] = vars[0] * 4.0;
      Fx = sqrt(4.0 - vars[0]) + vars[1];
      break;
        
    case 9: {
      double sum = 0;
      //      cout << "9x: " << vars[0] << " ";
      for (int i=1; i<args.GetInt(3); i++)
	sum += vars[i]/double(args.GetInt(3)-1);
      double Gx = 1+9*sum;
      Fx = Gx * (1.0 - sqrt(vars[0]/Gx));
      break;
    }
      
    case 10: {
      double sum = 0;
      for (int i=1; i<args.GetInt(3); i++)
	sum += vars[i]/double(args.GetInt(3)-1);
      double Gx = 1+9*sum;
      Fx = Gx * (1.0 - pow(vars[0]/Gx, 2.0));
      break;
    }
      
    case 11: {
      double sum = 0;
      for (int i=1; i<args.GetInt(3); i++)
	sum += vars[i]/double(args.GetInt(3)-1);
      double Gx = 1+9*sum;
      Fx = Gx * (1 - sqrt(vars[0]/Gx) - (vars[0]/Gx)*(sin(3.14159*vars[0]*10)));
      break;
    }
        
    case 12: {
      vars[0] = vars[0]*.9+.1;
      Fx = vars[0];
      break;
    }
        
    case 13: {
      vars[0] = vars[0]*.9+.1;
      vars[1] = vars[1]*5;
      Fx = (1+vars[1])/vars[0];
      break;
    }
        
    case 14: {
      vars[0] = vars[0]*6-3;
      vars[1] = vars[1]*6-3;
      Fx = .5*(vars[0]*vars[0]+vars[1]*vars[1]) + sin(vars[0]*vars[0]+vars[1]*vars[1]);
      break;
    }
        
    case 15: {
      vars[0] = vars[0]*6-3;
      vars[1] = vars[1]*6-3;
      Fx = pow((3*vars[0]-2*vars[1]+4),2)/8.0 + pow((vars[0]-vars[1]+1),2)/27.0 + 15;
      break;
    }
        
    case 16: {
      vars[0] = vars[0]*6-3;
      vars[1] = vars[1]*6-3;
      Fx = 1.0/(vars[0]*vars[0]+vars[1]*vars[1]+1) - 1.1*exp(-vars[0]*vars[0]-vars[1]*vars[1]);
      break;
    }
        
    case 17: {
      double sum = 0;
      for (int i=1; i<args.GetInt(3); i++)
	sum += (pow((vars[i]*6-3),2)-10*cos(4*3.14159*(vars[i]*6-3)))/10.0;
      double Gx = 10+sum;
      Fx = Gx * (1.0 - sqrt(vars[0]/Gx));
      break;
    }
      
    default:
      quality = .001;
    }
  }

  ctx.SetTaskValue(Fx);
  if (args.GetDouble(3) < 0.0) {
    double q1 = (args.GetDouble(1) - Fx+.001);
    double q2 = (args.GetDouble(1) - args.GetDouble(2)+.001);
    assert(q1 > 0.0);
    assert(q2 > 0.0);
    quality = q1 / q2;
  } 
  else {
    if (args.GetDouble(4) < 0.0) {
      if (Fx <= (args.GetDouble(1) - args.GetDouble(2))*args.GetDouble(3) + args.GetDouble(2)) {
        quality = 1.0;
      } else {
        quality = 0.0;
      }
    }
    else {
      if ( (Fx >= (args.GetDouble(1) - args.GetDouble(2))*args.GetDouble(3) + args.GetDouble(2)) &&
	   (Fx <= (args.GetDouble(1) - args.GetDouble(2))*args.GetDouble(4) + args.GetDouble(2)) ){
        quality = 1.0;
      } else {
        quality = 0.0;
      }
    }
  }
  
  // because want org to only have 1 shot to use outputs for all functions at once, even if they
  // output numbers that give a quality of 0 on a function, still want to mark it as completed
  // so give it a very low quality instead of 0 (if using limited resources they still will get
  // no reward because set the minimum consumed to max*.001, meaning even if they get the max
  // possible fraction they'll be below minimum allowed consumed and will consume nothing
    
  if (quality > 1) {
    cout << "\n\nquality > 1!  quality= " << quality << "  Fx= " << Fx << endl;
  }
  
  if (quality < 0.001) return .001;
  
  return quality;
}


void cTaskLib::Load_Mult(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("threshold", 0, -1);
  // Double Arguments
  schema.AddEntry("halflife", 0, cArgSchema::SCHEMA_DOUBLE);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "Multiplication", &cTaskLib::Task_Mult, 0, args);
}


double cTaskLib::Task_Mult(cTaskContext& ctx) const
{
  double quality = 0.0;
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const long long test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  
  long long diff = ((long long)INT_MAX + 1) * 2;
  
  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      if (i == j) continue;
      long long cur_diff = ::llabs((long long)(input_buffer[i] * input_buffer[j]) - test_output);
      if (cur_diff < diff) diff = cur_diff;
    }
  }
  
  int threshold = args.GetInt(0);
  
  if (threshold < 0 || diff <= threshold) { // Negative threshold == infinite
    // If within threshold range, quality decays based on absolute difference
    double halflife = -1.0 * fabs(args.GetDouble(0));
    quality = pow(2.0, static_cast<double>(diff) / halflife);
  }
  
  return quality;
}


void cTaskLib::Load_Div(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("threshold", 0, -1);
  // Double Arguments
  schema.AddEntry("halflife", 0, cArgSchema::SCHEMA_DOUBLE);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "Division", &cTaskLib::Task_Div, 0, args);
}


double cTaskLib::Task_Div(cTaskContext& ctx) const
{
  double quality = 0.0;
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const long long test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  
  long long diff = ((long long)INT_MAX + 1) * 2;
  
  for (int i = 0; i < input_size; i ++) {
    for (int j = 0; j < input_size; j ++) {
      if (i == j || input_buffer[j] == 0) continue;
      long long cur_diff = ::llabs((long long)(input_buffer[i] / input_buffer[j]) - test_output);
      if (cur_diff < diff) diff = cur_diff;
    }
  }
  
  int threshold = args.GetInt(0);
  
  if (threshold < 0 || diff <= threshold) { // Negative threshold == infinite
    // If within threshold range, quality decays based on absolute difference
    double halflife = -1.0 * fabs(args.GetDouble(0));
    quality = pow(2.0, static_cast<double>(diff) / halflife);
  }
  
  return quality;
}


void cTaskLib::Load_Log(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("threshold", 0, -1);
  // Double Arguments
  schema.AddEntry("halflife", 0, cArgSchema::SCHEMA_DOUBLE);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "Logarithm (natural)", &cTaskLib::Task_Log, 0, args);
}


double cTaskLib::Task_Log(cTaskContext& ctx) const
{
  double quality = 0.0;
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const long long test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  
  long long diff = ((long long)INT_MAX + 1) * 2;
  
  for (int i = 0; i < input_size; i ++) {
    long long cur_diff = ::llabs((long long)(log(fabs(double(input_buffer[i] ? input_buffer[i] : 1)))) - test_output);
    if (cur_diff < diff) diff = cur_diff;
  }
  
  int threshold = args.GetInt(0);
  
  if (threshold < 0 || diff <= threshold) { // Negative threshold == infinite
    // If within threshold range, quality decays based on absolute difference
    double halflife = -1.0 * fabs(args.GetDouble(0));
    quality = pow(2.0, static_cast<double>(diff) / halflife);
  }
  
  return quality;
}


void cTaskLib::Load_Log2(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("threshold", 0, -1);
  // Double Arguments
  schema.AddEntry("halflife", 0, cArgSchema::SCHEMA_DOUBLE);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "Logarithm (base-2)", &cTaskLib::Task_Log2, 0, args);
}

double cTaskLib::Task_Log2(cTaskContext& ctx) const
{
  double quality = 0.0;
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const long long test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  
  long long diff = ((long long)INT_MAX + 1) * 2;
  
  for (int i = 0; i < input_size; i ++) {
    long long cur_diff = ::llabs((long long)(log2(fabs(double(input_buffer[i] ? input_buffer[i] : 1)))) - test_output);
    if (cur_diff < diff) diff = cur_diff;
  }
  
  int threshold = args.GetInt(0);
  
  if (threshold < 0 || diff <= threshold) { // Negative threshold == infinite
    // If within threshold range, quality decays based on absolute difference
    double halflife = -1.0 * fabs(args.GetDouble(0));
    quality = pow(2.0, static_cast<double>(diff) / halflife);
  }
  
  return quality;
}


void cTaskLib::Load_Log10(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("threshold", 0, -1);
  // Double Arguments
  schema.AddEntry("halflife", 0, cArgSchema::SCHEMA_DOUBLE);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "Logarithm (base-10)", &cTaskLib::Task_Log10, 0, args);
}


double cTaskLib::Task_Log10(cTaskContext& ctx) const
{
  double quality = 0.0;
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const long long test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  
  long long diff = ((long long)INT_MAX + 1) * 2;
  
  for (int i = 0; i < input_size; i ++) {
    long long cur_diff = ::llabs((long long)(log10(fabs(double(input_buffer[i] ? input_buffer[i] : 1)))) - test_output);
    if (cur_diff < diff) diff = cur_diff;
  }
  
  int threshold = args.GetInt(0);
  
  if (threshold < 0 || diff <= threshold) { // Negative threshold == infinite
    // If within threshold range, quality decays based on absolute difference
    double halflife = -1.0 * fabs(args.GetDouble(0));
    quality = pow(2.0, static_cast<double>(diff) / halflife);
  }
  
  return quality;
}


void cTaskLib::Load_Sqrt(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("threshold", 0, -1);
  // Double Arguments
  schema.AddEntry("halflife", 0, cArgSchema::SCHEMA_DOUBLE);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "Square Root", &cTaskLib::Task_Sqrt, 0, args);
}


double cTaskLib::Task_Sqrt(cTaskContext& ctx) const
{
  double quality = 0.0;
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const long long test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  
  long long diff = ((long long)INT_MAX + 1) * 2;
  
  for (int i = 0; i < input_size; i ++) {
    long long cur_diff = ::llabs((long long)(sqrt(fabs(double(input_buffer[i])))) - test_output);
    if (cur_diff < diff) diff = cur_diff;
  }
  
  int threshold = args.GetInt(0);
  
  if (threshold < 0 || diff <= threshold) { // Negative threshold == infinite
    // If within threshold range, quality decays based on absolute difference
    double halflife = -1.0 * fabs(args.GetDouble(0));
    quality = pow(2.0, static_cast<double>(diff) / halflife);
  }
  
  return quality;
}


void cTaskLib::Load_Sine(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("threshold", 0, -1);
  // Double Arguments
  schema.AddEntry("halflife", 0, cArgSchema::SCHEMA_DOUBLE);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "Sine", &cTaskLib::Task_Sine, 0, args);
}


double cTaskLib::Task_Sine(cTaskContext& ctx) const
{
  double quality = 0.0;
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const long long test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  
  long long diff = ((long long)INT_MAX + 1) * 2;
  
  for (int i = 0; i < input_size; i ++) {
    long long cur_diff = ::llabs((long long)(sin(double(input_buffer[i]) / dCastPrecision) * dCastPrecision) - test_output);
    if (cur_diff < diff) diff = cur_diff;
  }
  
  int threshold = args.GetInt(0);
  
  if (threshold < 0 || diff <= threshold) { // Negative threshold == infinite
    // If within threshold range, quality decays based on absolute difference
    double halflife = -1.0 * fabs(args.GetDouble(0));
    quality = pow(2.0, static_cast<double>(diff) / halflife);
  }
  
  return quality;
}


void cTaskLib::Load_Cosine(const cString& name, const cString& argstr, cEnvReqs& envreqs,
			   cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("threshold", 0, -1);
  // Double Arguments
  schema.AddEntry("halflife", 0, cArgSchema::SCHEMA_DOUBLE);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "Cosine", &cTaskLib::Task_Cosine, 0, args);
}


double cTaskLib::Task_Cosine(cTaskContext& ctx) const
{
  double quality = 0.0;
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  
  const tBuffer<int>& input_buffer = ctx.GetInputBuffer();
  const long long test_output = ctx.GetOutputBuffer()[0];
  const int input_size = input_buffer.GetNumStored();
  
  long long diff = ((long long)INT_MAX + 1) * 2;
  
  for (int i = 0; i < input_size; i ++) {
    long long cur_diff = ::llabs((long long)(cos(double(input_buffer[i]) / dCastPrecision) * dCastPrecision) - test_output);
    if (cur_diff < diff) diff = cur_diff;
  }
  
  int threshold = args.GetInt(0);
  
  if (threshold < 0 || diff <= threshold) { // Negative threshold == infinite
    // If within threshold range, quality decays based on absolute difference
    double halflife = -1.0 * fabs(args.GetDouble(0));
    quality = pow(2.0, static_cast<double>(diff) / halflife);
  }
  
  return quality;
}


double cTaskLib::Task_CommEcho(cTaskContext& ctx) const
{
  const int test_output = ctx.GetOutputBuffer()[0];
  
  tConstListIterator<tBuffer<int> > buff_it(ctx.GetNeighborhoodInputBuffers());  
  
  while (buff_it.Next() != NULL) {
    const tBuffer<int>& cur_buff = *(buff_it.Get());
    const int buff_size = cur_buff.GetNumStored();
    for (int i = 0; i < buff_size; i++) {
      if (test_output == cur_buff[i]) return 1.0;
    }
  }
  
  return 0.0;
}


double cTaskLib::Task_CommNot(cTaskContext& ctx) const
{
  const int test_output = ctx.GetOutputBuffer()[0];
  
  tConstListIterator<tBuffer<int> > buff_it(ctx.GetNeighborhoodInputBuffers());  
  
  while (buff_it.Next() != NULL) {
    const tBuffer<int>& cur_buff = *(buff_it.Get());
    const int buff_size = cur_buff.GetNumStored();
    for (int i = 0; i < buff_size; i++) {
      if (test_output == (0-(cur_buff[i]+1))) return 1.0;
    }
  }
  
  return 0.0;
}


double cTaskLib::Task_NetSend(cTaskContext& ctx) const
{
  return 1.0 * ctx.GetOrganism()->NetCompleted();
}


double cTaskLib::Task_NetReceive(cTaskContext& ctx) const
{
  if (ctx.GetOrganism()->NetIsValid()) return 1.0;
  return 0.0;
}


//TODO: add movement tasks here

double cTaskLib::Task_MoveUpGradient(cTaskContext& ctx) const
{
  if(ctx.GetOrganism()->GetGradientMovement() == 1.0)
    return 1.0;
  return 0.0;
}


double cTaskLib::Task_MoveNeutralGradient(cTaskContext& ctx) const
{
  if(ctx.GetOrganism()->GetGradientMovement() == 0.0)
    return 1.0;
  return 0.0;
}


double cTaskLib::Task_MoveDownGradient(cTaskContext& ctx) const
{
  if(ctx.GetOrganism()->GetGradientMovement() == -1.0)
    return 1.0;
  return 0.0;
}


double cTaskLib::Task_MoveNotUpGradient(cTaskContext& ctx) const
{
  if(Task_MoveUpGradient(ctx))
    return 0.0;
  return 1.0;
}


double cTaskLib::Task_MoveToRightSide(cTaskContext& ctx) const
{
  cDeme* deme = ctx.GetOrganism()->GetDeme();
  std::pair<int, int> location = deme->GetCellPosition(ctx.GetOrganism()->GetCellID());
  
  if (location.first == m_world->GetConfig().WORLD_X.Get() - 1) return 1.0;
  return 0.0;
}


double cTaskLib::Task_MoveToLeftSide(cTaskContext& ctx) const
{
  cDeme* deme = ctx.GetOrganism()->GetDeme();
  std::pair<int, int> location = deme->GetCellPosition(ctx.GetOrganism()->GetCellID());
  
  if (location.first == 0) return 1.0;
  return 0.0;
}


double cTaskLib::Task_Move(cTaskContext& ctx) const
{
  if(ctx.GetOrganism()->GetCellID() != ctx.GetOrganism()->GetPrevSeenCellID()) {
    ctx.GetOrganism()->SetPrevSeenCellID(ctx.GetOrganism()->GetCellID());
    return 1.0;
  }
  
  return 0.0;

} //End cTaskLib::Task_Move()


double cTaskLib::Task_MoveToTarget(cTaskContext& ctx) const
//Note - a generic version of this is now at - Task_MoveToMovementEvent
{
  cOrganism* org = ctx.GetOrganism();
  
  if (org->GetCellID() == -1) return 0.0;		
	
  cDeme* deme = org->GetDeme();
  assert(deme);
  
  int cell_data = org->GetCellData();
  if (cell_data <= 0) return 0.0;
  
  int current_cell = deme->GetRelativeCellID(org->GetCellID());
  int prev_target = deme->GetRelativeCellID(org->GetPrevTaskCellID());
  
  // If the organism is currently on a target cell, see which target cell it previously
  // visited.  Since we want them to move back and forth, only reward if we are on
  // a different target cell.

  if (cell_data > 1) 
  {
    if (current_cell == prev_target) {
      // At some point, we may want to return a fraction
      return 0;
    } else {
      org->AddReachedTaskCell();
      org->SetPrevTaskCellID(current_cell);
      return 1.0;
    }
  }

  return 0;

} //End cTaskLib::TaskMoveToTarget()


double cTaskLib::Task_MoveToMovementEvent(cTaskContext& ctx) const
{
  cOrganism* org = ctx.GetOrganism();
  
  if (org->GetCellID() == -1) return 0.0;		
	
  cDeme* deme = org->GetDeme();
  assert(deme);
  
  int cell_data = org->GetCellData();
  if (cell_data <= 0) return 0.0;
    
  for (int i = 0; i < deme->GetNumMovementPredicates(); i++) {
    if (deme->GetMovPredicate(i)->GetEvent(0)->GetEventID() == cell_data) {
      org->AddReachedTaskCell();
      org->SetPrevTaskCellID(cell_data);
      return 1.0;
    }
  }
  return 0.0;
}


double cTaskLib::Task_MoveBetweenMovementEvent(cTaskContext& ctx) const
{	
  cOrganism* org = ctx.GetOrganism();

  if (org->GetCellID() == -1) return 0.0;
	
  cDeme* deme = org->GetDeme();
  assert(deme);

  int cell_data = org->GetCellData();
  
  int prev_target = deme->GetRelativeCellID(org->GetPrevTaskCellID());

  // NOTE: as of now, orgs aren't rewarded if they touch a target more than
  //   once in a row.  Could be useful in the future to have fractional reward
  //   or something.
  if ( (cell_data <= 0) || (cell_data == prev_target) ) return 0.0;
    
  for (int i = 0; i < deme->GetNumMovementPredicates(); i++) {
    // NOTE: having problems with calling the GetNumEvents function for some reason.  FIXME
    //int num_events = deme.GetMovPredicate(i)->GetNumEvents;
    int num_events = 2;

    if (num_events == 1) {
      if ( (deme->GetMovPredicate(i)->GetEvent(0)->IsActive()) &&
          (deme->GetMovPredicate(i)->GetEvent(0)->GetEventID() == cell_data) ) {
        org->AddReachedTaskCell();
        org->SetPrevTaskCellID(cell_data);
        return 1.0;
      }
    } else {
      for (int j = 0; j < num_events; j++) {
        cDemeCellEvent* event = deme->GetMovPredicate(i)->GetEvent(j);
        if( (event != NULL) && (event->IsActive()) && (event->GetEventID() == cell_data) ) {
          org->AddReachedTaskCell();
          org->SetPrevTaskCellID(cell_data);
          return 1.0;
        }
      }
    }
  }
  return 0.0;
}


double cTaskLib::Task_MoveToEvent(cTaskContext& ctx) const
{
  cOrganism* org = ctx.GetOrganism();
  
  if (org->GetCellID() == -1) return 0.0;
	
  cDeme* deme = org->GetDeme();
  assert(deme);
  
  int cell_data = org->GetCellData();
  if (cell_data <= 0) return 0.0;
    
  for (int i = 0; i < deme->GetNumEvents(); i++) {
    if (deme->GetCellEvent(i)->GetEventID() == cell_data) return 1.0;
  }
  return 0.0;
}


double cTaskLib::Task_EventKilled(cTaskContext& ctx) const
{
  if (ctx.GetOrganism()->GetEventKilled()) return 1.0;
  return 0.0;
}



void cTaskLib::Load_SGPathTraversal(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;

  // Integer Arguments
  schema.AddEntry("pathlen", 0, cArgSchema::SCHEMA_INT);
  
  // String Arguments
  schema.AddEntry("sgname", 0, cArgSchema::SCHEMA_STRING);
  schema.AddEntry("poison", 1, cArgSchema::SCHEMA_STRING);
  
  // Double Arguments
//  schema.AddEntry("halflife", 0, cArgSchema::SCHEMA_DOUBLE);
//  schema.AddEntry("base", 1, 2.0);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "State Grid Path Traversal", &cTaskLib::Task_SGPathTraversal, 0, args);
}


double cTaskLib::Task_SGPathTraversal(cTaskContext& ctx) const
{
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  const cStateGrid& sg = ctx.GetOrganism()->GetStateGrid();

  if (sg.GetName() != args.GetString(0)) return 0.0;

  int state = sg.GetStateID(args.GetString(1));
  if (state < 0) return 0.0;
  
  const tSmartArray<int>& ext_mem = ctx.GetExtendedMemory();
  
  // Build and sort history
  const int history_offset = 3 + sg.GetNumStates();
  tArray<int> history(ext_mem.GetSize() - history_offset);
  for (int i = 0; i < history.GetSize(); i++) history[i] = ext_mem[i + history_offset];
  tArrayUtils::QSort(history);
  
  // Calculate how many unique non-poison cells have been touched
  int traversed = 0;
  int last = -1;
  for (int i = 0; i < history.GetSize(); i++) {
    if (history[i] == last) continue;
    last = history[i];
    if (sg.GetStateAt(last) != state) traversed++;
  }
  
  traversed -= ext_mem[3 + state];
  
  double quality = 0.0;
  
//  double halflife = -1.0 * fabs(args.GetDouble(0));
//  quality = pow(args.GetDouble(1), (double)(args.GetInt(0) - ((traversed >= 0) ? traversed : 0)) / halflife);
  quality = (double)((traversed >= 0) ? traversed : 0) / (double)args.GetInt(0);
  
  return quality;
}  


/* This task provides major points for perfect strings and some points for just
   storing stuff. */
double cTaskLib::Task_CreatePerfectStrings(cTaskContext& ctx) const
{
  double bonus = 0.0;
  int min = -1;
  int temp = 0;
  for (unsigned int i = 0; i<m_strings.size(); i++) {
    temp = ctx.GetOrganism()->GetNumberStringsOnHand(i); 
    
    // Figure out what the minimum amount of a string is.
    if ((min == -1) || (temp < min)){
      min = temp;
    }
  }
  
  // Bonus for creating perfect strings!
  bonus = min; 
	
  // Add in some value for just creating stuff
  for (unsigned int i = 0; i<m_strings.size(); i++) {
    temp = ctx.GetOrganism()->GetNumberStringsOnHand(i); 
    
    if (temp > min) { 
      bonus += (temp - min); 
    }
  } 
  
  // Update stats
  m_world->GetStats().IncPerfectMatch(min);
  if (min > 0) m_world->GetStats().IncPerfectMatchOrg();
  
  return bonus; 
}


void cTaskLib::Load_FormSpatialGroup(const cString& name, const cString& argstr,
				     cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("group_size", 0, 1);
  
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "FormSpatialGroups", &cTaskLib::Task_FormSpatialGroup, 0, args);
}


double cTaskLib::Task_FormSpatialGroup(cTaskContext& ctx) const
{
  double t = (double) ctx.GetTaskEntry()->GetArguments().GetInt(0);
  double reward = 0.0;
  int group_id = 0; 
  if (ctx.GetOrganism()->HasOpinion()) {
    group_id = ctx.GetOrganism()->GetOpinion().first;
  }
  double g = (double) m_world->GetPopulation().NumberOfOrganismsInGroup(group_id);
  double num = (t-g) * (t-g);
  double denom = (t*t);
  
  reward = 1 - (num/denom);
  if (reward < 0) reward = 0;
  /*if (orgs_in_group < ideal_group_size) {
    reward = orgs_in_group*orgs_in_group;
    } else {
    reward = ideal_group_size*ideal_group_size;
    }
    reward = reward / ideal_group_size;*/
  return reward;
}


/* Reward organisms for having a given group-id, provided the group is under the 
   max number of members. */

void cTaskLib::Load_FormSpatialGroupWithID(const cString& name, const cString& argstr,
					   cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  // Integer Arguments
  schema.AddEntry("group_size", 0, 1);
  schema.AddEntry("group_id", 1, 1);
    
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "FormSpatialGroupWithID", &cTaskLib::Task_FormSpatialGroupWithID, 0, args);
  
  // Add this group id to the list in the instructions file. 
  m_world->GetEnvironment().AddGroupID(args->GetInt(1));
}


double cTaskLib::Task_FormSpatialGroupWithID(cTaskContext& ctx) const
{
  double t = (double) ctx.GetTaskEntry()->GetArguments().GetInt(0);
  int des_group_id = ctx.GetTaskEntry()->GetArguments().GetInt(1);
  
  double reward = 0.0;
  int group_id = -1; 
  if (ctx.GetOrganism()->HasOpinion()) {
    group_id = ctx.GetOrganism()->GetOpinion().first;
  }
	
  // If the organism is in the group...
  if (group_id == des_group_id) {
    double g = (double) m_world->GetPopulation().NumberOfOrganismsInGroup(group_id);
    // If the population size is less than the max size
    if (g < t) {
      reward = 1;
    } else {
      double num = (t-g) * (t-g);
      double denom = (t*t);
      
      if (denom > 0) {
	reward = 1 - (num/denom);
	if (reward < 0) reward = 0;
      } else {
	reward = 0;
      }
    }	
  }

  return reward;
}

/* Reward organisms for having a given group-id.*/
void cTaskLib::Load_LiveOnPatchRes(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  
  schema.AddEntry("patch_id", 0, 1);

  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if (args) NewTask(name, "LiveOnPatchRes", &cTaskLib::Task_LiveOnPatchRes, 0, args);
	
  // Add this patch id to the list in the instructions file. 
  m_world->GetEnvironment().AddGroupID(args->GetInt(0));
	
}

double cTaskLib::Task_LiveOnPatchRes(cTaskContext& ctx) const
{
  int des_patch_id = ctx.GetTaskEntry()->GetArguments().GetInt(0);
  
  double reward = 0.0;
  int patch_id = -1; 
  if (ctx.GetOrganism()->HasOpinion()) {
    patch_id = ctx.GetOrganism()->GetOpinion().first;
  }
  
  // If the organism is in the group...
  if (patch_id == des_patch_id) {
    reward = 1;
  }
  
  return reward;
}




void cTaskLib::Load_AllOnes(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  schema.AddEntry("length", 0, 0);		
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);	
  envreqs.SetMinOutputs(args->GetInt(0));
  if (args) NewTask(name, "all-ones", &cTaskLib::Task_AllOnes, 0, args);
}


double cTaskLib::Task_AllOnes(cTaskContext& ctx) const
{
  tBuffer<int> buf(ctx.GetOutputBuffer());
  double num_ones = 0.0;
  int length = ctx.GetTaskEntry()->GetArguments().GetInt(0);
  
  for(int i=0; i<length; ++i) {
    num_ones += buf[i];
  }
	
  return (num_ones/length);
}


void cTaskLib::Load_RoyalRoad(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  schema.AddEntry("length", 0, 0);
  schema.AddEntry("block_count", 1, 0);
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);	
  envreqs.SetMinOutputs(args->GetInt(0));
  if (args) NewTask(name, "royal-road", &cTaskLib::Task_RoyalRoad, 0, args);
}

double cTaskLib::Task_RoyalRoad(cTaskContext& ctx) const
{
  // block size
  int length = ctx.GetTaskEntry()->GetArguments().GetInt(0);
  int block_count = ctx.GetTaskEntry()->GetArguments().GetInt(1);
  int block_size = floor(double(length) / double(block_count));
  int block_reward; 
  int current_spot;
  double total_reward = 0.0;
  tBuffer<int> buf(ctx.GetOutputBuffer());
  
  // Cycle through each block. If a block is correct, then add a reward.
  for (int i=0; i<block_count; ++i) {
    block_reward = 1;
    // AND the elements of each block.
    for (int j=0; j<block_size; ++j) {
      current_spot = i*block_size + j;
      block_reward &= buf[current_spot];
    }
    
    //				if (block_reward) total_reward += (block_size);
    if (block_reward) total_reward ++;
  }
  
  return (total_reward/block_count);
}


void cTaskLib::Load_RoyalRoadWithDitches(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback)
{
  cArgSchema schema;
  schema.AddEntry("length", 0, 0);
  schema.AddEntry("block_count", 1, 0);
  schema.AddEntry("width", 2, 0);
  schema.AddEntry("height", 3, 0);
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);	
  envreqs.SetMinOutputs(args->GetInt(0));
  if (args) NewTask(name, "royal-road-wd", &cTaskLib::Task_RoyalRoadWithDitches, 0, args);
}


double cTaskLib::Task_RoyalRoadWithDitches(cTaskContext& ctx) const
{
  // block size
  int length = ctx.GetTaskEntry()->GetArguments().GetInt(0);
  int block_count = ctx.GetTaskEntry()->GetArguments().GetInt(1);
  int block_size = floor(double(length) / double(block_count));
  int block_correct;
  int num_b_blocks = 0;
  int current_spot;
  double total_reward = 0.0;
  int width = ctx.GetTaskEntry()->GetArguments().GetInt(2);
  int height = ctx.GetTaskEntry()->GetArguments().GetInt(3);
  int next_case = 1;
  int block_type = -1; // -1 undefined; 0 X; 1 A; 2 B
  tBuffer<int> buf(ctx.GetOutputBuffer());
  
  // Cycle through each block. If a block is correct, then add a reward.
  for (int i=0; i<block_count; ++i) {
    block_correct = 1;
    block_type = -1;
    
    
    // Identify the type of block... 
    // Check for block A
    for (int j=0; j<(block_size); ++j) {
      current_spot = i*block_size + j;
      block_correct &= buf[current_spot];
    } 
    
    if (block_correct) block_type = 1;
    
    // Check for block B
    if (block_type == -1) {
      block_correct = 1;
      for (int j=0; j<block_size; ++j) {
	
	current_spot = i*block_size + j;
	
	// For starter's lets just check for blocks of type B
	if (j < (block_size -width)) { 
	  if (buf[current_spot] == 0) block_correct = 0;
	} else {
	  if (buf[current_spot] == 1) block_correct = 0;
	}
	// this should escape the loop if the block reward is set to 0.
	if (block_correct == 0) break;
      }
      if (block_correct) block_type = 2;
    }

    // Else consider it an X
    if(block_type == -1) block_type = 0;
    
    // Based on the type of block... change states....
    switch(next_case){
    case 1:
      if(block_type == 0) next_case = 2;
      if(block_type == 1) {
	total_reward = num_b_blocks + 2; 
	next_case = 3; 
      } 
      if(block_type == 2) num_b_blocks++;
      break;
    case 2:
      if(block_type == 1) total_reward = num_b_blocks + 2 - height;
      next_case = 3;
      break;
    case 3: 		
      break;
    default: 
      break;
    }
  }
	
  return (total_reward/block_count);
}

	
	
//! Load the command line for checking an opinion.
void cTaskLib::Load_OpinionIs(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback) {
  cArgSchema schema;
  schema.AddEntry("opinion", 0, cArgSchema::SCHEMA_INT);
  cArgContainer* args = cArgContainer::Load(argstr, schema, feedback);
  if(args) {
    NewTask(name, "Whether organism's opinion is set to value.", &cTaskLib::Task_OpinionIs, 0, args);
  }
} 
	
//! This task is complete if this organism's current opinion is set to a configured value.
double cTaskLib::Task_OpinionIs(cTaskContext& ctx) const
{
  cOrganism* org = ctx.GetOrganism();
  const cArgContainer& args = ctx.GetTaskEntry()->GetArguments();
  int opinion = args.GetInt(0);
  
  if (org->HasOpinion()) {
    return org->GetOpinion().first == opinion;
  } else {
    return 0.0;
  }
}

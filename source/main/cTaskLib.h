/*
 *  cTaskLib.h
 *  Avida
 *
 *  Called "task_lib.hh" prior to 12/5/05.
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

#ifndef cTaskLib_h
#define cTaskLib_h

#ifndef cTaskContext_h
#include "cTaskContext.h"
#endif
#ifndef cTaskEntry_h
#include "cTaskEntry.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef cWorld_h
#include "cWorld.h"
#endif
#ifndef cStats_h
#include "cStats.h"
#endif



class cEnvReqs;
class cString;
class cWorld;


class cTaskLib
{
private:
  cWorld* m_world;
  tArray<cTaskEntry*> task_array;

  // What extra information should be sent along when we are evaluating
  // which tasks have been performed?
  bool use_neighbor_input;
  bool use_neighbor_output;

  enum req_list
  {
    REQ_NEIGHBOR_INPUT=1,
    REQ_NEIGHBOR_OUTPUT=2, 
    UNUSED_REQ_C=4,
    UNUSED_REQ_D=8
  };
  

  cTaskLib(const cTaskLib&); // @not_implemented
  cTaskLib& operator=(const cTaskLib&); // @not_implemented

public:
  cTaskLib(cWorld* world) : m_world(world), use_neighbor_input(false), use_neighbor_output(false) { ; }
  ~cTaskLib();

  int GetSize() const { return task_array.GetSize(); }

  cTaskEntry* AddTask(const cString& name, const cString& info, cEnvReqs& envreqs, cUserFeedback* feedback);
  const cTaskEntry& GetTask(int id) const { return *(task_array[id]); }
  cTaskEntry * GetTaskReference(int id) { return task_array[id]; }

  void SetupTests(cTaskContext& ctx) const;
  inline double TestOutput(cTaskContext& ctx) const { return (this->*(ctx.GetTaskEntry()->GetTestFun()))(ctx); }

  bool UseNeighborInput() const { return use_neighbor_input; }
  bool UseNeighborOutput() const { return use_neighbor_output; }
	
	// Get the strings that parameterize the MatchString tasks
	vector<cString> GetMatchStrings(); 
	cString GetMatchString(int x);
	int GetNumberOfMatchStrings() { return m_strings.size(); } 

private: 
	// Store the strings used by the MatchString tasks
	vector<cString> m_strings; 
  
  
private:
  
  void NewTask(const cString& name, const cString& desc, tTaskTest task_fun, int reqs = 0, cArgContainer* args = NULL);

  inline double FractionalReward(unsigned int supplied, unsigned int correct);  

  // All tasks must be declared here, taking a cTaskContext reference as the sole input and
  // returning a double between 0.0 and 1.0 indicating the quality of how well the task was
  // performed.

  // Basic Tasks
  double Task_Echo(cTaskContext& ctx) const;
  double Task_Add(cTaskContext& ctx) const;
  double Task_Add3(cTaskContext& ctx) const;
  double Task_Sub(cTaskContext& ctx) const;
  double Task_DontCare(cTaskContext& ctx) const;

  // All 1- and 2-Input Logic Functions
  double Task_Not(cTaskContext& ctx) const;
  double Task_Nand(cTaskContext& ctx) const;
  double Task_And(cTaskContext& ctx) const;
  double Task_OrNot(cTaskContext& ctx) const;
  double Task_Or(cTaskContext& ctx) const;
  double Task_AndNot(cTaskContext& ctx) const;
  double Task_Nor(cTaskContext& ctx) const;
  double Task_Xor(cTaskContext& ctx) const;
  double Task_Equ(cTaskContext& ctx) const;

	// resource dependent
	double Task_Nand_ResourceDependent(cTaskContext& ctx) const;
	double Task_Nor_ResourceDependent(cTaskContext& ctx) const;
			
	
  // All 3-Input Logic Functions
  double Task_Logic3in_AA(cTaskContext& ctx) const;
  double Task_Logic3in_AB(cTaskContext& ctx) const;
  double Task_Logic3in_AC(cTaskContext& ctx) const;
  double Task_Logic3in_AD(cTaskContext& ctx) const;
  double Task_Logic3in_AE(cTaskContext& ctx) const;
  double Task_Logic3in_AF(cTaskContext& ctx) const;
  double Task_Logic3in_AG(cTaskContext& ctx) const;
  double Task_Logic3in_AH(cTaskContext& ctx) const;
  double Task_Logic3in_AI(cTaskContext& ctx) const;
  double Task_Logic3in_AJ(cTaskContext& ctx) const;
  double Task_Logic3in_AK(cTaskContext& ctx) const;
  double Task_Logic3in_AL(cTaskContext& ctx) const;
  double Task_Logic3in_AM(cTaskContext& ctx) const;
  double Task_Logic3in_AN(cTaskContext& ctx) const;
  double Task_Logic3in_AO(cTaskContext& ctx) const;
  double Task_Logic3in_AP(cTaskContext& ctx) const;
  double Task_Logic3in_AQ(cTaskContext& ctx) const;
  double Task_Logic3in_AR(cTaskContext& ctx) const;
  double Task_Logic3in_AS(cTaskContext& ctx) const;
  double Task_Logic3in_AT(cTaskContext& ctx) const;
  double Task_Logic3in_AU(cTaskContext& ctx) const;
  double Task_Logic3in_AV(cTaskContext& ctx) const;
  double Task_Logic3in_AW(cTaskContext& ctx) const;
  double Task_Logic3in_AX(cTaskContext& ctx) const;
  double Task_Logic3in_AY(cTaskContext& ctx) const;
  double Task_Logic3in_AZ(cTaskContext& ctx) const;
  double Task_Logic3in_BA(cTaskContext& ctx) const;
  double Task_Logic3in_BB(cTaskContext& ctx) const;
  double Task_Logic3in_BC(cTaskContext& ctx) const;
  double Task_Logic3in_BD(cTaskContext& ctx) const;
  double Task_Logic3in_BE(cTaskContext& ctx) const;
  double Task_Logic3in_BF(cTaskContext& ctx) const;
  double Task_Logic3in_BG(cTaskContext& ctx) const;
  double Task_Logic3in_BH(cTaskContext& ctx) const;
  double Task_Logic3in_BI(cTaskContext& ctx) const;
  double Task_Logic3in_BJ(cTaskContext& ctx) const;
  double Task_Logic3in_BK(cTaskContext& ctx) const;
  double Task_Logic3in_BL(cTaskContext& ctx) const;
  double Task_Logic3in_BM(cTaskContext& ctx) const;
  double Task_Logic3in_BN(cTaskContext& ctx) const;
  double Task_Logic3in_BO(cTaskContext& ctx) const;
  double Task_Logic3in_BP(cTaskContext& ctx) const;
  double Task_Logic3in_BQ(cTaskContext& ctx) const;
  double Task_Logic3in_BR(cTaskContext& ctx) const;
  double Task_Logic3in_BS(cTaskContext& ctx) const;
  double Task_Logic3in_BT(cTaskContext& ctx) const;
  double Task_Logic3in_BU(cTaskContext& ctx) const;
  double Task_Logic3in_BV(cTaskContext& ctx) const;
  double Task_Logic3in_BW(cTaskContext& ctx) const;
  double Task_Logic3in_BX(cTaskContext& ctx) const;
  double Task_Logic3in_BY(cTaskContext& ctx) const;
  double Task_Logic3in_BZ(cTaskContext& ctx) const;
  double Task_Logic3in_CA(cTaskContext& ctx) const;
  double Task_Logic3in_CB(cTaskContext& ctx) const;
  double Task_Logic3in_CC(cTaskContext& ctx) const;
  double Task_Logic3in_CD(cTaskContext& ctx) const;
  double Task_Logic3in_CE(cTaskContext& ctx) const;
  double Task_Logic3in_CF(cTaskContext& ctx) const;
  double Task_Logic3in_CG(cTaskContext& ctx) const;
  double Task_Logic3in_CH(cTaskContext& ctx) const;
  double Task_Logic3in_CI(cTaskContext& ctx) const;
  double Task_Logic3in_CJ(cTaskContext& ctx) const;
  double Task_Logic3in_CK(cTaskContext& ctx) const;
  double Task_Logic3in_CL(cTaskContext& ctx) const;
  double Task_Logic3in_CM(cTaskContext& ctx) const;
  double Task_Logic3in_CN(cTaskContext& ctx) const;
  double Task_Logic3in_CO(cTaskContext& ctx) const;
  double Task_Logic3in_CP(cTaskContext& ctx) const;

  // Arbitrary 1-Input Math Tasks
  double Task_Math1in_AA(cTaskContext& ctx) const;
  double Task_Math1in_AB(cTaskContext& ctx) const;
  double Task_Math1in_AC(cTaskContext& ctx) const;
  double Task_Math1in_AD(cTaskContext& ctx) const;
  double Task_Math1in_AE(cTaskContext& ctx) const;
  double Task_Math1in_AF(cTaskContext& ctx) const;
  double Task_Math1in_AG(cTaskContext& ctx) const;
  double Task_Math1in_AH(cTaskContext& ctx) const;
  double Task_Math1in_AI(cTaskContext& ctx) const;
  double Task_Math1in_AJ(cTaskContext& ctx) const;
  double Task_Math1in_AK(cTaskContext& ctx) const;
  double Task_Math1in_AL(cTaskContext& ctx) const;
  double Task_Math1in_AM(cTaskContext& ctx) const;
  double Task_Math1in_AN(cTaskContext& ctx) const;
  double Task_Math1in_AO(cTaskContext& ctx) const;
  double Task_Math1in_AP(cTaskContext& ctx) const;

  // Arbitrary 2-Input Math Tasks
  double Task_Math2in_AA(cTaskContext& ctx) const;
  double Task_Math2in_AB(cTaskContext& ctx) const;
  double Task_Math2in_AC(cTaskContext& ctx) const;
  double Task_Math2in_AD(cTaskContext& ctx) const;
  double Task_Math2in_AE(cTaskContext& ctx) const;
  double Task_Math2in_AF(cTaskContext& ctx) const;
  double Task_Math2in_AG(cTaskContext& ctx) const;
  double Task_Math2in_AH(cTaskContext& ctx) const;
  double Task_Math2in_AI(cTaskContext& ctx) const;
  double Task_Math2in_AJ(cTaskContext& ctx) const;
  double Task_Math2in_AK(cTaskContext& ctx) const;
  double Task_Math2in_AL(cTaskContext& ctx) const;
  double Task_Math2in_AM(cTaskContext& ctx) const;
  double Task_Math2in_AN(cTaskContext& ctx) const;
  double Task_Math2in_AO(cTaskContext& ctx) const;
  double Task_Math2in_AP(cTaskContext& ctx) const;
  double Task_Math2in_AQ(cTaskContext& ctx) const;
  double Task_Math2in_AR(cTaskContext& ctx) const;
  double Task_Math2in_AS(cTaskContext& ctx) const;
  double Task_Math2in_AT(cTaskContext& ctx) const;
  double Task_Math2in_AU(cTaskContext& ctx) const;
  double Task_Math2in_AV(cTaskContext& ctx) const;

  // Arbitrary 3-Input Math Tasks
  double Task_Math3in_AA(cTaskContext& ctx) const;
  double Task_Math3in_AB(cTaskContext& ctx) const;
  double Task_Math3in_AC(cTaskContext& ctx) const;
  double Task_Math3in_AD(cTaskContext& ctx) const;
  double Task_Math3in_AE(cTaskContext& ctx) const;
  double Task_Math3in_AF(cTaskContext& ctx) const;
  double Task_Math3in_AG(cTaskContext& ctx) const;
  double Task_Math3in_AH(cTaskContext& ctx) const;
  double Task_Math3in_AI(cTaskContext& ctx) const;
  double Task_Math3in_AJ(cTaskContext& ctx) const;
  double Task_Math3in_AK(cTaskContext& ctx) const;
  double Task_Math3in_AL(cTaskContext& ctx) const;
  double Task_Math3in_AM(cTaskContext& ctx) const;
  
  // Matching Tasks
  void Load_MatchStr(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_MatchStr(cTaskContext& ctx) const;
	void Load_MatchProdStr(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_MatchProdStr(cTaskContext& ctx) const;
  void Load_MatchNumber(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_MatchNumber(cTaskContext& ctx) const;

  // Sequence Tasks
  void Load_SortInputs(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_SortInputs(cTaskContext& ctx) const;
  void Load_FibonacciSequence(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_FibonacciSequence(cTaskContext& ctx) const;

  // Math Tasks
  void Load_Mult(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_Mult(cTaskContext& ctx) const;
  void Load_Div(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_Div(cTaskContext& ctx) const;
  void Load_Log(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_Log(cTaskContext& ctx) const;
  void Load_Log2(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_Log2(cTaskContext& ctx) const;
  void Load_Log10(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_Log10(cTaskContext& ctx) const;
  void Load_Sqrt(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_Sqrt(cTaskContext& ctx) const;
  void Load_Sine(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_Sine(cTaskContext& ctx) const;
  void Load_Cosine(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_Cosine(cTaskContext& ctx) const;

  // Optimization Tasks
  void Load_Optimize(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_Optimize(cTaskContext& ctx) const;
  
  
  // Communication Tasks
  double Task_CommEcho(cTaskContext& ctx) const;
  double Task_CommNot(cTaskContext& ctx) const;
  
  // Network Tasks
  double Task_NetSend(cTaskContext& ctx) const;
  double Task_NetReceive(cTaskContext& ctx) const;

  // Movement tasks (temp, rely on hack)
  double Task_MoveUpGradient(cTaskContext& ctx) const;
  double Task_MoveNeutralGradient(cTaskContext& ctx) const;
  double Task_MoveDownGradient(cTaskContext& ctx) const;
  double Task_MoveNotUpGradient(cTaskContext& ctx) const;
  double Task_MoveToRightSide(cTaskContext& ctx) const;
  double Task_MoveToLeftSide(cTaskContext& ctx) const;

  // BDC Movement tasks
  double Task_Move(cTaskContext& ctx) const;
  double Task_MoveToTarget(cTaskContext& ctx) const;
  double Task_MoveToMovementEvent(cTaskContext& ctx) const;
  double Task_MoveBetweenMovementEvent(cTaskContext& ctx) const;

  // movement
  double Task_MoveToEvent(cTaskContext& ctx) const;
  double Task_EventKilled(cTaskContext& ctx) const;

  // State Grid Tasks
  void Load_SGPathTraversal(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
  double Task_SGPathTraversal(cTaskContext& ctx) const;  
	
	// reputation
	double Task_CreatePerfectStrings(cTaskContext& ctx) const; 
	
	// group formation 
	void Load_FormSpatialGroup(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
	double Task_FormSpatialGroup(cTaskContext& ctx) const; 
	void Load_FormSpatialGroupWithID(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
	double Task_FormSpatialGroupWithID(cTaskContext& ctx) const; 
	void Load_LiveOnPatchRes(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
	double Task_LiveOnPatchRes(cTaskContext& ctx) const; 
	
	// String Matching Tasks
	void Load_AllOnes(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
	double Task_AllOnes(cTaskContext& ctx) const;
	void Load_RoyalRoad(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
	double Task_RoyalRoad(cTaskContext& ctx) const;
	void Load_RoyalRoadWithDitches(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
	double Task_RoyalRoadWithDitches(cTaskContext& ctx) const;
	
	// division of labor
	void Load_OpinionIs(const cString& name, const cString& argstr, cEnvReqs& envreqs, cUserFeedback* feedback);
	double Task_OpinionIs(cTaskContext& ctx) const;
	

	
};

#endif

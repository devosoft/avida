/*
 *  cEnvironment.h
 *  Avida
 *
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

/*!  Classes to hold information about the environmentthat contains information
     about resources and reactions (which allow rewards or punishments
     to organisms doing certain tasks). */

#ifndef cEnvironment_h
#define cEnvironment_h

#include "cMutationRates.h"
#include "cReactionLib.h"
#include "cResourceLib.h"
#include "cString.h"
#include "cTaskLib.h"
#include "tArraySet.h"
#include "tList.h"

#include <set>



class cAvidaContext;
class cReaction;
class cReactionRequisite;
class cReactionProcess;
class cReactionResult;
class cStateGrid;
class cTaskContext;
class cUserFeedback;
class cWorld;
template <class T> class tArray;
template <class T> class tBuffer;


class cEnvironment
{
private:
  cWorld* m_world;
  
  cMutationRates mut_rates;
  cResourceLib resource_lib;
  cReactionLib reaction_lib;
  cTaskLib m_tasklib;

  int m_input_size;
  int m_output_size;
  bool m_true_rand;
  
  bool m_use_specific_inputs; // Use specific inputs, rather than generating random ones
  tArray<int>  m_specific_inputs;
  
  unsigned int m_mask;
  
  tArray<cStateGrid*> m_state_grids;

	std::set<int> possible_group_ids;
  
  
  cEnvironment(); // @not_implemented
  cEnvironment(const cEnvironment&); // @not_implemented
  cEnvironment& operator=(const cEnvironment&); // @not_implemented

public:
  cEnvironment(cWorld* world);
  ~cEnvironment();

  bool Load(const cString& filename, const cString& working_dir, cUserFeedback* feedback = NULL);
  bool LoadLine(cString line, cUserFeedback* feedback = NULL);  // Reads in a single environment configuration line

  // Interaction with the organisms
  void SetupInputs(cAvidaContext& ctx, tArray<int>& input_array, bool random = true) const;
  void SetSpecificInputs(const tArray<int> in_input_array) { m_use_specific_inputs = true; m_specific_inputs = in_input_array; }
  void SetSpecificRandomMask(unsigned int mask) { m_mask = mask; }
  void SwapInputs(cAvidaContext& ctx, tArray<int>& src_input_array, tArray<int>& dest_input_array) const;


  bool TestInput(cReactionResult& result, const tBuffer<int>& inputs,
                 const tBuffer<int>& outputs, const tArray<double>& resource_count) const;

  bool TestOutput(cAvidaContext& ctx, cReactionResult& result, cTaskContext& taskctx,
                  const tArray<int>& task_count, tArray<int>& reaction_count,
                  const tArray<double>& resource_count, const tArray<double>& rbins_count,
                  bool is_parasite=false) const;

  // Accessors
  int GetNumTasks() const { return m_tasklib.GetSize(); }
  const cTaskEntry& GetTask(int id) const { return m_tasklib.GetTask(id); }
  bool UseNeighborInput() const { return m_tasklib.UseNeighborInput(); }
  bool UseNeighborOutput() const { return m_tasklib.UseNeighborOutput(); }
	vector<cString> GetMatchStringsFromTask() { return m_tasklib.GetMatchStrings(); }
	cString GetMatchString(int x) { return m_tasklib.GetMatchString(x); }
	int GetNumberOfMatchStrings() { return m_tasklib.GetNumberOfMatchStrings(); }	

  
  int GetNumReactions() const { return reaction_lib.GetSize(); }
  const cResourceLib& GetResourceLib() const { return resource_lib; }
  const cReactionLib& GetReactionLib() const { return reaction_lib; }
  const cMutationRates& GetMutRates() const { return mut_rates; }

  cResourceLib& GetResourceLib() { return resource_lib; }
  cReactionLib& GetReactionLib() { return reaction_lib; }
  cMutationRates& GetMutRates() { return mut_rates; }
  
  int GetNumStateGrids() const { return m_state_grids.GetSize(); }
  const cStateGrid& GetStateGrid(int sg) const { return *m_state_grids[sg]; }  

  int GetInputSize()  const { return m_input_size; };
  int GetOutputSize() const { return m_output_size; };

  const cString& GetReactionName(int reaction_id) const;
  double GetReactionValue(int reaction_id);
  bool SetReactionValue(cAvidaContext& ctx, const cString& name, double value);
  bool SetReactionValueMult(const cString& name, double value_mult);
  bool SetReactionInst(const cString& name, cString inst_name);
  bool SetReactionMinTaskCount(const cString& name, int min_count);
  bool SetReactionMaxTaskCount(const cString& name, int max_count);
  bool SetReactionTask(const cString& name, const cString& task);
  bool SetResourceInflow(const cString& name, double _inflow );
  bool SetResourceOutflow(const cString& name, double _outflow );
	

	void AddGroupID(int new_id) { possible_group_ids.insert(new_id); }
	bool IsGroupID(int test_id);
	std::set<int> GetGroupIDs() { return possible_group_ids; }
	

private:
  
  bool ParseSetting(cString entry, cString& var_name, cString& var_value, const cString& var_type, cUserFeedback* feedback);
  bool AssertInputInt(const cString& input, const cString& name, const cString& type, cUserFeedback* feedback);
  bool AssertInputDouble(const cString& input, const cString& name, const cString& type, cUserFeedback* feedback);
  bool AssertInputBool(const cString& input, const cString& name, const cString& type, cUserFeedback* feedback);
  bool AssertInputValid(void* input, const cString& name, const cString& type, const cString& value, cUserFeedback* feedback);
  
  bool LoadReactionProcess(cReaction* reaction, cString desc, cUserFeedback* feedback);
  bool LoadReactionRequisite(cReaction* reaction, cString desc, cUserFeedback* feedback);
  bool LoadResource(cString desc, cUserFeedback* feedback);
  bool LoadCell(cString desc, cUserFeedback* feedback);
  bool LoadReaction(cString desc, cUserFeedback* feedback);
  bool LoadStateGrid(cString desc, cUserFeedback* feedback);
  bool LoadSetActive(cString desc, cUserFeedback* feedback);
  
  double GetTaskProbability(cAvidaContext& ctx, cTaskContext& taskctx, 
                            const tList<cReactionProcess>& req_proc, bool& force_mark_task) const;
  
  bool TestRequisites(const tList<cReactionRequisite>& req_list, int task_count, 
                      const tArray<int>& reaction_count, const bool on_divide = false) const;
  void DoProcesses(cAvidaContext& ctx, const tList<cReactionProcess>& process_list, 
                   const tArray<double>& resource_count, const tArray<double>& rbin_count,
                   const double task_quality, const double task_probability,
                   const int task_count, const int reaction_id, 
                   cReactionResult& result, cTaskContext& taskctx) const;
    
};

#endif

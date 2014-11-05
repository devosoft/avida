/*
 *  cEnvironment.h
 *  Avida
 *
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
#include "tList.h"

#include <set>


namespace Avida {
  class Feedback;
};
class cContextPhenotype;
class cContextReactionRequisite;
class cAvidaContext;
class cReaction;
class cReactionRequisite;
class cReactionProcess;
class cReactionResult;
class cStateGrid;
class cTaskContext;
class cWorld;
template <class T> class tBuffer;

using namespace Avida;


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
  Apto::Array<int>  m_specific_inputs;
  
  unsigned int m_mask;
  
  Apto::Array<cStateGrid*> m_state_grids;

  std::set<int> possible_group_ids;
  std::set<int> possible_target_ids;
  std::set<int> possible_habitats;
  Apto::Array<int> pp_fts;
  
  bool m_hammers;
  bool m_paths;
  
  cEnvironment(); // @not_implemented
  cEnvironment(const cEnvironment&); // @not_implemented
  cEnvironment& operator=(const cEnvironment&); // @not_implemented

public:
  cEnvironment(cWorld* world);
  ~cEnvironment();

  bool Load(const cString& filename, const cString& working_dir, Feedback& feedback, const Apto::Map<Apto::String, Apto::String>* defs = NULL);
  bool LoadLine(cString line, Feedback& feedback);  // Reads in a single environment configuration line

  // Interaction with the organisms
  void SetupInputs(cAvidaContext& ctx, Apto::Array<int>& input_array, bool random = true) const;
  void SetSpecificInputs(const Apto::Array<int> in_input_array) { m_use_specific_inputs = true; m_specific_inputs = in_input_array; }
  void SetSpecificRandomMask(unsigned int mask) { m_mask = mask; }
  void SwapInputs(cAvidaContext& ctx, Apto::Array<int>& src_input_array, Apto::Array<int>& dest_input_array) const;


  bool TestInput(cReactionResult& result, const tBuffer<int>& inputs,
                 const tBuffer<int>& outputs, const Apto::Array<double>& resource_count) const;

  bool TestOutput(cAvidaContext& ctx, cReactionResult& result, cTaskContext& taskctx,
                  const Apto::Array<int>& task_count, Apto::Array<int>& reaction_count,
                  const Apto::Array<double>& resource_count, const Apto::Array<double>& rbins_count,
                  bool is_parasite=false, cContextPhenotype* context_phenotype = 0) const;

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
  double GetReactionValue(const cString& name);
  double GetReactionValue(int reaction_id);
  bool SetReactionValue(cAvidaContext& ctx, const cString& name, double value);
  bool SetReactionValueMult(const cString& name, double value_mult);
  bool SetReactionInst(const cString& name, cString inst_name);
  bool SetReactionMinTaskCount(const cString& name, int min_count);
  bool SetReactionMaxTaskCount(const cString& name, int max_count);
  bool SetReactionMinCount(const cString& name, int reaction_min_count);
  bool SetReactionMaxCount(const cString& name, int reaction_max_count);
  bool SetReactionTask(const cString& name, const cString& task);
  bool SetResourceInflow(const cString& name, double _inflow );
  bool SetResourceOutflow(const cString& name, double _outflow );
  bool ChangeResource(cReaction* reaction, const cString& res, int process_num = 0);
	
  void AddGroupID(int new_id) { possible_group_ids.insert(new_id); }
  bool IsGroupID(int test_id);
  std::set<int> GetGroupIDs() { return possible_group_ids; }

  void AddTargetID(int new_id) { possible_target_ids.insert(new_id); SetAttackPreyFTList(); }
  bool IsTargetID(int test_id);
  std::set<int> GetTargetIDs() { return possible_target_ids; }
  void SetAttackPreyFTList();
  Apto::Array<int> GetAttackPreyFTList() { return pp_fts; }

  void AddHabitat(int new_habitat) { possible_habitats.insert(new_habitat); }
  bool IsHabitat(int test_habitat);
  bool HasHammer() { return m_hammers; }
  bool HasPath() { return m_paths; }
  std::set<int> GetHabitats() { return possible_habitats; }

private:
  
  bool ParseSetting(cString entry, cString& var_name, cString& var_value, const cString& var_type, Feedback& feedback);
  bool AssertInputInt(const cString& input, const cString& name, const cString& type, Feedback& feedback);
  bool AssertInputDouble(const cString& input, const cString& name, const cString& type, Feedback& feedback);
  bool AssertInputBool(const cString& input, const cString& name, const cString& type, Feedback& feedback);
  bool AssertInputValid(void* input, const cString& name, const cString& type, const cString& value, Feedback& feedback);
  
  bool LoadReactionProcess(cReaction* reaction, cString desc, Feedback& feedback);
  bool LoadReactionRequisite(cReaction* reaction, cString desc, Feedback& feedback);
  bool LoadContextReactionRequisite(cReaction* reaction, cString desc, Feedback& feedback); 
  bool LoadResource(cString desc, Feedback& feedback);
  bool LoadCell(cString desc, Feedback& feedback);
  bool LoadReaction(cString desc, Feedback& feedback);
  bool LoadStateGrid(cString desc, Feedback& feedback);
  bool LoadSetActive(cString desc, Feedback& feedback);
  
  bool LoadGradientResource(cString desc, Feedback& feedback);
  double GetTaskProbability(cAvidaContext& ctx, cTaskContext& taskctx,

                            const tList<cReactionProcess>& req_proc, bool& force_mark_task) const;
  
  bool TestRequisites(cTaskContext& taskctx, const cReaction* cur_reaction, int task_count,
                      const Apto::Array<int>& reaction_count, const bool on_divide = false, bool is_parasite=false) const;
  bool TestContextRequisites(const cReaction* cur_reaction, int task_count, 
                      const Apto::Array<int>& reaction_count, const bool on_divide = false) const;
  void DoProcesses(cAvidaContext& ctx, const tList<cReactionProcess>& process_list, 
                   const Apto::Array<double>& resource_count, const Apto::Array<double>& rbin_count,
                   const double task_quality, const double task_probability,
                   const int task_count, const int reaction_id, 
                   cReactionResult& result, cTaskContext& taskctx) const;
    
};

#endif

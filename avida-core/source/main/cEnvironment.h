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
#include "cTaskLib.h"


namespace Avida {
  class Feedback;
};
class cAvidaContext;
class cReaction;
class cReactionRequisite;
class cReactionProcess;
class cReactionResult;
class cTaskContext;
class cWorld;
template <class T> class tBuffer;

using namespace Avida;


class cEnvironment
{
private:
  cWorld* m_world;
  
  cMutationRates mut_rates;
  cReactionLib reaction_lib;
  cTaskLib m_tasklib;

  int m_input_size;
  int m_output_size;
  bool m_true_rand;
  
  bool m_use_specific_inputs; // Use specific inputs, rather than generating random ones
  Apto::Array<int>  m_specific_inputs;
  
  unsigned int m_mask;
  
  std::set<int> possible_target_ids;
  std::set<int> possible_habitats;

public:
  cEnvironment(cWorld* world);
  ~cEnvironment();

  bool Load(const cString& filename, const cString& working_dir, Feedback& feedback, const Apto::Map<Apto::String, Apto::String>* defs = NULL);
  bool LoadLine(cString line, Feedback& feedback);  // Reads in a single environment configuration line

  // Interaction with the organisms
  void SetupInputs(cAvidaContext& ctx, Apto::Array<int>& input_array, bool random = true) const;
  void SetSpecificInputs(const Apto::Array<int> in_input_array) { m_use_specific_inputs = true; m_specific_inputs = in_input_array; }
  void SetSpecificRandomMask(unsigned int mask) { m_mask = mask; }


  bool TestOutput(cAvidaContext& ctx, cReactionResult& result, cTaskContext& taskctx,
                  const Apto::Array<int>& task_count, Apto::Array<int>& reaction_count,
                  const Apto::Array<double>& resource_count, const Apto::Array<double>& rbins_count) const;

  // Accessors
  int GetNumTasks() const { return m_tasklib.GetSize(); }
  const cTaskEntry& GetTask(int id) const { return m_tasklib.GetTask(id); }

  
  int GetNumReactions() const { return reaction_lib.GetSize(); }
  const cReactionLib& GetReactionLib() const { return reaction_lib; }
  const cMutationRates& GetMutRates() const { return mut_rates; }

  cReactionLib& GetReactionLib() { return reaction_lib; }
  cMutationRates& GetMutRates() { return mut_rates; }
  
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
	

  void AddTargetID(int new_id) { possible_target_ids.insert(new_id); SetAttackPreyFTList(); }
  bool IsTargetID(int test_id);
  std::set<int> GetTargetIDs() { return possible_target_ids; }

  void AddHabitat(int new_habitat) { possible_habitats.insert(new_habitat); }
  bool IsHabitat(int test_habitat);
  std::set<int> GetHabitats() { return possible_habitats; }

  
private:
  bool ParseSetting(cString entry, cString& var_name, cString& var_value, const cString& var_type, Feedback& feedback);
  bool AssertInputInt(const cString& input, const cString& name, const cString& type, Feedback& feedback);
  bool AssertInputDouble(const cString& input, const cString& name, const cString& type, Feedback& feedback);
  bool AssertInputBool(const cString& input, const cString& name, const cString& type, Feedback& feedback);
  bool AssertInputValid(void* input, const cString& name, const cString& type, const cString& value, Feedback& feedback);
  
  bool LoadReactionProcess(cReaction* reaction, cString desc, Feedback& feedback);
  bool LoadReactionRequisite(cReaction* reaction, cString desc, Feedback& feedback);
  bool LoadResource(cString desc, Feedback& feedback);
  bool LoadCell(cString desc, Feedback& feedback);
  bool LoadReaction(cString desc, Feedback& feedback);
  bool LoadSetActive(cString desc, Feedback& feedback);
  
  bool LoadDynamicResource(cString desc, Feedback& feedback);
  
  bool TestRequisites(cTaskContext& taskctx, const cReaction* cur_reaction, int task_count,
                      const Apto::Array<int>& reaction_count, const bool on_divide = false) const;
  void DoProcesses(cAvidaContext& ctx, const tList<cReactionProcess>& process_list, 
                   const Apto::Array<double>& resource_count, const Apto::Array<double>& rbin_count,
                   const double task_quality,
                   const int task_count, const int reaction_id, 
                   cReactionResult& result, cTaskContext& taskctx) const;
    
};

#endif

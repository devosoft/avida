/*
 *  cEnvironment.h
 *  Avida
 *
 *  Called "environment.hh" prior to 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cEnvironment_h
#define cEnvironment_h

#ifndef cInstSet_h
#include "cInstSet.h"
#endif
#ifndef cMutationLib_h
#include "cMutationLib.h"
#endif
#ifndef cMutationRates_h
#include "cMutationRates.h"
#endif
#ifndef cReactionLib_h
#include "cReactionLib.h"
#endif
#ifndef cResourceLib_h
#include "cResourceLib.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef cTaskLib_h
#include "cTaskLib.h"
#endif
#ifndef defs_h
#include "defs.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif


class cAvidaContext;
class cReaction;
class cReactionRequisite;
template <class T> class tArray;
class cReactionProcess;
class cReactionResult;
template <class T> class tBuffer;
class cTaskContext;
class cWorld;

class cEnvironment {
#if USE_tMemTrack
  tMemTrack<cEnvironment> mt;
#endif
private:
  cWorld* m_world;
  
  // Keep libraries of resources, reactions, and tasks.
  cResourceLib resource_lib;
  cReactionLib reaction_lib;
  cMutationLib mutation_lib;
  cTaskLib m_tasklib;
  cInstSet inst_set;
  cMutationRates mut_rates;

  int m_input_size;
  
  
  static bool ParseSetting(cString entry, cString& var_name, cString& var_value, const cString& var_type);
  static bool AssertInputInt(const cString& input, const cString& name, const cString& type);
  static bool AssertInputDouble(const cString& input, const cString& name, const cString& type);
  static bool AssertInputBool(const cString& input, const cString& name, const cString& type);
  static bool AssertInputValid(void* input, const cString& name, const cString& type, const cString& value);

  bool LoadReactionProcess(cReaction * reaction, cString desc);
  bool LoadReactionRequisite(cReaction * reaction, cString desc);
  bool LoadResource(cString desc);
  bool LoadCell(cString desc);
  bool LoadReaction(cString desc);
  bool LoadMutation(cString desc);

  bool LoadSetActive(cString desc);

  bool TestRequisites(const tList<cReactionRequisite>& req_list, int task_count, const tArray<int>& reaction_count, const bool on_divide = false) const;
  void DoProcesses(cAvidaContext& ctx, const tList<cReactionProcess>& process_list, const tArray<double>& resource_count,
                   const double task_quality, const int task_count, cReactionResult& result) const;

  cEnvironment(); // @not_implemented
  cEnvironment(const cEnvironment&); // @not_implemented
  cEnvironment& operator=(const cEnvironment&); // @not_implemented

public:
  cEnvironment(cWorld* world) : m_world(world), m_tasklib(world), inst_set(world), m_input_size(INPUT_SIZE_DEFAULT) { mut_rates.Setup(world); }
  ~cEnvironment() { ; }

  bool Load(const cString& filename);  // Reads the environment from disk.
  bool LoadLine(cString line);  // Reads in a single environment configuration line

  // Interaction with the organisms
  void SetupInputs(cAvidaContext& ctx, tArray<int>& input_array) const;

  bool TestInput(cReactionResult& result, const tBuffer<int>& inputs,
                 const tBuffer<int>& outputs, const tArray<double>& resource_count ) const;

  bool TestOutput(cAvidaContext& ctx, cReactionResult& result, cTaskContext& taskctx,
                  const tArray<int>& task_count, const tArray<int>& reaction_count,
                  const tArray<double>& resource_count) const;

  // Accessors
  const cResourceLib& GetResourceLib() const { return resource_lib; }
  const cReactionLib& GetReactionLib() const { return reaction_lib; }
  const cMutationLib& GetMutationLib() const { return mutation_lib; }
  const cTaskLib& GetTaskLib() const { return m_tasklib; }
  const cMutationRates& GetMutRates() const { return mut_rates; }

  cResourceLib& GetResourceLib() { return resource_lib; }
  cReactionLib& GetReactionLib() { return reaction_lib; }
  cMutationRates& GetMutRates() { return mut_rates; }
  
  int GetInputSize() { return m_input_size; }

  double GetReactionValue(int& reaction_id);
  bool SetReactionValue(cAvidaContext& ctx, const cString& name, double value);
  bool SetReactionValueMult(const cString& name, double value_mult);
  bool SetReactionInst(const cString& name, cString inst_name);
};


#ifdef ENABLE_UNIT_TESTS
namespace nEnvironment {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

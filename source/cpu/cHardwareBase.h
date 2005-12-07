/*
 *  cHardwareBase.h
 *  Avida
 *
 *  Created by David on 11/17/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cHardwareBase_h
#define cHardwareBase_h

#include <iostream>

using namespace std;

class cCodeLabel;
class cCPUMemory;
class cGenome;
class cHardwareTracer;
class cHeadCPU;
class cInjectGenotype;
class cInstruction;
class cInstSet;
class cOrganism;
class cString;
class cWorld;

class cHardwareBase {
protected:
  cWorld* m_world;
  cOrganism* organism;       // Organism using this hardware.
  cInstSet* m_inst_set;        // Instruction set being used.
  cHardwareTracer* m_tracer; // Set this if you want execution traced.

public:
  cHardwareBase(cWorld* world, cOrganism* in_organism, cInstSet* inst_set)
    : m_world(world), organism(in_organism), m_inst_set(inst_set), m_tracer(NULL)
  {
    assert(organism != NULL);
  }
  virtual ~cHardwareBase() { ; }

  // --------  Organism ---------
  cOrganism* GetOrganism() { return organism; }
  const cInstSet& GetInstSet() { return *m_inst_set; }

  
  // --------  Core Functionality  --------
  virtual void Reset() = 0;
  virtual void SingleProcess() = 0;
  virtual void ProcessBonusInst(const cInstruction& inst) = 0;
  
  
  // --------  Helper methods  --------
  virtual int GetType() const = 0;
  virtual bool OK() = 0;
  virtual void PrintStatus(std::ostream& fp) = 0;
  void SetTrace(cHardwareTracer* tracer) { m_tracer = tracer; }
  
  
  // --------  Stack Manipulation...  --------
  virtual int GetStack(int depth = 0, int stack_id = -1, int in_thread = -1) const = 0;
  
  
  // --------  Head Manipulation (including IP)  --------
  virtual const cHeadCPU& GetHead(int head_id) const = 0;
  virtual cHeadCPU& GetHead(int head_id) = 0;
  virtual const cHeadCPU& GetHead(int head_id, int thread) const = 0;
  virtual cHeadCPU& GetHead(int head_id, int thread) = 0;
  
  virtual const cHeadCPU& IP() const = 0;
  virtual cHeadCPU& IP() = 0;
  virtual const cHeadCPU& IP(int thread) const = 0;
  virtual cHeadCPU& IP(int thread) = 0;
  
  
  // --------  Label Manipulation  -------
  virtual const cCodeLabel& GetLabel() const = 0;
  virtual cCodeLabel& GetLabel() = 0;
  
  
  // --------  Memory Manipulation  --------
  virtual const cCPUMemory& GetMemory() const = 0;
  virtual cCPUMemory& GetMemory() = 0;
  virtual const cCPUMemory& GetMemory(int value) const = 0;
  virtual cCPUMemory& GetMemory(int value) = 0;
  
  
  // --------  Register Manipulation  --------
  virtual const int GetRegister(int reg_id) const = 0;
  virtual int& GetRegister(int reg_id) = 0;
  
  
  // --------  Thread Manipulation  --------
  virtual bool ForkThread() = 0;
  virtual bool KillThread() = 0;
  virtual void PrevThread() = 0;
  virtual void NextThread() = 0;
  virtual void SetThread(int value) = 0;
  virtual cInjectGenotype* GetCurThreadOwner() = 0;
  virtual cInjectGenotype* GetThreadOwner(int in_thread) = 0;
  virtual void SetThreadOwner(cInjectGenotype* in_genotype) = 0;
  
  
  // --------  Parasite Stuff  --------
  virtual int TestParasite() const = 0;
  virtual bool InjectHost(const cCodeLabel& in_label, const cGenome& injection) = 0;
  virtual int InjectThread(const cCodeLabel& in_label, const cGenome& injection) = 0;
  
  
  // --------  Accessors  --------
  virtual int GetNumThreads() const = 0;
  virtual int GetCurThread() const = 0;
  virtual int GetCurThreadID() const = 0;
  virtual int GetThreadDist() const = 0;
  
  
  // --------  Mutation  --------
  virtual int PointMutate(const double mut_rate) = 0;
  virtual bool TriggerMutations(int trigger) = 0;
  virtual bool TriggerMutations(int trigger, cHeadCPU& cur_head) = 0;
  
  
protected:
  // --------  No-Operation Instruction --------
  bool Inst_Nop();  // A no-operation instruction that does nothing! 
};

#endif

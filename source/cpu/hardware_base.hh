//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_BASE_HH
#define HARDWARE_BASE_HH

#include <iostream>

#ifndef INSTRUCTION_HH
#include "instruction.hh"
#endif

using namespace std;

class cCodeLabel;
class cCPUMemory;
class cGenome;
class cInstruction; // aggregate
class cInstSet;
class cOrganism;

class cHardwareBase {
protected:
  cOrganism * organism;       // Organism using this hardware.
  cInstSet * inst_set;        // Instruction set being used.
  int viewer_lock;            // Used if the viewer should only lock onto
                              //  one aspect of the hardware.

  ostream * trace_fp;         // Set this if you want execution traced.

  static int instance_count;
public:
  cHardwareBase(cOrganism * in_organism, cInstSet * in_inst_set);
  virtual ~cHardwareBase();
  virtual void Recycle(cOrganism * new_organism, cInstSet * in_inst_set);

  // --------  Organism ---------
  cOrganism * GetOrganism() { return organism; }

  // --------  Instruction Library  --------
  const cInstSet & GetInstSet() { return *inst_set; }
  int GetNumInst();
  cInstruction GetRandomInst();
  virtual void SetInstSet(cInstSet & in_inst_set) { inst_set = &in_inst_set; }

  // --------  No-Operation Instruction --------
  bool Inst_Nop();  // A no-operation instruction that does nothing! 

  // --------  Interaction with Viewer --------
  int & ViewerLock() { return viewer_lock; }

  // --------  Core Functionality  --------
  virtual void Reset() = 0;
  virtual void SingleProcess() = 0;
  virtual void ProcessBonusInst(const cInstruction & inst) = 0;
  virtual void LoadGenome(const cGenome & new_genome) = 0;
  virtual bool OK() = 0;

  // --------  Other Virtual Tools --------
  virtual int GetType() const = 0;
  virtual bool InjectHost(const cCodeLabel & in_label,
		      const cGenome & injection) = 0;
  virtual int InjectThread(const cCodeLabel & in_label,
			   const cGenome & injection) = 0;

  // --------  Input and Output --------
  virtual void PrintStatus(std::ostream & fp) = 0;
  virtual void SaveState(std::ostream & fp) = 0;
  virtual void LoadState(std::istream & fp) = 0;

  void SetTrace(ostream * in_fp) { trace_fp = in_fp; }


  // --------  Mutations (Must be Virtual)  --------
  virtual int PointMutate(const double mut_rate) = 0;
  virtual bool TriggerMutations(int trigger) = 0;

  // --------  @CAO Should be rethought?  --------
  virtual cCPUMemory & GetMemory() = 0;
  virtual cCPUMemory & GetMemory(int) = 0;

  // --------  DEBUG ---------
  static int GetInstanceCount() { return instance_count; }
};

#endif

/*
 *  cOrganism.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cOrganism_h
#define cOrganism_h

#include <fstream>
#include <deque>

#ifndef cCPUMemory_h
#include "cCPUMemory.h"
#endif
#ifndef sCPUStats_h
#include "sCPUStats.h"
#endif
#ifndef cGenome_h
#include "cGenome.h"
#endif
#ifndef cLocalMutations_h
#include "cLocalMutations.h"
#endif
#ifndef cMutationRates_h
#include "cMutationRates.h"
#endif
#ifndef cPhenotype_h
#include "cPhenotype.h"
#endif
#ifndef cOrgInterface_h
#include "cOrgInterface.h"
#endif
#ifndef tBuffer_h
#include "tBuffer.h"
#endif

/**
 * The cOrganism class controls the running and manages all the statistics
 * about a creature.  Effectively the chemistry acting on the genome.
 **/

class cAvidaContext;
class cHardwareBase;
class cGenotype;
class cInjectGenotype;
class cLineage;
class cOrgMessage;
class cEnvironment;
class cCodeLabel;

class cOrganism
{
protected:
  cWorld* m_world;
  cHardwareBase* hardware;  // The actual machinary running this organism.
  cGenotype* genotype;      // Information about organisms with this genome.
  cPhenotype phenotype;      // Descriptive attributes of organism.
  const cGenome initial_genome;        // Initial genome; can never be changed!
  std::deque<cInjectGenotype*> parasites; // List of all parasites associated with
                                    // this organism.
  cMutationRates mut_rates;            // Rate of all possible mutations.
  cLocalMutations mut_info;            // Info about possible mutations;
  cOrgInterface* m_interface;  // Interface back to the population.

  // Input and Output with the environment
  int input_pointer;
  tBuffer<int> input_buf;
  tBuffer<int> output_buf;
  tBuffer<int> send_buf;
  tBuffer<int> receive_buf;

  // Communication
  int sent_value;         // What number is this org sending?
  bool sent_active;       // Has this org sent a number?
  int test_receive_pos;   // In a test CPU, what message to receive next?

  // Other stats
  cCPUMemory child_genome; // Child genome, while under construction.
  sCPUStats cpu_stats;     // Info for statistics

  int max_executed;      // Max number of instruction executed before death.

  int lineage_label;     // a lineages tag; inherited unchanged in offspring
  cLineage* lineage;    // A lineage descriptor... (different from label)

  tBuffer<cOrgMessage> inbox;
  tBuffer<cOrgMessage> sent;
  
  //tBuffer<cOrgMessage*> m_net_pending;
  

#ifdef DEBUG
  bool initialized;      // Has this CPU been initialized yet, w/hardware.
#endif
  bool is_running;       // Does this organism have the CPU?


  cOrganism(); // @not_implemented
  cOrganism(const cOrganism&); // @not_implemented
  cOrganism& operator=(const cOrganism&); // @not_implemented
  
public:
  cOrganism(cWorld* world, cAvidaContext& ctx, const cGenome& in_genome);
  ~cOrganism();

  cHardwareBase& GetHardware() { return *hardware; }
  cOrganism* GetNeighbor() { assert(m_interface); return m_interface->GetNeighbor(); }
  int GetNeighborhoodSize() { assert(m_interface); return m_interface->GetNumNeighbors(); }
  void Rotate(int direction) { assert(m_interface); m_interface->Rotate(direction); }
  void DoBreakpoint() { assert(m_interface); m_interface->Breakpoint(); }
  int GetNextInput() { assert(m_interface); return m_interface->GetInputAt(input_pointer); }
  void Die() { assert(m_interface); m_interface->Die(); }
  void Kaboom() { assert(m_interface); m_interface->Kaboom();}
  int GetCellID() { assert(m_interface); return m_interface->GetCellID(); }
  int GetDebugInfo() { assert(m_interface); return m_interface->Debug(); }

  bool GetSentActive() { return sent_active; }
  void SendValue(int value) { sent_active = true; sent_value = value; }
  int RetrieveSentValue() { sent_active = false; return sent_value; }
  int ReceiveValue();

  void UpdateMerit(double new_merit) { assert(m_interface); m_interface->UpdateMerit(new_merit); }
  
  // Input & Output Testing
  void DoInput(const int value);
  void DoOutput(cAvidaContext& ctx, const int value);

  // Message stuff
  void SendMessage(cOrgMessage & mess);
  bool ReceiveMessage(cOrgMessage & mess);

  bool InjectParasite(const cGenome & genome);
  bool InjectHost(const cCodeLabel & in_label, const cGenome & genome);
  void AddParasite(cInjectGenotype * cur);
  cInjectGenotype & GetParasite(int x);
  int GetNumParasites();
  void ClearParasites();
		      
  int OK();

  double GetTestFitness();
  double CalcMeritRatio();

  cCPUMemory & ChildGenome() { return child_genome; }
  sCPUStats & CPUStats() { return cpu_stats; }

  bool TestCopyMut(cAvidaContext& ctx) const { return MutationRates().TestCopyMut(ctx); }
  bool TestDivideMut(cAvidaContext& ctx) const { return MutationRates().TestDivideMut(ctx); }
  bool TestDivideIns(cAvidaContext& ctx) const { return MutationRates().TestDivideIns(ctx); }
  bool TestDivideDel(cAvidaContext& ctx) const { return MutationRates().TestDivideDel(ctx); }
  bool TestParentMut(cAvidaContext& ctx) const { return MutationRates().TestParentMut(ctx); }
  bool TestCrossover(cAvidaContext& ctx) const { return MutationRates().TestCrossover(ctx); }
  bool TestAlignedCrossover(cAvidaContext& ctx) const { return MutationRates().TestAlignedCrossover(ctx); }
  
  double GetCopyMutProb() const { return MutationRates().GetCopyMutProb(); }
  void SetCopyMutProb(double _p) { return MutationRates().SetCopyMutProb(_p); }
  void SetDivMutProb(double _p) { return MutationRates().SetDivMutProb(_p); }

  double GetInsMutProb() const { return MutationRates().GetInsMutProb(); }
  double GetDelMutProb() const { return MutationRates().GetDelMutProb(); }
  double GetDivMutProb() const { return MutationRates().GetDivMutProb(); }
  double GetParentMutProb() const { return MutationRates().GetParentMutProb();}


  bool GetTestOnDivide() const;
  bool GetFailImplicit() const;

  bool GetRevertFatal() const;
  bool GetRevertNeg() const;
  bool GetRevertNeut() const;
  bool GetRevertPos() const;

  bool GetSterilizeFatal() const;
  bool GetSterilizeNeg() const;
  bool GetSterilizeNeut() const;
  bool GetSterilizePos() const;


  // Access to private variables
  int GetMaxExecuted() const { return max_executed; }

  void SetLineageLabel( int in_label ) { lineage_label = in_label; }
  int GetLineageLabel() const { return lineage_label; }
  
  void SetLineage( cLineage * in_lineage ) { lineage = in_lineage; }
  cLineage * GetLineage() const { return lineage; }

  void SetGenotype(cGenotype * in_genotype) { genotype = in_genotype; }
  cGenotype * GetGenotype() const { return genotype; }

  const cMutationRates & MutationRates() const { return mut_rates; }
  cMutationRates & MutationRates() { return mut_rates; }
  const cLocalMutations & GetLocalMutations() const { return mut_info; }
  cLocalMutations & GetLocalMutations() { return mut_info; }
  
  const cOrgInterface& GetOrgInterface() const { assert(m_interface); return *m_interface; }
  cOrgInterface& GetOrgInterface() { assert(m_interface); return *m_interface; }
  void SetOrgInterface(cOrgInterface* interface);
  
  const cGenome& GetGenome() const { return initial_genome; }
  
  /*
  int GetCurGestation() const;
  */
  const cPhenotype & GetPhenotype() const { return phenotype; }
  cPhenotype & GetPhenotype() { return phenotype; }

  // --------  DEBUG ---------
  void SetRunning(bool in_running) { is_running = in_running; }
  bool GetIsRunning() { return is_running; }


  void PrintStatus(std::ostream& fp, const cString & next_name);
  
  // Divide functions
  bool Divide_CheckViable();
  bool ActivateDivide(cAvidaContext& ctx);
  
  // Other Special Functions
  void Fault(int fault_loc, int fault_type, cString fault_desc="");
  
};

#endif


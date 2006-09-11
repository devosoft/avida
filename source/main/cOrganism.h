/*
 *  cOrganism.h
 *  Avida
 *
 *  Called "organism.hh" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cOrganism_h
#define cOrganism_h

#include <fstream>

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
#ifndef cOrgSeqMessage_h
#include "cOrgSeqMessage.h"
#endif
#ifndef cOrgSourceMessage_h
#include "cOrgSourceMessage.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tBuffer_h
#include "tBuffer.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif
#ifndef tSmartArray_h
#include "tSmartArray.h"
#endif
#ifndef cSaleItem_h
#include "cSaleItem.h"
#endif

/**
 * The cOrganism class controls the running and manages all the statistics
 * about a creature.  Effectively the chemistry acting on the genome.
 **/

class cAvidaContext;
class cCodeLabel;
class cHardwareBase;
class cGenotype;
class cInjectGenotype;
class cLineage;
class cOrgMessage;
class cOrgSinkMessage;
class cEnvironment;
class cCodeLabel;

class cOrganism
{
protected:
  cWorld* m_world;
  cHardwareBase* m_hardware;            // The actual machinary running this organism.
  cGenotype* genotype;                  // Information about organisms with this genome.
  cPhenotype phenotype;                 // Descriptive attributes of organism.
  const cGenome initial_genome;         // Initial genome; can never be changed!
  tArray<cInjectGenotype*> m_parasites; // List of all parasites associated with this organism.
  cMutationRates mut_rates;             // Rate of all possible mutations.
  cLocalMutations mut_info;             // Info about possible mutations;
  cOrgInterface* m_interface;           // Interface back to the population.
  int m_id;								// unique id for each org, is just the number it was born

  // Input and Output with the environment
  int input_pointer;
  tBuffer<int> input_buf;
  tBuffer<int> output_buf;
  tBuffer<int> send_buf;
  tBuffer<int> receive_buf;
  tBuffer<int> received_messages;
  tList<tListNode<cSaleItem> > sold_items;

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
  
  class cNetSupport
  {
  public:
    tList<cOrgSinkMessage> pending;
    tSmartArray<cOrgSinkMessage*> received;
    tSmartArray<cOrgSourceMessage> sent;
    tSmartArray<cOrgSeqMessage> seq; 
    int last_seq;
    
    cNetSupport() : last_seq(0) { ; }
    ~cNetSupport();
  };
  cNetSupport* m_net;

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

  cHardwareBase& GetHardware() { return *m_hardware; }
  cOrganism* GetNeighbor() { assert(m_interface); return m_interface->GetNeighbor(); }
  int GetNeighborhoodSize() { assert(m_interface); return m_interface->GetNumNeighbors(); }
  void Rotate(int direction) { assert(m_interface); m_interface->Rotate(direction); }
  void DoBreakpoint() { assert(m_interface); m_interface->Breakpoint(); }
  int GetNextInput() { assert(m_interface); return m_interface->GetInputAt(input_pointer); }
  void Die() { assert(m_interface); m_interface->Die(); }
  void Kaboom(int dist) { assert(m_interface); m_interface->Kaboom(dist);}
  int GetCellID() { assert(m_interface); return m_interface->GetCellID(); }
  int GetDebugInfo() { assert(m_interface); return m_interface->Debug(); }
  int GetID() { return m_id; }
  bool GetSentActive() { return sent_active; }
  void SendValue(int value) { sent_active = true; sent_value = value; }
  int RetrieveSentValue() { sent_active = false; return sent_value; }
  int ReceiveValue();
  void SellValue(const int data, const int label, const int sell_price);
  int BuyValue(const int label, const int buy_price);
  tListNode<tListNode<cSaleItem> >* AddSoldItem(tListNode<cSaleItem>* );
  tList<tListNode<cSaleItem> >* GetSoldItems() { return &sold_items; }
  void UpdateMerit(double new_merit) { assert(m_interface); m_interface->UpdateMerit(new_merit); }
  
  // Input & Output Testing
  void DoInput(const int value);
  void DoOutput(cAvidaContext& ctx, const int value);

  // Message stuff
  void SendMessage(cOrgMessage & mess);
  bool ReceiveMessage(cOrgMessage & mess);
  
  // Network Stuff
  void NetGet(cAvidaContext& ctx, int& value, int& seq);
  void NetSend(cAvidaContext& ctx, int value);
  cOrgSinkMessage* NetPop() { return m_net->pending.PopRear(); }
  bool NetReceive(int& value);
  bool NetValidate(cAvidaContext& ctx, int value);
  bool NetRemoteValidate(cAvidaContext& ctx, int value);
  int NetLast() { return m_net->last_seq; }
  void NetReset();

  bool InjectParasite(const cGenome& genome);
  bool InjectHost(const cCodeLabel& in_label, const cGenome& genome);
  void AddParasite(cInjectGenotype* cur) { m_parasites.Push(cur); }
  cInjectGenotype& GetParasite(int x) { return *m_parasites[x]; }
  int GetNumParasites() { return m_parasites.GetSize(); }
  void ClearParasites() { m_parasites.Resize(0); }
		      
  int OK();

  double GetTestFitness(cAvidaContext& ctx);
  double CalcMeritRatio();

  cCPUMemory& ChildGenome() { return child_genome; }
  sCPUStats& CPUStats() { return cpu_stats; }

  bool TestCopyMut(cAvidaContext& ctx) const { return MutationRates().TestCopyMut(ctx); }
  bool TestDivideMut(cAvidaContext& ctx) const { return MutationRates().TestDivideMut(ctx); }
  bool TestDivideIns(cAvidaContext& ctx) const { return MutationRates().TestDivideIns(ctx); }
  bool TestDivideDel(cAvidaContext& ctx) const { return MutationRates().TestDivideDel(ctx); }
  bool TestParentMut(cAvidaContext& ctx) const { return MutationRates().TestParentMut(ctx); }
  
  double GetCopyMutProb() const { return MutationRates().GetCopyMutProb(); }
  void SetCopyMutProb(double _p) { return MutationRates().SetCopyMutProb(_p); }
  void SetDivMutProb(double _p) { return MutationRates().SetDivMutProb(_p); }

  double GetInsMutProb() const { return MutationRates().GetInsMutProb(); }
  double GetDelMutProb() const { return MutationRates().GetDelMutProb(); }
  double GetDivMutProb() const { return MutationRates().GetDivMutProb(); }
  double GetParentMutProb() const { return MutationRates().GetParentMutProb();}

  double GetInjectInsProb() const { return MutationRates().GetInjectInsProb(); }
  double GetInjectDelProb() const { return MutationRates().GetInjectDelProb(); }
  double GetInjectMutProb() const { return MutationRates().GetInjectMutProb(); }
  

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
  double GetNeutralMin() const;
  double GetNeutralMax() const;

  // Access to private variables
  int GetMaxExecuted() const { return max_executed; }

  void SetLineageLabel( int in_label ) { lineage_label = in_label; }
  int GetLineageLabel() const { return lineage_label; }
  
  void SetLineage( cLineage * in_lineage ) { lineage = in_lineage; }
  cLineage * GetLineage() const { return lineage; }

  void SetGenotype(cGenotype * in_genotype) { genotype = in_genotype; }
  cGenotype * GetGenotype() const { return genotype; }

  const cMutationRates& MutationRates() const { return mut_rates; }
  cMutationRates& MutationRates() { return mut_rates; }
  const cLocalMutations& GetLocalMutations() const { return mut_info; }
  cLocalMutations& GetLocalMutations() { return mut_info; }
  
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


#ifdef ENABLE_UNIT_TESTS
namespace nOrganism {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif


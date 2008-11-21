/*
 *  cOrganism.h
 *  Avida
 *
 *  Called "organism.hh" prior to 12/5/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

#ifndef cOrganism_h
#define cOrganism_h

#include <iostream>
#include <string>
#include <vector>
#include <deque>

#ifndef cCPUMemory_h
#include "cCPUMemory.h"
#endif
//#ifndef sCPUStats_h
//#include "sCPUStats.h"
//#endif
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
#ifndef cOrgMessage_h
#include "cOrgMessage.h"
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


class cAvidaContext;
class cCodeLabel;
class cEnvironment;
class cGenotype;
class cHardwareBase;
class cInjectGenotype;
class cInstSet;
class cLineage;
class cOrgSinkMessage;
class cSaleItem;



class cOrganism
{
protected:
  cWorld* m_world;
  cHardwareBase* m_hardware;              // The actual machinery running this organism.
  cGenotype* m_genotype;                  // Information about organisms with this genome.
  cPhenotype m_phenotype;                 // Descriptive attributes of organism.
  const cGenome m_initial_genome;         // Initial genome; can never be changed!
  tArray<cInjectGenotype*> m_parasites;   // List of all parasites associated with this organism.
  cMutationRates m_mut_rates;             // Rate of all possible mutations.
  cLocalMutations m_mut_info;             // Info about possible mutations;
  cOrgInterface* m_interface;             // Interface back to the population.
  int m_id;                               // unique id for each org, is just the number it was born
  int m_lineage_label;                    // a lineages tag; inherited unchanged in offspring
  cLineage* m_lineage;                    // A lineage descriptor... (different from label)
	int cclade_id;				                  // @MRR Coalescence clade information (set in cPopulation)
  tArray<double> m_rbins;				          // Holds amount of resources internal to org
  																				// Note: m_rbins gets its proper size in SetOrgInterface()
  
	// Other stats
  cCPUMemory m_child_genome;              // Child genome, while under construction.

  // Input and Output with the environment
  int m_input_pointer;
  tBuffer<int> m_input_buf;
  tBuffer<int> m_output_buf;
  tBuffer<int> m_received_messages;
  tList<tListNode<cSaleItem> > m_sold_items;

  // Communication
  int m_sent_value;         // What number is this org sending?
  bool m_sent_active;       // Has this org sent a number?
  int m_test_receive_pos;   // In a test CPU, what message to receive next?

  double m_gradient_movement;  // TEMP.  Remove once movement tasks are implemented.
  bool m_pher_drop;	   // Is the organism dropping pheromone?

  int m_max_executed;      // Max number of instruction executed before death.  
  bool m_is_running;       // Does this organism have the CPU?
  bool m_is_sleeping;      // Is this organisms sleeping?
  bool m_is_dead;          // Is this organism dead?

  bool killed_event;
  
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
  
  
  void initialize(cAvidaContext& ctx);
  
  cOrganism(); // @not_implemented
  cOrganism(const cOrganism&); // @not_implemented
  cOrganism& operator=(const cOrganism&); // @not_implemented
  
public:
  cOrganism(cWorld* world, cAvidaContext& ctx, const cGenome& in_genome);
  cOrganism(cWorld* world, cAvidaContext& ctx, const cGenome& in_genome, cInstSet* inst_set);
  ~cOrganism();

  // --------  Accessor Methods  --------
  void SetGenotype(cGenotype* in_genotype) { m_genotype = in_genotype; }
  cGenotype* GetGenotype() const { return m_genotype; }
  const cPhenotype& GetPhenotype() const { return m_phenotype; }
  cPhenotype& GetPhenotype() { return m_phenotype; }
  void SetPhenotype(cPhenotype& _in_phenotype) { m_phenotype = _in_phenotype; }

  const cGenome& GetGenome() const { return m_initial_genome; }
  
  const cMutationRates& MutationRates() const { return m_mut_rates; }
  cMutationRates& MutationRates() { return m_mut_rates; }
  const cLocalMutations& GetLocalMutations() const { return m_mut_info; }
  cLocalMutations& GetLocalMutations() { return m_mut_info; }
  
  const cOrgInterface& GetOrgInterface() const { assert(m_interface); return *m_interface; }
  cOrgInterface& GetOrgInterface() { assert(m_interface); return *m_interface; }
  void SetOrgInterface(cOrgInterface* interface);
  
  void SetLineageLabel(int in_label) { m_lineage_label = in_label; }
  int GetLineageLabel() const { return m_lineage_label; }  
  void SetLineage(cLineage* in_lineage) { m_lineage = in_lineage; }
  cLineage* GetLineage() const { return m_lineage; }
  
	void SetCCladeLabel( int in_label ) { cclade_id = in_label; };  //@MRR
	int  GetCCladeLabel() const { return cclade_id; }
	
	const tArray<double>& GetRBins() const { return m_rbins; }
  tArray<double>& GetRBins() { return m_rbins; }
  double GetRBin(int index) { return m_rbins[index]; }
  double GetRBinsTotal();
  void SetRBins(const tArray<double>& rbins_in);
  void SetRBin(const int index, const double value);
  void AddToRBin(const int index, const double value);

  int GetMaxExecuted() const { return m_max_executed; }
  
  cCPUMemory& ChildGenome() { return m_child_genome; }

  void SetRunning(bool in_running) { m_is_running = in_running; }
  bool IsRunning() { return m_is_running; }

  void SetSleeping(bool in_sleeping) { m_is_sleeping = in_sleeping; }
  bool IsSleeping() { return m_is_sleeping; }
  
  bool IsDead() { return m_is_dead; }
  
  bool GetPheromoneStatus() { return m_pher_drop; }
  void TogglePheromone() { m_pher_drop = (m_pher_drop == true) ? false : true; }
  void SetPheromone(bool newval) { m_pher_drop = newval; }

  // --------  cOrgInterface Methods  --------
  cHardwareBase& GetHardware() { return *m_hardware; }
  cOrganism* GetNeighbor() { return m_interface->GetNeighbor(); }
  bool IsNeighborCellOccupied() { return m_interface->IsNeighborCellOccupied(); }
  int GetNeighborhoodSize() { return m_interface->GetNumNeighbors(); }
  int GetFacing() { assert(m_interface); return m_interface->GetFacing(); }  // Returns the facing of this organism.
  int GetNeighborCellContents() const { return m_interface->GetNeighborCellContents(); }
  void Rotate(int direction) { m_interface->Rotate(direction); }
  void DoBreakpoint() { m_interface->Breakpoint(); }
  int GetNextInput() { return m_interface->GetInputAt(m_input_pointer); }
  int GetNextInput(int& in_input_pointer) { return m_interface->GetInputAt(in_input_pointer); } //@JEB alternate for GX
  tBuffer<int>& GetInputBuf() { return m_input_buf; }
  tBuffer<int>& GetOutputBuf() { return m_output_buf; }
  void Die() { m_interface->Die(); m_is_dead = true; }
  void Kaboom(int dist) { m_interface->Kaboom(dist);}
  void SpawnDeme() { m_interface->SpawnDeme(); }
  int GetCellID() { return m_interface->GetCellID(); }
  int GetDebugInfo() { return m_interface->Debug(); }
  int GetID() { return m_id; }
  bool GetSentActive() { return m_sent_active; }
  void SendValue(int value) { m_sent_active = true; m_sent_value = value; }
  int RetrieveSentValue() { m_sent_active = false; return m_sent_value; }
  int ReceiveValue();
  void SellValue(const int data, const int label, const int sell_price);
  int BuyValue(const int label, const int buy_price);
  tListNode<tListNode<cSaleItem> >* AddSoldItem(tListNode<cSaleItem>* node) { return m_sold_items.PushRear(node); }
  tList<tListNode<cSaleItem> >* GetSoldItems() { return &m_sold_items; }
  void UpdateMerit(double new_merit) { m_interface->UpdateMerit(new_merit); }

  int GetPrevSeenCellID() const { return m_interface->GetPrevSeenCellID(); }
  int GetPrevTaskCellID() const { return m_interface->GetPrevTaskCellID(); }
  void SetPrevSeenCellID(int id) const { m_interface->SetPrevSeenCellID(id); }
  void SetPrevTaskCellID(int id) const { m_interface->SetPrevTaskCellID(id); }
  int GetNumTaskCellsReached() const { return m_interface->GetNumTaskCellsReached(); }
  void AddReachedTaskCell() { m_interface->AddReachedTaskCell(); }

  int GetCellData() { return m_interface->GetCellData(); }
  void SetCellData(const int data) { m_interface->SetCellData(data); }  

  // --------  Input and Output Methods  --------
  void DoInput(const int value);
  void DoInput(tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value);

  /* These different flavors of DoOutput are "frontends" to the main DoOutput
  that follows - One DoOutput to rule them all, etc., etc. */
  //! Check tasks based on the current state of this organism's IO & message buffers.
  void DoOutput(cAvidaContext& ctx, const bool on_divide=false);
  //! Add the passed-in value to this organism's output buffer, and check tasks (on_divide=false).
  void DoOutput(cAvidaContext& ctx, const int value);
  //! Check tasks based on the passed-in IO buffers and value (on_divide=false).
  void DoOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value);  


protected:
  /*! The main DoOutput function.  The DoOutputs above all forward to this function. */
  void DoOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, 
                tBuffer<int>& output_buffer, const bool on_divide, const bool net_valid);

public:
  void ClearInput() { m_input_buf.Clear(); }
  void ResetInput() {m_input_pointer = 0; m_input_buf.Clear(); };
  void AddOutput(int val) { m_output_buf.Add(val); }

  // --------  Divide Methods  --------
  bool Divide_CheckViable();
  bool ActivateDivide(cAvidaContext& ctx);
  
  
  // --------  Networking Support  --------
  void NetGet(cAvidaContext& ctx, int& value, int& seq);
  void NetSend(cAvidaContext& ctx, int value);
  cOrgSinkMessage* NetPop() { return m_net->pending.PopRear(); }
  bool NetReceive(int& value);
  bool NetValidate(cAvidaContext& ctx, int value);
  bool NetRemoteValidate(cAvidaContext& ctx, int value);
  int NetLast() { return m_net->last_seq; }
  void NetReset();

  
  // --------  Parasite Interactions  --------
  bool InjectParasite(const cCodeLabel& label, const cGenome& genome);
  bool InjectHost(const cCodeLabel& in_label, const cGenome& genome);
  void AddParasite(cInjectGenotype* cur) { m_parasites.Push(cur); }
  cInjectGenotype& GetParasite(int x) { return *m_parasites[x]; }
  int GetNumParasites() const { return m_parasites.GetSize(); }
  void ClearParasites();
  

  // --------  Support Methods  --------
  double GetTestFitness(cAvidaContext& ctx);
  double CalcMeritRatio();
  
  void PrintStatus(std::ostream& fp, const cString& next_name);
  void PrintFinalStatus(std::ostream& fp, int time_used, int time_allocated) const;
  void Fault(int fault_loc, int fault_type, cString fault_desc="");

  void NewTrial();

  // --------  Mutation Rate Convenience Methods  --------
  bool TestCopyMut(cAvidaContext& ctx) const { return m_mut_rates.TestCopyMut(ctx); }
  bool TestInsMut(cAvidaContext& ctx) const { return m_mut_rates.TestInsMut(ctx); }
  bool TestDelMut(cAvidaContext& ctx) const { return m_mut_rates.TestDelMut(ctx); }
  bool TestCopySlip(cAvidaContext& ctx) const { return m_mut_rates.TestCopySlip(ctx); }
  bool TestDivideMut(cAvidaContext& ctx) const { return m_mut_rates.TestDivideMut(ctx); }
  bool TestDivideIns(cAvidaContext& ctx) const { return m_mut_rates.TestDivideIns(ctx); }
  bool TestDivideDel(cAvidaContext& ctx) const { return m_mut_rates.TestDivideDel(ctx); }
  bool TestDivideSlip(cAvidaContext& ctx) const { return m_mut_rates.TestDivideSlip(ctx); } 
  bool TestParentMut(cAvidaContext& ctx) const { return m_mut_rates.TestParentMut(ctx); }
  
  double GetCopyMutProb() const { return m_mut_rates.GetCopyMutProb(); }
  void SetCopyMutProb(double _p) { return m_mut_rates.SetCopyMutProb(_p); }
  void SetDivMutProb(double _p) { return m_mut_rates.SetDivMutProb(_p); }

  double GetInsMutProb() const { return m_mut_rates.GetInsMutProb(); }
  double GetDelMutProb() const { return m_mut_rates.GetDelMutProb(); }
  double GetDivMutProb() const { return m_mut_rates.GetDivMutProb(); }
  double GetUniformMutProb() const { return m_mut_rates.GetUniformMutProb(); }
  double GetParentMutProb() const { return m_mut_rates.GetParentMutProb();}

  double GetInjectInsProb() const { return m_mut_rates.GetInjectInsProb(); }
  double GetInjectDelProb() const { return m_mut_rates.GetInjectDelProb(); }
  double GetInjectMutProb() const { return m_mut_rates.GetInjectMutProb(); }
  

  // --------  Configuration Convenience Methods  --------
  bool GetTestOnDivide() const;
  int GetFailImplicit() const;

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

  // -------- Messaging support --------
public:
  // Use a deque instead of vector for amortized constant-time removal
  // from the front of the list, to efficiently support message list
  // size caps
  typedef std::deque<cOrgMessage> message_list_type; //!< Container-type for cOrgMessages.
  
  //! Called when this organism attempts to send a message.
  bool SendMessage(cAvidaContext& ctx, cOrgMessage& msg);
  bool BroadcastMessage(cAvidaContext& ctx, cOrgMessage& msg);
  //! Called when this organism has been sent a message.
  void ReceiveMessage(cOrgMessage& msg);
  //! Called when this organism attempts to move a received message into its CPU.
  const cOrgMessage* RetrieveMessage();
  //! Returns the list of all messsages received by this organism.
  const message_list_type& GetReceivedMessages() { InitMessaging(); return m_msg->received; }
  //! Returns the list of all messages sent by this organism.
  const message_list_type& GetSentMessages() { InitMessaging(); return m_msg->sent; }
  
protected:
  /*! Contains all the different data structures needed to support messaging within
  cOrganism.  Inspired by cNetSupport (above), the idea is to minimize impact on
  organisms that DON'T use messaging. */
  struct cMessagingSupport
  {
    cMessagingSupport() : retrieve_index(0) { }

    message_list_type sent; //!< List of all messages sent by this organism.
    message_list_type received; //!< List of all messages received by this organism.
    message_list_type::size_type retrieve_index; //!< Index of next message that can be retrieved.
  };
  
  /*! This member variable is lazily initialized whenever any of the messaging
  methods are used.  (My kingdom for boost::shared_ptr.) */
  cMessagingSupport* m_msg;
  
  //! Called to check for (and initialize) messaging support within this organism.
  inline void InitMessaging() { if(!m_msg) m_msg = new cMessagingSupport(); }
  // -------- End of messaging support --------

  // -------- Movement TEMP --------
public:
  double GetGradientMovement() const {
    return m_gradient_movement;
  }
  
  void SetGradientMovement(const double value) {
    m_gradient_movement = value;
  }

  // -------- BDC Movement ---------
  void Move(cAvidaContext& ctx);

  
  /***** context switch********/
  bool BcastAlarmMSG(cAvidaContext& ctx, int jump_label, int bcast_range);
  void moveIPtoAlarmLabel(int jump_label);
  
  void DivideOrgTestamentAmongDeme(double value) { m_interface->DivideOrgTestamentAmongDeme(value); }
  
  void SetEventKilled() { killed_event = true; }
  bool GetEventKilled() { return killed_event; }
  
  // -------- Opinion support --------
  /*  Organisms express an opinion at a given point in time.  We can assume that they
   hold this opinion until they express a new one.  The semantics of opinions are
   left to the particular tasks or fitness functions in use; opinions are merely a generic
   approach that we as developers can use to evaluate when an organism has made a decision.
   
   If we ever have a need for organisms to express different kinds of opinions, this code
   can easily be adapted for that purpose (e.g., change the OpinionList typedef to a
   std::map<int, DatedOpinion>, where the key represents the kind of opinion being expressed).
   
   As with other such types of "extended" functionality, opinion support is encapsulated in
   a lazily-initialized struct.
   */  
public:
  typedef int Opinion; //!< Typedef for an opinion.
  typedef std::pair<Opinion, int> DatedOpinion; //!< Typedef for an opinion held at a given update.
  typedef std::vector<DatedOpinion> DatedOpinionList; //!< Typedef for a list of dated opinions.
  //! Called to set this organism's opinion.
  void SetOpinion(const Opinion& opinion);
  //! Retrieve this organism's current opinion.
  const DatedOpinion& GetOpinion() { InitOpinions(); return m_opinion->opinion_list.back(); }
  //! Retrieve all opinions expressed during this organism's lifetime.
  const DatedOpinionList& GetOpinions() { InitOpinions(); return m_opinion->opinion_list; }
  //! Return whether this organism has an opinion.
  bool HasOpinion() { InitOpinions(); return m_opinion->opinion_list.size(); }
  
protected:
  //! Initialize opinion support.
  inline void InitOpinions() { if(!m_opinion) { m_opinion = new cOpinionSupport(); } }
  //! Container for the data used to support opinions.
  struct cOpinionSupport
  {
    DatedOpinionList opinion_list; //!< All opinions expressed by this organism during its lifetime.
  };
  cOpinionSupport* m_opinion; //!< Lazily-initialized pointer to the opinion data.
  // -------- End of opinion support --------
	
	// -------- Synchronization support --------
public:
  //! Called when a neighboring organism issues a "flash" instruction.    
  void ReceiveFlash();
  //! Sends a "flash" to all neighboring organisms.
  void SendFlash(cAvidaContext& ctx);
  // -------- End of synchronization support --------	
};


#endif


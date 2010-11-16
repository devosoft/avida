/*
 *  cOrganism.h
 *  Avida
 *
 *  Called "organism.hh" prior to 12/5/05.
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

#ifndef cOrganism_h
#define cOrganism_h

#include "cBioUnit.h"
#include "cCPUMemory.h"
#include "cGenomeTestMetrics.h"
#include "cGenome.h"
#include "cMutationRates.h"
#include "cPhenotype.h"
#include "cOrgInterface.h"
#include "cOrgSeqMessage.h"
#include "cOrgSourceMessage.h"
#include "cOrgMessage.h"
#include "tArray.h"
#include "tBuffer.h"
#include "tList.h"
#include "tSmartArray.h"

#include <deque>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <map>

class cAvidaContext;
class cBioGroup;
class cEnvironment;
class cHardwareBase;
class cInstSet;
class cLineage;
class cOrgSinkMessage;
class cSaleItem;
class cStateGrid;



class cOrganism : public cBioUnit
{
private:
  cWorld* m_world;
  cHardwareBase* m_hardware;              // The actual machinery running this organism.
  cPhenotype m_phenotype;                 // Descriptive attributes of organism.
  eBioUnitSource m_src;
  cString m_src_args;
  const cGenome m_initial_genome;         // Initial genome; can never be changed!
  tArray<cBioUnit*> m_parasites;   // List of all parasites associated with this organism.
  cMutationRates m_mut_rates;             // Rate of all possible mutations.
  cOrgInterface* m_interface;             // Interface back to the population.
  int m_id;                               // unique id for each org, is just the number it was born
  int m_lineage_label;                    // a lineages tag; inherited unchanged in offspring
  cLineage* m_lineage;                    // A lineage descriptor... (different from label)
	int cclade_id;				                  // @MRR Coalescence clade information (set in cPopulation)
  
	// Other stats
  cGenome m_offspring_genome;              // Child genome, while under construction.

  // Input and Output with the environment
  int m_input_pointer;
  tBuffer<int> m_input_buf;
  tBuffer<int> m_output_buf;
  tBuffer<int> m_received_messages;
  tList<tListNode<cSaleItem> > m_sold_items;
  
  int m_cur_sg;

  // Communication
  int m_sent_value;         // What number is this org sending?
  bool m_sent_active;       // Has this org sent a number?
  int m_test_receive_pos;   // In a test CPU, what message to receive next?

  double m_gradient_movement;  // TEMP.  Remove once movement tasks are implemented.
  bool m_pher_drop;	   // Is the organism dropping pheromone?
  double frac_energy_donating;  // What fraction of the organism's energy is it donating

  int m_max_executed;      // Max number of instruction executed before death.  
  bool m_is_running;       // Does this organism have the CPU?
  bool m_is_sleeping;      // Is this organism sleeping?
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
    bool valid;
    int completed;
    
    cNetSupport() : last_seq(0), valid(false), completed(0) { ; }
    ~cNetSupport();
  };
  cNetSupport* m_net;
  
  
  cOrganism(); // @not_implemented
  cOrganism(const cOrganism&); // @not_implemented
  cOrganism& operator=(const cOrganism&); // @not_implemented
  
public:
  cOrganism(cWorld* world, cAvidaContext& ctx, const cGenome& genome, int parent_generation,
            eBioUnitSource src, const cString& src_args = "");
  ~cOrganism();
  
  // --------  cBioUnit Methods  --------
  eBioUnitSource GetUnitSource() const { return m_src; }
  const cString& GetUnitSourceArgs() const { return m_src_args; }
  const cGenome& GetGenome() const { return m_initial_genome; }
  

  // --------  Support Methods  --------
  inline double GetTestFitness(cAvidaContext& ctx) const;
  inline double GetTestMerit(cAvidaContext& ctx) const;
  inline double GetTestColonyFitness(cAvidaContext& ctx) const;
  double CalcMeritRatio();
  
  void HardwareReset(cAvidaContext& ctx);
  void NotifyDeath();
  
  void PrintStatus(std::ostream& fp, const cString& next_name);
  void PrintFinalStatus(std::ostream& fp, int time_used, int time_allocated) const;
  void Fault(int fault_loc, int fault_type, cString fault_desc="");
  
  void NewTrial();
  
  
  // --------  Accessor Methods  --------
  const cPhenotype& GetPhenotype() const { return m_phenotype; }
  cPhenotype& GetPhenotype() { return m_phenotype; }
  void SetPhenotype(cPhenotype& _in_phenotype) { m_phenotype = _in_phenotype; }

  const cMutationRates& MutationRates() const { return m_mut_rates; }
  cMutationRates& MutationRates() { return m_mut_rates; }
  
  const cOrgInterface& GetOrgInterface() const { assert(m_interface); return *m_interface; }
  cOrgInterface& GetOrgInterface() { assert(m_interface); return *m_interface; }
  void SetOrgInterface(cAvidaContext& ctx, cOrgInterface* interface);
  
  void SetLineageLabel(int in_label) { m_lineage_label = in_label; }
  int GetLineageLabel() const { return m_lineage_label; }  
  void SetLineage(cLineage* in_lineage) { m_lineage = in_lineage; }
  cLineage* GetLineage() const { return m_lineage; }
  
	void SetCCladeLabel( int in_label ) { cclade_id = in_label; };  //@MRR
	int  GetCCladeLabel() const { return cclade_id; }
	
	const tArray<double>& GetRBins() const { return m_phenotype.GetCurRBinsAvail(); }
  double GetRBin(int index) { return m_phenotype.GetCurRBinAvail(index); }
  double GetRBinsTotal();
  void SetRBins(const tArray<double>& rbins_in);
  void SetRBin(const int index, const double value);
  void AddToRBin(const int index, const double value);
  void IncCollectSpecCount(const int spec_id);

  int GetMaxExecuted() const { return m_max_executed; }
  
  cGenome& OffspringGenome() { return m_offspring_genome; }

  void SetRunning(bool in_running) { m_is_running = in_running; }
  bool IsRunning() { return m_is_running; }

  inline void SetSleeping(bool in_sleeping);
  bool IsSleeping() { return m_is_sleeping; }
  
  bool IsDead() { return m_is_dead; }
  
  bool IsInterrupted();
  
  bool GetPheromoneStatus() { return m_pher_drop; }
  void TogglePheromone() { m_pher_drop = (m_pher_drop == true) ? false : true; }
  void SetPheromone(bool newval) { m_pher_drop = newval; }
  
  double GetFracEnergyDonating() { return frac_energy_donating; }
  void SetFracEnergyDonating(double newval) { assert(newval >= 0); assert(newval <= 1); frac_energy_donating = newval; }
  
  const cStateGrid& GetStateGrid() const;
  
  
  double GetVitality() const;

  
  // --------  cOrgInterface Methods  --------
  cHardwareBase& GetHardware() { return *m_hardware; }
  int GetID() { return m_id; }

  int GetCellID() { return m_interface->GetCellID(); }
  int GetDemeID() { return m_interface->GetDemeID(); }
  cDeme* GetDeme() { return m_interface->GetDeme(); }
  
  int GetCellData() { return m_interface->GetCellData(); }
  int GetCellDataOrgID() { return m_interface->GetCellDataOrgID(); }
  int GetCellDataUpdate() { return m_interface->GetCellDataUpdate(); }
  void SetCellData(const int data) { m_interface->SetCellData(data); }  
  int GetFacedCellData() { return m_interface->GetFacedCellData(); }
  int GetFacedCellDataOrgID() { return m_interface->GetFacedCellDataOrgID(); }
  int GetFacedCellDataUpdate() { return m_interface->GetFacedCellDataUpdate(); }
  
  cOrganism* GetNeighbor() { return m_interface->GetNeighbor(); }
  bool IsNeighborCellOccupied() { return m_interface->IsNeighborCellOccupied(); }
  int GetNeighborhoodSize() { return m_interface->GetNumNeighbors(); }
  int GetFacing() { assert(m_interface); return m_interface->GetFacing(); }  // Returns the facing of this organism.
  int GetFacedCellID() { assert(m_interface); return m_interface->GetFacedCellID(); }  // Returns the facing of this organism.
  int GetNeighborCellContents() const { return m_interface->GetNeighborCellContents(); }
  void Rotate(int direction) { m_interface->Rotate(direction); }
  
  void DoBreakpoint() { m_interface->Breakpoint(); }
  
  int GetInputAt(int i) { return m_interface->GetInputAt(i); }
  int GetNextInput() { return m_interface->GetInputAt(m_input_pointer); }
  int GetNextInput(int& in_input_pointer) { return m_interface->GetInputAt(in_input_pointer); }
  tBuffer<int>& GetInputBuf() { return m_input_buf; }
  tBuffer<int>& GetOutputBuf() { return m_output_buf; }
  void Die() { m_interface->Die(); m_is_dead = true; }
  void Kaboom(int dist) { m_interface->Kaboom(dist);}
  void SpawnDeme() { m_interface->SpawnDeme(); }
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

  void JoinGroup(int group_id) { m_interface->JoinGroup(group_id); }
  void LeaveGroup(int group_id) { m_interface->LeaveGroup(group_id); }

  
  // --------  Input and Output Methods  --------
  void DoInput(const int value);
  void DoInput(tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value);

  /* These different flavors of DoOutput are "frontends" to the main DoOutput
  that follows - One DoOutput to rule them all, etc., etc. */
  //! Check tasks based on the current state of this organism's IO & message buffers.
  void DoOutput(cAvidaContext& ctx, const bool on_divide=false);
  //! Add the passed-in value to this organism's output buffer, and check tasks (on_divide=false).
  void DoOutput(cAvidaContext& ctx, const int value);
  //! Check if we're calling this from a parasite.
  void DoOutput(cAvidaContext& ctx, const int value, bool is_parasite);
  //! Check tasks based on the passed-in IO buffers and value (on_divide=false).
  void DoOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value);  
  
  
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
  void NetValidate(cAvidaContext& ctx, int value);
  bool NetRemoteValidate(cAvidaContext& ctx, int value);
  int NetLast() { return m_net->last_seq; }
  bool NetIsValid() { if (m_net) return m_net->valid; else return false; }
  int NetCompleted() { if (m_net) return m_net->completed; else return 0; }

  
  // --------  Parasite Interactions  --------
  bool InjectParasite(cBioUnit* parent, const cString& label, const cSequence& genome);
  bool ParasiteInfectHost(cBioUnit* parasite);
  int GetNumParasites() const { return m_parasites.GetSize(); }
  const tArray<cBioUnit*>& GetParasites() const { return m_parasites; }
  void ClearParasites();

  // --------  Mutation Rate Convenience Methods  --------
  bool TestCopyMut(cAvidaContext& ctx) const { return m_mut_rates.TestCopyMut(ctx); }
  bool TestCopyIns(cAvidaContext& ctx) const { return m_mut_rates.TestCopyIns(ctx); }
  bool TestCopyDel(cAvidaContext& ctx) const { return m_mut_rates.TestCopyDel(ctx); }
  bool TestCopyUniform(cAvidaContext& ctx) const { return m_mut_rates.TestCopyUniform(ctx); }
  bool TestCopySlip(cAvidaContext& ctx) const { return m_mut_rates.TestCopySlip(ctx); }
	
  bool TestDivideMut(cAvidaContext& ctx) const { return m_mut_rates.TestDivideMut(ctx); }
  bool TestDivideIns(cAvidaContext& ctx) const { return m_mut_rates.TestDivideIns(ctx); }
  bool TestDivideDel(cAvidaContext& ctx) const { return m_mut_rates.TestDivideDel(ctx); }
  
  unsigned int NumDividePoissonMut(cAvidaContext& ctx) const { return m_mut_rates.NumDividePoissonMut(ctx); }
  unsigned int NumDividePoissonIns(cAvidaContext& ctx) const { return m_mut_rates.NumDividePoissonIns(ctx); }
  unsigned int NumDividePoissonDel(cAvidaContext& ctx) const { return m_mut_rates.NumDividePoissonDel(ctx); }
  unsigned int NumDividePoissonSlip(cAvidaContext& ctx) const { return m_mut_rates.NumDividePoissonSlip(ctx); }

  bool TestDivideUniform(cAvidaContext& ctx) const { return m_mut_rates.TestDivideUniform(ctx); }
  bool TestDivideSlip(cAvidaContext& ctx) const { return m_mut_rates.TestDivideSlip(ctx); } 
  
  bool TestParentMut(cAvidaContext& ctx) const { return m_mut_rates.TestParentMut(ctx); }
  bool TestDeath(cAvidaContext& ctx) const { return m_mut_rates.TestDeath(ctx); }
  
  double GetCopyMutProb() const { return m_mut_rates.GetCopyMutProb(); }
  double GetCopyInsProb() const { return m_mut_rates.GetCopyInsProb(); }
  double GetCopyDelProb() const { return m_mut_rates.GetCopyDelProb(); }
  double GetCopyUniformProb() const { return m_mut_rates.GetCopyUniformProb(); }
  double GetCopySlipProb() const { return m_mut_rates.GetCopySlipProb(); }

  void SetCopyMutProb(double _p) { return m_mut_rates.SetCopyMutProb(_p); }
  void SetDivMutProb(double _p) { return m_mut_rates.SetDivMutProb(_p); }

  double GetDivInsProb() const { return m_mut_rates.GetDivInsProb(); }
  double GetDivDelProb() const { return m_mut_rates.GetDivDelProb(); }
  double GetDivMutProb() const { return m_mut_rates.GetDivMutProb(); }
  double GetDivUniformProb() const { return m_mut_rates.GetDivUniformProb(); }
  double GetDivSlipProb() const { return m_mut_rates.GetDivSlipProb(); }

  double GetParentMutProb() const { return m_mut_rates.GetParentMutProb();}
  double GetDeathProb() const { return m_mut_rates.GetDeathProb();}

  double GetInjectInsProb() const { return m_mut_rates.GetInjectInsProb(); }
  double GetInjectDelProb() const { return m_mut_rates.GetInjectDelProb(); }
  double GetInjectMutProb() const { return m_mut_rates.GetInjectMutProb(); }
  

  // --------  Configuration Convenience Methods  --------
  bool GetTestOnDivide() const;
  int GetSterilizeUnstable() const;

  bool GetRevertFatal() const;
  bool GetRevertNeg() const;
  bool GetRevertNeut() const;
  bool GetRevertPos() const;
  bool GetRevertTaskLoss() const;

  bool GetSterilizeFatal() const;
  bool GetSterilizeNeg() const;
  bool GetSterilizeNeut() const;
  bool GetSterilizePos() const;
  bool GetSterilizeTaskLoss() const;
  double GetNeutralMin() const;
  double GetNeutralMax() const;

  
  // -------- Messaging support --------
public:
  typedef std::deque<cOrgMessage> message_list_type; //!< Container-type for cOrgMessages.
  
  //! Called when this organism attempts to send a message.
  bool SendMessage(cAvidaContext& ctx, cOrgMessage& msg);
	//! Called when this organism attempts to broadcast a message.
  bool BroadcastMessage(cAvidaContext& ctx, cOrgMessage& msg, int depth);
  //! Called when this organism has been sent a message.
  void ReceiveMessage(cOrgMessage& msg);
  //! Called when this organism attempts to move a received message into its CPU.
	std::pair<bool, cOrgMessage> RetrieveMessage();
  //! Returns the list of all messsages received by this organism.
  const message_list_type& GetReceivedMessages() { InitMessaging(); return m_msg->received; }
  //! Returns the list of all messages sent by this organism.
  const message_list_type& GetSentMessages() { InitMessaging(); return m_msg->sent; }
	//! Use at your own rish; clear all the message buffers.
	void FlushMessageBuffers() { InitMessaging(); m_msg->sent.clear(); m_msg->received.clear(); }
	int PeekAtNextMessageType() { InitMessaging(); return m_msg->received.front().GetMessageType(); }

private:
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
	//! Called as the bottom-half of a successfully sent message.
	void MessageSent(cAvidaContext& ctx, cOrgMessage& msg);
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
public:
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
  typedef std::deque<DatedOpinion> DatedOpinionList; //!< Typedef for a list of dated opinions.
  //! Called to set this organism's opinion.
  void SetOpinion(const Opinion& opinion);
  //! Retrieve this organism's current opinion.
  const DatedOpinion& GetOpinion() { InitOpinions(); return m_opinion->opinion_list.back(); }
  //! Retrieve all opinions expressed during this organism's lifetime.
  const DatedOpinionList& GetOpinions() { InitOpinions(); return m_opinion->opinion_list; }
  //! Return whether this organism has an opinion.
  bool HasOpinion() { InitOpinions(); return m_opinion->opinion_list.size(); }
	//! remove all opinions
	void ClearOpinion() { InitOpinions(); m_opinion->opinion_list.clear(); }
  
private:
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

	
	// -------- Neighborhood support --------
public:
	typedef std::set<int> Neighborhood; //!< Typedef for a neighborhood snapshot.
	//! Get the current neighborhood.
	Neighborhood GetNeighborhood();
	//! Loads this organism's current neighborhood into memory.
	void LoadNeighborhood();
	//! Has the current neighborhood changed from what is in memory?
	bool HasNeighborhoodChanged();
	
protected:
	//! Initialize neighborhood support.
	inline void InitNeighborhood() { if(!m_neighborhood) { m_neighborhood = new cNeighborhoodSupport(); } }
	//! Container for neighborhood support.
	struct cNeighborhoodSupport {
		cNeighborhoodSupport() : loaded(false) { }
		bool loaded;
		Neighborhood neighbors;
	};
	cNeighborhoodSupport* m_neighborhood; //!< Lazily-initialized pointer to the neighborhood data.

	
  // -------- Reputation support --------	
public: 
	// Deduct amount number of self raw materials
	bool SubtractSelfRawMaterials(int amount); 
	// Deduct amount number of other raw materials
	bool SubtractOtherRawMaterials(int amount); 
	// receive raw materials from others
	bool AddOtherRawMaterials(int amount, int donor_id);
	// receive raw materials 
	bool AddRawMaterials(int amount, int donor_id);
	// receive raw materials 
	void AddSelfRawMaterials(int amount) { if (m_self_raw_materials < 10) m_self_raw_materials += amount;}
	// retrieve the organism's own amount of raw materials
	int GetSelfRawMaterials() { return m_self_raw_materials; }
	// retrieve the amount of raw materials collected from others
	int GetOtherRawMaterials() { return m_other_raw_materials; }
	// get the organism's reputation
	int GetReputation(); 
	// set the organism's reputation
	void SetReputation(int rep);
	// update the reputation to be an average on the basis of this new info
	void SetAverageReputation(int rep);
	// update the reputation by addint this new information 
	void AddReputation(int rep) { SetReputation(GetReputation() + rep); }
	// increment reputation
	void IncReputation() { SetReputation(GetReputation() + 1); }
	// get number of donors
	int GetNumberOfDonors() { return donor_list.size(); }
	// organism donated
	void Donated(){m_num_donate++;}
	// get number of donations
	int GetNumberOfDonations() { return m_num_donate; }
	// get number of donations received
	int GetNumberOfDonationsReceived() { return m_num_donate_received; }
	// get amout of donations received
	int GetAmountOfDonationsReceived() { return m_amount_donate_received; }
	// organism reciprocated
	void Reciprocated() {m_num_reciprocate++;}
	// get number of reciprocations
	int GetNumberOfReciprocations() { return m_num_reciprocate; }
	// was the organism a donor
	bool IsDonor(int neighbor_id); 
	
	// Check if buffer contains this string; return # bits correct
	int MatchOutputBuffer(cString string_to_match);
	
	// Add a donor
	void AddDonor(int org_id) { donor_list.insert(org_id); }
	// Set tag 
	void SetTag(int new_tag, int bits) { m_tag = make_pair(new_tag, bits); }
	// Set tag
	void SetTag(pair < int, int > new_tag)  { m_tag = new_tag; }
	// Update tag
	void UpdateTag(int new_tag, int bits); 
	// Get tag
	int GetTagLabel() { return m_tag.first; }
	pair < int, int > GetTag() { return m_tag; }
	// Get number of failed reputation increases
	int GetFailedReputationIncreases() { return m_failed_reputation_increases; }
	
	// Clear the output buffer
	void SetOutputNegative1();
	void AddDonatedLineage(int lin) { donating_lineages.insert(lin); }
	int GetNumberOfDonatedLineages() { return donating_lineages.size(); }
	void InitStringMap(); 
	bool ProduceString(int i);  
	int GetNumberStringsProduced(int i) { return  m_string_map[i].prod_string; }
	int GetNumberStringsOnHand(int i) { return m_string_map[i].on_hand; }
	bool DonateString(int string_tag, int amount); 
	bool ReceiveString(int string_tag, int amount, int donor_id); 
	bool CanReceiveString(int string_tag, int amount); 
	
protected:
	// The organism's own raw materials
	int m_self_raw_materials; 
	// The raw materials an oranism has collected from others
	int m_other_raw_materials;
  // Organisms that have donated to this organism
	set<int> donor_list;
	// Strings this organism has received. 
	set<int> donating_lineages;
	// number of donations
	int m_num_donate;
	// number of donations received
	int m_num_donate_received;
	// amount of donations received
	int m_amount_donate_received;
	// number of reciprocations
	int m_num_reciprocate;
	// reputation minimum for donation/rotation 
	// based on Nowak89
	int m_k;
	// int number of reputation increase failures
	int m_failed_reputation_increases;
	std::pair < int, int > m_tag;
	
  /*! Contains all the different data structures needed to 
	 track strings, production of strings, and donation/trade 
	 of strings. It is inspired by the cMessagingSupport*/
  struct cStringSupport
  {
    cStringSupport() 
		{ prod_string = 0; received_string = 0; on_hand = 0; }
		cString m_string; //!< The string being tracked
		int prod_string; //!< The number of times this string has been produced. 
		int received_string; //!< The number of times this string has been received.
		int on_hand; //!< The number of copies of the string this organism has on hand
  };
  
	/* This member variable is a map of tags to strings. It can
	 be used to track production, consumption, and donation of 
	 strings. */
	std::map < int, cStringSupport > m_string_map;

	
	// -------- HGT conjugation support --------
public:
	//! Called when this individual is the donor organism during conjugation.
	void DoHGTDonation();
	
	
	// -------- Division of Labor support --------
public: 
	void DonateResConsumedToDeme(); //! donate consumed resources to the deme.
	
	
	
	
	
	// -------- Internal Support Methods --------
private:
  void initialize(cAvidaContext& ctx);
  
  /*! The main DoOutput function.  The DoOutputs above all forward to this function. */
  void doOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const bool on_divide, bool is_parasite=false);
};


inline double cOrganism::GetTestFitness(cAvidaContext& ctx) const {
  return cGenomeTestMetrics::GetMetrics(ctx, GetBioGroup("genotype"))->GetFitness();
}

inline double cOrganism::GetTestMerit(cAvidaContext& ctx) const {
  return cGenomeTestMetrics::GetMetrics(ctx, GetBioGroup("genotype"))->GetMerit();
}

inline double cOrganism::GetTestColonyFitness(cAvidaContext& ctx) const {
  return cGenomeTestMetrics::GetMetrics(ctx, GetBioGroup("genotype"))->GetColonyFitness();
}


inline void cOrganism::SetSleeping(bool sleeping)
{
  m_is_sleeping = sleeping;
  
  if (sleeping) m_interface->BeginSleep();
  else m_interface->EndSleep();
}


#endif


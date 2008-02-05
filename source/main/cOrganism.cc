/*
 *  cOrganism.cc
 *  Avida
 *
 *  Called "organism.cc" prior to 12/5/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#include "cOrganism.h"

#include "cAvidaContext.h"
#include "nHardware.h"
#include "cEnvironment.h"
#include "functions.h"
#include "cGenome.h"
#include "cGenomeUtil.h"
#include "cGenotype.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInjectGenotype.h"
#include "cInstSet.h"
#include "cOrgSinkMessage.h"
#include "cPopulationCell.h"
#include "cPopulation.h"
#include "cStringUtil.h"
#include "cTaskContext.h"
#include "cTools.h"
#include "cWorld.h"
#include "cStats.h"

#include <iomanip>

using namespace std;


cOrganism::cOrganism(cWorld* world, cAvidaContext& ctx, const cGenome& in_genome)
  : m_world(world)
  , m_genotype(NULL)
  , m_phenotype(world)
  , m_initial_genome(in_genome)
  , m_mut_info(world->GetEnvironment().GetMutationLib(), in_genome.GetSize())
  , m_interface(NULL)
  , m_lineage_label(-1)
  , m_lineage(NULL)
  , m_input_pointer(0)
  , m_input_buf(world->GetEnvironment().GetInputSize())
  , m_output_buf(world->GetEnvironment().GetOutputSize())
  , m_received_messages(RECEIVED_MESSAGES_SIZE)
  , m_sent_value(0)
  , m_sent_active(false)
  , m_test_receive_pos(0)
  , m_max_executed(-1)
  , m_is_running(false)
  , m_is_sleeping(false)
  , m_is_dead(false)
  , m_net(NULL)
  , m_msg(0)
{
  // Initialization of structures...
  m_hardware = m_world->GetHardwareManager().Create(this);
//  m_cpu_stats.Setup();

  if (m_world->GetConfig().DEATH_METHOD.Get() > DEATH_METHOD_OFF) {
    m_max_executed = m_world->GetConfig().AGE_LIMIT.Get();
    if (m_world->GetConfig().AGE_DEVIATION.Get() > 0.0) {
      m_max_executed += (int) (ctx.GetRandom().GetRandNormal() * m_world->GetConfig().AGE_DEVIATION.Get());
    }
    if (m_world->GetConfig().DEATH_METHOD.Get() == DEATH_METHOD_MULTIPLE) {
      m_max_executed *= m_initial_genome.GetSize();
    }

    // m_max_executed must be positive or an organism will not die!
    if (m_max_executed < 1) m_max_executed = 1;
  }
  
  if (m_world->GetConfig().NET_ENABLED.Get()) m_net = new cNetSupport();
  m_id = m_world->GetStats().GetTotCreatures();
}


cOrganism::~cOrganism()
{
  assert(m_is_running == false);
  delete m_hardware;
  delete m_interface;
  if (m_net != NULL) delete m_net;
  if(!m_msg) delete m_msg;
}

cOrganism::cNetSupport::~cNetSupport()
{
  while (pending.GetSize()) delete pending.Pop();
  for (int i = 0; i < received.GetSize(); i++) delete received[i];
}

void cOrganism::SetOrgInterface(cOrgInterface* interface)
{
  delete m_interface;
  m_interface = interface;
}


double cOrganism::GetTestFitness(cAvidaContext& ctx)
{
  assert(m_interface);
  return m_genotype->GetTestFitness(ctx);
}
  
int cOrganism::ReceiveValue()
{
  assert(m_interface);
  const int out_value = m_interface->ReceiveValue();
  return out_value;
}

void cOrganism::SellValue(const int data, const int label, const int sell_price)
{
	if (m_sold_items.GetSize() < 10)
	{
		assert (m_interface);
		m_interface->SellValue(data, label, sell_price, m_id);
		m_world->GetStats().AddMarketItemSold();
	}
}

int cOrganism::BuyValue(const int label, const int buy_price)
{
	assert (m_interface);
	const int receive_value = m_interface->BuyValue(label, buy_price);
	if (receive_value != 0)
	{
		// put this value in storage place for recieved values
		m_received_messages.Add(receive_value);
		// update loss of buy_price to merit
		double cur_merit = GetPhenotype().GetMerit().GetDouble();
		cur_merit -= buy_price;
		UpdateMerit(cur_merit);
		m_world->GetStats().AddMarketItemBought();
	}
	return receive_value;
}


void cOrganism::DoInput(const int value)
{
  DoInput(m_input_buf, m_output_buf, value);
}


void cOrganism::DoInput(tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value)
{
  input_buffer.Add(value);
  m_phenotype.TestInput(input_buffer, output_buffer);
}


void cOrganism::DoOutput(cAvidaContext& ctx, const bool on_divide)
{
  DoOutput(ctx, m_input_buf, m_output_buf, on_divide, false);
}


void cOrganism::DoOutput(cAvidaContext& ctx, const int value)
{
  m_output_buf.Add(value);
  DoOutput(ctx, m_input_buf, m_output_buf, false, NetValidate(ctx, value));
}


void cOrganism::DoOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value)
{
  output_buffer.Add(value);
  DoOutput(ctx, input_buffer, output_buffer, false, NetValidate(ctx, value));
}


void cOrganism::DoOutput(cAvidaContext& ctx, 
                         tBuffer<int>& input_buffer, 
                         tBuffer<int>& output_buffer,
                         const bool on_divide,
                         const bool net_valid)
{
  const int deme_id = m_interface->GetDemeID();
  const tArray<double> & global_resource_count = m_interface->GetResources();
  const tArray<double> & deme_resource_count = m_interface->GetDemeResources(deme_id);
  
  tList<tBuffer<int> > other_input_list;
  tList<tBuffer<int> > other_output_list;
  
  // If tasks require us to consider neighbor inputs, collect them...
  if (m_world->GetEnvironment().UseNeighborInput()) {
    const int num_neighbors = m_interface->GetNumNeighbors();
    for (int i = 0; i < num_neighbors; i++) {
      m_interface->Rotate();
      cOrganism * cur_neighbor = m_interface->GetNeighbor();
      if (cur_neighbor == NULL) continue;
      
      other_input_list.Push( &(cur_neighbor->m_input_buf) );
    }
  }
  
  // If tasks require us to consider neighbor outputs, collect them...
  if (m_world->GetEnvironment().UseNeighborOutput()) {
    const int num_neighbors = m_interface->GetNumNeighbors();
    for (int i = 0; i < num_neighbors; i++) {
      m_interface->Rotate();
      cOrganism * cur_neighbor = m_interface->GetNeighbor();
      if (cur_neighbor == NULL) continue;
      
      other_output_list.Push( &(cur_neighbor->m_output_buf) );
    }
  }
  
  // Do the testing of tasks performed...
  
  tArray<double> global_res_change(global_resource_count.GetSize());
  global_res_change.SetAll(0.0);
  tArray<double> deme_res_change(deme_resource_count.GetSize());
  deme_res_change.SetAll(0.0);
  tArray<int> insts_triggered;
  
  tBuffer<int>* received_messages_point = &m_received_messages;
  if (!m_world->GetConfig().SAVE_RECEIVED.Get()) received_messages_point = NULL;
  
  cTaskContext taskctx(m_interface, input_buffer, output_buffer, other_input_list, 
                       other_output_list, net_valid, 0, on_divide, received_messages_point, this);
                       
  //combine global and deme resource counts
  const tArray<double> globalAndDeme_resource_count = global_resource_count + deme_resource_count;
  tArray<double> globalAndDeme_res_change = global_res_change + deme_res_change;
  
  bool task_completed = m_phenotype.TestOutput(ctx, taskctx, globalAndDeme_resource_count, globalAndDeme_res_change, insts_triggered);
  
  //disassemble global and deme resource counts
  global_res_change = globalAndDeme_res_change.Subset(0, global_res_change.GetSize());
  deme_res_change = globalAndDeme_res_change.Subset(global_res_change.GetSize(), globalAndDeme_res_change.GetSize());
    
  if(m_world->GetConfig().ENERGY_ENABLED.Get() && m_world->GetConfig().APPLY_ENERGY_METHOD.Get() == 1 && task_completed) {
    m_phenotype.RefreshEnergy();
    m_phenotype.ApplyToEnergyStore();
    double newMerit = cMerit::EnergyToMerit(GetPhenotype().GetStoredEnergy() * GetPhenotype().GetEnergyUsageRatio(), m_world);
    if(newMerit != -1) {
      m_interface->UpdateMerit(newMerit);
    }
  }
 
  m_interface->UpdateResources(global_res_change);
  
  //update deme resources
  m_interface->UpdateDemeResources(deme_res_change);  

  //if(m_world->GetConfig().CLEAR_ON_OUTPUT.Get()) input_buffer.Clear();  @JEB Not fully implemented 

  for (int i = 0; i < insts_triggered.GetSize(); i++) {
    const int cur_inst = insts_triggered[i];
    m_hardware->ProcessBonusInst(ctx, cInstruction(cur_inst));
  }
}

void cOrganism::NetGet(cAvidaContext& ctx, int& value, int& seq)
{
  assert(m_net);
  seq = m_net->seq.GetSize();
  m_net->seq.Resize(seq + 1);
  value = ctx.GetRandom().GetUInt(1 << 16);
  m_net->seq[seq].SetValue(value);
}

void cOrganism::NetSend(cAvidaContext& ctx, int value)
{
  assert(m_net);
  int index = -1;
  
  // Search for previously sent value
  for (int i = m_net->sent.GetSize() - 1; i >= 0; i--) {
    if (m_net->sent[i].GetValue() == value) {
      index = i;
      m_net->sent[i].SetSent();
      break;
    }
  }
  
  // If not found, add new message
  if (index == -1) {
    index = m_net->sent.GetSize();
    m_net->sent.Resize(index + 1);
    m_net->sent[index] = cOrgSourceMessage(value);
  }
  
  // Test if this message will be dropped
  const double drop_prob = m_world->GetConfig().NET_DROP_PROB.Get();
  if (drop_prob > 0.0 && ctx.GetRandom().P(drop_prob)) {
    m_net->sent[index].SetDropped();
    return;
  }
  
  // Test if this message will be corrupted
  int actual_value = value;
  const double mut_prob = m_world->GetConfig().NET_MUT_PROB.Get();
  if (mut_prob > 0.0 && ctx.GetRandom().P(mut_prob)) {
    switch (m_world->GetConfig().NET_MUT_TYPE.Get())
    {
      case 0: // Flip a single random bit
        actual_value ^= 1 << ctx.GetRandom().GetUInt(31);
        m_net->sent[index].SetCorrupted();
        break;
      case 1: // Flip the last bit
        actual_value ^= 1;
        m_net->sent[index].SetCorrupted();
        break;
      default:
        // invalid selection, no action
        break;
    }
  }
  
  assert(m_interface);
  cOrgSinkMessage* msg = new cOrgSinkMessage(m_interface->GetCellID(), value, actual_value);
  m_net->pending.Push(msg);
}

bool cOrganism::NetReceive(int& value)
{
  assert(m_net && m_interface);
  cOrgSinkMessage* msg = m_interface->NetReceive();
  if (msg == NULL) {
    value = 0;
    return false;
  }
  
  m_net->received.Push(msg);
  value = msg->GetActualValue();
  return true;
}

bool cOrganism::NetValidate(cAvidaContext& ctx, int value)
{
  if(!m_net) return false;

  assert(m_net);
  
  if (0xFFFF0000 & value) return false;
  
  for (int i = 0; i < m_net->received.GetSize(); i++) {
    cOrgSinkMessage* msg = m_net->received[i];
    if (!msg->GetValidated() && (msg->GetOriginalValue() & 0xFFFF) == value) {
      msg->SetValidated();
      assert(m_interface);
      return m_interface->NetRemoteValidate(ctx, msg);
    }
  }
    
  return false;
}

bool cOrganism::NetRemoteValidate(cAvidaContext& ctx, int value)
{
  assert(m_net);

  bool found = false;
  for (int i = m_net->last_seq; i < m_net->seq.GetSize(); i++) {
    cOrgSeqMessage& msg = m_net->seq[i];
    if (msg.GetValue() == value && !msg.GetReceived()) {
      m_net->seq[i].SetReceived();
      found = true;
      break;
    }
  }
  if (!found) return false;

  int completed = 0;
  while (m_net->last_seq < m_net->seq.GetSize() && m_net->seq[m_net->last_seq].GetReceived()) {
    completed++;
    m_net->last_seq++;
  }
  
  if (completed) {
    assert(m_interface);
    const tArray<double>& resource_count = m_interface->GetResources();
    
    tList<tBuffer<int> > other_input_list;
    tList<tBuffer<int> > other_output_list;
    
    // If tasks require us to consider neighbor inputs, collect them...
    if (m_world->GetEnvironment().UseNeighborInput()) {
      const int num_neighbors = m_interface->GetNumNeighbors();
      for (int i = 0; i < num_neighbors; i++) {
        m_interface->Rotate();
        cOrganism * cur_neighbor = m_interface->GetNeighbor();
        if (cur_neighbor == NULL) continue;
        
        other_input_list.Push( &(cur_neighbor->m_input_buf) );
      }
    }
    
    // If tasks require us to consider neighbor outputs, collect them...
    if (m_world->GetEnvironment().UseNeighborOutput()) {
      const int num_neighbors = m_interface->GetNumNeighbors();
      for (int i = 0; i < num_neighbors; i++) {
        m_interface->Rotate();
        cOrganism * cur_neighbor = m_interface->GetNeighbor();
        if (cur_neighbor == NULL) continue;
        
        other_output_list.Push( &(cur_neighbor->m_output_buf) );
      }
    }
        
    // Do the testing of tasks performed...
    m_output_buf.Add(value);
    tArray<double> res_change(resource_count.GetSize());
    tArray<int> insts_triggered;

    cTaskContext taskctx(m_interface, m_input_buf, m_output_buf, other_input_list, other_output_list, false, completed);
    m_phenotype.TestOutput(ctx, taskctx, resource_count, res_change, insts_triggered);
    m_interface->UpdateResources(res_change);
    
    for (int i = 0; i < insts_triggered.GetSize(); i++) {
      const int cur_inst = insts_triggered[i];
      m_hardware->ProcessBonusInst(ctx, cInstruction(cur_inst) );
    }
}
  
  return true;
}

void cOrganism::NetReset()
{
  if (m_net) {
    while (m_net->pending.GetSize()) delete m_net->pending.Pop();
    for (int i = 0; i < m_net->received.GetSize(); i++) delete m_net->received[i];
    m_net->received.Resize(0);
    m_net->sent.Resize(0);
    m_net->seq.Resize(0);
  }
}


bool cOrganism::InjectParasite(const cCodeLabel& label, const cGenome& injected_code)
{
  assert(m_interface);
  return m_interface->InjectParasite(this, label, injected_code);
}

bool cOrganism::InjectHost(const cCodeLabel& label, const cGenome& injected_code)
{
  return m_hardware->InjectHost(label, injected_code);
}

void cOrganism::ClearParasites()
{
  for (int i = 0; i < m_parasites.GetSize(); i++) m_parasites[i]->RemoveParasite();
  m_parasites.Resize(0);
}


double cOrganism::CalcMeritRatio()
{
  const double age = (double) m_phenotype.GetAge();
  const double merit = m_phenotype.GetMerit().GetDouble();
  return (merit > 0.0) ? (age / merit ) : age;
}


bool cOrganism::GetTestOnDivide() const { return m_interface->TestOnDivide(); }
bool cOrganism::GetFailImplicit() const { return m_world->GetConfig().FAIL_IMPLICIT.Get(); }

bool cOrganism::GetRevertFatal() const { return m_world->GetConfig().REVERT_FATAL.Get(); }
bool cOrganism::GetRevertNeg() const { return m_world->GetConfig().REVERT_DETRIMENTAL.Get(); }
bool cOrganism::GetRevertNeut() const { return m_world->GetConfig().REVERT_NEUTRAL.Get(); }
bool cOrganism::GetRevertPos() const { return m_world->GetConfig().REVERT_BENEFICIAL.Get(); }

bool cOrganism::GetSterilizeFatal() const { return m_world->GetConfig().STERILIZE_FATAL.Get(); }
bool cOrganism::GetSterilizeNeg() const { return m_world->GetConfig().STERILIZE_DETRIMENTAL.Get(); }
bool cOrganism::GetSterilizeNeut() const { return m_world->GetConfig().STERILIZE_NEUTRAL.Get(); }
bool cOrganism::GetSterilizePos() const { return m_world->GetConfig().STERILIZE_BENEFICIAL.Get(); }
double cOrganism::GetNeutralMin() const { return m_world->GetConfig().NEUTRAL_MIN.Get(); }
double cOrganism::GetNeutralMax() const { return m_world->GetConfig().NEUTRAL_MAX.Get(); }


void cOrganism::PrintStatus(ostream& fp, const cString& next_name)
{
  fp << "---------------------------" << endl;
  m_hardware->PrintStatus(fp);
  m_phenotype.PrintStatus(fp);
  fp << endl;

  fp << setbase(16) << setfill('0');
  
  fp << "Input (env):";
  for (int i = 0; i < m_input_buf.GetCapacity(); i++) {
    int j = i; // temp holder, because GetInputAt self adjusts the input pointer
    fp << " 0x" << setw(8) << m_interface->GetInputAt(j);
  }
  fp << endl;
  
  fp << "Input (buf):";
  for (int i = 0; i < m_hardware->GetInputBuf().GetNumStored(); i++) fp << " 0x" << setw(8) << m_hardware->GetInputBuf()[i];
  fp << endl;

  fp << "Output:     ";
  for (int i = 0; i < m_hardware->GetOutputBuf().GetNumStored(); i++) fp << " 0x" << setw(8) << m_hardware->GetOutputBuf()[i];
  fp << endl;
  
  fp << setfill(' ') << setbase(10);
    
  fp << "---------------------------" << endl;
  fp << "ABOUT TO EXECUTE: " << next_name << endl;
}

void cOrganism::PrintFinalStatus(ostream& fp, int time_used, int time_allocated) const
{
  fp << "---------------------------" << endl;
  m_phenotype.PrintStatus(fp);
  fp << endl;

  if (time_used == time_allocated) {
    fp << endl << "# TIMEOUT: No offspring produced." << endl;
  } else if (m_hardware->GetMemory().GetSize() == 0) {
    fp << endl << "# ORGANISM DEATH: No offspring produced." << endl;
  } else {
    fp << endl;
    fp << "# Final Memory: " << m_hardware->GetMemory().AsString() << endl;
    fp << "# Child Memory: " << m_child_genome.AsString() << endl;
  }
}


bool cOrganism::Divide_CheckViable()
{
  // Make sure required task (if any) has been performed...
  const int required_task = m_world->GetConfig().REQUIRED_TASK.Get();
  const int immunity_task = m_world->GetConfig().IMMUNITY_TASK.Get();

  if (required_task != -1 && m_phenotype.GetCurTaskCount()[required_task] == 0) { 
    if (immunity_task==-1 || m_phenotype.GetCurTaskCount()[immunity_task] == 0) {
      Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
            cStringUtil::Stringf("Lacks required task (%d)", required_task));
      return false; //  (divide fails)
    } 
  }
  
  if (GetPhenotype().GetCurBonus() < m_world->GetConfig().REQUIRED_BONUS.Get()) return false;
  
  const int required_reaction = m_world->GetConfig().REQUIRED_REACTION.Get();
  if (required_reaction != -1 && m_phenotype.GetCurTaskCount()[required_reaction] == 0) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
          cStringUtil::Stringf("Lacks required reaction (%d)", required_reaction));
    return false; //  (divide fails)
  }
  
  // Make sure the parent is fertile
  if ( m_phenotype.IsFertile() == false ) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR, "Infertile organism");
    return false; //  (divide fails)
  }

  return true;  // Organism has no problem with divide...
}


// This gets called after a successful divide to deal with the child. 
// Returns true if parent lives through this process.

bool cOrganism::ActivateDivide(cAvidaContext& ctx)
{
  assert(m_interface);
  // Test tasks one last time before actually dividing, pass true so 
  // know that should only test "divide" tasks here
  DoOutput(ctx, true);

  // Activate the child!  (Keep Last: may kill this organism!)
  return m_interface->Divide(ctx, this, m_child_genome);
}


void cOrganism::Fault(int fault_loc, int fault_type, cString fault_desc)
{
  (void) fault_loc;
  (void) fault_type;
  (void) fault_desc;

#if FATAL_ERRORS
  if (fault_type == FAULT_TYPE_ERROR) {
    m_phenotype.IsFertile() = false;
  }
#endif

#if FATAL_WARNINGS
  if (fault_type == FAULT_TYPE_WARNING) {
    m_phenotype.IsFertile() = false;
  }
#endif

#if BREAKPOINTS
  m_phenotype.SetFault(fault_desc);
#endif

  m_phenotype.IncErrors();
}

void cOrganism::NewTrial()
{
  //More should be reset here... @JEB
  GetPhenotype().NewTrial();
  m_input_pointer = 0;
  m_input_buf.Clear();
  m_output_buf.Clear();
}


bool cOrganism::SendMessage(cAvidaContext& ctx, cOrgMessage& msg)
{
  assert(m_interface);
  InitMessaging();

  // If we're able to succesfully send the message...
  if(m_interface->SendMessage(msg)) {
    // save it...
    m_msg->sent.push_back(msg);
    // stat-tracking...
    m_world->GetStats().SentMessage(msg);
    // check to see if we've performed any tasks...
    DoOutput(ctx);
    // and set the receiver-pointer of this message to NULL.  We don't want to
    // walk this list later thinking that the receivers are still around.
    m_msg->sent.back().SetReceiver(0);
    return true;
  }
  
  return false;
}


void cOrganism::ReceiveMessage(cOrgMessage& msg)
{
  InitMessaging();
  msg.SetReceiver(this);    
  m_msg->received.push_back(msg);
}


const cOrgMessage* cOrganism::RetrieveMessage()
{
  InitMessaging();

  if(m_msg->retrieve_index < m_msg->received.size()) {
    return &m_msg->received.at(m_msg->retrieve_index++);
  }
  
  return 0;
}

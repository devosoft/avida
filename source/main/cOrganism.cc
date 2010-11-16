/*
 *  cOrganism.cc
 *  Avida
 *
 *  Called "organism.cc" prior to 12/5/05.
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

#include "cOrganism.h"

#include "cAvidaContext.h"
#include "cBioGroup.h"
#include "cDeme.h"
#include "cEnvironment.h"
#include "cSequence.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInstSet.h"
#include "cOrgSinkMessage.h"
#include "cPopulationCell.h"
#include "cStateGrid.h"
#include "cStringUtil.h"
#include "cTaskContext.h"
#include "cWorld.h"
#include "cWorldDriver.h"
#include "cStats.h"
#include "nHardware.h"

#include <algorithm>
#include <iomanip>
#include <utility>

using namespace std;


cOrganism::cOrganism(cWorld* world, cAvidaContext& ctx, const cGenome& genome, int parent_generation, eBioUnitSource src,
                     const cString& src_args)
  : m_world(world)
  , m_phenotype(world, parent_generation, world->GetHardwareManager().GetInstSet(genome.GetInstSet()).GetNumNops())
  , m_src(src)
  , m_src_args(src_args)
  , m_initial_genome(genome)
  , m_interface(NULL)
  , m_lineage_label(-1)
  , m_lineage(NULL)
  , m_input_pointer(0)
  , m_input_buf(world->GetEnvironment().GetInputSize())
  , m_output_buf(world->GetEnvironment().GetOutputSize())
  , m_received_messages(RECEIVED_MESSAGES_SIZE)
  , m_cur_sg(0)
  , m_sent_value(0)
  , m_sent_active(false)
  , m_test_receive_pos(0)
  , m_pher_drop(false)
  , frac_energy_donating(m_world->GetConfig().ENERGY_SHARING_PCT.Get())
  , m_max_executed(-1)
  , m_is_running(false)
  , m_is_sleeping(false)
  , m_is_dead(false)
  , killed_event(false)
  , m_net(NULL)
  , m_msg(0)
  , m_opinion(0)
  , m_neighborhood(0)
  , m_self_raw_materials(world->GetConfig().RAW_MATERIAL_AMOUNT.Get())
  , m_other_raw_materials(0)
  , m_num_donate(0)
  , m_num_donate_received(0)
  , m_amount_donate_received(0)
  , m_num_reciprocate(0)
  , m_failed_reputation_increases(0)
  , m_tag(make_pair(-1, 0))

{
  m_hardware = m_world->GetHardwareManager().Create(ctx, this, m_initial_genome);
  
  initialize(ctx);
}



void cOrganism::initialize(cAvidaContext& ctx)
{
  m_phenotype.SetInstSetSize(m_hardware->GetInstSet().GetSize());
  
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
	
	// randomize the amout of raw materials an organism has at its 
	// disposal.
	if (m_world->GetConfig().RANDOMIZE_RAW_MATERIAL_AMOUNT.Get()) {
		int raw_mat = m_world->GetConfig().RAW_MATERIAL_AMOUNT.Get();
		m_self_raw_materials = m_world->GetRandom().GetUInt(0, raw_mat+1); 
	}
}



cOrganism::~cOrganism()
{
  assert(m_is_running == false);
  delete m_hardware;
  delete m_interface;
  if(m_net) delete m_net;
  if(m_msg) delete m_msg;
  if(m_opinion) delete m_opinion;  
  for (int i = 0; i < m_parasites.GetSize(); i++) delete m_parasites[i];
  if(m_neighborhood) delete m_neighborhood;
}

cOrganism::cNetSupport::~cNetSupport()
{
  while (pending.GetSize()) delete pending.Pop();
  for (int i = 0; i < received.GetSize(); i++) delete received[i];
}

void cOrganism::SetOrgInterface(cAvidaContext& ctx, cOrgInterface* interface)
{
  delete m_interface;
  m_interface = interface;
  
  HardwareReset(ctx);
}

const cStateGrid& cOrganism::GetStateGrid() const { return m_world->GetEnvironment().GetStateGrid(m_cur_sg); }

double cOrganism::GetVitality() const {
  double mean_age = m_world->GetStats().SumCreatureAge().Ave();
  double age_stddev = m_world->GetStats().SumCreatureAge().StdDeviation();
  int org_age = m_phenotype.GetAge();
  const int resource = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  double res_level = m_phenotype.GetCurRBinAvail(resource);
  
  double vitality = 0.0;
  
  if (org_age < (mean_age - age_stddev) || org_age > (mean_age + age_stddev)) {
    vitality = m_world->GetConfig().VITALITY_BIN_EXTREMES.Get() * res_level;
  } else {
    vitality = m_world->GetConfig().VITALITY_BIN_CENTER.Get() * res_level;    
  }
  
  return vitality;
}


double cOrganism::GetRBinsTotal()
{
	double total = 0;
	for(int i = 0; i < m_phenotype.GetCurRBinsAvail().GetSize(); i++)
	{total += m_phenotype.GetCurRBinsAvail()[i];}
	
	return total;
}

void cOrganism::SetRBins(const tArray<double>& rbins_in) 
{ 
	m_phenotype.SetCurRBinsAvail(rbins_in);
}

void cOrganism::SetRBin(const int index, const double value) 
{ 
	m_phenotype.SetCurRBinAvail(index, value);
}

void cOrganism::AddToRBin(const int index, const double value) 
{ 
	m_phenotype.AddToCurRBinAvail(index, value);
	
	if(value > 0)
	{ m_phenotype.AddToCurRBinTotal(index, value); }
}  

void cOrganism::IncCollectSpecCount(const int spec_id)
{
  int current_count = m_phenotype.GetCurCollectSpecCount(spec_id);
  m_phenotype.SetCurCollectSpecCount(spec_id, current_count + 1);
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
  if (m_net) m_net->valid = false;
  doOutput(ctx, m_input_buf, m_output_buf, on_divide, false);
}


void cOrganism::DoOutput(cAvidaContext& ctx, const int value)
{
  m_output_buf.Add(value);
  NetValidate(ctx, value);
  doOutput(ctx, m_input_buf, m_output_buf, false, false);
}

void cOrganism::DoOutput(cAvidaContext& ctx, const int value, bool is_parasite)
{
  m_output_buf.Add(value);
  NetValidate(ctx, value);  
  doOutput(ctx, m_input_buf, m_output_buf, false, (bool)is_parasite);
}

void cOrganism::DoOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value)
{
  output_buffer.Add(value);
  NetValidate(ctx, value);
  doOutput(ctx, input_buffer, output_buffer, false, false);
}


void cOrganism::doOutput(cAvidaContext& ctx, 
                         tBuffer<int>& input_buffer, 
                         tBuffer<int>& output_buffer,
                         const bool on_divide,
                         bool is_parasite)
{  
  const int deme_id = m_interface->GetDemeID();
  const tArray<double> & global_resource_count = m_interface->GetResources();
  const tArray<double> & deme_resource_count = m_interface->GetDemeResources(deme_id);
  const tArray< tArray<int> > & cell_id_lists = m_interface->GetCellIdLists();
  
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
  tArray<cString> insts_triggered;
  
  tBuffer<int>* received_messages_point = &m_received_messages;
  if (!m_world->GetConfig().SAVE_RECEIVED.Get()) received_messages_point = NULL;
  
  cTaskContext taskctx(this, input_buffer, output_buffer, other_input_list, other_output_list,
                       m_hardware->GetExtendedMemory(), on_divide, received_messages_point);
                       
  //combine global and deme resource counts
  tArray<double> globalAndDeme_resource_count = global_resource_count + deme_resource_count;
  tArray<double> globalAndDeme_res_change = global_res_change + deme_res_change;
  
  // set any resource amount to 0 if a cell cannot access this resource
  int cell_id=GetCellID();
  if (cell_id_lists.GetSize())
  {
	  for (int i=0; i<cell_id_lists.GetSize(); i++)
	  {
		  // if cell_id_lists have been set then we have to check if this cell is in the list
		  if (cell_id_lists[i].GetSize()) {
			  int j;
			  for (j=0; j<cell_id_lists[i].GetSize(); j++)
			  {
				  if (cell_id==cell_id_lists[i][j])
					  break;
			  }
			  if (j==cell_id_lists[i].GetSize())
				  globalAndDeme_resource_count[i]=0;
		  }
	  }
  }

  bool task_completed = m_phenotype.TestOutput(ctx, taskctx, globalAndDeme_resource_count, 
                                               m_phenotype.GetCurRBinsAvail(), globalAndDeme_res_change, 
                                               insts_triggered, is_parasite);
											   
  // Handle merit increases that take the organism above it's current population merit
  if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    double cur_merit = m_phenotype.CalcCurrentMerit();
    if (m_phenotype.GetMerit().GetDouble() < cur_merit) m_interface->UpdateMerit(cur_merit);
  }
 
  //disassemble global and deme resource counts
  global_res_change = globalAndDeme_res_change.Subset(0, global_res_change.GetSize());
  deme_res_change = globalAndDeme_res_change.Subset(global_res_change.GetSize(), globalAndDeme_res_change.GetSize());
    
  if(m_world->GetConfig().ENERGY_ENABLED.Get() && m_world->GetConfig().APPLY_ENERGY_METHOD.Get() == 1 && task_completed) {
    m_phenotype.RefreshEnergy();
    m_phenotype.ApplyToEnergyStore();
    double newMerit = m_phenotype.ConvertEnergyToMerit(m_phenotype.GetStoredEnergy() * m_phenotype.GetEnergyUsageRatio());
		m_interface->UpdateMerit(newMerit);
		if(GetPhenotype().GetMerit().GetDouble() == 0.0) {
			GetPhenotype().SetToDie();
		}
  }
  m_interface->UpdateResources(global_res_change);

  //update deme resources
  m_interface->UpdateDemeResources(deme_res_change);  

  for (int i = 0; i < insts_triggered.GetSize(); i++) 
    m_hardware->ProcessBonusInst(ctx, m_hardware->GetInstSet().GetInst(insts_triggered[i]));
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

void cOrganism::NetValidate(cAvidaContext& ctx, int value)
{
  if (!m_net) return;

  m_net->valid = false;
  
  if (0xFFFF0000 & value) return;
  
  for (int i = 0; i < m_net->received.GetSize(); i++) {
    cOrgSinkMessage* msg = m_net->received[i];
    if (!msg->GetValidated() && (msg->GetOriginalValue() & 0xFFFF) == value) {
      msg->SetValidated();
      assert(m_interface);
      m_net->valid = m_interface->NetRemoteValidate(ctx, msg);
      break;
    }
  }
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

  m_net->valid = false;
  int& completed = m_net->completed;
  completed = 0;
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
    tArray<cString> insts_triggered;

    cTaskContext taskctx(this, m_input_buf, m_output_buf, other_input_list, other_output_list,
                         m_hardware->GetExtendedMemory());
    m_phenotype.TestOutput(ctx, taskctx, resource_count, m_phenotype.GetCurRBinsAvail(), res_change, insts_triggered);
    m_interface->UpdateResources(res_change);
    
    for (int i = 0; i < insts_triggered.GetSize(); i++)
      m_hardware->ProcessBonusInst(ctx, m_hardware->GetInstSet().GetInst(insts_triggered[i]));
  }
  
  return true;
}

void cOrganism::HardwareReset(cAvidaContext& ctx)
{
  if (m_world->GetEnvironment().GetNumStateGrids() > 0 && m_interface) {
    // Select random state grid in the environment
    m_cur_sg = m_interface->GetStateGridID(ctx);
    
    const cStateGrid& sg = GetStateGrid();
    
    tArray<int> sg_state(3 + sg.GetNumStates(), 0);
    sg_state[0] = sg.GetInitialX();
    sg_state[1] = sg.GetInitialY();
    sg_state[2] = sg.GetInitialFacing();
    
    m_hardware->SetupExtendedMemory(sg_state);
  }

  if (m_net) {
    while (m_net->pending.GetSize()) delete m_net->pending.Pop();
    for (int i = 0; i < m_net->received.GetSize(); i++) delete m_net->received[i];
    m_net->received.Resize(0);
    m_net->sent.Resize(0);
    m_net->seq.Resize(0);
  }
}

void cOrganism::NotifyDeath()
{
  // Update Sleeping State
  if (m_is_sleeping) {
    m_is_sleeping = false;
    GetDeme()->DecSleepingCount();
  }

  // Return currently stored internal resources to the world
  if (m_world->GetConfig().USE_RESOURCE_BINS.Get() && m_world->GetConfig().RETURN_STORED_ON_DEATH.Get()) {
  	m_interface->UpdateResources(GetRBins());
  }
  
	// Make sure the group composition is updated.
	if (m_world->GetConfig().USE_FORM_GROUPS.Get() && HasOpinion()) m_interface->LeaveGroup(GetOpinion().first);  
}



bool cOrganism::InjectParasite(cBioUnit* parent, const cString& label, const cSequence& injected_code)
{
  assert(m_interface);
  return m_interface->InjectParasite(this, parent, label, injected_code);
}

bool cOrganism::ParasiteInfectHost(cBioUnit* parasite)
{
  if (!m_hardware->ParasiteInfectHost(parasite)) return false;
  
  m_parasites.Push(parasite);
  return true;
}

void cOrganism::ClearParasites()
{
  for (int i = 0; i < m_parasites.GetSize(); i++) delete m_parasites[i];
  m_parasites.Resize(0);
}


double cOrganism::CalcMeritRatio()
{
  const double age = (double) m_phenotype.GetAge();
  const double merit = m_phenotype.GetMerit().GetDouble();
  return (merit > 0.0) ? (age / merit ) : age;
}


bool cOrganism::GetTestOnDivide() const { return m_interface->TestOnDivide(); }
int cOrganism::GetSterilizeUnstable() const { return m_world->GetConfig().STERILIZE_UNSTABLE.Get(); }

bool cOrganism::GetRevertFatal() const { return m_world->GetConfig().REVERT_FATAL.Get(); }
bool cOrganism::GetRevertNeg() const { return m_world->GetConfig().REVERT_DETRIMENTAL.Get(); }
bool cOrganism::GetRevertNeut() const { return m_world->GetConfig().REVERT_NEUTRAL.Get(); }
bool cOrganism::GetRevertPos() const { return m_world->GetConfig().REVERT_BENEFICIAL.Get(); }
bool cOrganism::GetRevertTaskLoss() const { return m_world->GetConfig().REVERT_TASKLOSS.Get(); }

bool cOrganism::GetSterilizeFatal() const { return m_world->GetConfig().STERILIZE_FATAL.Get(); }
bool cOrganism::GetSterilizeNeg() const { return m_world->GetConfig().STERILIZE_DETRIMENTAL.Get(); }
bool cOrganism::GetSterilizeNeut() const { return m_world->GetConfig().STERILIZE_NEUTRAL.Get(); }
bool cOrganism::GetSterilizePos() const { return m_world->GetConfig().STERILIZE_BENEFICIAL.Get(); }
bool cOrganism::GetSterilizeTaskLoss() const { return m_world->GetConfig().STERILIZE_TASKLOSS.Get(); }
double cOrganism::GetNeutralMin() const { return m_world->GetConfig().NEUTRAL_MIN.Get(); }
double cOrganism::GetNeutralMax() const { return m_world->GetConfig().NEUTRAL_MAX.Get(); }


void cOrganism::PrintStatus(ostream& fp, const cString& next_name)
{
  fp << "---------------------------" << endl;
	fp << "U:" << m_world->GetStats().GetUpdate() << endl;
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
    fp << "# Child Memory: " << m_offspring_genome.GetSequence().AsString() << endl;
  }
}


bool cOrganism::Divide_CheckViable()
{
  // Make sure required task (if any) has been performed...
  const int required_task = m_world->GetConfig().REQUIRED_TASK.Get();
  const int immunity_task = m_world->GetConfig().IMMUNITY_TASK.Get();

  if (required_task != -1 && m_phenotype.GetCurTaskCount()[required_task] == 0) { 
    if (immunity_task ==-1 || m_phenotype.GetCurTaskCount()[immunity_task] == 0) {
      Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
            cStringUtil::Stringf("Lacks required task (%d)", required_task));
      return false; //  (divide fails)
    } 
  }
  
  if (GetPhenotype().GetCurBonus() < m_world->GetConfig().REQUIRED_BONUS.Get()) return false;
  
  const int required_reaction = m_world->GetConfig().REQUIRED_REACTION.Get();
  const int immunity_reaction = m_world->GetConfig().IMMUNITY_REACTION.Get();
  const int single_reaction = m_world->GetConfig().REQUIRE_SINGLE_REACTION.Get();

  if (single_reaction == 0 && required_reaction != -1 && m_phenotype.GetCurReactionCount()[required_reaction] == 0)   {
    if (immunity_reaction == -1 || m_phenotype.GetCurReactionCount()[immunity_reaction] == 0) {  
      Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
          cStringUtil::Stringf("Lacks required reaction (%d)", required_reaction));
      return false; //  (divide fails)
    }
  }
  
  if(single_reaction != 0)
  {
    bool toFail = true;
    tArray<int> reactionCounts = m_phenotype.GetCurReactionCount();
    for (int i=0; i<reactionCounts.GetSize(); i++)
    {
      if (reactionCounts[i] > 0) toFail = false;
    }
    
    if(toFail)
    {
      Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
            cStringUtil::Stringf("Lacks any reaction required for divide"));
      return false; //  (divide fails)
    }
  }
  
  
  // Test for required resource availability (must be stored in an internal resource bin)
  const int required_resource = m_world->GetConfig().REQUIRED_RESOURCE.Get();
  const double required_resource_level = m_world->GetConfig().REQUIRED_RESOURCE_LEVEL.Get();
  if (required_resource != -1) {
    const double resource_level = m_phenotype.GetCurRBinAvail(required_resource);
    if ((required_resource_level > 0.0 && resource_level < required_resource_level) ||
        (required_resource_level == 0.0 && resource_level == 0.0)) return false;
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
  
  // Handle successful divide consumption of require resource
  const int required_resource = m_world->GetConfig().REQUIRED_RESOURCE.Get();
  const double required_resource_level = m_world->GetConfig().REQUIRED_RESOURCE_LEVEL.Get();
  if (required_resource != -1 && required_resource_level > 0.0) AddToRBin(required_resource, -required_resource_level);
  
  // Activate the child!  (Keep Last: may kill this organism!)
  return m_interface->Divide(ctx, this, m_offspring_genome);
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


/*! Called as the bottom-half of a successfully sent message.
 */
void cOrganism::MessageSent(cAvidaContext& ctx, cOrgMessage& msg) {
	// check to see if we should store it:
	const int bsize = m_world->GetConfig().MESSAGE_SEND_BUFFER_SIZE.Get();

	if((bsize > 0) || (bsize == -1)) {
		// yep; store it:
		m_msg->sent.push_back(msg);
		// and set the receiver-pointer of this message to NULL.  We don't want to
		// walk this list later thinking that the receivers are still around.
		m_msg->sent.back().SetReceiver(0);
		// if our buffer is too large, chop off old messages:
		while((bsize != -1) && (static_cast<int>(m_msg->sent.size()) > bsize)) {
			m_msg->sent.pop_front();
		}
	}	
}


/*! Send a message to the currently faced organism.  Stat-tracking is done over
 in cPopulationInterface.  Remember that this code WILL be called from within the
 test CPU!  (Also, BroadcastMessage funnels down to code in the population interface
 too, so this way all the message sending code is in the same place.)
 */
bool cOrganism::SendMessage(cAvidaContext& ctx, cOrgMessage& msg) {
  assert(m_interface);
  InitMessaging();

  // check to see if we've performed any tasks:
  DoOutput(ctx, static_cast<int>(msg.GetData()));

  // if we sent the message:
  if(m_interface->SendMessage(msg)) {
		MessageSent(ctx, msg);
    return true;
  }
	// importantly, m_interface->SendMessage() fails if we're running in the test CPU.
	return false;
}


/*! Broadcast a message to all organisms out to the given depth.
 */
bool cOrganism::BroadcastMessage(cAvidaContext& ctx, cOrgMessage& msg, int depth) {
  assert(m_interface);
  InitMessaging();
	
	// if we broadcasted the message:
	if(m_interface->BroadcastMessage(msg, depth)) {
		MessageSent(ctx, msg);
    return true;
  }
	
	// Again, m_interface->BroadcastMessage() fails if we're running in the test CPU.
	return false;
}


/*! Called when this organism receives a message from another.
 */
void cOrganism::ReceiveMessage(cOrgMessage& msg) {
  InitMessaging();
	
	// don't store more messages than we're configured to.
	const int bsize = m_world->GetConfig().MESSAGE_RECV_BUFFER_SIZE.Get();
	if((bsize != -1) && (bsize <= static_cast<int>(m_msg->received.size()))) {
		switch(m_world->GetConfig().MESSAGE_RECV_BUFFER_BEHAVIOR.Get()) {
			case 0: // drop oldest message
				m_msg->received.pop_front();
				break;
			case 1: // drop this message
				return;
			default: // error
				m_world->GetDriver().RaiseFatalException(-1, "MESSAGE_RECV_BUFFER_BEHAVIOR is set to an invalid value.");
				assert(false);
		}
	}

	msg.SetReceiver(this);
	m_msg->received.push_back(msg);
  
  if (m_world->GetConfig().ACTIVE_MESSAGES_ENABLED.Get() > 0) {
    // then create new thread and load its registers
    m_hardware->InterruptThread(cHardwareBase::MSG_INTERRUPT);
  }
}


/*! Called to when this organism tries to load its CPU with the contents of a
 previously-received message.  In a change from previous versions, pop the message
 off the front.
 
 \return A pair (b, msg): if b is true, then msg was received; if b is false, then msg was not received.
 */
std::pair<bool, cOrgMessage> cOrganism::RetrieveMessage() {
  InitMessaging();
	std::pair<bool, cOrgMessage> ret = std::make_pair(false, cOrgMessage());	
	
	if(m_msg->received.size() > 0) {
		ret.second = m_msg->received.front();
		ret.first = true;
		m_msg->received.pop_front();
	}
	
	return ret;
}

void cOrganism::Move(cAvidaContext& ctx)
{
  assert(m_interface);
  
  /*********************/
  // TEMP.  Remove once movement tasks are implemented.
  if (GetCellData() < GetFacedCellData()) { // move up gradient
    SetGradientMovement(1.0);
  } else if(GetCellData() == GetFacedCellData()) {
    SetGradientMovement(0.0);
  } else { // move down gradient
    SetGradientMovement(-1.0);    
  }
  /*********************/    
  
  int fromcellID = GetCellID();
  int destcellID = GetFacedCellID();
  
  // Actually perform the move
  m_interface->Move(ctx, fromcellID, destcellID);
  
  // updates movement predicates
  m_world->GetStats().Move(*this);
  
  // Pheromone drop stuff
  double pher_amount = 0; // this is used in the logging
  int drop_mode = -1;

  // If organism is dropping pheromones, mark the appropriate cell(s)
  if (m_world->GetConfig().PHEROMONE_ENABLED.Get() == 1 && GetPheromoneStatus() == true) {
    pher_amount = m_world->GetConfig().PHEROMONE_AMOUNT.Get();
    drop_mode = m_world->GetConfig().PHEROMONE_DROP_MODE.Get();
    
    cDeme* deme = GetDeme();
    
    if (drop_mode == 0) {
      deme->AddPheromone(fromcellID, pher_amount / 2);
      deme->AddPheromone(destcellID, pher_amount / 2);
    } else if(drop_mode == 1) {
      deme->AddPheromone(fromcellID, pher_amount);
    } else if(drop_mode == 2) {
      deme->AddPheromone(destcellID, pher_amount);
    }
  } // End laying pheromone
  
  // Write some logging information if LOG_PHEROMONE is set.  This is done
  // out here so that non-pheromone moves are recorded.
  if (m_world->GetConfig().LOG_PHEROMONE.Get() == 1 &&
      m_world->GetStats().GetUpdate() >= m_world->GetConfig().MOVETARGET_LOG_START.Get()) {
    cDataFile& df = m_world->GetDataFile("movelog.dat");
    
    int rel_srcid = GetDeme()->GetRelativeCellID(fromcellID);
    int rel_destid = GetDeme()->GetRelativeCellID(destcellID);
    
    cString UpdateStr = cStringUtil::Stringf("%d,%d,%d,%d,%d,%f,%d,5",  m_world->GetStats().GetUpdate(), GetID(), GetDeme()->GetDemeID(), rel_srcid, rel_destid, pher_amount, drop_mode);
    df.WriteRaw(UpdateStr);
  }
  
  DoOutput(ctx);
  
  if (m_world->GetConfig().ACTIVE_MESSAGES_ENABLED.Get() > 0) {
    // then create new thread and load its registers
    m_hardware->InterruptThread(cHardwareBase::MOVE_INTERRUPT);
  }
} //End cOrganism::Move()

bool cOrganism::BcastAlarmMSG(cAvidaContext& ctx, int jump_label, int bcast_range) {
  assert(m_interface);
  
  // If we're able to succesfully send an alarm...
  if(m_interface->BcastAlarm(jump_label, bcast_range)) {
    // check to see if we've performed any tasks...
    DoOutput(ctx);
    return true;
  }  
  return false;
}

void cOrganism::moveIPtoAlarmLabel(int jump_label) {
  // move IP to alarm_label
  m_hardware->Jump_To_Alarm_Label(jump_label);
}


/*! Called to set this organism's opinion, which remains valid until a new opinion
 is expressed.
 */
void cOrganism::SetOpinion(const Opinion& opinion) {
  InitOpinions();
	
	const int bsize = m_world->GetConfig().OPINION_BUFFER_SIZE.Get();	

	if(bsize == 0) {
		m_world->GetDriver().RaiseFatalException(-1, "OPINION_BUFFER_SIZE is set to an invalid value.");
	}	
	
	if((bsize > 0) || (bsize == -1)) {
		m_opinion->opinion_list.push_back(std::make_pair(opinion, m_world->GetStats().GetUpdate()));
		// if our buffer is too large, chop off old messages:
		while((bsize != -1) && (static_cast<int>(m_opinion->opinion_list.size()) > bsize)) {
			m_opinion->opinion_list.pop_front();
		}
	}
}


/*! Called when an organism receives a flash from a neighbor. */
void cOrganism::ReceiveFlash() {
  m_hardware->ReceiveFlash();
}


/*! Called by the "flash" instruction. */
void cOrganism::SendFlash(cAvidaContext& ctx) {
  assert(m_interface);
  
  // Check to see if we should lose the flash:
  if((m_world->GetConfig().SYNC_FLASH_LOSSRATE.Get() > 0.0) &&
     (m_world->GetRandom().P(m_world->GetConfig().SYNC_FLASH_LOSSRATE.Get()))) {
    return;
  }
  
  // Flash not lost; continue.
  m_interface->SendFlash();
  m_world->GetStats().SentFlash(*this);
  DoOutput(ctx);
}


cOrganism::Neighborhood cOrganism::GetNeighborhood() {
	Neighborhood neighbors;
	for(int i=0; i<GetNeighborhoodSize(); ++i, Rotate(1)) {
		if(IsNeighborCellOccupied()) {
			neighbors.insert(GetNeighbor()->GetID());
		}
	}	
	return neighbors;
}


void cOrganism::LoadNeighborhood() {
	InitNeighborhood();
	m_neighborhood->neighbors = GetNeighborhood();
	m_neighborhood->loaded = true;
}


bool cOrganism::HasNeighborhoodChanged() {
	InitNeighborhood();
	// Must have loaded the neighborhood first:
	if(!m_neighborhood->loaded) return false;
	
	// Ok, get the symmetric difference between the old neighborhood and the current neighborhood:
	Neighborhood symdiff;
	Neighborhood current = GetNeighborhood();	
	std::set_symmetric_difference(m_neighborhood->neighbors.begin(),
																m_neighborhood->neighbors.end(),
																current.begin(),
																current.end(),
																std::insert_iterator<Neighborhood>(symdiff, symdiff.begin()));
	
	// If the symmetric difference is empty, then nothing has changed -- return 
	return !symdiff.empty();
}


/* Called when raw materials are donated to others or when the 
 raw materials are consumed. Amount is the number of resources 
 donated. The boolean flag is used to indicate if the donation 
 was successful... It would fail if the organism did not have 
 that many resources. */
bool cOrganism::SubtractSelfRawMaterials (int amount)
{
	bool isSuccessful = false;
	if (amount <= m_self_raw_materials) { 
		isSuccessful = true; 
		m_self_raw_materials -= amount;
	}
	return isSuccessful;
}


/* Called when other raw materials are consumed. Amount is the 
 number of resources consumed. The boolean flag is used to 
 indicate if the donation was successful... It would fail if 
 the organism did not have that many resources. */
bool cOrganism::SubtractOtherRawMaterials (int amount)
{
	bool isSuccessful = false;
	if (amount <= m_other_raw_materials) { 
		isSuccessful = true; 
		m_other_raw_materials -= amount;
	}
	return isSuccessful;
}

/* Called when raw materials are received from others. Amount 
 is the number of resources received. The boolean flag is used 
 to indicate if the reception was successful, which should always
 be the case... */

bool cOrganism::AddOtherRawMaterials (int amount, int donor_id) {
	bool isSuccessful = true;
	m_other_raw_materials += amount;
	donor_list.insert(donor_id);
	m_num_donate_received += amount;	
	m_amount_donate_received++;	
	return isSuccessful;
}

/* Called when raw materials are received from others. Amount 
 is the number of resources received. The boolean flag is used 
 to indicate if the reception was successful, which should always
 be the case... 
 
 This version is used if there is only one resource that is both
 donated and recieved.
 */

bool cOrganism::AddRawMaterials (int amount, int donor_id) {
	bool isSuccessful = true;
	m_self_raw_materials += amount;
	donor_list.insert(donor_id);	
	m_num_donate_received += amount;
	m_amount_donate_received++;
	return isSuccessful;
}


/* Get an organism's reputation, which is expressed as an 
 opinion. 0 is the default reputation (this should be refactored
 to be cleaner). */
int cOrganism::GetReputation() {
	int rep =0;
	if (HasOpinion()) {
		rep = GetOpinion().first;
	}
	return rep;
}

/* Set an organism's reputation */
void cOrganism::SetReputation(int rep) {
	SetOpinion(rep);
	return;
}

/* An organism's reputation is based on a running average*/
void cOrganism::SetAverageReputation(int rep){
	int current_total = GetReputation() * m_opinion->opinion_list.size(); 
	int new_rep = (current_total + rep)/(m_opinion->opinion_list.size()+1);
	SetReputation(new_rep);
}


/* Check if an organism has previously donated to this organism */
bool cOrganism::IsDonor(int neighbor_id) 
{
	bool found = false;
	if (donor_list.find(neighbor_id) != donor_list.end()) {
		found = true;
	}
	return found;
}



/* Update the tag. If the organism was not already tagged, 
 or the new tag is the same as the old tag, or the number
 of bits is > than the old tag, update.*/
void cOrganism::UpdateTag(int new_tag, int bits)
{
	unsigned int rand_int = m_world->GetRandom().GetUInt(0, 2);
	if ((m_tag.first == -1) || 
			(m_tag.first == new_tag) ||
			(m_tag.second < bits)) {
		m_tag = make_pair(new_tag, bits);
	} else if ((m_tag.second == bits) && rand_int){ 		
		m_tag = make_pair(new_tag, bits);
	}
}


/* See if the output buffer matches the string */
int cOrganism::MatchOutputBuffer(cString string_to_match)
{
	tBuffer<int> org_str (GetOutputBuf());
	int num_matched =0; 
	for (int j = 0; j < string_to_match.GetSize(); j++)
	{
		if (string_to_match[j]=='0' && org_str[j]==0 ||
				string_to_match[j]=='1' && org_str[j]==1)
			num_matched++;
	}
	return num_matched;
}


void cOrganism::SetOutputNegative1() 
{ 
	for (int i=0; i<GetOutputBuf().GetCapacity(); i++) {
		AddOutput(-1);
	}
	m_output_buf.Clear(); 
}

/* Initialize the string tracking map */
void cOrganism::InitStringMap() 
{
	if (!m_string_map.size()) { 
		// Get the strings from the task lib. 
		std::vector < cString > temp_strings = m_world->GetEnvironment().GetMatchStringsFromTask(); 
		// Create structure for each of them. 
		for (unsigned int i=0; i < temp_strings.size(); i++){
			m_string_map[i].m_string = temp_strings[i]; 
		}
	}
}


bool cOrganism::ProduceString(int i)  
{ 
	bool val = false; 
	int cap = m_world->GetConfig().STRING_AMOUNT_CAP.Get(); 
	if ((cap == -1) || (m_string_map[i].on_hand < cap)) 
	{
		m_string_map[i].prod_string++; 
		m_string_map[i].on_hand++;
		val = true;
	}
	return val;
}

/* Donate a string*/
bool cOrganism::DonateString(int string_tag, int amount)
{
	bool val = false; 
	if (m_string_map[string_tag].on_hand >= amount) {
		val = true;
		m_string_map[string_tag].on_hand -= amount;
	}
	return val;
	
}

/* Receive a string*/
bool cOrganism::ReceiveString(int string_tag, int amount, int donor_id)
{
	bool val = false; 
	int cap = m_world->GetConfig().STRING_AMOUNT_CAP.Get(); 
	if ((cap == -1) || (m_string_map[string_tag].on_hand < cap)) 
	{
		m_string_map[string_tag].received_string++; 
		m_string_map[string_tag].on_hand++;
		donor_list.insert(donor_id);	
		m_num_donate_received += amount;
		m_amount_donate_received++;
		val = true;
	}
	return val;
}

/* Check to see if this amount is below the organism's cap*/
bool cOrganism::CanReceiveString(int string_tag, int amount)
{
	bool val = false; 
	int cap = m_world->GetConfig().STRING_AMOUNT_CAP.Get(); 
	if ((cap == -1) || (m_string_map[string_tag].on_hand < cap)) 
	{
		val = true;
	}
	return val;
	
}

bool cOrganism::IsInterrupted()
{
  for (int k = 0; k< GetHardware().GetNumThreads(); ++k) if (GetHardware().GetThreadMessageTriggerType(k) != -1) return true;
  return false;
}

void cOrganism::DonateResConsumedToDeme()
{
	cDeme* deme = m_interface->GetDeme();
	
	if(deme) {
		deme->AddResourcesConsumed(m_phenotype.GetResourcesConsumed());
	}	
	return;
}

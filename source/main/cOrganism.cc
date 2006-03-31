/*
 *  cOrganism.cc
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cOrganism.h"

#include "cAvidaContext.h"
#include "nHardware.h"
#include "cEnvironment.h"
#include "functions.h"
#include "cGenome.h"
#include "cGenomeUtil.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInjectGenotype.h"
#include "cInstSet.h"
#include "cInstUtil.h"
#include "cOrgMessage.h"
#include "cOrgSinkMessage.h"
#include "cStringUtil.h"
#include "cTaskContext.h"
#include "cTools.h"
#include "cWorld.h"

#include <iomanip>

using namespace std;


cOrganism::cOrganism(cWorld* world, cAvidaContext& ctx, const cGenome& in_genome)
  : m_world(world)
  , genotype(NULL)
  , phenotype(world)
  , initial_genome(in_genome)
  , mut_info(world->GetEnvironment().GetMutationLib(), in_genome.GetSize())
  , m_interface(NULL)
  , input_pointer(0)
  , input_buf(INPUT_BUF_SIZE)
  , output_buf(OUTPUT_BUF_SIZE)
  , send_buf(SEND_BUF_SIZE)
  , receive_buf(RECEIVE_BUF_SIZE)
  , sent_value(0)
  , sent_active(false)
  , test_receive_pos(0)
  , max_executed(-1)
  , lineage_label(-1)
  , lineage(NULL)
  , inbox(0)
  , sent(0)
  , m_net(NULL)
  , is_running(false)
{
  // Initialization of structures...
  hardware = m_world->GetHardwareManager().Create(this);
  cpu_stats.Setup();

  if (m_world->GetConfig().DEATH_METHOD.Get() > 0) {
    max_executed = m_world->GetConfig().AGE_LIMIT.Get();
    if (m_world->GetConfig().AGE_DEVIATION.Get() > 0.0) {
      max_executed += (int) (ctx.GetRandom().GetRandNormal() * m_world->GetConfig().AGE_DEVIATION.Get());
    }
    if (m_world->GetConfig().DEATH_METHOD.Get() == DEATH_METHOD_MULTIPLE) {
      max_executed *= initial_genome.GetSize();
    }

    // max_executed must be positive or an organism will not die!
    if (max_executed < 1) max_executed = 1;
  }
  
  if (m_world->GetConfig().NET_ENABLED.Get()) m_net = new cNetSupport();
}


cOrganism::~cOrganism()
{
  assert(is_running == false);
  delete hardware;
  delete m_interface;
  if (m_net != NULL) delete m_net;
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


double cOrganism::GetTestFitness()
{
  assert(m_interface);
  return m_interface->TestFitness();
}
  
int cOrganism::ReceiveValue()
{
  assert(m_interface);
  const int out_value = m_interface->ReceiveValue();
  receive_buf.Add(out_value);
  return out_value;
}


void cOrganism::DoInput(const int value)
{
  input_buf.Add(value);
  phenotype.TestInput(input_buf, output_buf);
}

void cOrganism::DoOutput(cAvidaContext& ctx, const int value)
{
  assert(m_interface);
  const tArray<double> & resource_count = m_interface->GetResources();

  tList<tBuffer<int> > other_input_list;
  tList<tBuffer<int> > other_output_list;

  // If tasks require us to consider neighbor inputs, collect them...
  if (m_world->GetEnvironment().GetTaskLib().UseNeighborInput() == true) {
    const int num_neighbors = m_interface->GetNumNeighbors();
    for (int i = 0; i < num_neighbors; i++) {
      m_interface->Rotate();
      cOrganism * cur_neighbor = m_interface->GetNeighbor();
      if (cur_neighbor == NULL) continue;

      other_input_list.Push( &(cur_neighbor->input_buf) );
    }
  }

  // If tasks require us to consider neighbor outputs, collect them...
  if (m_world->GetEnvironment().GetTaskLib().UseNeighborOutput() == true) {
    const int num_neighbors = m_interface->GetNumNeighbors();
    for (int i = 0; i < num_neighbors; i++) {
      m_interface->Rotate();
      cOrganism * cur_neighbor = m_interface->GetNeighbor();
      if (cur_neighbor == NULL) continue;

      other_output_list.Push( &(cur_neighbor->output_buf) );
    }
  }
  
  bool net_valid = false;
  if (m_net) net_valid = NetValidate(ctx, value);

  // Do the testing of tasks performed...
  output_buf.Add(value);
  tArray<double> res_change(resource_count.GetSize());
  tArray<int> insts_triggered;
  cTaskContext taskctx(input_buf, output_buf, other_input_list, other_output_list, net_valid, 0);
  phenotype.TestOutput(ctx, taskctx, send_buf, receive_buf, resource_count, res_change, insts_triggered);
  m_interface->UpdateResources(res_change);

  for (int i = 0; i < insts_triggered.GetSize(); i++) {
    const int cur_inst = insts_triggered[i];
    hardware->ProcessBonusInst(ctx, cInstruction(cur_inst) );
  }
}

void cOrganism::SendMessage(cOrgMessage & mess)
{
  assert(m_interface);
  if(m_interface->SendMessage(mess))
    sent.Add(mess);
  else
    {
      //perhaps some kind of message error buffer?
    }
}

bool cOrganism::ReceiveMessage(cOrgMessage & mess)
{
  inbox.Add(mess);
  return true;
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
    actual_value ^= 1 << ctx.GetRandom().GetUInt(31); // Flip a single random bit
    m_net->sent[index].SetCorrupted();
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
  while (m_net->seq[m_net->last_seq].GetReceived()) {
    completed++;
    m_net->last_seq++;
  }
  
  if (completed) {
    assert(m_interface);
    const tArray<double>& resource_count = m_interface->GetResources();
    
    tList<tBuffer<int> > other_input_list;
    tList<tBuffer<int> > other_output_list;
    
    // If tasks require us to consider neighbor inputs, collect them...
    if (m_world->GetEnvironment().GetTaskLib().UseNeighborInput() == true) {
      const int num_neighbors = m_interface->GetNumNeighbors();
      for (int i = 0; i < num_neighbors; i++) {
        m_interface->Rotate();
        cOrganism * cur_neighbor = m_interface->GetNeighbor();
        if (cur_neighbor == NULL) continue;
        
        other_input_list.Push( &(cur_neighbor->input_buf) );
      }
    }
    
    // If tasks require us to consider neighbor outputs, collect them...
    if (m_world->GetEnvironment().GetTaskLib().UseNeighborOutput() == true) {
      const int num_neighbors = m_interface->GetNumNeighbors();
      for (int i = 0; i < num_neighbors; i++) {
        m_interface->Rotate();
        cOrganism * cur_neighbor = m_interface->GetNeighbor();
        if (cur_neighbor == NULL) continue;
        
        other_output_list.Push( &(cur_neighbor->output_buf) );
      }
    }
        
    // Do the testing of tasks performed...
    output_buf.Add(value);
    tArray<double> res_change(resource_count.GetSize());
    tArray<int> insts_triggered;
    cTaskContext taskctx(input_buf, output_buf, other_input_list, other_output_list, false, completed);
    phenotype.TestOutput(ctx, taskctx, send_buf, receive_buf, resource_count, res_change, insts_triggered);
    m_interface->UpdateResources(res_change);
    
    for (int i = 0; i < insts_triggered.GetSize(); i++) {
      const int cur_inst = insts_triggered[i];
      hardware->ProcessBonusInst(ctx, cInstruction(cur_inst) );
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


bool cOrganism::InjectParasite(const cGenome & injected_code)
{
  assert(m_interface);
  return m_interface->InjectParasite(this, injected_code);
}

bool cOrganism::InjectHost(const cCodeLabel & label, const cGenome & injected_code)
{
  return hardware->InjectHost(label, injected_code);
}

void cOrganism::AddParasite(cInjectGenotype * in_genotype)
{
  parasites.push_back(in_genotype);
}

cInjectGenotype & cOrganism::GetParasite(int x)
{
  return *parasites[x];
}

int cOrganism::GetNumParasites()
{
  return parasites.size();
}

void cOrganism::ClearParasites()
{
  parasites.clear();
}

int cOrganism::OK()
{
  if (!hardware->OK()) return false;
  if (!phenotype.OK()) return false;

  return true;
}


double cOrganism::CalcMeritRatio()
{
  const double age = (double) phenotype.GetAge();
  const double merit = phenotype.GetMerit().GetDouble();
  return (merit > 0.0) ? (age / merit ) : age;
}


bool cOrganism::GetTestOnDivide() const { return m_world->GetTestOnDivide();}
bool cOrganism::GetFailImplicit() const { return m_world->GetConfig().FAIL_IMPLICIT.Get(); }

bool cOrganism::GetRevertFatal() const { return m_world->GetConfig().REVERT_FATAL.Get(); }
bool cOrganism::GetRevertNeg()   const { return m_world->GetConfig().REVERT_DETRIMENTAL.Get(); }
bool cOrganism::GetRevertNeut()  const { return m_world->GetConfig().REVERT_NEUTRAL.Get(); }
bool cOrganism::GetRevertPos()   const { return m_world->GetConfig().REVERT_BENEFICIAL.Get(); }

bool cOrganism::GetSterilizeFatal() const{return m_world->GetConfig().STERILIZE_FATAL.Get();}
bool cOrganism::GetSterilizeNeg()  const { return m_world->GetConfig().STERILIZE_DETRIMENTAL.Get(); }
bool cOrganism::GetSterilizeNeut() const { return m_world->GetConfig().STERILIZE_NEUTRAL.Get();}
bool cOrganism::GetSterilizePos()  const { return m_world->GetConfig().STERILIZE_BENEFICIAL.Get(); }


void cOrganism::PrintStatus(ostream& fp, const cString & next_name)
{
  fp << "---------------------------" << endl;
  hardware->PrintStatus(fp);
  phenotype.PrintStatus(fp);
  fp << "---------------------------" << endl;
  fp << "ABOUT TO EXECUTE: " << next_name << endl;
}


bool cOrganism::Divide_CheckViable()
{
  // Make sure required task (if any) has been performed...
  const int required_task = m_world->GetConfig().REQUIRED_TASK.Get();
  const int immunity_task = m_world->GetConfig().IMMUNITY_TASK.Get();

  if (required_task != -1 && phenotype.GetCurTaskCount()[required_task] == 0) { 
    if (immunity_task==-1 || phenotype.GetCurTaskCount()[immunity_task] == 0) {
      Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
            cStringUtil::Stringf("Lacks required task (%d)", required_task));
      return false; //  (divide fails)
    } 
  }

  const int required_reaction = m_world->GetConfig().REQUIRED_REACTION.Get();
  if (required_reaction != -1 && phenotype.GetCurTaskCount()[required_reaction] == 0) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
          cStringUtil::Stringf("Lacks required reaction (%d)", required_reaction));
    return false; //  (divide fails)
  }

  // Make sure the parent is fertile
  if ( phenotype.IsFertile() == false ) {
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
  // Activate the child!  (Keep Last: may kill this organism!)
  return m_interface->Divide(ctx, this, child_genome);
}


void cOrganism::Fault(int fault_loc, int fault_type, cString fault_desc)
{
  (void) fault_loc;
  (void) fault_type;
  (void) fault_desc;

#ifdef FATAL_ERRORS
  if (fault_type == FAULT_TYPE_ERROR) {
    phenotype.IsFertile() = false;
  }
#endif

#ifdef FATAL_WARNINGS
  if (fault_type == FAULT_TYPE_WARNING) {
    phenotype.IsFertile() = false;
  }
#endif

#ifdef BREAKPOINTS
  phenotype.SetFault(fault_desc);
#endif

  phenotype.IncErrors();
}

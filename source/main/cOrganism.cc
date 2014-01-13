/*
 *  cOrganism.cc
 *  Avida
 *
 *  Called "organism.cc" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cOrganism.h"

#include "avida/core/Feedback.h"
#include "avida/core/UniverseDriver.h"

#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cPopulationCell.h"
#include "cStringUtil.h"
#include "cTaskContext.h"
#include "cWorld.h"
#include "cStats.h"

using namespace std;
using namespace Avida;


// Creation Policies
// --------------------------------------------------------------------------------------------------------------

cOrganism::cOrganism(cWorld* world, cAvidaContext& ctx, const Genome& genome, int parent_generation, Systematics::Source src)
  : m_world(world)
  , m_phenotype(world, parent_generation, world->GetHardwareManager().GetInstSet(genome.Properties().Get(s_ext_prop_name_instset).StringValue()).GetNumNops())
  , m_org_list_index(-1)
  , m_input_pointer(0)
  , m_input_buf(world->GetEnvironment().GetInputSize())
  , m_output_buf(world->GetEnvironment().GetOutputSize())
  , m_max_executed(-1)
  , m_forage_target(-1)
  , m_show_ft(-1)
  , m_has_set_ft(false)
  , m_teach(false)
  , m_parent_teacher(false)
  , m_parent_ft(-1)
  , m_p_merit(0)
{
	// initializing this here because it may be needed during hardware creation:
	m_id = m_world->GetStats().GetTotCreatures();
  initialize(ctx);
}

void cOrganism::initialize(cAvidaContext& ctx)
{
  m_phenotype.SetInstSetSize(m_hardware->GetInstSet().GetSize());
  const_cast<Genome&>(m_initial_genome).Properties().SetValue(s_ext_prop_name_instset,(const char*)m_hardware->GetInstSet().GetInstSetName());
  
  if (m_world->GetConfig().DEATH_METHOD.Get() > DEATH_METHOD_OFF) {
    m_max_executed = m_world->GetConfig().AGE_LIMIT.Get();
    if (m_world->GetConfig().AGE_DEVIATION.Get() > 0.0) {
      m_max_executed += (int) (ctx.GetRandom().GetRandNormal() * m_world->GetConfig().AGE_DEVIATION.Get());
    }
    if (m_world->GetConfig().DEATH_METHOD.Get() == DEATH_METHOD_MULTIPLE) {
      ConstInstructionSequencePtr seq;
      seq.DynamicCastFrom(m_initial_genome.Representation());
      m_max_executed *= seq->GetSize();
    }
    
    // m_max_executed must be positive or an organism will not die!
    if (m_max_executed < 1) m_max_executed = 1;
  }
  
  
}

cOrganism::~cOrganism()
{  
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
  if (m_world->GetConfig().USE_AVATARS.Get()) doAVOutput(ctx, m_input_buf, m_output_buf, on_divide);
  else doOutput(ctx, m_input_buf, m_output_buf, on_divide);
}

void cOrganism::DoOutput(cAvidaContext& ctx, const int value)
{
  m_output_buf.Add(value);
  if (m_world->GetConfig().USE_AVATARS.Get()) doAVOutput(ctx, m_input_buf, m_output_buf, false);
  else doOutput(ctx, m_input_buf, m_output_buf, false);
}


void cOrganism::DoOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value)
{
  output_buffer.Add(value);
  if (m_world->GetConfig().USE_AVATARS.Get()) doAVOutput(ctx, input_buffer, output_buffer, false);
  else doOutput(ctx, input_buffer, output_buffer, false);
}


void cOrganism::doOutput(cAvidaContext& ctx, 
                         tBuffer<int>& input_buffer, 
                         tBuffer<int>& output_buffer,
                         const bool on_divide)
{  
  const Apto::Array<double> & global_resource_count = m_interface->GetResources(ctx);
  
  tList<tBuffer<int> > other_input_list;
  tList<tBuffer<int> > other_output_list;
  
  // If tasks require us to consider neighbor inputs, collect them...
  if (m_world->GetEnvironment().UseNeighborInput()) {
    const int num_neighbors = m_interface->GetNumNeighbors();
    for (int i = 0; i < num_neighbors; i++) {
      m_interface->Rotate(ctx);
      cOrganism * cur_neighbor = m_interface->GetNeighbor();
      if (cur_neighbor == NULL) continue;
      
      other_input_list.Push( &(cur_neighbor->m_input_buf) );
    }
  }
  
  // If tasks require us to consider neighbor outputs, collect them...
  if (m_world->GetEnvironment().UseNeighborOutput()) {
    const int num_neighbors = m_interface->GetNumNeighbors();
    for (int i = 0; i < num_neighbors; i++) {
      m_interface->Rotate(ctx);
      cOrganism * cur_neighbor = m_interface->GetNeighbor();
      if (cur_neighbor == NULL) continue;
      
      other_output_list.Push( &(cur_neighbor->m_output_buf) );
    }
  }
  
  // Do the testing of tasks performed...
  
  
  Apto::Array<double> global_res_change(global_resource_count.GetSize());
  global_res_change.SetAll(0.0);
  Apto::Array<cString> insts_triggered;
  
  
  cTaskContext taskctx(this, input_buffer, output_buffer, other_input_list, other_output_list,
                       m_hardware->GetExtendedMemory(), on_divide);
  
  
  m_phenotype.TestOutput(ctx, taskctx, global_resource_count, m_phenotype.GetCurRBinsAvail(), global_res_change,
                         insts_triggered);
  
  // Handle merit increases that take the organism above it's current population merit
  if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    double cur_merit = m_phenotype.CalcCurrentMerit();
    if (m_phenotype.GetMerit().GetDouble() < cur_merit) m_interface->UpdateMerit(cur_merit);
  }
  
  m_interface->UpdateResources(ctx, global_res_change);

  for (int i = 0; i < insts_triggered.GetSize(); i++)
    m_hardware->ProcessBonusInst(ctx, m_hardware->GetInstSet().GetInst(insts_triggered[i]));
}

void cOrganism::doAVOutput(cAvidaContext& ctx, 
                         tBuffer<int>& input_buffer, 
                         tBuffer<int>& output_buffer,
                         const bool on_divide)
{  
  //Avatar output has to be seperate from doOutput to ensure avatars, not the true orgs, are triggering reactions
  
  tList<tBuffer<int> > other_input_list;
  tList<tBuffer<int> > other_output_list;
  
  // If tasks require us to consider neighbor inputs, collect them...
  if (m_world->GetEnvironment().UseNeighborInput()) {
    const int num_neighbors = m_interface->GetAVNumNeighbors();
    for (int i = 0; i < num_neighbors; i++) {
      m_interface->Rotate(ctx);
      const Apto::Array<cOrganism*>& cur_neighbors = m_interface->GetFacedAVs();
      for (int i = 0; i < cur_neighbors.GetSize(); i++) {
        if (cur_neighbors[i] == NULL) continue;
        other_input_list.Push( &(cur_neighbors[i]->m_input_buf) );
      }
    }
  }
  
  // If tasks require us to consider neighbor outputs, collect them...
  if (m_world->GetEnvironment().UseNeighborOutput()) {
    const int num_neighbors = m_interface->GetAVNumNeighbors();
    for (int i = 0; i < num_neighbors; i++) {
      m_interface->Rotate(ctx);
      const Apto::Array<cOrganism*>& cur_neighbors = m_interface->GetFacedAVs();
      for (int i = 0; i < cur_neighbors.GetSize(); i++) {
        if (cur_neighbors[i] == NULL) continue;
        other_output_list.Push( &(cur_neighbors[i]->m_output_buf) );
      }
    }
  }
  
  // Do the testing of tasks performed...
  Apto::Array<double> avatar_res_change(m_world->GetEnvironment().GetResDefLib().GetSize());
  avatar_res_change.SetAll(0.0);

  Apto::Array<cString> insts_triggered;
  
  cTaskContext taskctx(this, input_buffer, output_buffer, other_input_list, other_output_list,
                       m_hardware->GetExtendedMemory(), on_divide);
  
  const Apto::Array<double>& av_res_count = m_interface->GetAVResources(ctx);
  
  
  m_phenotype.TestOutput(ctx, taskctx, av_res_count, m_phenotype.GetCurRBinsAvail(), avatar_res_change, insts_triggered);
  
  // Handle merit increases that take the organism above it's current population merit
  if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    double cur_merit = m_phenotype.CalcCurrentMerit();
    if (m_phenotype.GetMerit().GetDouble() < cur_merit) m_interface->UpdateMerit(cur_merit);
  }
  
  
  m_interface->UpdateAVResources(ctx, avatar_res_change);
  
  for (int i = 0; i < insts_triggered.GetSize(); i++) 
    m_hardware->ProcessBonusInst(ctx, m_hardware->GetInstSet().GetInst(insts_triggered[i]));
}

void cOrganism::NotifyDeath(cAvidaContext& ctx)
{
  // Return currently stored internal resources to the world
  if (m_world->GetConfig().USE_RESOURCE_BINS.Get() && m_world->GetConfig().RETURN_STORED_ON_DEATH.Get()) {
  	if (m_world->GetConfig().USE_AVATARS.Get()) m_interface->UpdateAVResources(ctx, GetRBins());
    else m_interface->UpdateResources(ctx, GetRBins());
  }  
}




double cOrganism::CalcMeritRatio()
{
  const double age = (double) m_phenotype.GetAge();
  const double merit = m_phenotype.GetMerit().GetDouble();
  return (merit > 0.0) ? (age / merit ) : age;
}




void cOrganism::PrintStatus(ostream& fp)
{
  fp << "---------------------------" << endl;
	fp << "U:" << m_world->GetStats().GetUpdate() << endl;
  if (m_hardware->GetType() != HARDWARE_TYPE_CPU_GP8) m_hardware->PrintStatus(fp);
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
    
    ConstInstructionSequencePtr seq;
    seq.DynamicCastFrom(m_offspring_genome.Representation());
    fp << "# Offspring Memory: " << seq->AsString() << endl;
  }
}

bool cOrganism::Divide_CheckViable(cAvidaContext& ctx)
{
  // Make sure required task (if any) has been performed...
  const int required_task = m_world->GetConfig().REQUIRED_TASK.Get();
  const int immunity_task = m_world->GetConfig().IMMUNITY_TASK.Get();
  if (m_world->GetConfig().REQUIRED_PRED_HABITAT.Get() != -1 || m_world->GetConfig().REQUIRED_PREY_HABITAT.Get() != -1) {
    int habitat_required = -1;
    double required_value = 0;
    if (m_forage_target <= -2) {
      habitat_required = m_world->GetConfig().REQUIRED_PRED_HABITAT.Get();
      required_value = m_world->GetConfig().REQUIRED_PRED_HABITAT_VALUE.Get();
    }
    else {
      habitat_required = m_world->GetConfig().REQUIRED_PREY_HABITAT.Get();
      required_value = m_world->GetConfig().REQUIRED_PREY_HABITAT_VALUE.Get();
    }
    if (habitat_required != -1) {
      bool has_req_res = false;
      const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
      double resource_count = 0;
      for (int i = 0; i < resource_lib.GetSize(); i ++) {
        if (resource_lib.GetResDef(i)->GetHabitat() == habitat_required) {
          if (!m_world->GetConfig().USE_AVATARS.Get()) resource_count = m_interface->GetResourceVal(ctx, i);
          else resource_count = m_interface->GetAVResourceVal(ctx, i);
          if (resource_count >= required_value) {
            has_req_res = true;
            break;
          }
        }
      }
      if (!has_req_res) return false;
    }
  }
  
  if (required_task != -1 && m_phenotype.GetCurTaskCount()[required_task] == 0) { 
    if (immunity_task ==-1 || m_phenotype.GetCurTaskCount()[immunity_task] == 0) {
      return false; //  (divide fails)
    } 
  }
  
  if (GetPhenotype().GetCurBonus() < m_world->GetConfig().REQUIRED_BONUS.Get()) return false;
  
  const int required_reaction = m_world->GetConfig().REQUIRED_REACTION.Get();
  const int immunity_reaction = m_world->GetConfig().IMMUNITY_REACTION.Get();
  const int single_reaction = m_world->GetConfig().REQUIRE_SINGLE_REACTION.Get();
  
  if (single_reaction == 0 && required_reaction != -1 && m_phenotype.GetCurReactionCount()[required_reaction] == 0 && \
      m_phenotype.GetStolenReactionCount()[required_reaction] == 0)   {
    if (immunity_reaction == -1 || m_phenotype.GetCurReactionCount()[immunity_reaction] == 0) {  
      return false; //  (divide fails)
    }
  }
  
  if (single_reaction != 0)
  {
    bool toFail = true;
    Apto::Array<int> reactionCounts = m_phenotype.GetCurReactionCount();
    for (int i=0; i<reactionCounts.GetSize(); i++)
    {
      if (reactionCounts[i] > 0) toFail = false;
    }
    
    if (toFail)
    {
      const Apto::Array<int>& stolenReactions = m_phenotype.GetStolenReactionCount();
      for (int i = 0; i < stolenReactions.GetSize(); i++)
      {
        if (stolenReactions[i] > 0) toFail = false;
      }
    }
    
    if (toFail) return false; //  (divide fails)
  }
  
  // Test for required resource availability (must be stored in an internal resource bin)
  const int required_resource = m_world->GetConfig().REQUIRED_RESOURCE.Get();
  const double required_resource_level = m_world->GetConfig().REQUIRED_RESOURCE_LEVEL.Get();
  if (required_resource != -1 && required_resource_level > 0.0) {
    const double resource_level = m_phenotype.GetCurRBinAvail(required_resource);
    if (resource_level < required_resource_level) return false;
    else AddToRBin(required_resource, -required_resource_level);
  }
  
  // Make sure the parent is fertile
  if (m_phenotype.IsFertile() == false) return false; //  (divide fails)
  
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
  return m_interface->Divide(ctx, this, m_offspring_genome);
}


bool cOrganism::Move(cAvidaContext& ctx)
{
  assert(m_interface);
  if (m_is_dead) return false;  
  
  int fromcellID = GetCellID();
  int destcellID = GetFacedCellID();
  
  int facing = GetFacedDir();
  
  // Actually perform the move
  if (m_interface->Move(ctx, fromcellID, destcellID)) {
    //Keep track of successful movement E/W and N/S in support of get-easterly and get-northerly for navigation
    //Skip counting if random < chance of miscounting a step.
    if (m_world->GetConfig().STEP_COUNTING_ERROR.Get()==0 || ctx.GetRandom().GetInt(0,101) > m_world->GetConfig().STEP_COUNTING_ERROR.Get()) {
      if (facing == 0) m_northerly = m_northerly - 1;       // N
      else if (facing == 1) {                           // NE
        m_northerly = m_northerly - 1; 
        m_easterly = m_easterly + 1;
      }  
      else if (facing == 2) m_easterly = m_easterly + 1;    // E
      else if (facing == 3) {                           // SE
        m_northerly = m_northerly + 1; 
        m_easterly = m_easterly + 1;
      }
      else if (facing == 4) m_northerly = m_northerly + 1;  // S
      else if (facing == 5) {                           // SW
        m_northerly = m_northerly + 1; 
        m_easterly = m_easterly - 1;
      }
      else if (facing == 6) m_easterly = m_easterly - 1;    // W    
      else if (facing == 7) {                           // NW
        m_northerly = m_northerly - 1; 
        m_easterly = m_easterly - 1;
      }      
    }
  }
  else return false;              
  
  // Check to make sure the organism is alive after the move
  if (m_phenotype.GetToDelete()) return false;
  
  
  // don't trigger reactions on move if you're not supposed to! 
  const cEnvironment& env = m_world->GetEnvironment();
  const int num_tasks = env.GetNumTasks();
  for (int i = 0; i < num_tasks; i++) {
    if (env.GetTask(i).GetDesc() == "move_up_gradient" || \
        env.GetTask(i).GetDesc() == "move_neutral_gradient" || \
        env.GetTask(i).GetDesc() == "move_down_gradient" || \
        env.GetTask(i).GetDesc() == "move_not_up_gradient" || \
        env.GetTask(i).GetDesc() == "move") {
      DoOutput(ctx);
      break;
    }
  }
  
  return true;    
} //End cOrganism::Move()


void cOrganism::SetForageTarget(cAvidaContext& ctx, int forage_target, bool inject) {
  if (m_parent_ft <= -2 && m_world->GetConfig().MAX_PRED.Get() && m_world->GetStats().GetNumTotalPredCreatures() >= m_world->GetConfig().MAX_PRED.Get()) m_interface->KillRandPred(ctx, this);
  else if (forage_target > -2 && m_world->GetConfig().MAX_PREY.Get() && m_world->GetStats().GetNumPreyCreatures() >= m_world->GetConfig().MAX_PREY.Get()) m_interface->KillRandPrey(ctx, this);

  // if using avatars, make sure you swap avatar lists if the org type changes!
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() == -2 || m_world->GetConfig().PRED_PREY_SWITCH.Get() > -1) {
    // change to pred
    if (forage_target == -2 && m_forage_target > -2) {
      if (!inject) m_interface->DecNumPreyOrganisms();
      m_interface->IncNumPredOrganisms();
    }
    else if (forage_target == -2 && m_forage_target < -2) {
      if (!inject) m_interface->DecNumTopPredOrganisms();
      m_interface->IncNumPredOrganisms();
    }
    // change to top pred
    else if (forage_target < -2 && m_forage_target > -2) {
      if (!inject) m_interface->DecNumPreyOrganisms();
      m_interface->IncNumTopPredOrganisms();
    }
    else if (forage_target < -2 && m_forage_target == -2) {
      if (!inject) m_interface->DecNumPredOrganisms();
      m_interface->IncNumTopPredOrganisms();
    }
    // change to prey
    else if (forage_target > -2 && m_forage_target == -2) {
      m_interface->IncNumPreyOrganisms();
      if (!inject) m_interface->DecNumPredOrganisms();
    }
    else if (forage_target > -2 && m_forage_target < -2) {
      m_interface->IncNumPreyOrganisms();
      if (!inject) m_interface->DecNumTopPredOrganisms();
    }
  }
  m_forage_target = forage_target;
  if (m_show_ft == -1) m_show_ft = m_forage_target;
}

void cOrganism::CopyParentFT(cAvidaContext& ctx) {
  bool copy_ft = true;
  // close potential loop-hole allowing orgs to switch ft to prey at birth, collect res,
  // switch ft to pred, and then copy parent to become prey again.
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() <= 0 || m_world->GetConfig().PRED_PREY_SWITCH.Get() == 2) {
    if (m_parent_ft > -2 && m_forage_target < -1) {
      copy_ft = false;
    }
  }
  if (copy_ft) SetForageTarget(ctx, m_parent_ft);
}


bool cOrganism::MoveAV(cAvidaContext& ctx)
{
  assert(m_interface);
  if (m_is_dead) return false;
  
  // Actually perform the move
  if (m_interface->MoveAV(ctx)) {
    //Keep track of successful movement E/W and N/S in support of get-easterly and get-northerly for navigation
    //Skip counting if random < chance of miscounting a step.
    if (m_world->GetConfig().STEP_COUNTING_ERROR.Get() == 0 || ctx.GetRandom().GetInt(0,101) > m_world->GetConfig().STEP_COUNTING_ERROR.Get()) {
      int facing = m_interface->GetAVFacing();

      if (facing == 0)
        m_northerly = m_northerly - 1;                  // N
      else if (facing == 1) {
        m_northerly = m_northerly - 1;                  // NE
        m_easterly = m_easterly + 1;
      }  
      else if (facing == 2)
        m_easterly = m_easterly + 1;                    // E
      else if (facing == 3) {
        m_northerly = m_northerly + 1;                  // SE
        m_easterly = m_easterly + 1;
      }
      else if (facing == 4)
        m_northerly = m_northerly + 1;                  // S
      else if (facing == 5) {
        m_northerly = m_northerly + 1;                  // SW
        m_easterly = m_easterly - 1;
      }
      else if (facing == 6)
        m_easterly = m_easterly - 1;                    // W    
      else if (facing == 7) {
        m_northerly = m_northerly - 1;                  // NW
        m_easterly = m_easterly - 1;
      }      
    }
    else return false;                  
  }
  
  // Check to make sure the organism is alive after the move
  if (m_phenotype.GetToDelete()) return false;
  
  return true;
}


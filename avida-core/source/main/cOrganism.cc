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
#include "avida/core/WorldDriver.h"

#include "cAvidaContext.h"
#include "cContextPhenotype.h"
#include "cDeme.h"
#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInstSet.h"
#include "cOrgSensor.h"
#include "cPopulationCell.h"
#include "cStateGrid.h"
#include "cStringUtil.h"
#include "cTaskContext.h"
#include "cWorld.h"
#include "cStats.h"
#include "nHardware.h"

#include <algorithm>
#include <iomanip>
#include <iterator>
#include <utility>

using namespace std;
using namespace Avida;

// Referenced external properties
// --------------------------------------------------------------------------------------------------------------
static const Apto::BasicString<Apto::ThreadSafe> s_ext_prop_name_instset("instset");


// Internal cOrganism Properties
// --------------------------------------------------------------------------------------------------------------

static PropertyDescriptionMap s_prop_desc_map;

static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_genome("genome");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_src_transmission_type("src_transmission_type");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_age("age");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_generation("generation");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_copied_size("last_copied_size");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_executed_size("last_exectuted_size");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_gestation_time("last_gestation_time");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_metabolic_rate("last_metabolic_rate");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_fitness("last_fitness");


// OrgPropRetrievalContainer - base container class for the global property map
// --------------------------------------------------------------------------------------------------------------

class OrgPropRetrievalContainer
{
public:
  virtual ~OrgPropRetrievalContainer() { ; }
  
  virtual const Property& Get(cOrganism*, const cOrganism::OrgPropertyMap*) const = 0;
};


// OrgPropOfType - concrete implementations of OrgPropRetrievalContainer for each necessary type
// --------------------------------------------------------------------------------------------------------------

template <class T> class OrgPropOfType : public OrgPropRetrievalContainer
{
private:
  typedef T (cOrganism::*RetrieveFunction)();
  
  PropertyID m_prop_id;
  RetrieveFunction m_fun;
  
public:
  OrgPropOfType(const PropertyID& prop_id, RetrieveFunction fun) : m_prop_id(prop_id), m_fun(fun) { ; }
  
  const Property& Get(cOrganism* org, const cOrganism::OrgPropertyMap* prop_map) const
  {
    return prop_map->SetTempProp(m_prop_id, (org->*m_fun)());
  }
};


// OrgGlobalPropMap and OrgGlobalPropMapSingletone - global singleton structure holding the global org prop map
// --------------------------------------------------------------------------------------------------------------

struct OrgGlobalPropMap
{
  Apto::Map<Apto::String, OrgPropRetrievalContainer*> prop_map;
  
  ~OrgGlobalPropMap()
  {
    for (Apto::Map<Apto::String, OrgPropRetrievalContainer*>::ValueIterator it = prop_map.Values(); it.Next();) {
      delete *it.Get();
    }
  }
};

typedef Apto::SingletonHolder<OrgGlobalPropMap, Apto::CreateWithNew, Apto::DestroyAtExit, Apto::ThreadSafe> OrgGlobalPropMapSingleton;


// cOrganism::Intialize() - static method that sets up global data structures
// --------------------------------------------------------------------------------------------------------------

void cOrganism::Initialize()
{
#define DEFINE_PROP(NAME, TYPE, FUNCTION, DESC) s_prop_desc_map.Set(s_prop_name_ ## NAME, DESC); \
  OrgGlobalPropMapSingleton::Instance().prop_map.Set(s_prop_name_ ## NAME, new OrgPropOfType<TYPE>(s_prop_name_ ## NAME, &cOrganism::FUNCTION));
  DEFINE_PROP(genome, Apto::String, getGenomeString, "Genome");
  DEFINE_PROP(src_transmission_type, int, getSrcTransmissionType, "Source Transmission Type");
  DEFINE_PROP(age, int, getAge, "Age");
  DEFINE_PROP(generation, int, getGeneration, "Generation");
  DEFINE_PROP(last_copied_size, int, getLastCopied, "Last Copied Size");
  DEFINE_PROP(last_executed_size, int, getLastExecuted, "Last Exectuted Size");
  DEFINE_PROP(last_gestation_time, int, getLastGestation, "Last Gestation Time");
  DEFINE_PROP(last_metabolic_rate, double, getLastMetabolicRate, "Last Metabolic Rage");
  DEFINE_PROP(last_fitness, double, getLastFitness, "Last Fitness");
#undef DEFINE_PROP
}



// Creation Policies
// --------------------------------------------------------------------------------------------------------------

cOrganism::cOrganism(cWorld* world, cAvidaContext& ctx, const Genome& genome, int parent_generation, Systematics::Source src)
  : m_world(world)
  , m_phenotype(world, parent_generation, world->GetHardwareManager().GetInstSet(genome.Properties().Get(s_ext_prop_name_instset).StringValue()).GetNumNops())
  , m_src(src)
  , m_initial_genome(genome)
  , m_interface(NULL)
  , m_lineage_label(-1)
  , m_lineage(NULL)
  , m_org_list_index(-1)
  , m_org_display(NULL)
  , m_queued_display_data(NULL)
  , m_display(false)
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
  , m_northerly(0)
  , m_easterly(0)
  , m_forage_target(-1)
  , m_show_ft(-1)
  , m_has_set_ft(false)
  , m_teach(false)
  , m_parent_teacher(false)
  , m_parent_ft(-1)
  , m_parent_group(world->GetConfig().DEFAULT_GROUP.Get())
  , m_p_merit(0)
  , m_beggar(false)
  , m_para_donate(world->GetConfig().PARASITE_VIRULENCE.Get())
  , m_guard(false)
  , m_num_guard(0)
  , m_num_deposits(0)
  , m_amount_deposited(0)
  , m_string_map(NULL)
  , m_num_point_mut(0)
  , m_av_in_index(-1)
  , m_av_out_index(-1)
  , m_prop_map(this)
{
	// initializing this here because it may be needed during hardware creation:
	m_id = m_world->GetStats().GetTotCreatures();
  
  m_hardware = m_world->GetHardwareManager().Create(ctx, this, genome);
  
  initialize(ctx);
}

void cOrganism::initialize(cAvidaContext& ctx)
{
  m_phenotype.SetInstSetSize(m_hardware->GetInstSet().GetSize());
  const_cast<Genome&>(m_initial_genome).Properties().SetValue(s_ext_prop_name_instset,(const char*)m_hardware->GetInstSet().GetInstSetName());
  m_phenotype.SetGroupAttackInstSetSize(m_world->GetStats().GetGroupAttackInsts(m_hardware->GetInstSet().GetInstSetName()).GetSize());
  
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
  
  m_repair = (m_world->GetConfig().POINT_MUT_REPAIR_START.Get());
  
	// randomize the amout of raw materials an organism has at its 
	// disposal.
	if (m_world->GetConfig().RANDOMIZE_RAW_MATERIAL_AMOUNT.Get()) {
		int raw_mat = m_world->GetConfig().RAW_MATERIAL_AMOUNT.Get();
		m_self_raw_materials = ctx.GetRandom().GetUInt(0, raw_mat+1);
	}
}

cOrganism::~cOrganism()
{  
  assert(m_is_running == false);
  delete m_hardware;
  delete m_interface;
  
  if(m_msg) delete m_msg;
  if(m_opinion) delete m_opinion;  
  if (m_neighborhood) delete m_neighborhood;
  delete m_org_display;
  delete m_queued_display_data;
  if (m_string_map) delete m_string_map;
}


const PropertyMap& cOrganism::Properties() const { return m_prop_map; }

void cOrganism::SetOrgInterface(cAvidaContext& ctx, cOrgInterface* org_interface)
{
  delete m_interface;
  m_interface = org_interface;
  
  HardwareReset(ctx);
}

const cStateGrid& cOrganism::GetStateGrid() const { return m_world->GetEnvironment().GetStateGrid(m_cur_sg); }

double cOrganism::GetVitality() const {
  double mean_age = m_world->GetStats().SumCreatureAge().Ave();
  double age_stddev = m_world->GetStats().SumCreatureAge().StdDeviation();
  int org_age = m_phenotype.GetAge();
  const int resource = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  double res_level = 0.0;
  if (resource >= 0) 
    res_level = m_phenotype.GetCurRBinAvail(resource);
  double vitality = 0.0;
  
  if (org_age < (mean_age - age_stddev) || org_age > (mean_age + age_stddev)) {
    vitality = m_world->GetConfig().VITALITY_BIN_EXTREMES.Get() * res_level;
  } else {
    vitality = m_world->GetConfig().VITALITY_BIN_CENTER.Get() * res_level;    
  }
  
  return vitality;
}

bool cOrganism::UpdateOrgDisplay() { 
  if (m_queued_display_data != NULL) {
    delete m_org_display;
    m_org_display = m_queued_display_data; 
    m_queued_display_data = NULL; 
    return true;
  }
  else return false;
}

void cOrganism::SetSimpDisplay(int display_part, int value)
{
  if (m_org_display == NULL) {
    m_org_display = new sOrgDisplay;
    m_display = true;
    m_org_display->distance = -99;
    m_org_display->direction = -99;
    m_org_display->value = -99;
    m_org_display->message = -99;

  }
  switch (display_part) {
    case 0:
      m_org_display->distance = value;
    case 1:
      m_org_display->direction = value;
    case 3:
      m_org_display->value = value;
    default:
      m_org_display->message = value;
  }
}

double cOrganism::GetRBinsTotal()
{
	double total = 0;
	for(int i = 0; i < m_phenotype.GetCurRBinsAvail().GetSize(); i++) {
    total += m_phenotype.GetCurRBinsAvail()[i];
  }
	
	return total;
}

void cOrganism::SetRBins(const Apto::Array<double>& rbins_in)
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
	if (value > 0) { 
    m_phenotype.AddToCurRBinTotal(index, value); 
  }
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

void cOrganism::DoInput(const int value)
{
  DoInput(m_input_buf, m_output_buf, value);
}

void cOrganism::DoInput(tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value)
{
  input_buffer.Add(value);
  m_phenotype.TestInput(input_buffer, output_buffer);
}

void cOrganism::DoOutput(cAvidaContext& ctx, const bool on_divide, cContextPhenotype* context_phenotype)
{
  if (m_world->GetConfig().USE_AVATARS.Get()) doAVOutput(ctx, m_input_buf, m_output_buf, on_divide, false, context_phenotype);
  else doOutput(ctx, m_input_buf, m_output_buf, on_divide, false, context_phenotype);
}

void cOrganism::DoOutput(cAvidaContext& ctx, const int value)
{
  m_output_buf.Add(value);
  if (m_world->GetConfig().USE_AVATARS.Get()) doAVOutput(ctx, m_input_buf, m_output_buf, false, false);
  else doOutput(ctx, m_input_buf, m_output_buf, false, false);
}

void cOrganism::DoOutput(cAvidaContext& ctx, const int value, bool is_parasite, cContextPhenotype* context_phenotype) 
{
  m_output_buf.Add(value);
  if (m_world->GetConfig().USE_AVATARS.Get()) doAVOutput(ctx, m_input_buf, m_output_buf, false, (bool)is_parasite, context_phenotype); 
  else doOutput(ctx, m_input_buf, m_output_buf, false, (bool)is_parasite, context_phenotype); 
}

void cOrganism::DoOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value)
{
  output_buffer.Add(value);
  if (m_world->GetConfig().USE_AVATARS.Get()) doAVOutput(ctx, input_buffer, output_buffer, false, false);
  else doOutput(ctx, input_buffer, output_buffer, false, false);
}


void cOrganism::doOutput(cAvidaContext& ctx, 
                         tBuffer<int>& input_buffer, 
                         tBuffer<int>& output_buffer,
                         const bool on_divide,
                         bool is_parasite, 
                         cContextPhenotype* context_phenotype)
{  
  const int deme_id = m_interface->GetDemeID();
  const Apto::Array<double> & global_resource_count = m_interface->GetResources(ctx);
  const Apto::Array<double> & deme_resource_count = m_interface->GetDemeResources(deme_id, ctx);
  const Apto::Array< Apto::Array<int> > & cell_id_lists = m_interface->GetCellIdLists();
  
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
  Apto::Array<double> deme_res_change(deme_resource_count.GetSize());
  deme_res_change.SetAll(0.0);
  Apto::Array<cString> insts_triggered;
  
  tBuffer<int>* received_messages_point = &m_received_messages;
  if (!m_world->GetConfig().SAVE_RECEIVED.Get()) received_messages_point = NULL;
  
  cTaskContext taskctx(this, input_buffer, output_buffer, other_input_list, other_output_list,
                       m_hardware->GetExtendedMemory(), on_divide, received_messages_point);
  
  //combine global and deme resource counts
  Apto::Array<double> globalAndDeme_resource_count = global_resource_count + deme_resource_count;
  Apto::Array<double> globalAndDeme_res_change = global_res_change + deme_res_change;
  
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
                                               insts_triggered, is_parasite, context_phenotype);
  
  // Handle merit increases that take the organism above it's current population merit
  if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    double cur_merit = m_phenotype.CalcCurrentMerit();
    if (m_phenotype.GetMerit().GetDouble() < cur_merit) m_interface->UpdateMerit(ctx, cur_merit);
  }
  
  //disassemble global and deme resource counts 
  for (int i = 0; i < global_res_change.GetSize(); i++) global_res_change[i] = globalAndDeme_res_change[i];
  for (int i = 0; i < deme_res_change.GetSize(); i++) deme_res_change[i] = globalAndDeme_res_change[i + global_res_change.GetSize()];
  
  if(m_world->GetConfig().ENERGY_ENABLED.Get() && m_world->GetConfig().APPLY_ENERGY_METHOD.Get() == 1 && task_completed) {
    m_phenotype.RefreshEnergy();
    m_phenotype.ApplyToEnergyStore();
    double newMerit = m_phenotype.ConvertEnergyToMerit(m_phenotype.GetStoredEnergy() * m_phenotype.GetEnergyUsageRatio());
    m_interface->UpdateMerit(ctx, newMerit);
    if(GetPhenotype().GetMerit().GetDouble() == 0.0) {
      GetPhenotype().SetToDie();
    }
  }
  m_interface->UpdateResources(ctx, global_res_change);

  //update deme resources
  m_interface->UpdateDemeResources(ctx, deme_res_change);

  for (int i = 0; i < insts_triggered.GetSize(); i++) 
    m_hardware->ProcessBonusInst(ctx, m_hardware->GetInstSet().GetInst(insts_triggered[i]));
}

void cOrganism::doAVOutput(cAvidaContext& ctx, 
                         tBuffer<int>& input_buffer, 
                         tBuffer<int>& output_buffer,
                         const bool on_divide,
                         bool is_parasite, 
                         cContextPhenotype* context_phenotype)
{  
  //Avatar output has to be seperate from doOutput to ensure avatars, not the true orgs, are triggering reactions
  //  const int deme_id = m_interface->GetDemeID();
  //  const tArray<double> & deme_resource_count = m_interface->GetDemeResources(deme_id, ctx); //todo: DemeAVResources
  const Apto::Array< Apto::Array<int> > & cell_id_lists = m_interface->GetCellIdLists();
  
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
  Apto::Array<double> avatar_res_change(m_world->GetEnvironment().GetResourceLib().GetSize());
  avatar_res_change.SetAll(0.0);

  //  tArray<double> deme_res_change(deme_resource_count.GetSize());
  //  deme_res_change.SetAll(0.0);

  Apto::Array<cString> insts_triggered;
  
  tBuffer<int>* received_messages_point = &m_received_messages;
  if (!m_world->GetConfig().SAVE_RECEIVED.Get()) received_messages_point = NULL;
  
  cTaskContext taskctx(this, input_buffer, output_buffer, other_input_list, other_output_list,
                       m_hardware->GetExtendedMemory(), on_divide, received_messages_point);
  
  //combine global and deme resource counts
  const Apto::Array<double>& av_res_count = m_interface->GetAVResources(ctx);
  Apto::Array<double> avatarAndDeme_res_count = av_res_count; // + deme_resource_count;
  Apto::Array<double> avatarAndDeme_res_change = avatar_res_change; // + deme_res_change;
  
  // set any resource amount to 0 if a cell cannot access this resource
  int cell_id = m_interface->GetAVCellID();
  if (cell_id_lists.GetSize())
  {
	  for (int i = 0; i < cell_id_lists.GetSize(); i++)
	  {
		  // if cell_id_lists have been set then we have to check if this cell is in the list
		  if (cell_id_lists[i].GetSize()) {
			  int j = 0;
			  for (j = 0; j < cell_id_lists[i].GetSize(); j++)
			  {
				  if (cell_id == cell_id_lists[i][j])
					  break;
			  }
			  if (j == cell_id_lists[i].GetSize())
				  avatarAndDeme_res_count[i] = 0;
		  }
	  }
  }
  
  bool task_completed = m_phenotype.TestOutput(ctx, taskctx, avatarAndDeme_res_count, 
                                               m_phenotype.GetCurRBinsAvail(), avatarAndDeme_res_change, 
                                               insts_triggered, is_parasite, context_phenotype);
  
  // Handle merit increases that take the organism above it's current population merit
  if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    double cur_merit = m_phenotype.CalcCurrentMerit();
    if (m_phenotype.GetMerit().GetDouble() < cur_merit) m_interface->UpdateMerit(ctx, cur_merit);
  }
  
  //disassemble avatar and deme resource counts
  for (int i = 0; i < avatar_res_change.GetSize(); i++) avatar_res_change[i] = avatarAndDeme_res_change[i];
//  deme_res_change = avatarAndDeme_res_change.Subset(avatar_res_change.GetSize(), avatarAndDeme_res_change.GetSize());
  
  if(m_world->GetConfig().ENERGY_ENABLED.Get() && m_world->GetConfig().APPLY_ENERGY_METHOD.Get() == 1 && task_completed) {
    m_phenotype.RefreshEnergy();
    m_phenotype.ApplyToEnergyStore();
    double newMerit = m_phenotype.ConvertEnergyToMerit(m_phenotype.GetStoredEnergy() * m_phenotype.GetEnergyUsageRatio());
		m_interface->UpdateMerit(ctx, newMerit);
		if(GetPhenotype().GetMerit().GetDouble() == 0.0) {
			GetPhenotype().SetToDie();
		}
  }
  m_interface->UpdateAVResources(ctx, avatar_res_change);
  //update deme resources
//  m_interface->UpdateDemeResources(ctx, deme_res_change);
  
  for (int i = 0; i < insts_triggered.GetSize(); i++) 
    m_hardware->ProcessBonusInst(ctx, m_hardware->GetInstSet().GetInst(insts_triggered[i]));
}

void cOrganism::HardwareReset(cAvidaContext& ctx)
{
  if (m_world->GetEnvironment().GetNumStateGrids() > 0 && m_interface) {
    // Select random state grid in the environment
    m_cur_sg = m_interface->GetStateGridID(ctx);
    
    const cStateGrid& sg = GetStateGrid();
    
    Apto::Array<int, Apto::Smart> sg_state(3 + sg.GetNumStates());
    sg_state.SetAll(0);
    
    sg_state[0] = sg.GetInitialX();
    sg_state[1] = sg.GetInitialY();
    sg_state[2] = sg.GetInitialFacing(); 
    
    m_hardware->SetupExtendedMemory(sg_state);
  }
  
  if (!m_world->GetConfig().INHERIT_OPINION.Get()) {
    ClearOpinion();
  }
  delete m_org_display;
  delete m_queued_display_data;
  m_org_display = NULL;
  m_queued_display_data = NULL;
  m_display = false;
}

void cOrganism::NotifyDeath(cAvidaContext& ctx)
{
  // Update Sleeping State
  if (m_is_sleeping) {
    m_is_sleeping = false;
    GetDeme()->DecSleepingCount();
  }
  
  // Return currently stored internal resources to the world
  if (m_world->GetConfig().USE_RESOURCE_BINS.Get() && m_world->GetConfig().RETURN_STORED_ON_DEATH.Get()) {
  	if (m_world->GetConfig().USE_AVATARS.Get()) m_interface->UpdateAVResources(ctx, GetRBins());
    else m_interface->UpdateResources(ctx, GetRBins());
  }
  
  // Make sure the group composition is updated.
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() && HasOpinion()) m_interface->LeaveGroup(GetOpinion().first);  
}



bool cOrganism::InjectParasite(Systematics::UnitPtr parent, const cString& label, const InstructionSequence& injected_code)
{
  assert(m_interface);
  return m_interface->InjectParasite(this, parent, label, injected_code);
}

bool cOrganism::ParasiteInfectHost(Systematics::UnitPtr parasite)
{
  if (!m_hardware->ParasiteInfectHost(parasite)) return false;
  
  m_parasites.Push(parasite);
  return true;
}

void cOrganism::ClearParasites()
{
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
bool cOrganism::GetRevertEquals() const { return m_world->GetConfig().REVERT_EQUALS.Get(); }

bool cOrganism::GetSterilizeFatal() const { return m_world->GetConfig().STERILIZE_FATAL.Get(); }
bool cOrganism::GetSterilizeNeg() const { return m_world->GetConfig().STERILIZE_DETRIMENTAL.Get(); }
bool cOrganism::GetSterilizeNeut() const { return m_world->GetConfig().STERILIZE_NEUTRAL.Get(); }
bool cOrganism::GetSterilizePos() const { return m_world->GetConfig().STERILIZE_BENEFICIAL.Get(); }
bool cOrganism::GetSterilizeTaskLoss() const { return m_world->GetConfig().STERILIZE_TASKLOSS.Get(); }
double cOrganism::GetNeutralMin() const { return m_world->GetConfig().NEUTRAL_MIN.Get(); }
double cOrganism::GetNeutralMax() const { return m_world->GetConfig().NEUTRAL_MAX.Get(); }


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

void cOrganism::PrintMiniTraceStatus(cAvidaContext& ctx, ostream & fp)
{
  m_hardware->PrintMiniTraceStatus(ctx, fp);
}

void cOrganism::PrintMiniTraceSuccess(ostream & fp, const int exec_success)
{
  m_hardware->PrintMiniTraceSuccess(fp, exec_success);
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
  if (GetPhenotype().GetCurBonus() < m_world->GetConfig().REQUIRED_BONUS.Get()) return false;
  
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
      const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
      double resource_count = 0;
      for (int i = 0; i < resource_lib.GetSize(); i ++) {
        if (resource_lib.GetResource(i)->GetHabitat() == habitat_required) {
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
      Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
            cStringUtil::Stringf("Lacks required task (%d)", required_task));
      return false; //  (divide fails)
    } 
  }
  
  const int required_reaction = m_world->GetConfig().REQUIRED_REACTION.Get();
  const int immunity_reaction = m_world->GetConfig().IMMUNITY_REACTION.Get();
  const int single_reaction = m_world->GetConfig().REQUIRE_SINGLE_REACTION.Get();
  
  if (single_reaction == 0 && required_reaction != -1 && m_phenotype.GetCurReactionCount()[required_reaction] == 0 && \
      m_phenotype.GetStolenReactionCount()[required_reaction] == 0)   {
    if (immunity_reaction == -1 || m_phenotype.GetCurReactionCount()[immunity_reaction] == 0) {  
      Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
            cStringUtil::Stringf("Lacks required reaction (%d)", required_reaction));
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
    
    if (toFail) {
      Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR, cStringUtil::Stringf("Lacks any reaction required for divide"));
      return false; //  (divide fails)
    }
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
  if ( m_phenotype.IsFertile() == false ) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR, "Infertile organism");
    return false; //  (divide fails)
  }
  
  // No zero merit offspring!
  int cur_merit_base = GetPhenotype().CalcSizeMerit();
  const int merit_default_bonus = m_world->GetConfig().MERIT_DEFAULT_BONUS.Get();
  int cur_bonus = GetPhenotype().GetCurBonus();
  if (merit_default_bonus) {
    cur_bonus = merit_default_bonus;
  }
  double off_merit = cur_merit_base * cur_bonus;
  
  if (m_world->GetConfig().INHERIT_MERIT.Get() == 0) {
    off_merit = cur_merit_base;
  }
  if (off_merit == 0) return false;
  
  return true;  // Organism has no problem with divide...
}


// This gets called after a successful divide to deal with the child. 
// Returns true if parent lives through this process.

bool cOrganism::ActivateDivide(cAvidaContext& ctx, cContextPhenotype* context_phenotype)
{
  assert(m_interface);
  // Test tasks one last time before actually dividing, pass true so 
  // know that should only test "divide" tasks here
  DoOutput(ctx, true, context_phenotype);
  
  // Activate the child!  (Keep Last: may kill this organism!)
  return m_interface->Divide(ctx, this, m_offspring_genome);
}


void cOrganism::Fault(int fault_loc, int fault_type, cString fault_desc)
{
  (void) fault_loc;
  (void) fault_type;
  (void) fault_desc;
  
  // FATAL_ERRORS
#if 0
  if (fault_type == FAULT_TYPE_ERROR) {
    m_phenotype.IsFertile() = false;
  }
#endif
  
  // FATAL_WARNINGS
#if 0
  if (fault_type == FAULT_TYPE_WARNING) {
    m_phenotype.IsFertile() = false;
  }
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
void cOrganism::MessageSent(cAvidaContext&, cOrgMessage& msg) {
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
bool cOrganism::SendMessage(cAvidaContext& ctx, cOrgMessage& msg)
{
  assert(m_interface);
  InitMessaging();

  // check to see if we've performed any tasks:
  if (m_world->GetConfig().CHECK_TASK_ON_SEND.Get()) {
    DoOutput(ctx, static_cast<int>(msg.GetData()));
  }
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
void cOrganism::ReceiveMessage(cOrgMessage& msg)
{
  InitMessaging();
	// don't store more messages than we're configured to.
	const int bsize = m_world->GetConfig().MESSAGE_RECV_BUFFER_SIZE.Get();
	if((bsize != -1) && (bsize <= static_cast<int>(m_msg->received.size()))) {
		switch (m_world->GetConfig().MESSAGE_RECV_BUFFER_BEHAVIOR.Get()) {
			case 0: // drop oldest message
				m_msg->received.pop_front();
				break;
			case 1: // drop this message
				return;
			default: // error
        m_world->GetDriver().Feedback().Error("MESSAGE_RECV_BUFFER_BEHAVIOR is set to an invalid value.");
        m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
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

bool cOrganism::Move(cAvidaContext& ctx)
{
  assert(m_interface);
  if (m_is_dead) return false;  
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
      deme->AddPheromone(fromcellID, pher_amount / 2, ctx); 
      deme->AddPheromone(destcellID, pher_amount / 2, ctx); 
    } else if(drop_mode == 1) {
      deme->AddPheromone(fromcellID, pher_amount, ctx); 
    } else if(drop_mode == 2) {
      deme->AddPheromone(destcellID, pher_amount, ctx); 
    }
  } // End laying pheromone
    
  // don't trigger reactions on move if you're not supposed to! 
  const cEnvironment& env = m_world->GetEnvironment();
  const int num_tasks = env.GetNumTasks();
  for (int i = 0; i < num_tasks; i++) {
    if (env.GetTask(i).GetDesc() == "move_up_gradient" || \
        env.GetTask(i).GetDesc() == "move_neutral_gradient" || \
        env.GetTask(i).GetDesc() == "move_down_gradient" || \
        env.GetTask(i).GetDesc() == "move_not_up_gradient" || \
        env.GetTask(i).GetDesc() == "move_to_right_side" || \
        env.GetTask(i).GetDesc() == "move_to_left_side" || \
        env.GetTask(i).GetDesc() == "move" || \
        env.GetTask(i).GetDesc() == "movetotarget" || \
        env.GetTask(i).GetDesc() == "movetoevent" || \
        env.GetTask(i).GetDesc() == "movebetweenevent" || \
        env.GetTask(i).GetDesc() == "move_to_event") {
      DoOutput(ctx);
      break;
    }
  }
  
  if (m_world->GetConfig().ACTIVE_MESSAGES_ENABLED.Get() > 0) {
    // then create new thread and load its registers
    m_hardware->InterruptThread(cHardwareBase::MOVE_INTERRUPT);
  }
  return true;    
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
    m_world->GetDriver().Feedback().Error("OPINION_BUFFER_SIZE is set to an invalid value.");
    m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
  }	
  
  if((bsize > 0) || (bsize == -1)) {
    m_opinion->opinion_list.push_back(std::make_pair(opinion, m_world->GetStats().GetUpdate()));
    // if our buffer is too large, chop off old messages:
    while((bsize != -1) && (static_cast<int>(m_opinion->opinion_list.size()) > bsize)) {
      m_opinion->opinion_list.pop_front();
    }
  }
  // if using avatars, make sure you swap avatar lists if the org's catorization changes!
}

// Checks if the organism has an opinion.
bool cOrganism::HasOpinion() {
  InitOpinions();
  if (m_opinion->opinion_list.empty()) return false;
  else return true;
}

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

/*! Called when an organism receives a flash from a neighbor. */
void cOrganism::ReceiveFlash() {
  m_hardware->ReceiveFlash();
}

/*! Called by the "flash" instruction. */
void cOrganism::SendFlash(cAvidaContext& ctx) {
  assert(m_interface);
  
  // Check to see if we should lose the flash:
  if((m_world->GetConfig().SYNC_FLASH_LOSSRATE.Get() > 0.0) &&
     (ctx.GetRandom().P(m_world->GetConfig().SYNC_FLASH_LOSSRATE.Get()))) {
    return;
  }
  
  // Flash not lost; continue.
  m_interface->SendFlash();
  m_world->GetStats().SentFlash(*this);
  DoOutput(ctx);
}


cOrganism::Neighborhood cOrganism::GetNeighborhood(cAvidaContext& ctx) {
	Neighborhood neighbors;
	for(int i=0; i<GetNeighborhoodSize(); ++i, Rotate(ctx, 1)) {
		if(IsNeighborCellOccupied()) {
			neighbors.insert(GetNeighbor()->GetID());
		}
	}	
	return neighbors;
}


void cOrganism::LoadNeighborhood(cAvidaContext& ctx) {
	InitNeighborhood();
	m_neighborhood->neighbors = GetNeighborhood(ctx);
	m_neighborhood->loaded = true;
}


bool cOrganism::HasNeighborhoodChanged(cAvidaContext& ctx) {
	InitNeighborhood();
	// Must have loaded the neighborhood first:
	if(!m_neighborhood->loaded) return false;
	
	// Ok, get the symmetric difference between the old neighborhood and the current neighborhood:
	Neighborhood symdiff;
	Neighborhood current = GetNeighborhood(ctx);
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
		if ((string_to_match[j]=='0' && org_str[j]==0) ||
				(string_to_match[j]=='1' && org_str[j]==1))
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
	if (!m_string_map) {
    m_string_map = new std::map < int, cStringSupport >;
		// Get the strings from the task lib. 
		std::vector < cString > temp_strings = m_world->GetEnvironment().GetMatchStringsFromTask(); 
		// Create structure for each of them. 
		for (unsigned int i=0; i < temp_strings.size(); i++){
			(*m_string_map)[i].m_string = temp_strings[i];
		}
	}
}


bool cOrganism::ProduceString(int i)  
{ 
	bool val = false; 
	int cap = m_world->GetConfig().STRING_AMOUNT_CAP.Get(); 
	if ((cap == -1) || ((*m_string_map)[i].on_hand < cap))
	{
		(*m_string_map)[i].prod_string++; 
		(*m_string_map)[i].on_hand++;
		val = true;
	}
	return val;
}

/* Donate a string*/
bool cOrganism::DonateString(int string_tag, int amount)
{
	bool val = false; 
	if ((*m_string_map)[string_tag].on_hand >= amount) {
		val = true;
		(*m_string_map)[string_tag].on_hand -= amount;
	}
	return val;
	
}

/* Receive a string*/
bool cOrganism::ReceiveString(int string_tag, int amount, int donor_id)
{
	bool val = false; 
	int cap = m_world->GetConfig().STRING_AMOUNT_CAP.Get(); 
	if ((cap == -1) || ((*m_string_map)[string_tag].on_hand < cap)) 
	{
		(*m_string_map)[string_tag].received_string++; 
		(*m_string_map)[string_tag].on_hand++;
		donor_list.insert(donor_id);	
		m_num_donate_received += amount;
		m_amount_donate_received++;
		val = true;
	}
	return val;
}

/* Check to see if this amount is below the organism's cap*/
bool cOrganism::CanReceiveString(int string_tag, int)
{
	bool val = false; 
	int cap = m_world->GetConfig().STRING_AMOUNT_CAP.Get(); 
	if ((cap == -1) || ((*m_string_map)[string_tag].on_hand < cap))
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
  
  // updates movement predicates
  //  m_world->GetStats().Move(*this);
  
  return true;    
}



// cOrganism::OrgPropertyMap implementation
// --------------------------------------------------------------------------------------------------------------

cOrganism::OrgPropertyMap::OrgPropertyMap(cOrganism* organism)
  : m_organism(organism), m_prop_int(s_prop_desc_map), m_prop_double(s_prop_desc_map), m_prop_string(s_prop_desc_map) { ; }
cOrganism::OrgPropertyMap::~OrgPropertyMap() { ; }

int cOrganism::OrgPropertyMap::GetSize() const
{
  return OrgGlobalPropMapSingleton::Instance().prop_map.GetSize();
}

bool cOrganism::OrgPropertyMap::Has(const PropertyID& p_id) const
{
  return OrgGlobalPropMapSingleton::Instance().prop_map.Has(p_id);
}

const Avida::Property& cOrganism::OrgPropertyMap::Get(const PropertyID& p_id) const
{
  OrgPropRetrievalContainer* container = NULL;
  if (OrgGlobalPropMapSingleton::Instance().prop_map.Get(p_id, container)) {
    return container->Get(m_organism, this);
  }

  return *s_default_prop;
}


bool cOrganism::OrgPropertyMap::SetValue(const PropertyID& p_id, const Apto::String& prop_value) { return false; }
bool cOrganism::OrgPropertyMap::SetValue(const PropertyID& p_id, const int prop_value) { return false; }
bool cOrganism::OrgPropertyMap::SetValue(const PropertyID& p_id, const double prop_value) { return false; }


bool cOrganism::OrgPropertyMap::operator==(const PropertyMap& p) const
{
  // Build distinct key sets
  Apto::Set<PropertyID> pm1pids, pm2pids;
  Apto::Map<PropertyID, OrgPropRetrievalContainer*>::KeyIterator it = OrgGlobalPropMapSingleton::Instance().prop_map.Keys();
  while (it.Next()) pm1pids.Insert(*it.Get());
  
  PropertyIDSet::ConstIterator pidit = p.PropertyIDs()->Begin();
  while (pidit.Next()) pm2pids.Insert(*pidit.Get());
  
  // Compare key sets
  if (pm1pids != pm2pids) return false;
  
  // Compare values
  it = OrgGlobalPropMapSingleton::Instance().prop_map.Keys();
  while (it.Next()) {
    OrgPropRetrievalContainer* container = NULL;
    if (OrgGlobalPropMapSingleton::Instance().prop_map.Get(*it.Get(), container)) {
      if (container->Get(m_organism, this) != p.Get(*it.Get())) return false;
    } else {
      return false;
    }
  }
  
  return true;
}

void cOrganism::OrgPropertyMap::Define(PropertyPtr p) { ; }
bool cOrganism::OrgPropertyMap::Remove(const PropertyID& p_id) { return false; }

Avida::ConstPropertyIDSetPtr cOrganism::OrgPropertyMap::PropertyIDs() const
{
  PropertyIDSetPtr pidset(new PropertyIDSet);
  
  Apto::Map<PropertyID, OrgPropRetrievalContainer*>::KeyIterator it = OrgGlobalPropMapSingleton::Instance().prop_map.Keys();
  while (it.Next()) pidset->Insert(*it.Get());
  
  return pidset;
}


bool cOrganism::OrgPropertyMap::Serialize(ArchivePtr) const
{
  // @TODO
  assert(false);
  return false;
}



// Property Map Retrieival Functions
// --------------------------------------------------------------------------------------------------------------

Apto::String cOrganism::getGenomeString() { return m_initial_genome.AsString(); }
int cOrganism::getSrcTransmissionType() { return m_src.transmission_type; }
int cOrganism::getAge() { return m_phenotype.GetAge(); }
int cOrganism::getGeneration() { return m_phenotype.GetGeneration(); }
int cOrganism::getLastCopied() { return m_phenotype.GetCopiedSize(); }
int cOrganism::getLastExecuted() { return m_phenotype.GetExecutedSize(); }
int cOrganism::getLastGestation() { return m_phenotype.GetGestationTime(); }
double cOrganism::getLastMetabolicRate() { return m_phenotype.GetLastMerit(); }
double cOrganism::getLastFitness() { return m_phenotype.GetFitness(); }

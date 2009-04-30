/*
 *  cGenotype.cc
 *  Avida
 *
 *  Called "genotype.cc" prior to 11/30/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#include "cGenotype.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cGenomeUtil.h"
#include "cHardwareManager.h"
#include "cMerit.h"
#include "cPhenotype.h"
#include "cTestCPU.h"
#include "cTools.h"
#include "cWorld.h"
#include "cPhenPlastGenotype.h"
#include "cPhenPlastSummary.h"
#include "cAvidaConfig.h"

using namespace std;

cGenotype::cGenotype(cWorld* world, int in_update_born, int in_id)
  : m_world(world)
  , genome(1)
  , name("001-no_name")
  , m_flag_threshold(false)
  , m_is_active(true)
  , m_track_parent_dist(m_world->GetConfig().TRACK_PARENT_DIST.Get())
  , defer_adjust(0)
  , id_num(in_id)
  , symbol(0)
  , map_color_id(-2)
  , birth_data(in_update_born)
  , m_phenplast(NULL)
  , num_organisms(0)
  , last_num_organisms(0)
  , total_organisms(0)
  , total_parasites(0)
  , species(NULL)
  , next(NULL)
  , prev(NULL)
{
}

cGenotype::~cGenotype()
{
  // Reset some of the variables to make sure program will crash if a deleted
  // cell is read!
  symbol = '!';
  map_color_id = -5;

  num_organisms = -1;
  total_organisms = -1;

  next = NULL;
  prev = NULL;
  
  if (m_phenplast != NULL)
    delete m_phenplast;

}

bool cGenotype::SaveClone(ofstream& fp)
{
  fp << id_num         << " ";
  fp << genome.GetSize() << " ";

  for (int i = 0; i < genome.GetSize(); i++) {
    fp << ((int) genome[i].GetOp()) << " ";
  }

  return true;
}

cGenotype* cGenotype::LoadClone(cWorld* world, ifstream& fp)
{
  int tmp_id = -1;
  int genome_size = 0;

  fp >> tmp_id;
  fp >> genome_size;

  cGenotype* ret = new cGenotype(world, 0, tmp_id);
  ret->genome = cGenome(genome_size);
  for (int i = 0; i < genome_size; i++) {
    cInstruction temp_inst;
    int inst_op;
    fp >> inst_op;
    temp_inst.SetOp(static_cast<unsigned char>(inst_op));
    ret->genome[i] = temp_inst;
    // @CAO add something here to load arguments for instructions.
  }
  
  return ret;
}

bool cGenotype::OK()
{
  // Check statistics
  assert( id_num >= 0 && num_organisms >= 0 && total_organisms >= 0 );
  assert( birth_data.update_born >= -1 && birth_data.parent_distance >= -1 );
  assert( sum_copied_size.Sum() >= 0 && sum_exe_size.Sum() >= 0 );
  assert( sum_gestation_time.Sum() >= 0 && sum_repro_rate.Sum() >= 0 );
  assert( sum_merit.Sum() >= 0 && sum_fitness.Sum() >= 0 );
  assert( tmp_sum_copied_size.Sum() >= 0 && tmp_sum_exe_size.Sum() >= 0 );
  assert( tmp_sum_gestation_time.Sum() >= 0 && tmp_sum_repro_rate.Sum() >= 0 );
  assert( tmp_sum_merit.Sum() >= 0 && tmp_sum_fitness.Sum() >= 0 );

  return true;
}

void cGenotype::AddMerit(const cMerit & in)
{
  sum_merit.Add(in.GetDouble());
}

void cGenotype::RemoveMerit(const cMerit & in)
{
  sum_merit.Subtract(in.GetDouble());
}

void cGenotype::SetParent(cGenotype* parent, cGenotype* parent2)
{
  birth_data.parent_genotype = parent;
  birth_data.parent2_genotype = parent2;

  if (parent == NULL) return;

  // If we have a real parent genotype, collect other data about parent.
  birth_data.ancestor_ids[0] = parent->GetID();
  birth_data.ancestor_ids[2] = parent->GetAncestorID(0);
  birth_data.ancestor_ids[3] = parent->GetAncestorID(1);
  if (parent2 != NULL) {
    birth_data.ancestor_ids[1] = parent2->GetID();
    birth_data.ancestor_ids[4] = parent2->GetAncestorID(0);
    birth_data.ancestor_ids[5] = parent2->GetAncestorID(1);    
  }

  if (m_track_parent_dist) birth_data.parent_distance = cGenomeUtil::FindEditDistance(genome, parent->genome);
  birth_data.parent_species = parent->GetSpecies();
  birth_data.gene_depth = parent->GetDepth() + 1;
  birth_data.lineage_label = parent->GetLineageLabel();
  parent->AddOffspringGenotype();
  if (parent2 != NULL && m_world->GetConfig().TRACK_MAIN_LINEAGE.Get() == 2) {
    parent2->AddOffspringGenotype();
  }

  // Initialize all stats to those of the parent genotype....
  tmp_sum_copied_size.Add(    parent->GetCopiedSize());
  tmp_sum_exe_size.Add(       parent->GetExecutedSize());
  tmp_sum_gestation_time.Add( parent->GetGestationTime());
  tmp_sum_repro_rate.Add(   1/parent->GetGestationTime());
  tmp_sum_merit.Add(          parent->GetMerit());
  tmp_sum_fitness.Add(        parent->GetFitness());
}

void cGenotype::UpdateReset()
{
  last_num_organisms = num_organisms;
  birth_data.birth_track.Next();
  birth_data.death_track.Next();
  birth_data.breed_out_track.Next();
  birth_data.breed_true_track.Next();
  birth_data.breed_in_track.Next();
}

void cGenotype::SetGenome(const cGenome & in_genome)
{
  genome = in_genome;
  // Zero Stats
  sum_copied_size.Clear();
  sum_exe_size.Clear();
  sum_gestation_time.Clear();
  sum_repro_rate.Clear();
  sum_merit.Clear();
  sum_fitness.Clear();
  tmp_sum_copied_size.Clear();
  tmp_sum_exe_size.Clear();
  tmp_sum_gestation_time.Clear();
  tmp_sum_repro_rate.Clear();
  tmp_sum_merit.Clear();
  tmp_sum_fitness.Clear();

  name.Set("%03d-no_name", genome.GetSize());
}

void cGenotype::CalcTestStats(cAvidaContext& ctx) const
{
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();

  cCPUTestInfo test_info;
  testcpu->TestGenome(ctx, test_info, genome);
  test_data.is_viable = test_info.IsViable();
  delete testcpu;

  // Setup all possible test values.
  cPhenotype & phenotype = test_info.GetTestPhenotype();
  test_data.fitness = test_info.GetGenotypeFitness();
  test_data.merit = phenotype.GetMerit().GetDouble();
  test_data.gestation_time = phenotype.GetGestationTime();
  test_data.executed_size = phenotype.GetExecutedSize();
  test_data.copied_size = phenotype.GetCopiedSize();
  test_data.colony_fitness = test_info.GetColonyFitness();
  test_data.generations = test_info.GetMaxDepth();
}


double cGenotype::GetTaskPlasticity(cAvidaContext& ctx, int task_id) const{
  if (m_phenplast == NULL)
    TestPlasticity(ctx);
  assert(task_id >= 0 && task_id < m_phenplast->m_task_probabilities.GetSize());
  return m_phenplast->m_task_probabilities[task_id];
} 

void cGenotype::TestPlasticity(cAvidaContext& ctx) const{
  cCPUTestInfo test_info;
  cPhenPlastGenotype pp(genome, m_world->GetConfig().GENOTYPE_PHENPLAST_CALC.Get(), test_info, m_world, ctx);
  m_phenplast = new cPhenPlastSummary(pp);
}


int cGenotype::GetPhyloDistance(cGenotype * test_genotype)
{
  if (GetID() == test_genotype->GetID()) {
    return 0;
  }
  if (GetID() == test_genotype->GetAncestorID(0) ||  // Parent of test
      GetID() == test_genotype->GetAncestorID(1) ||  // Parent of test
      test_genotype->GetID() == GetAncestorID(0) ||  // Child of test
      test_genotype->GetID() == GetAncestorID(1)     // Child of test
      ) {
    return 1;
  }
  if (GetID() == test_genotype->GetAncestorID(2) ||  // Grandparent of test
      GetID() == test_genotype->GetAncestorID(3) ||  // Grandparent of test
      GetID() == test_genotype->GetAncestorID(4) ||  // Grandparent of test
      GetID() == test_genotype->GetAncestorID(5) ||  // Grandparent of test
      test_genotype->GetID() == GetAncestorID(2) ||  // Grandchild of test
      test_genotype->GetID() == GetAncestorID(3) ||  // Grandchild of test
      test_genotype->GetID() == GetAncestorID(4) ||  // Grandchild of test
      test_genotype->GetID() == GetAncestorID(5) ||  // Grandchild of test
      GetAncestorID(0) == test_genotype->GetAncestorID(0) || // Sibling of test
      GetAncestorID(0) == test_genotype->GetAncestorID(1) || // Sibling of test
      GetAncestorID(1) == test_genotype->GetAncestorID(0) || // Sibling of test
      GetAncestorID(1) == test_genotype->GetAncestorID(1)    // Sibling of test
      ) {
    return 2;
  }
  if (GetAncestorID(0) == test_genotype->GetAncestorID(2) || // Uncle of test
      GetAncestorID(0) == test_genotype->GetAncestorID(3) || // Uncle of test
      GetAncestorID(0) == test_genotype->GetAncestorID(4) || // Uncle of test
      GetAncestorID(0) == test_genotype->GetAncestorID(5) || // Uncle of test
      GetAncestorID(1) == test_genotype->GetAncestorID(2) || // Uncle of test
      GetAncestorID(1) == test_genotype->GetAncestorID(3) || // Uncle of test
      GetAncestorID(1) == test_genotype->GetAncestorID(4) || // Uncle of test
      GetAncestorID(1) == test_genotype->GetAncestorID(5) || // Uncle of test
      test_genotype->GetAncestorID(0) == GetAncestorID(2) || // Nephew of test
      test_genotype->GetAncestorID(0) == GetAncestorID(3) || // Nephew of test
      test_genotype->GetAncestorID(0) == GetAncestorID(4) || // Nephew of test
      test_genotype->GetAncestorID(0) == GetAncestorID(5) || // Nephew of test
      test_genotype->GetAncestorID(1) == GetAncestorID(2) || // Nephew of test
      test_genotype->GetAncestorID(1) == GetAncestorID(3) || // Nephew of test
      test_genotype->GetAncestorID(1) == GetAncestorID(4) || // Nephew of test
      test_genotype->GetAncestorID(1) == GetAncestorID(5)    // Nephew of test
      ) {
    return 3;
  }

  if (GetAncestorID(2) == test_genotype->GetAncestorID(2) || // First Cousins
      GetAncestorID(2) == test_genotype->GetAncestorID(3) ||
      GetAncestorID(2) == test_genotype->GetAncestorID(4) ||
      GetAncestorID(2) == test_genotype->GetAncestorID(5) ||
      GetAncestorID(3) == test_genotype->GetAncestorID(2) ||
      GetAncestorID(3) == test_genotype->GetAncestorID(3) ||
      GetAncestorID(3) == test_genotype->GetAncestorID(4) ||
      GetAncestorID(3) == test_genotype->GetAncestorID(5) ||
      GetAncestorID(4) == test_genotype->GetAncestorID(2) ||
      GetAncestorID(4) == test_genotype->GetAncestorID(3) ||
      GetAncestorID(4) == test_genotype->GetAncestorID(4) ||
      GetAncestorID(4) == test_genotype->GetAncestorID(5) ||
      GetAncestorID(5) == test_genotype->GetAncestorID(2) ||
      GetAncestorID(5) == test_genotype->GetAncestorID(3) ||
      GetAncestorID(5) == test_genotype->GetAncestorID(4) ||
      GetAncestorID(5) == test_genotype->GetAncestorID(5)
      ) {
    return 4;
  }
  
  return 5;
}

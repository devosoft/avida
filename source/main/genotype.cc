//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "genotype.hh"

#include "cConfig.h"
#include "cCPUTestInfo.h"
#include "genome_util.hh"
#include "merit.hh"
#include "organism.hh"
#include "phenotype.hh"
#include "cTestCPU.h"
#include "tools.hh"

using namespace std;

class cSpecies;

///////////////////////////
//  cGenotype
///////////////////////////

cGenotype::cGenotype(int in_update_born, int in_id)
  : genome(1)
  , name("001-no_name")
  , flag_threshold(false)
  , is_active(true)
  , defer_adjust(0)
  , symbol(0)
  , birth_data(in_update_born)
  , num_organisms(0)
  , last_num_organisms(0)
  , total_organisms(0)
  , total_parasites(0)
  , species(NULL)
  , next(NULL)
  , prev(NULL)
{
  static int next_id = 1;
  
  if ( in_id >= 0 )
    next_id = in_id;
  
  id_num = next_id++;
}

cGenotype::~cGenotype()
{
  // Reset some of the variables to make sure program will crash if a deleted
  // cell is read!

  symbol = '!';

  num_organisms = -1;
  total_organisms = -1;

  next = NULL;
  prev = NULL;
}

bool cGenotype::SaveClone(ofstream & fp)
{
  fp << id_num         << " ";
  fp << genome.GetSize() << " ";

  for (int i = 0; i < genome.GetSize(); i++) {
    fp << ((int) genome[i].GetOp()) << " ";
  }

  return true;
}

bool cGenotype::LoadClone(ifstream & fp)
{
  int genome_size = 0;

  fp >> id_num;
  fp >> genome_size;

  genome = cGenome(genome_size);
  for (int i = 0; i < genome_size; i++) {
    cInstruction temp_inst;
    int inst_op;
    fp >> inst_op;
    temp_inst.SetOp((UCHAR) inst_op);
    genome[i] = temp_inst;
    // @CAO add something here to load arguments for instructions.
  }

  return true;
}

bool cGenotype::OK()
{
  bool ret_value = true;

  // Check the components...

  if (!genome.OK()) ret_value = false;

  // And the statistics
  assert( id_num >= 0 && num_organisms >= 0 && total_organisms >= 0 );
  assert( birth_data.update_born >= -1 && birth_data.parent_distance >= -1 );
  assert( sum_copied_size.Sum() >= 0 && sum_exe_size.Sum() >= 0 );
  assert( sum_gestation_time.Sum() >= 0 && sum_repro_rate.Sum() >= 0 );
  assert( sum_merit.Sum() >= 0 && sum_fitness.Sum() >= 0 );
  assert( tmp_sum_copied_size.Sum() >= 0 && tmp_sum_exe_size.Sum() >= 0 );
  assert( tmp_sum_gestation_time.Sum() >= 0 && tmp_sum_repro_rate.Sum() >= 0 );
  assert( tmp_sum_merit.Sum() >= 0 && tmp_sum_fitness.Sum() >= 0 );

  return ret_value;
}

void cGenotype::SetParent(cGenotype * parent, cGenotype * parent2)
{
  birth_data.parent_genotype = parent;
  birth_data.parent2_genotype = parent2;

  // If we have a real parent genotype, collect other data about parent.
  if (parent == NULL) return;
  birth_data.ancestor_ids[0] = parent->GetID();
  birth_data.ancestor_ids[2] = parent->GetAncestorID(0);
  birth_data.ancestor_ids[3] = parent->GetAncestorID(1);
  if (parent2 != NULL) {
    birth_data.ancestor_ids[1] = parent2->GetID();
    birth_data.ancestor_ids[4] = parent2->GetAncestorID(0);
    birth_data.ancestor_ids[5] = parent2->GetAncestorID(1);    
  }

  birth_data.parent_distance =
    cGenomeUtil::FindEditDistance(genome, parent->genome);
  birth_data.parent_species = parent->GetSpecies();
  birth_data.gene_depth = parent->GetDepth() + 1;
  birth_data.lineage_label = parent->GetLineageLabel();
  parent->AddOffspringGenotype();
  if (parent2 != NULL && cConfig::GetTrackMainLineage() == 2) {
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

void cGenotype::Mutate()  // Check each location to be mutated.
{
  int i;

  for (i = 0; i < genome.GetSize(); i++) {
    if (true) { // g_random.GetUInt()) {     //@CAO always true!
      genome[i].SetOp(g_random.GetUInt(cConfig::GetNumInstructions()));
      // Flag command as having been mutated? @CAO
    }
  }
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

void cGenotype::CalcTestStats() const
{
  //  cerr << ".......Calculating test stats..." << endl;

  cCPUTestInfo test_info;
  cTestCPU::TestGenome(test_info, genome);
  test_data.is_viable = test_info.IsViable();

  // Setup all possible test values.
  cPhenotype & phenotype = test_info.GetTestOrganism()->GetPhenotype();
  test_data.fitness = test_info.GetGenotypeFitness();
  test_data.merit = phenotype.GetMerit().GetDouble();
  test_data.gestation_time = phenotype.GetGestationTime();
  test_data.executed_size = phenotype.GetExecutedSize();
  test_data.copied_size = phenotype.GetCopiedSize();
  test_data.colony_fitness = test_info.GetColonyFitness();
  test_data.generations = test_info.GetMaxDepth();
}


bool cGenotype::GetTestViable() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.is_viable;
}


double cGenotype::GetTestFitness() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.fitness;
}


double cGenotype::GetTestMerit() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.merit;
}


int cGenotype::GetTestGestationTime() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.gestation_time;
}


int cGenotype::GetTestExecutedSize() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.executed_size;
}


int cGenotype::GetTestCopiedSize() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.copied_size;
}


double cGenotype::GetTestColonyFitness() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.colony_fitness;
}


int cGenotype::GetTestGenerations() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.generations;
}

void cGenotype::SetSpecies(cSpecies * in_species)
{
  species = in_species;
}

void cGenotype::AddMerit(const cMerit & in)
{
  sum_merit.Add(in.GetDouble());
}

void cGenotype::RemoveMerit(const cMerit & in)
{
  sum_merit.Subtract(in.GetDouble());
}

void cGenotype::Deactivate(int update)
{
  is_active = false;
  birth_data.update_deactivated = update;
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


int cGenotype::AddOrganism()
{
  total_organisms++;
  return num_organisms++;
}

int cGenotype::RemoveOrganism()
{
  birth_data.death_track.Inc();
  return num_organisms--;
}


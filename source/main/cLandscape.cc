//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cLandscape.h"

#include "cCPUMemory.h"
#include "cEnvironment.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "stats.hh"             // For GetUpdate in outputs...
#include "cTestCPU.h"
#include "cTestUtil.h"
#include "tools.hh"

using namespace std;


////////////////
//  cLandscape
////////////////

cLandscape::cLandscape(const cGenome & in_genome, const cInstSet & in_inst_set)
  : inst_set(in_inst_set), base_genome(1), peak_genome(1)
{
  site_count = NULL;
  Reset(in_genome);
}

cLandscape::~cLandscape()
{
  if (site_count != NULL) delete [] site_count;
}

void cLandscape::Reset(const cGenome & in_genome)
{
  base_genome       = in_genome;
  peak_genome       = in_genome;
  base_fitness    = 0.0;
  base_merit      = 0.0;
  base_gestation  = 0;
  peak_fitness    = 0.0;
  total_fitness   = 0.0;
  total_sqr_fitness = 0.0;
  distance        = 0;
  del_distance    = 0;
  ins_distance    = 0;
  trials          = 0;

  total_count   = 0;
  dead_count    = 0;
  neg_count     = 0;
  neut_count    = 0;
  pos_count     = 0;

  pos_size	= 0;
  neg_size	= 0;

  total_epi_count   = 0;
  pos_epi_count	= 0;
  neg_epi_count	= 0; 
  no_epi_count	= 0; 
  dead_epi_count= 0; 

  pos_epi_size	= 0;
  neg_epi_size	= 0;
  no_epi_size	= 0;

  if (site_count != NULL) delete [] site_count;
  site_count = new int[base_genome.GetSize() + 1];
  for (int i = 0; i <= base_genome.GetSize(); i++) site_count[i] = 0;

  total_entropy = 0.0;
  complexity = 0.0;
  neut_min = 0.0;
  neut_max = 0.0;
}

void cLandscape::ProcessGenome(cGenome & in_genome)
{
  cTestCPU::TestGenome(test_info, in_genome);

  test_fitness = test_info.GetColonyFitness();

  total_fitness += test_fitness;
  total_sqr_fitness += test_fitness * test_fitness;
  total_count++;
  if (test_fitness == 0) {
    dead_count++;
  } else if (test_fitness < neut_min) {
    neg_count++;
    neg_size = neg_size + test_fitness  ;
  } else if (test_fitness <= neut_max) {
    neut_count++;
  } else {
    pos_count++;
    pos_size = pos_size + test_fitness  ;
    if (test_fitness > peak_fitness) {
      peak_fitness = test_fitness;
      peak_genome = in_genome;
    }
  }
}

void cLandscape::ProcessBase()
{
  // Collect info on base creature.

  cTestCPU::TestGenome(test_info, base_genome);

  cPhenotype & phenotype = test_info.GetColonyOrganism()->GetPhenotype();
  base_fitness = phenotype.GetFitness();
  base_merit = phenotype.GetMerit().GetDouble();
  base_gestation = phenotype.GetGestationTime();
   
  peak_fitness = base_fitness;
  peak_genome = base_genome;
  
  neut_min = base_fitness * nHardware::FITNESS_NEUTRAL_MIN;
  neut_max = base_fitness * nHardware::FITNESS_NEUTRAL_MAX;
  
}

void cLandscape::Process(int in_distance)
{
  distance = in_distance;

  // Get the info about the base creature.
  ProcessBase();

  // Now Process the new creature at the proper distance.
  Process_Body(base_genome, distance, 0);

  // Calculate the complexity...

  double max_ent = log((double) inst_set.GetSize());
  for (int i = 0; i < base_genome.GetSize(); i++) {
    total_entropy += (log((double) site_count[i] + 1) / max_ent);
  }
  complexity = base_genome.GetSize() - total_entropy;
}


// For distances greater than one, this needs to be called recursively.

void cLandscape::Process_Body(cGenome & cur_genome, int cur_distance,
			      int start_line)
{
  const int max_line = base_genome.GetSize() - cur_distance + 1;
  const int inst_size = inst_set.GetSize();

  cGenome mod_genome(cur_genome);

  // Loop through all the lines of genome, testing trying all combinations.
  for (int line_num = start_line; line_num < max_line; line_num++) {
    int cur_inst = base_genome[line_num].GetOp();

    // Loop through all instructions...
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      if (cur_inst == inst_num) continue;

      mod_genome[line_num].SetOp(inst_num);
      if (cur_distance <= 1) {
	ProcessGenome(mod_genome);
	if (test_info.GetColonyFitness() >= neut_min) site_count[line_num]++;
      } else {
	Process_Body(mod_genome, cur_distance - 1, line_num + 1);
      }
    }

    mod_genome[line_num].SetOp(cur_inst);
  }

}

void cLandscape::ProcessDelete()
{
  // Get the info about the base creature.
  ProcessBase();

  const int max_line = base_genome.GetSize();
  cCPUMemory mod_genome(base_genome);

  // Loop through all the lines of genome, testing all deletions.
  for (int line_num = 0; line_num < max_line; line_num++) {
    int cur_inst = base_genome[line_num].GetOp();
    mod_genome.Remove(line_num);
    ProcessGenome(mod_genome);
    if (test_info.GetColonyFitness() >= neut_min) site_count[line_num]++;
    mod_genome.Insert(line_num, cInstruction(cur_inst));
  }

}

void cLandscape::ProcessInsert()
{
  // Get the info about the base creature.
  ProcessBase();

  const int max_line = base_genome.GetSize();
  const int inst_size = inst_set.GetSize();

  cCPUMemory mod_genome(base_genome);

  // Loop through all the lines of genome, testing all insertions.
  for (int line_num = 0; line_num <= max_line; line_num++) {
    // Loop through all instructions...
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      mod_genome.Insert(line_num, cInstruction(inst_num));
      ProcessGenome(mod_genome);
      if (test_info.GetColonyFitness() >= neut_min) site_count[line_num]++;
      mod_genome.Remove(line_num);
    }
  }

}

// Prediction for a landscape where n sites are _randomized_.
void cLandscape::PredictWProcess(ostream & fp, int update)
{
  distance = 1;

  // Get the info about the base creature.
  ProcessBase();
  if (base_fitness == 0.0) return;

  BuildFitnessChart();
  const int genome_size = fitness_chart.GetNumRows();
  const int inst_size = fitness_chart.GetNumCols();
  const double min_neut_fitness = 0.99;
  const double max_neut_fitness = 1.01;

  // Loop through the entries printing them and doing additional
  // calculations.
  int total_pos_found = 0;
  int total_neut_found = 0;
  int total_neg_found = 0;
  int total_dead_found = 0;
  double max_fitness = 1.0;
  double total_fitness = 0.0;
  double total_sqr_fitness = 0.0;

  for (int row = 0; row < genome_size; row++) {
    double max_line_fitness = 1.0;
    for (int col = 0; col < inst_size; col++) {
      double & cur_fitness = fitness_chart(row, col);
      cur_fitness /= base_fitness;
      total_fitness += cur_fitness;
      total_sqr_fitness += cur_fitness * cur_fitness;
      if (cur_fitness > max_neut_fitness) total_pos_found++;
      else if (cur_fitness > min_neut_fitness) total_neut_found++;
      else if (cur_fitness > 0.0) total_neg_found++;

      if (cur_fitness > max_line_fitness) max_line_fitness = cur_fitness;
    }
    max_fitness *= max_line_fitness;
  }

  const int total_tests = genome_size * inst_size;
  total_dead_found = total_tests - total_pos_found - total_neut_found - total_neg_found;
  fp << update << " "
     << "1 "
     << ((double) total_dead_found / (double) total_tests) << " "
     << ((double) total_neg_found / (double) total_tests)  << " "
     << ((double) total_neut_found / (double) total_tests) << " "
     << ((double) total_pos_found / (double) total_tests)  << " "
     << total_tests                                        << " "
     << total_neut_found + total_pos_found                 << " "
     << total_fitness / (double) total_tests               << " "
     << total_sqr_fitness / (double) total_tests           << " "
     << endl;
  fp.flush();

  // Sample the table out to 10 mutations
  const int max_muts = 10;
  const int min_found = 100;
  const int min_tests = 10000;
  const int max_tests = 1000000000;

  double test_fitness;
  for (int num_muts = 2; num_muts <= max_muts; num_muts++) {
    total_pos_found = 0;
    total_neut_found = 0;
    total_neg_found = 0;
    total_fitness = 0.0;
    total_sqr_fitness = 0.0;
    tArray<int> mut_lines(num_muts);

    int test_id = 0;
    while ((test_id < min_tests) ||
	   (test_id < max_tests && (total_neut_found + total_pos_found) < min_found)) {
      g_random.Choose(genome_size, mut_lines);
      test_fitness = 1.0;
      for (int j = 0; j < num_muts && test_fitness != 0.0; j++) {	
	test_fitness *=
	  fitness_chart(mut_lines[j], g_random.GetUInt(inst_size));
      }
      total_fitness += test_fitness;
      total_sqr_fitness += test_fitness * test_fitness;
      if (test_fitness > max_neut_fitness) total_pos_found++;
      else if (test_fitness > min_neut_fitness) total_neut_found++;
      else if (test_fitness > 0.0) total_neg_found++;

      test_id++;
    }

    total_dead_found = test_id - total_pos_found - total_neut_found - total_neg_found;
    fp << update                                         << " " //  1
       << num_muts                                       << " " //  2
       << ((double) total_dead_found / (double) test_id) << " " //  3
       << ((double) total_neg_found / (double) test_id)  << " " //  4
       << ((double) total_neut_found / (double) test_id) << " " //  5
       << ((double) total_pos_found / (double) test_id)  << " " //  6
       << test_id                                        << " " //  7
       << total_neut_found + total_pos_found             << " " //  8
       << total_fitness / (double) test_id               << " " //  9
       << total_sqr_fitness / (double) test_id           << " " // 10
       << endl;
    fp.flush();

    if (total_pos_found + total_neut_found < min_found / 2) break;
  }

  // Calculate the complexity...

  double max_ent = log((double) inst_set.GetSize());
  for (int i = 0; i < base_genome.GetSize(); i++) {
    total_entropy += (log((double) site_count[i] + 1) / max_ent);
  }
  complexity = base_genome.GetSize() - total_entropy;
}


// Prediction for a landscape where n sites are _mutated_.
void cLandscape::PredictNuProcess(ostream & fp, int update)
{
  distance = 1;

  // Get the info about the base creature.
  ProcessBase();
  if (base_fitness == 0.0) return;

  BuildFitnessChart();
  const int genome_size = fitness_chart.GetNumRows();
  const int inst_size = fitness_chart.GetNumCols();
  const double min_neut_fitness = 0.99;
  const double max_neut_fitness = 1.01;

  // Loop through the entries printing them and doing additional
  // calculations.
  int total_pos_found = 0;
  int total_neut_found = 0;
  int total_neg_found = 0;
  int total_dead_found = 0;
  int total_live_found = 0;
  double max_fitness = 1.0;
  double max_found_fitness = 0.0;
  double total_fitness = 0.0;
  double total_sqr_fitness = 0.0;

  for (int row = 0; row < genome_size; row++) {
    double max_line_fitness = 1.0;
    int base_inst = base_genome[row].GetOp();
    for (int col = 0; col < inst_size; col++) {
      if (col == base_inst) continue; // Only consider changes to line!
      double & cur_fitness = fitness_chart(row, col);
      cur_fitness /= base_fitness;
      total_fitness += cur_fitness;
      total_sqr_fitness += cur_fitness * cur_fitness;
      if (cur_fitness > max_neut_fitness) total_pos_found++;
      else if (cur_fitness > min_neut_fitness) total_neut_found++;
      else if (cur_fitness > 0.0) total_neg_found++;

      if (cur_fitness > max_line_fitness) max_line_fitness = cur_fitness;
    }
    max_fitness *= max_line_fitness;
    if (max_line_fitness > max_found_fitness) max_found_fitness = max_line_fitness;
  }

  const int total_tests = genome_size * inst_size;
  total_live_found = total_pos_found + total_neut_found + total_neg_found;
  total_dead_found = total_tests - total_live_found;
  fp << update                                             << " "
     << "1 "
     << ((double) total_dead_found / (double) total_tests) << " "
     << ((double) total_neg_found / (double) total_tests)  << " "
     << ((double) total_neut_found / (double) total_tests) << " "
     << ((double) total_pos_found / (double) total_tests)  << " "
     << total_tests                                        << " "
     << total_live_found                                   << " "
     << total_fitness / (double) total_tests               << " "
     << total_sqr_fitness / (double) total_tests           << " "
     << max_found_fitness                                   << " "
     << endl;
  fp.flush();

  // Sample the table out to 10 mutations
  const int max_muts = 10;
  const int min_found = 100;
  const int min_tests = 10000;
  const int max_tests = 1000000000;

  double test_fitness;
  for (int num_muts = 2; num_muts <= max_muts; num_muts++) {
    total_pos_found = 0;
    total_neut_found = 0;
    total_neg_found = 0;
    total_fitness = 0.0;
    total_sqr_fitness = 0.0;
    max_found_fitness = 0.0;
    tArray<int> mut_lines(num_muts);

    int test_id = 0;
    while ((test_id < min_tests) ||
	   (test_id < max_tests && (total_neg_found + total_neut_found + total_pos_found) < min_found)) {
      g_random.Choose(genome_size, mut_lines);
      test_fitness = 1.0;
      for (int j = 0; j < num_muts && test_fitness != 0.0; j++) {	
	int base_inst = base_genome[ mut_lines[j] ].GetOp();
	int mut_inst = g_random.GetUInt(inst_size);
	while (mut_inst == base_inst) mut_inst = g_random.GetUInt(inst_size);
	test_fitness *= fitness_chart(mut_lines[j], mut_inst);
	if (test_fitness == 0.0) break;
      }
      total_fitness += test_fitness;
      total_sqr_fitness += test_fitness * test_fitness;
      if (test_fitness > max_found_fitness) max_found_fitness = test_fitness;

      if (test_fitness > max_neut_fitness) total_pos_found++;
      else if (test_fitness > min_neut_fitness) total_neut_found++;
      else if (test_fitness > 0.0) total_neg_found++;

      test_id++;
    }

    total_live_found = total_neg_found + total_neut_found + total_pos_found;
    total_dead_found = test_id - total_live_found;


    fp << update                                         << " " //  1
       << num_muts                                       << " " //  2
       << ((double) total_dead_found / (double) test_id) << " " //  3
       << ((double) total_neg_found / (double) test_id)  << " " //  4
       << ((double) total_neut_found / (double) test_id) << " " //  5
       << ((double) total_pos_found / (double) test_id)  << " " //  6
       << test_id                                        << " " //  7
       << total_live_found                               << " " //  8
       << total_fitness / (double) test_id               << " " //  9
       << total_sqr_fitness / (double) test_id           << " " // 10
       << max_found_fitness                              << " " // 11
       << endl;
    fp.flush();

    if (total_live_found < min_found / 2) break;
  }

  // Calculate the complexity...

  double max_ent = log((double) inst_set.GetSize());
  for (int i = 0; i < base_genome.GetSize(); i++) {
    total_entropy += (log((double) site_count[i] + 1) / max_ent);
  }
  complexity = base_genome.GetSize() - total_entropy;
}


void cLandscape::SampleProcess(int in_trials)
{
  trials = in_trials;  // Trials _per line_
  distance = 1;

  cGenome mod_genome(base_genome);
  int genome_size = base_genome.GetSize();

  ProcessBase();

  // Loop through all the lines of genome, testing each line.
  for (int line_num = 0; line_num < genome_size; line_num++) {
    cInstruction cur_inst( base_genome[line_num] );

    for (int i = 0; i < trials; i++) {
      // Choose the new instruction for that line...
      cInstruction new_inst( inst_set.GetRandomInst() );
      if (cur_inst == new_inst) { i--; continue; }

      // Make the change, and test it!
      mod_genome[line_num] = new_inst;
      ProcessGenome(mod_genome);
    }

    mod_genome[line_num] = cur_inst;
  }
}


int cLandscape::RandomProcess(int in_trials, int in_distance, int min_found,
			       int max_trials, bool print_if_found)
{
  distance = in_distance;

  cGenome mod_genome(base_genome);
  int genome_size = base_genome.GetSize();

  ProcessBase();

  int mut_num;
  tArray<int> mut_lines(distance);

  // Loop through all the lines of genome, testing many combinations.
  int cur_trial = 0;
  int total_found = 0;

//  for (cur_trial = 0;
//       (cur_trial < in_trials) ||
//	 (total_found < min_found && cur_trial < max_trials);
//       cur_trial++) {
// Way too confusing and not being used, commented it out DM

  for (cur_trial = 0; cur_trial < in_trials; cur_trial++) { 

    // Choose the lines to mutate...
    g_random.Choose(genome_size, mut_lines);

    // Choose the new instructions for those lines...
    for (mut_num = 0; mut_num < distance; mut_num++) {
      const cInstruction new_inst( inst_set.GetRandomInst() );
      const cInstruction & cur_inst = base_genome[ mut_lines[mut_num] ];
      if (cur_inst == new_inst) {
	mut_num--;
	continue;
      }

      mod_genome[ mut_lines[mut_num] ] = new_inst;
    }

    // And test it!

    ProcessGenome(mod_genome);

    // if (test_info.IsViable()) {
    //if (test_fitness >= neut_min) {
      // total_found++;
      // If we are supposed to print those found, do so!
      //  if (print_if_found) {
      //    cString filename;
      //    filename.Set("creature.land.%d.%d", distance, cur_trial);
      //    cTestUtil::PrintGenome(mod_genome, filename);
      //  }
    //}


    // And reset the genome.
    for (mut_num = 0; mut_num < distance; mut_num++) {
      mod_genome[ mut_lines[mut_num] ] = base_genome[ mut_lines[mut_num] ];
    }
  }

  trials = cur_trial;

  return total_found;
}

void cLandscape::BuildFitnessChart()
{
  // First, resize the fitness_chart.
  const int max_line = base_genome.GetSize();
  const int inst_size = inst_set.GetSize();
  fitness_chart.ResizeClear(max_line, inst_size);

  cGenome mod_genome(base_genome);

  // Loop through all the lines of genome, testing trying all combinations.
  for (int line_num = 0; line_num < max_line; line_num++) {
    int cur_inst = base_genome[line_num].GetOp();

    // Loop through all instructions...
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      if (cur_inst == inst_num) {
	fitness_chart(line_num, inst_num) = base_fitness;
	continue;
      }

      mod_genome[line_num].SetOp(inst_num);
      ProcessGenome(mod_genome);
      fitness_chart(line_num, inst_num) = test_info.GetColonyFitness();
    }

    mod_genome[line_num].SetOp(cur_inst);
  }

}

void cLandscape::TestPairs(int in_trials, ostream & fp)
{
  trials = in_trials;

  ProcessBase();
  if (base_fitness == 0.0) return;

  BuildFitnessChart();

  cGenome mod_genome(base_genome);
  const int genome_size = base_genome.GetSize();

  tArray<int> mut_lines(2);
  tArray<cInstruction> mut_insts(2);

  // Loop through all the lines of genome, testing many combinations.
  for (int i = 0; i < trials; i++) {
    // Choose the lines to mutate...
    g_random.Choose(genome_size, mut_lines);

    // Choose the new instructions for those lines...
    for (int mut_num = 0; mut_num < 2; mut_num++) {
      const cInstruction new_inst( inst_set.GetRandomInst() );
      const cInstruction & cur_inst = base_genome[ mut_lines[mut_num] ];
      if (cur_inst == new_inst) {
	mut_num--;
	continue;
      }

      mut_insts[mut_num] = new_inst;
    }

    TestMutPair(mod_genome, mut_lines[0], mut_lines[1], mut_insts[0],
		mut_insts[1], fp);

  }

}


void cLandscape::TestAllPairs(ostream & fp)
{
  ProcessBase();
  if (base_fitness == 0.0) return;

  BuildFitnessChart();

  const int max_line = base_genome.GetSize();
  const int inst_size = inst_set.GetSize();
  cGenome mod_genome(base_genome);
  cInstruction inst1, inst2;

  // Loop through all the lines of genome, testing trying all combinations.
  for (int line1_num = 0; line1_num < max_line - 1; line1_num++) {
    for (int line2_num = line1_num + 1; line2_num < max_line; line2_num++) {

      // Loop through all instructions...
      for (int inst1_num = 0; inst1_num < inst_size; inst1_num++) {
	inst1.SetOp(inst1_num);
	if (inst1 == base_genome[line1_num]) continue;
	for (int inst2_num = 0; inst2_num < inst_size; inst2_num++) {
	  inst2.SetOp(inst2_num);
	  if (inst2 == base_genome[line2_num]) continue;
	  TestMutPair(mod_genome, line1_num, line2_num, inst1, inst2, fp);
	} // inst2_num loop
      } //inst1_num loop;

    } // line2_num loop
  } // line1_num loop.

}


void cLandscape::HillClimb(ofstream & fp)
{
  cGenome cur_genome(base_genome);
  int gen = 0;
  HillClimb_Body(fp, cur_genome, gen);
}

void cLandscape::HillClimb_Body(ofstream & fp, cGenome & cur_genome,
				int & gen)
{
  cCPUMemory mod_genome(base_genome);

  const int inst_size = inst_set.GetSize();

  double pos_frac = 1.0;

  bool finished = false;
  while (finished == false) {
    if (pos_frac == 0.0) finished = true;

    // Search the landscape for the next best.
    Reset(cur_genome);
    const int max_line = cur_genome.GetSize();

    // Try all Mutations...
    Process(1);

    // Try Insertion Mutations.

    mod_genome = cur_genome;
    for (int line_num = 0; line_num <= max_line; line_num++) {
      // Loop through all instructions...
      for (int inst_num = 0; inst_num < inst_size; inst_num++) {
	mod_genome.Insert(line_num, cInstruction(inst_num));
	ProcessGenome(mod_genome);
	mod_genome.Remove(line_num);
      }
    }

    // Try all deletion mutations.

    for (int line_num = 0; line_num < max_line; line_num++) {
      int cur_inst = cur_genome[line_num].GetOp();
      mod_genome.Remove(line_num);
      ProcessGenome(mod_genome);
      mod_genome.Insert(line_num, cInstruction(cur_inst));
    }

    pos_frac = GetProbPos();

    // Print the information on the current best.
    HillClimb_Print(fp, cur_genome, gen);

    // Move on to the peak genome found.
    cur_genome = GetPeakGenome();
    gen++;
  }
}

void cLandscape::HillClimb_Neut(ofstream & fp)
{
}

void cLandscape::HillClimb_Rand(ofstream & fp)
{
}

void cLandscape::HillClimb_Print(ofstream & fp, const cGenome & _genome, const int gen) const
{
  cCPUTestInfo test_info;
  cTestCPU::TestGenome(test_info, _genome);
  cPhenotype &colony_phenotype = test_info.GetColonyOrganism()->GetPhenotype();
  fp << gen << " "
     << colony_phenotype.GetMerit().GetDouble() << " "
     << colony_phenotype.GetGestationTime() << " "
     << colony_phenotype.GetFitness() << " "
     << _genome.GetSize() << " "
     << GetProbDead() << " "
     << GetProbNeg() << " "
     << GetProbNeut() << " "
     << GetProbPos() << " "
     << endl;
}

double cLandscape::TestMutPair(cGenome & mod_genome, int line1, int line2,
    const cInstruction & mut1, const cInstruction & mut2, ostream & fp)
{
  mod_genome[line1] = mut1;
  mod_genome[line2] = mut2;
  cTestCPU::TestGenome(test_info, mod_genome);
  double combo_fitness = test_info.GetColonyFitness() / base_fitness;

  mod_genome[line1] = base_genome[line1];
  mod_genome[line2] = base_genome[line2];

  double mut1_fitness = fitness_chart(line1, mut1.GetOp()) / base_fitness;
  double mut2_fitness = fitness_chart(line2, mut2.GetOp()) / base_fitness;
  double mult_combo = mut1_fitness * mut2_fitness;

  /*
  fp << line1        << " "
     << line2        << " "
     << ( (int) mut1.GetOp() ) << " "
     << ( (int) mut2.GetOp() ) << " ";

  fp << ( fitness_chart(line1, mut1.GetOp()) / base_fitness ) << " "
     << ( fitness_chart(line2, mut2.GetOp()) / base_fitness ) << " "
     << combo_fitness << endl;
  */

  total_epi_count++;
  if ((mut1_fitness==0 || mut2_fitness==0)&&(combo_fitness==0)) {
    dead_epi_count++;
  } else if (combo_fitness < mult_combo) {
    neg_epi_count++;
    neg_epi_size = neg_epi_size + combo_fitness;
  } else if (combo_fitness > mult_combo) {
    pos_epi_count++;
    pos_epi_size = pos_epi_size + combo_fitness;
  } else {
    no_epi_count++;
    no_epi_size = no_epi_size + combo_fitness;
    }
  
  return combo_fitness;
}


void cLandscape::PrintStats(ofstream & fp, int update)
{
  fp << update                 << " "   // 1
     << GetProbDead()          << " "   // 2
     << GetProbNeg()           << " "   // 3
     << GetProbNeut()          << " "   // 4
     << GetProbPos()           << " "   // 5
//     << pos_size         << " "   // 6
//     << pos_count         << " "   // 6
     << GetAvPosSize()         << " "   // 6
     << GetAvNegSize()         << " "   // 7
     << total_count            << " "   // 8
     << distance               << " "   // 9
//     << neut_min           << "   "   // 10
//     << neut_max           << "   "   // 10
//     << nHardware::FITNESS_NEUTRAL_MIN           << "   "   // 10
//     << nHardware::FITNESS_NEUTRAL_MAX           << "   "   // 10
     << base_fitness           << " "   // 10
     << base_merit             << " "   // 11
     << base_gestation         << " "   // 12
     << peak_fitness           << " "   // 13
     << GetAveFitness()        << " "   // 14
     << GetAveSqrFitness()     << " "   // 15
     << total_entropy          << " "   // 16
     << complexity             << " "   // 17
     << GetProbEpiDead()       << " "   // 18
     << GetProbEpiPos()        << " "   // 19
     << GetProbEpiNeg()        << " "   // 20
     << GetProbNoEpi()         << " "   // 21
     << GetAvPosEpiSize()      << " "   // 22
     << GetAvNegEpiSize()      << " "   // 23
     << GetAvNoEpiSize()      << " "   // 24
     << total_epi_count        << endl; // 25
  fp.flush();
}

void cLandscape::PrintEntropy(ofstream & fp)
{
  double max_ent = log((double) inst_set.GetSize());
  for (int j = 0; j < base_genome.GetSize(); j++) {
    fp << (log((double) site_count[j] + 1) / max_ent) << " ";
  }
  fp << endl;
  fp.flush();
}

void cLandscape::PrintSiteCount(ofstream & fp)
{
  for (int j = 0; j < base_genome.GetSize(); j++) {
    fp << site_count[j] << " ";
  }
  fp << endl;
  fp.flush();
}


void cLandscape::PrintBase(cString filename)
{
  cTestUtil::PrintGenome(base_genome, filename);
}

void cLandscape::PrintPeak(cString filename)
{
  cTestUtil::PrintGenome(peak_genome, filename);
}


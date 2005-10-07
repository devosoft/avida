//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef SPECIES_HH
#include "cSpecies.h"
#endif

#ifndef CPU_TEST_INFO_HH
#include "cCPUTestInfo.h"
#endif
#ifndef FUNCTIONS_HH
#include "functions.h"
#endif
#ifndef GENOTYPE_HH
#include "cGenotype.h"
#endif
#ifndef GENOME_UTIL_HH
#include "cGenomeUtil.h"
#endif
#ifndef TEST_CPU_HH
#include "cTestCPU.h"
#endif

using namespace std;


/////////////////
//  cSpecies
/////////////////

cSpecies::cSpecies(const cGenome & in_genome, int update )
  : genome(in_genome)
  , update_born(update)
{
  static int species_count = 0;
  id_num = species_count++;
  parent_id = -1;
  symbol = '+';

  num_genotypes = 0;
  num_threshold = 0;
  total_organisms = 0;
  total_genotypes = 0;

  queue_type = SPECIES_QUEUE_NONE;

  next = NULL;
  prev = NULL;

  // Track distance of each genome as it is passed in...

  for (int i = 0; i < SPECIES_MAX_DISTANCE; i++) {
    genotype_distance[i] = 0;
  }
}

cSpecies::~cSpecies()
{
  int i, total_count = 0;
  for (i = 0; i < SPECIES_MAX_DISTANCE; i++) {
    total_count += genotype_distance[i];
  }

#ifdef TEST
  // Only print out the non-trivial species.

  if (total_count > 1) {
    FILE * fp = stats.GetTestFP();

    fprintf(fp, "Species %3d: ", id_num);
    for (i = 0; i < SPECIES_MAX_DISTANCE; i++) {
      fprintf(fp, "%2d ", genotype_distance[i]);
    }
    fprintf(fp, "\n");
    fflush(fp);
  }
#endif
}

// This method compares a second genome to this species and determines
// how different it is.  A -1 indicates a critical failure (like one of
// the genomes being considered is not viable), otherwise the count
// Of failed crossovers is returned.
// @CAO Todo: Check phenotypes, allow failure proportional to size.

int cSpecies::Compare(const cGenome & test_genome, int max_fail_count)
{
  cCPUTestInfo test_info;

  // First, make some phenotypic comparisons between organisms.
  // For now, just check that they both copy-true.

  cTestCPU::TestGenome(test_info, test_genome);

  // If the organisms aren't viable, return a -1...
  if (test_info.IsViable() == false) {
    return -1;
  }

  // Find the optimal offset between organisms, and related variables.
  // @CAO: For the moment, lets not worry about offsets...
  // The first line of B is at line 'offset' of A. 
  //int offset = cGenomeUtil::FindBestOffset(genome, test_genome);


  // Figure out how much of the organisms overlap, and how much doesn't.
  int overlap = Min(genome.GetSize(), test_genome.GetSize());
  int fail_count = Max(genome.GetSize(), test_genome.GetSize()) - overlap;


  // Do the crossovers at all posible points.
  // Start with the first direction crossover...

  bool cross1_viable = true;
  bool cross2_viable = true;
  cGenome cross_genome1(genome);
  cGenome cross_genome2(test_genome);

  for (int i = 0; i < overlap; i++) {
    // If this position has changed, check if crossovers here are viable.
    if (test_genome[i] != genome[i]) {
      // Continue crossing over on each side...
      cross_genome1[i] = test_genome[i];
      cross_genome2[i] = genome[i];
   
      // Run each side, and determine viability...
      cTestCPU::TestGenome(test_info, cross_genome1);
      cross1_viable = test_info.IsViable();

      cTestCPU::TestGenome(test_info, cross_genome2);
      cross2_viable = test_info.IsViable();
    }

    if (cross1_viable == false) fail_count++;
    if (cross2_viable == false) fail_count++;

    if (max_fail_count != -1 && fail_count > max_fail_count) break;
  }


  return fail_count;
}

bool cSpecies::OK()
{
  assert(id_num >= 0);  // Species has negative ID value!
  assert(genome.OK());  // Species genome not registering as OK!

  // Assert valid statistics in species.
  assert(total_organisms >= 0 && total_genotypes >= 0 &&
	 num_threshold >= 0 && num_genotypes >= 0);

  // Finally, make sure the species is registered as being in a queue.

  assert(queue_type >= 0 && queue_type <= 3); // Species not in a proper queue

  return true;
}

void cSpecies::AddThreshold(cGenotype & in_genotype)
{
  const int distance = 
    cGenomeUtil::FindEditDistance(genome, in_genotype.GetGenome());

  if (distance >= 0 && distance < SPECIES_MAX_DISTANCE)
    genotype_distance[distance]++;

  num_threshold++;
}

void cSpecies::RemoveThreshold(cGenotype & in_genotype)
{
  total_genotypes++;
  total_organisms += in_genotype.GetTotalOrganisms();
  num_threshold--;
}

void cSpecies::AddGenotype()
{
  num_genotypes++;
}

void cSpecies::RemoveGenotype() {
  num_genotypes--;
}

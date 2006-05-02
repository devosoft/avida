/*
 *  cAnalyzeUtil.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cAnalyzeUtil_h
#define cAnalyzeUtil_h

#include <fstream>

class cGenome;
class cInstSet;
class cPopulation;
class cWorld;
class cString;

// This is a static class used to do various forms of complex analysis
// on genomes.

class cAnalyzeUtil {
private:
public:
  // Generic test-CPU analysis
  static void TestGenome(cWorld* world, const cGenome & genome, cInstSet & inst_set,
			 std::ofstream& fp, int update);

  static void TestInsSizeChangeRobustness(cWorld* world, std::ofstream& fp,
                 const cInstSet & inst_set, const cGenome & in_genome,
                 int num_trials, int update);


  // Landscape-based analysis
  static cGenome CalcLandscape(cWorld* world, int dist, const cGenome & genome,
			       cInstSet & inst_set);
  static void PairTestLandscape(cWorld* world, const cGenome & genome, cInstSet & inst_set,
				int sample_size=0, int update=-1);


  // Population-wide analysis
  static void CalcConsensus(cWorld* world, int lines_saved);

  static void AnalyzePopulation(cWorld* world, std::ofstream& fp,
				double sample_prob=1, bool landscape=false,
				bool save_genotype=false);

  static void PrintDetailedFitnessData(cWorld* world, cString& datafn,
    cString& histofn, cString& histo_testCPU_fn, bool save_max_f_genotype,
    bool print_fitness_histo, double hist_fmax, double hist_fstep);

  static void PrintGeneticDistanceData(cWorld* world, std::ofstream& fp,
				const char *creature_name );
  static void GeneticDistancePopDump(cWorld* world, std::ofstream& fp,
		    const char * creature_name, bool save_creatures=false);

  static void TaskSnapshot(cWorld* world, std::ofstream& fp);
  static void TaskGrid(cWorld* world, std::ofstream& fp);
  static void PrintViableTasksData(cWorld* world, std::ofstream& fp);
  static void PrintTreeDepths(cWorld* world, std::ofstream& fp);

  static void PrintDepthHistogram(cWorld* world, std::ofstream& fp);
  static void PrintGenotypeAbundanceHistogram(cWorld* world, std::ofstream& fp);
  static void PrintSpeciesAbundanceHistogram(cWorld* world, std::ofstream& fp);

  // this adds support for evan dorn's InstructionHistogramEvent.  -- kgn
  static void PrintInstructionAbundanceHistogram(cWorld* world, std::ofstream& fp);
  // -- kgn

#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif

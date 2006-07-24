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


  // Population-wide analysis
  static void CalcConsensus(cWorld* world, int lines_saved);

  static void TaskSnapshot(cWorld* world, std::ofstream& fp);
  static void TaskGrid(cWorld* world, std::ofstream& fp);
  static void PrintViableTasksData(cWorld* world, std::ofstream& fp);
  static void PrintTreeDepths(cWorld* world, std::ofstream& fp);
};

#ifdef ENABLE_UNIT_TESTS
namespace nAnalyzeUtil {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

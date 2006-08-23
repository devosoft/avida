/*
 *  cGenotype_TestData.h
 *  Avida
 *
 *  Called "genotype_test_data.hh" prior to 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cGenotype_TestData_h
#define cGenotype_TestData_h

class cGenotype_TestData {
public:
  cGenotype_TestData() : fitness(-1) { ; }
  ~cGenotype_TestData() { ; }

  bool is_viable;

  double fitness;
  double merit;
  int gestation_time;
  int executed_size;
  int copied_size;
  double colony_fitness;
  int generations;
};


#ifdef ENABLE_UNIT_TESTS
namespace nGenotype_TestData {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

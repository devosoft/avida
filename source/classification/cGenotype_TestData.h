/*
 *  cGenotype_TestData.h
 *  Avida
 *
 *  Created by David on 11/30/05.
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

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

#endif

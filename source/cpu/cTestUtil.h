/*
 *  cTestUtil.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cTestUtil_h
#define cTestUtil_h

#ifndef cString_h
#include "cString.h"
#endif

// ------------------------------------------------------------------------
//  This class uses test CPUs in combination with genotypes and all types
//  of hardware in order to produce more useful test info.
// ------------------------------------------------------------------------

class cGenome;
class cGenotype;
class cInjectGenotype;
class cWorld;

class cTestUtil
{
private:
  cTestUtil(); // @not_implemented
  
public:
  static void PrintGenome(cWorld* world, const cGenome & genome, cString filename="",
                          cGenotype * genotype=NULL, int update_out=-1);
  static void PrintGenome(cWorld* world, cInjectGenotype * genotype, const cGenome & genome, 
                          cString filename="", int update_out=-1);
};


#ifdef ENABLE_UNIT_TESTS
namespace nTestUtil {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef TEST_UTIL_HH
#define TEST_UTIL_HH

#ifndef STRING_HH
#include "cString.h"
#endif

// ------------------------------------------------------------------------
//  This class uses test CPUs in combination with genotypes and all types
//  of hardware in order to produce more useful test info.
// ------------------------------------------------------------------------

class cString; // aggregate
class cGenome;
class cGenotype;
class cInjectGenotype;

class cTestUtil {
public:
  static void PrintGenome(const cGenome & genome, cString filename="",
			  cGenotype * genotype=NULL, int update_out=-1);
  static void PrintGenome(cInjectGenotype * genotype, const cGenome & genome, 
			  cString filename="", int update_out=-1);
};

#endif

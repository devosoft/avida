//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INST_UTIL_HH
#define INST_UTIL_HH

#include <iostream>

#ifndef GENOME_HH
#include "cGenome.h"
#endif

class cInitFile;
class cInstSet;
class cString;
class cInstUtil {
public:

  // ========= Genome-File Interaction =========
  // Saving and loading of files.  These functions assume that the genome is
  // the only thing in the file unless 'Internal' is in the function name
  // (Internal genomes must begin with a number that indicates genome length)

  static cGenome LoadGenome(const cString &filename, const cInstSet &inst_set);
  static cGenome LoadInternalGenome(std::istream & fp, const cInstSet &inst_set);
  static void SaveGenome(std::ostream& fp, const cInstSet & inst_set,
			 const cGenome &gen);
  static void SaveInternalGenome(std::ostream& fp, const cInstSet &inst_set,
				 const cGenome &gen);

  // ========= Genome Construction =========
  static cGenome RandomGenome(int length, const cInstSet & inst_set);
};

#endif

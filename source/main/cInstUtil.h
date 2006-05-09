/*
 *  cInstUtil.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cInstUtil_h
#define cInstUtil_h

#include <iostream>

#ifndef cGenome_h
#include "cGenome.h"
#endif

class cAvidaContext;
class cInitFile;
class cInstSet;
class cString;

class cInstUtil
{
private:
  cInstUtil(); // @not_implemented
  
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
  static cGenome RandomGenome(cAvidaContext& ctx, int length, const cInstSet & inst_set);
};


#ifdef ENABLE_UNIT_TESTS
namespace nInstUtil {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif

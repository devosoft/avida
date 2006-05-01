/*
 *  cGenotypeBatch.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cGenotypeBatch_h
#define cGenotypeBatch_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

// cGenotypeBatch      : Collection of cAnalyzeGenotypes

class cAnalyzeGenotype;

class cGenotypeBatch {
private:
  tListPlus<cAnalyzeGenotype> genotype_list;
  cString name;
  bool is_lineage;
  bool is_aligned;
private:
  // disabled copy constructor.
  cGenotypeBatch(const cGenotypeBatch &);
public:
  cGenotypeBatch() : name(""), is_lineage(false), is_aligned(false) { ; }
  ~cGenotypeBatch() { ; }

  tListPlus<cAnalyzeGenotype> & List() { return genotype_list; }
  cString & Name() { return name; }
  bool IsLineage() { return is_lineage; }
  bool IsAligned() { return is_aligned; }

  void SetLineage(bool _val=true) { is_lineage = _val; }
  void SetAligned(bool _val=true) { is_aligned = _val; }


public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

#endif

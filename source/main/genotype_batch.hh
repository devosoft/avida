//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef GENOTYPE_BATCH_HH
#define GENOTYPE_BATCH_HH

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef TLIST_HH
#include "tList.hh"
#endif

// cGenotypeBatch      : Collection of cAnalyzeGenotypes

template <class T> class tList; // aggregate
class cString; // aggregate
class cAnalyzeGenotype;

class cGenotypeBatch {
private:
  tListPlus<cAnalyzeGenotype> genotype_list;
  cString name;
  bool is_lineage;
  bool is_aligned;
public:
  cGenotypeBatch() : name(""), is_lineage(false), is_aligned(false) { ; }
  ~cGenotypeBatch() { ; }

  tListPlus<cAnalyzeGenotype> & List() { return genotype_list; }
  cString & Name() { return name; }
  bool IsLineage() { return is_lineage; }
  bool IsAligned() { return is_aligned; }

  void SetLineage(bool _val=true) { is_lineage = _val; }
  void SetAligned(bool _val=true) { is_aligned = _val; }

};

#endif

//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INJECT_GENOTYPE_ELEMENT_HH
#define INJECT_GENOTYPE_ELEMENT_HH

#ifndef DEFS_HH
#include "defs.h"
#endif

class cInjectGenotype;
class cInjectGenotypeElement {
private:
  cInjectGenotype * inject_genotype;
  cInjectGenotypeElement * next;
  cInjectGenotypeElement * prev;
public:
  inline cInjectGenotypeElement(cInjectGenotype * in_gen=NULL) : inject_genotype(in_gen) {
    next = NULL;  prev = NULL;
  }
  inline ~cInjectGenotypeElement() { ; }

  inline cInjectGenotype * GetInjectGenotype() const { return inject_genotype; }
  inline cInjectGenotypeElement * GetNext() const { return next; }
  inline cInjectGenotypeElement * GetPrev() const { return prev; }

  inline void SetNext(cInjectGenotypeElement * in_next) { next = in_next; }
  inline void SetPrev(cInjectGenotypeElement * in_prev) { prev = in_prev; }
};

#endif

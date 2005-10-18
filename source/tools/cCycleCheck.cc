/*
Copyright (C) 1993 - 2004 California Institute of Technology

Read the COPYING and README files, or contact 'avida@alife.org',
before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
*/

#include "cCycleCheck.h"
#include "cID.h"

#include <set>

/*!
\brief Internal class; used by cCycleCheck to store private lists of classes or objects encountered in recursive tests.

\remarks
This class is used internally to prevent cycles in recursive operations on collections of classes or objects.

\par See also
cCycleCheck

For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting

\ingroup recursiveTesting
*/
struct cCycleCheck::cCycleCheckPrv {
  /*!
  \brief Internal class; used by cCycleCheck::cCycleCheckPrv to provide a comparator for comparing c-style strings.

  \remarks This class is used internally to prevent cycles in recursive operations on collections of classes or objects; it is
  probably not well-suited for any other use.

  The name of this class is passed as the \e Comparator template parameter in declarations of std::set, specialized for
  collections of c-style strings.
  
  \par See also
  cCycleCheck
  \n
  cCycleCheck::cCycleCheckPrv
  
  For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting

  \ingroup recursiveTesting
  */
  struct cLessThanStrCmpFunctor {
    /*! Called by std::set<const char *, cLessThanStrCmpFunctor> to compare two strings. */
    bool operator()(const char* s1, const char* s2) const {
      return strcmp(s1, s2) < 0;
  } };
  /*! A collection of names of classes encountered during a recursive unit test. */
  std::set<const char *, cLessThanStrCmpFunctor> m_str_set;
  /*! A collection of IDs of objects encountered during a recursive object-verification. */
  std::set<const cID *> m_id_set;
};

cCycleCheck::cCycleCheck(bool recurse): m_cyck_prv(new cCycleCheckPrv), m_recurse(recurse) {}
cCycleCheck::~cCycleCheck(){ delete m_cyck_prv; }
bool cCycleCheck::ckStr(const char *str){ return m_cyck_prv->m_str_set.insert(str).second; }
bool cCycleCheck::ckID(const cID &id){ return m_cyck_prv->m_id_set.insert(&id).second; }


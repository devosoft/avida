/*
Copyright (C) 1993 - 2004 California Institute of Technology

Read the COPYING and README files, or contact 'avida@alife.org',
before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
*/

#ifndef VERIFIER_SUITE_RECURSER_HH
#define VERIFIER_SUITE_RECURSER_HH

#ifndef CYCLE_CHECK_HH
#include "cCycleCheck.hh"
#endif

#ifndef TEST_CASE_H
#include "yaktest/cTestCase.hh"
#endif


/*!
\brief Internal class; base class for tVerifierSuiteRecurser.

\remark This class is used internally as the superclass of tVerifierSuiteRecurser, and cannot be directly instantiated. Its
purpose is stripping the template-argument part of a tVerifierSuiteRecurser object's type so that the type-stripped object can
be passed to specialized instances of the tVerifierSuite template class. This class also removes code from
tVerifierSuiteRecurser that need not be specific to tVerifierSuiteRecurser's template parameter (with the goal of reducing
compilation time).

\par See also
tVerifierSuiteRecurser

For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting.

\ingroup recursiveTesting
*/
class cVerifierSuiteRecurser : public cTestCase {
private:
  cVerifierSuiteRecurser(); // disabled.
protected:
  cVerifierSuiteRecurser(bool recurse): m_cyck(recurse) {}
public:
  cCycleCheck m_cyck;
};

#endif

/*
Copyright (C) 1993 - 2004 California Institute of Technology

Read the COPYING and README files, or contact 'avida@alife.org',
before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
*/

#ifndef T_VERIFIER_SUITE_RECURSER_HH
#define T_VERIFIER_SUITE_RECURSER_HH

#ifndef VERIFIER_SUITE_RECURSER_HH
#include "cVerifierSuiteRecurser.hh"
#endif

#ifndef T_VERIFIER_SUITE_HH
#include "tVerifierSuite.hh"
#endif


/*!
\brief Internal class; used by the macro avdRecursiveTestsDefnMacro().

\remark This class is used internally by the macro avdRecursiveTestsDefnMacro to collect the result of recursive verficiation
of a verifiable object.

This class inherits indirectly from the class \e cTestCase from the third-party library \e yaktest. When the \e verify()
function defined in the macro avdRecursiveTestsDefnMacro calls the inherited member function \e runTest() of this class, the
superclass \e cTestCase invokes the member function \e test() of this class. \e runTest() returns the result of the test as
an instance of the \e yaktest class \cTestResult, and this instance is returned by \e verify().

\par See also
avdRecursiveTestsDefnMacro()
\n
tVerifierSuite

For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting.

\ingroup recursiveTesting
*/
template <class T> struct tVerifierSuiteRecurser : public cVerifierSuiteRecurser {
  /*! The object to recursively verify. */
  T &m_obj;
  /*!
  \brief constructs a tVerifierSuiteRecurser to test \e obj. If \e recurse is true then the test will recurse.
  */
  tVerifierSuiteRecurser(T &obj, bool recurse): cVerifierSuiteRecurser(recurse), m_obj(obj) {}
  /*
  \brief indirectly called (via the \e yaktest library) to test \e m_obj and optionally recursively test its subobjects.
  */
  virtual void test(){ if(m_cyck.ckID(m_obj.id())){ tVerifierSuite<T> verifier(*this); verifier.verify(m_obj); } }
};

#endif

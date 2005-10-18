/*
Copyright (C) 1993 - 2004 California Institute of Technology

Read the COPYING and README files, or contact 'avida@alife.org',
before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
*/

#ifndef T_UNIT_TEST_SUITE_RECURSER_HH
#define T_UNIT_TEST_SUITE_RECURSER_HH

#ifndef CYCLE_CHECK_HH
#include "cCycleCheck.h"
#endif
#ifndef T_UNIT_TEST_SUITE_HH
#include "tUnitTestSuite.hh"
#endif

#ifndef TEST_CASE_H
#include "yaktest/cTestCase.hh"
#endif


/*!
\brief Internal class; used by the macro avdRecursiveTestsDefnMacro().

\remark This class is used internally by the macro avdRecursiveTestsDefnMacro to collect the result of a recursive unit test.

This class inherits from the class \e cTestCase from the third-party library \e yaktest. When the \e unitTest() function
defined in the macro avdRecursiveTestsDefnMacro calls the inherited member function \e runTest() of this class, the
superclass \e cTestCase invokes the member function \e test() of this class. \e runTest() returns the result of the test as
an instance of the \e yaktest class \cTestResult, and this instance is returned by \e unitTest().

\par See also
avdRecursiveTestsDefnMacro()
\n
tUnitTestSuite

For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting.

\ingroup recursiveTesting
*/
template <class T> struct tUnitTestSuiteRecurser : public cTestCase {
  cCycleCheck m_cyck;
  tUnitTestSuiteRecurser(bool recurse): m_cyck(recurse) {
    m_cyck.ckStr(tUnitTestSuite<T>::getClassName());
    adoptTestCase(new tUnitTestSuite<T>(m_cyck));
  }
};

#endif

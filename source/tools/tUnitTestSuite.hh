/*
Copyright (C) 1993 - 2004 California Institute of Technology

Read the COPYING and README files, or contact 'avida@alife.org',
before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
*/

#ifndef T_UNIT_TEST_SUITE_HH
#define T_UNIT_TEST_SUITE_HH

#ifndef CYCLE_CHECK_HH
#include "cCycleCheck.hh"
#endif
#ifndef TEST_CASE_H
#include "yaktest/cTestCase.hh"
#endif


/*!
\brief Internal class; base class for tUnitTestSuite.

\remark This class is used internally as the superclass of tUnitTestSuite, and cannot be directly instantiated. Its purpose is
removing code from tUnitTestSuite that need not be specific to tUnitTestSuite's template parameter (with the goal of reducing
compilation time).

\par See also
tUnitTestSuite

For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting.

\ingroup recursiveTesting
*/
class cUnitTestSuite : public cTestCase {
private:
  cUnitTestSuite(); // disabled.
protected:
  cCycleCheck &m_cyck;
  /*!
  \brief Can only be called by the subclass tUnitTestSuite. See tUnitTestSuite.
  */
  cUnitTestSuite(cCycleCheck &cyck): m_cyck(cyck){}
  /*!
  \brief A template function used to specify recursively-run test-suites.
  */
  template <class U> void adoptUnitTestSuite();
  /*!
  \brief
  Another name for the member function adoptTestCase() inherited from the superclass cTestCase.

  (The class cTestCase is defined in the third-party library \e yaktest.) This function is called to load test-cases for this
  unit-test suite.
  */
  void adoptUnitTestCase(cTestCase *tc){ adoptTestCase(tc); }
};


/*!
\brief Provides recursive unit-test suites template.

This template class is indirectly specialized as a consequence of calling the macro \c avdRecursiveTestsDeclMacro() in a
public section of a class declaration, and the macro \c avdRecursiveTestsDefnMacro() in that class' implementation. Each of
these macros takes two arguments; the first is the name of the class being declared and defined, and the second is the name of
its superclass (although this second argument is currently unused).

Once these macros have been called in the declaration and definition of class \e Class, and the compiler generates code for
the specialization \e tUnitTestSuite \<Class\>, the member function adoptUnitTests() remains undefined.

The definition of adoptUnitTests() must be written by the programmer, and provides the means to specify both unit-test cases
for the test-suite \e tUnitTestSuite \<Class\>, and test-suites for other classes on which \e Class depends and which should
be run recursively.

(Similarly the programmer must define the member function \e tVerifierSuite::verify() of the template class specialization
\e tVerifierSuite \<Class\>. For details, see the documentation for tVerifierSuite.)

\par Important inherited members
\n
cUnitTestSuite::adoptUnitTestSuite()
\n
cUnitTestSuite::adoptUnitTestCase()

\par See also
tVerifierSuite
\n
avdRecursiveTestsDeclMacro()
\n
avdRecursiveTestsDefnMacro()

For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting.

\ingroup recursiveTesting
*/
template <class T> class tUnitTestSuite : public cUnitTestSuite {
private:
  tUnitTestSuite(); // disabled.
public:
  /*!
  \brief Constructs a unit-test suite instance which can recursively run other unit-test suites.

  This constructor is not called directly except in the body of the macro avdRecursiveTestsDefnMacro().
  */
  tUnitTestSuite(cCycleCheck &cyck): cUnitTestSuite(cyck) {
    adoptUnitTests();
  }
  /*!
  \brief Used to indicate test-cases and recursive test-suites which should be run by tUnitTestSuite specializations.

  This member function is not defined in the general case, and for each specialization of tUnitTestSuite, a version of this
  member function must be written. The specialized member function will be indirectly called to load the test-cases and
  recursive unit-test suites it specifies.
  */
  void adoptUnitTests();
  /*!
  \brief Returns a unique name for specializations of this template class.

  This specialized member function is automatically defined when the macro avdRecursiveTestsDefnMacro() is called. It provides
  a unique name for the template class specialization, and this name is used by instances of class cCycleCheck to identify
  this class when avoiding cycles in recursive unit-tests.
  */
  static const char *getClassName();
};

template <class U> void cUnitTestSuite::adoptUnitTestSuite(){
  if(m_cyck.recurse() && m_cyck.ckStr(tUnitTestSuite<U>::getClassName()))
    adoptTestCase(new tUnitTestSuite<U>(m_cyck));
}

#endif

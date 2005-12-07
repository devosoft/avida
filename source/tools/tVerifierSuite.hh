/*
 *  tVerifierSuite.hh
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef tVerifierSuite_hh
#define tVerifierSuite_hh
#endif

#ifndef cVerifierSuiteRecurser_h
#include "cVerifierSuiteRecurser.h"
#endif

/*!
\brief Internal class; base class for tVerifierSuite.

\remark This class is used internally as the superclass of tVerifierSuite, and cannot be directly instantiated. Its purpose is
removing code from tVerifierSuite that need not be specific to tVerifierSuite's template parameter (with the goal of reducing
compilation time).

\par See also
tVerifierSuite.

For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting.

\ingroup recursiveTesting
*/
class cVerifierSuite {
private:
  cVerifierSuite(); // disabled.
protected:
  cVerifierSuiteRecurser &m_recurser;
  /*!
  \brief Can only be called by the subclass tVerifierSuite. See tVerifierSuite.
  */
  cVerifierSuite(cVerifierSuiteRecurser &recurser): m_recurser(recurser){}
  /*!
  \brief Specifies a recursively-run subobject-verification suite, and the subobject to be verified.
  */
  template <class U> void adoptVerifierSuite(U &obj);

  /* @{ */
  /*!
  \brief convenience functions permitting use of \e yaktest testing macros in tVerifierSuite instances.

  These functions are generally not called directly but through \e yaktest testing macros of the same name, sans the 'Stub'
  suffix. See the \e yaktest documentation for more details.

  \note to developers:
  These functions, when called by the \e yaktest testing macros, must be forwarded to a cTestCase instance since this class
  doesn't inherit from cTestCase. This class has a cVerifierSuiteRecurser, which is a cTestCase, and will serve.
  */
  bool testNonErrorStub(
    bool condition,
    const std::string& condition_name,
    const std::string& file_name,
    const std::string& function_name,
    long line_number
  ){ return m_recurser.testNonErrorStub(condition, condition_name, file_name, function_name, line_number); }
  bool testIsTrueStub(
    bool condition,
    const std::string& condition_name,
    const std::string& file_name,
    const std::string& function_name,
    long line_number
  ){ return m_recurser.testIsTrueStub(condition, condition_name, file_name, function_name, line_number); }
  bool testIntIsEqualStub(
    long actual,
    long expected,
    const std::string& file_name,
    const std::string& function_name,
    long line_number
  ){ return m_recurser.testIntIsEqualStub(actual, expected, file_name, function_name, line_number); }
  bool testDoubleIsEqualStub(
    double actual,
    double expected,
    const std::string& file_name,
    const std::string& function_name,
    long line_number,
    double tolerance = 0.005
  ){ return m_recurser.testDoubleIsEqualStub(actual, expected, file_name, function_name, line_number, tolerance); }
  bool testStringIsEqualStub(
    const std::string& actual,
    const std::string& expected,
    const std::string& file_name,
    const std::string& function_name,
    long line_number
  ){ return m_recurser.testStringIsEqualStub(actual, expected, file_name, function_name, line_number); }
  template <class T> bool testIsEqualStub(
    const T& actual,
    const T& expected,
    const std::string& file_name,
    const std::string& function_name,
    long line_number
  ){ return m_recurser.testIsEqualStub(actual, expected, file_name, function_name, line_number); }
  /* @} */
};

/*!
\brief Provides recursive object-verification suites template.

This template class is indirectly specialized as a consequence of calling the macro \c avdRecursiveTestsDeclMacro() in a
public section of a class declaration, and the macro \c avdRecursiveTestsDefnMacro() in that class' implementation. Each of
these macros takes two arguments; the first is the name of the class being declared and defined, and the second is the name of
its superclass (although this second argument is currently unused).

Once these macros have been called in the declaration and definition of class \e Class, and the compiler generates code for
the specialization \e tVerifierSuite \<Class\>, the member function verify() remains undefined.

The definition of verify() must be written by the programmer, and provides the means to test instances of \e Class and to
recursively test sub-objects of the instance.

(Similarly the programmer must define the member function \e tUnitTestSuite::adoptUnitTests() of the template class
specialization \e tUnitTestSuite \<Class\>. For details, see the documentation for tUnitTestSuite.)

\par See also
tUnitTestSuite
\n
avdRecursiveTestsDeclMacro()
\n
avdRecursiveTestsDefnMacro()

For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting.

\ingroup recursiveTesting
*/
template <class T> class tVerifierSuite : public cVerifierSuite {
private:
  tVerifierSuite(); // disabled.
public:
  /*!
  \brief Constructs an object-verification suite instance which can recursively test sub-objects.

  This constructor is not called directly except in the body of the macro avdRecursiveTestsDefnMacro().
  */
  tVerifierSuite(cVerifierSuiteRecurser &recurser): cVerifierSuite(recurser) {}
  /*!
  \brief Used to verify objects and to recursively run test-suites for verifying sub-objects.

  This member function is not defined in the general case, and for each specialization of tVerifierSuite, a version of this
  member function must be written. The specialized member function will be indirectly called to verify an instance of the
  template parameter, and to recursively run test-suites for verifying its sub-objects.
  */
  void verify(T &obj);
};

template <class U> void cVerifierSuite::adoptVerifierSuite(U &obj){
  if(m_recurser.m_cyck.recurse() && m_recurser.m_cyck.ckID(obj.id())){
    tVerifierSuite<U> verifier(m_recurser); verifier.verify( obj);
} }

/*
Copyright (C) 1993 - 2004 California Institute of Technology

Read the COPYING and README files, or contact 'avida@alife.org',
before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
*/

#ifndef RECURSIVE_TESTS_DECL_HDRS_HH
#define RECURSIVE_TESTS_DECL_HDRS_HH

/*!
\defgroup recursiveTesting Recursive unit-testing and recursive object-verification

\brief
When Avida is compiled in Debug mode, many classes can be recursively unit-tested, and many objects can be recursively
verified. These classes and macros form the support code for this functionality. Most of these classes are used internally,
and instantiating them outside of the recursive-testing system them won't be useful, but together they form hooks that you can
use to provide recursive unit-test and object-verification suites for new classes.

As of version 3.0, Avida incorporates scripting in its design, and Avida can be instantiated and run interactively from
within scripting shells. Among other things this permits us to write a short scripts to configure an Avida run, try to bring
that run into some particular state, and then examine that state in close detail without recompiling Avida.

We use this scripting support to help troubleshoot Avida. Specifically, many objects (instances of C++ classes) occurring in
Avida have two extra functions \e unitTest() and \e verify() when Avida is compiled in Debug mode. These functions can be
called on such an object from a script or a scripting shell to verify that object's state and data, and to unit-test its
class. For convenience these functions optionally recursively call the same functions on the classes or objects on which the
object depends.

Notwithstanding the complexity of a particular unit-test of a new class, or a verifier-function for instances of that class,
it's easy to connect new suites of unit-tests and object-verifiers to the new class. The process in brief involves calling the
macro avdRecursiveTestsDeclMacro() from a public section of the class definition, calling the macro
avdRecursiveTestsDefnMacro() in the class implementation, and defining the bodies of the functions
tUnitTestSuite<>::adoptUnitTests() and tVerifierSuite<>::verify(), specialized for the new class.

The body of tUnitTestSuite<>::adoptUnitTests() will both load unit-tests for the new class, and name other classes whose
unit-test suites should be recursively loaded and run. Similarly, given an instance of the new class,
tVerifierSuite<>::verify() will both run verifiers on the instance, and name subobjects that should be tested by their own
verifier suites.

If your object is named \e obj, calling \e obj.unitTest() will indirectly call tUnitTestSuite<>::adoptUnitTests(), and calling
\e obj.verify() will indirectly call tVerifierSuite<>::verify(). Both of \e obj.unitTest() and \e obj.verify() return a \e
yaktest \e cTestResult object containing a list of failed tests encountered, and the contents of the list can be examined
using the \e cTestResult member functions, or by instantiating a \e yaktest \e cTextTestInterpreter and calling its member
function \e interpretation() with the \e cTestResult as its argument; this will cause the list of failed tests to be printed,
with details, to standard-out.


*/

#ifndef NDEBUG
  #ifndef ID_HH
  #include "cID.hh"
  #endif
  #ifndef RECURSIVE_TESTS_MACROS_HH
  #include "recursive_tests_macros.hh"
  #endif
  
  #ifndef TEST_RESULT_H
  #include "yaktest/cTestResult.hh"
  #endif
#endif

#endif

/*
Copyright (C) 1993 - 2004 California Institute of Technology

Read the COPYING and README files, or contact 'avida@alife.org',
before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
*/

#ifndef RECURSIVE_TESTS_MACROS_HH
#define RECURSIVE_TESTS_MACROS_HH

template <class T> class tUnitTestSuite;
template <class T> class tVerifierSuite;

#ifndef NDEBUG

  /*!\brief
  A macro to declare recursive unit-testing and verification method hooks in your class.
  
  This macro takes as its arguments the name of your class and the name of its superclass, respectively.  Use this macro in a
  public section of your class prototype to declare the member functions
  
    \code
    virtual cTestResult unitTest(bool recursive);
    virtual cTestResult verify(bool recursive);
    \endcode
  
  These functions provide means to recursively unit-test your class, and to recursively verify instances of your class.

  \par See also
  tUnitTestSuite
  \n
  tVerifierSuite

  \ingroup recursiveTesting
  */
  #define avdRecursiveTestsDeclMacro(thisClass, superclass) \
    \
  public: \
    friend class tVerifierSuite<thisClass>; \
    static cTestResult unitTest(bool recurse); \
    virtual cTestResult verify(bool recurse); \
  private: \
    cID m_id; \
  public: \
    cID &id(){ return m_id; } \

  /*!\brief
  A macro to define recursive unit-testing and verification method hooks in your class.
  
  This macro takes as its arguments the name of your class and the name of its superclass, respectively.  Use this macro in a
  public section of your class prototype to define the member functions
  
    \code
    virtual cTestResult unitTest(bool recursive);
    virtual cTestResult verify(bool recursive);
    \endcode
  
  These functions provide means to recursively unit-test your class, and to recursively verify instances of your class.

  \par See also
  tUnitTestSuite
  \n
  tVerifierSuite

  \ingroup recursiveTesting
  */
  #define avdRecursiveTestsDefnMacro(thisClass, superclass) \
  \
  template <> const char * tUnitTestSuite<thisClass>::getClassName(){ return "tUnitTestSuite<" #thisClass ">"; } \
  template <> void tUnitTestSuite<thisClass>::adoptUnitTests(); \
  cTestResult thisClass::unitTest(bool recurse){ \
    tUnitTestSuiteRecurser<thisClass> utr(recurse); utr.runTest(); return utr.lastResult(); \
  } \
  cTestResult thisClass::verify(bool recurse){ \
    tVerifierSuiteRecurser<thisClass> vr(*this, recurse); vr.runTest(); return vr.lastResult(); \
  }

/* disabled 'cuz the object-verification suite doesn't need it. might be convenient to reenable later. */
//  template <> const char * tVerifierSuite<thisClass>::getClassName(){ return "tVerifierSuite<" #thisClass ">"; }

#else /* NDEBUG */
  
  /*
  When not compiling in Debug mode, still give the unit-test and object-verify suites access to this class.
  */
  #define avdRecursiveTestsDeclMacro(thisClass, superclass) \
    \
    friend class tUnitTestSuite<thisClass>; \
    friend class tVerifierSuite<thisClass>; \

  #define avdRecursiveTestsDefnMacro(thisClass, superclass) \
  \
  template <> const char * tUnitTestSuite<thisClass>::getClassName(){ return "tUnitTestSuite<" #thisClass ">"; }
#endif /* NDEBUG */


#endif

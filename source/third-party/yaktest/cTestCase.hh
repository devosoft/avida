#ifndef TEST_CASE_H
#define TEST_CASE_H

#ifndef TEST_RESULT_H
#include "cTestResult.hh"
#endif
#ifndef STRINGABLE_H
#include "cStringable.hh"
#endif

#include <boost/shared_ptr.hpp>

#include <vector>

#ifdef WIN32
#include <sstream>
using std::ostringstream;
#endif

/*!
\defgroup
*/

/*!
\brief A composite test case

Its atomic test can be redefined by overriding test(); it can also serve
as a suite of tests by adding cTestCases to it via the addTestCase
function.  It functions as a composite object (see the composite pattern
in Design Patterns, p 163) without methods for getting individual
children.
*/
class cTestCase 
{
 protected:
  //private methods and data
  //!the result of the last test
  cTestResult m_last_result;
  //!the collection of subtests
  std::vector<boost::shared_ptr<cTestCase> >m_subtests;

 public:
  //!runs the subtests
  virtual void runSubtests( void );

 public:
  //construction/destruction
  cTestCase( void );
  cTestCase(const cTestCase &);
  virtual ~cTestCase( void );

  //composition methods
  //! adds a child to the test case
  virtual void adoptTestCase( cTestCase* new_case );
  //! adds a child to the test case, for use by boost.python
  virtual void pyAdoptTestCase(boost::shared_ptr<cTestCase> new_case );
  //! number of child test cases
  int subtestCount( void ) const;
  
  //testing methods
  //! runs the test.  This is a template method (Design Patterns 325)
  // Made virtual so subclasses could redefine the template method.  RS 2/21/01
  // specificly I want a subclass that won't exit until it sees a QT signal
  virtual void runTest( void );
  //! tests to see if the supplied condition is true; called by the test_is_true macro
  bool testNonErrorStub( bool condition, 
                         const std::string& condition_name,
                         const std::string& file_name,
                         const std::string& function_name,
                         long line_number,
                         const std::string& msg = "" );
  //! tests to see if the supplied condition is true; called by the test_is_true macro
  bool testIsTrueStub( bool condition, 
                       const std::string& condition_name,
                       const std::string& file_name,
                       const std::string& function_name,
                       long line_number,
                       const std::string& msg = "" );
  //! tests to see if two longs are equal
  bool testIntIsEqualStub( long actual,
                           long expected,
                           const std::string& file_name,
                           const std::string& function_name,
                           long line_number,
                           const std::string& msg = "" );
  //! tests to see if two doubles are within limits
  bool testDoubleIsEqualStub( double actual,
                              double expected,
                              const std::string& file_name,
                              const std::string& function_name,
                              long line_number,
                              double tolerance = 0.005,
                              const std::string& msg = "" );
  //! tests to see if two strings are equal
  bool testStringIsEqualStub( const std::string& actual,
                              const std::string& expected,
                              const std::string& file_name,
                              const std::string& function_name,
                              long line_number,
                              const std::string& msg = "" );
  //! creates an "equality test failed" problem report
  cTestProblem createEqualityProblemReport( const std::string& actual_message,
                                            const std::string& expected_message,
                                            const std::string& file_name,
                                            const std::string& function_name,
                                            long line_number,
                                            const std::string& msg = "" );
  
                                 
  template< class T > bool testIsEqualStub( const T& actual,
                                            const T& expected,
                                            const std::string& file_name,
                                            const std::string& function_name,
                                            long line_number,
                                            const std::string& msg = "" )
    {
      if ( actual == expected ) 
        {
          m_last_result.addSuccess();
          return true;
        }
      else
        {

//msvc choked on the dynamic cast RS 2/14/2001
// static cast will cause runtime error because dynamic cast returns null if the cast is incorrect 
#ifdef LINUX
          const stringable* actual_ptr = dynamic_cast< const stringable* >( &actual );
          const stringable* expected_ptr = dynamic_cast< const stringable* >( &expected );
          std::string actual_message = 
            ( actual_ptr == NULL ) ? "" : actual_ptr->to_string();
          std::string expected_message = 
            ( expected_ptr == NULL ) ? "" : expected_ptr->to_string();
          m_last_result.addFailure( createEqualityProblemReport( actual_message,
                                                                 expected_message,
                                                                 file_name,
                                                                 function_name,
                                                                 line_number ) );
#endif // LINUX
        //will use strstreams to get generic i/o working
        //just make sure all components have an operator <<
#ifdef WIN32
          ostringstream actual_message;
          ostringstream expected_message; 

          actual_message << actual;
          expected_message << expected;
          m_last_result.addFailure( createEqualityProblemReport( actual_message.str(),
                                                                 expected_message.str(),
                                                                 file_name,
                                                                 function_name,
                                                                 line_number ) );

#endif // WIN32
          return false;
        }
    };
  //! check the last result for final test results
  const cTestResult& lastResult( void ) const;

  //overrideables
  //! sets up the test.  Key method to override
  virtual void setUp( void );
  //! tears down after the test, and cleans up   Key method to override
  virtual void tearDown( void );
  //!the test itself.    Key method to override
  /*! It should either use the test_is_true/test_is_equal methods or 
   * append to the result itself
   */
  virtual void test( void );
};


//these macros are used instead of the code stubs in cTestCase
//in order to use preprocessor features to get file_name/line number
#ifdef WIN32
  #define test_non_error( condition ) testNonErrorStub( (condition), (#condition), __FILE__, (""), __LINE__ )
  #define test_is_true( condition ) testIsTrueStub( (condition), (#condition), __FILE__, (""), __LINE__ )
  #define test_int_is_equal( actual_value, expected_value ) testIntIsEqualStub( actual_value, expected_value, __FILE__, (""), __LINE__ )
  #define test_double_is_equal( actual_value, expected_value ) testDoubleIsEqualStub( actual_value, expected_value, __FILE__, (""), __LINE__ )
  #define double_is_within_tolerance( actual_value, expected_value, tolerance ) testDoubleIsEqualStub( actual_value, expected_value, __FILE__, (""), __LINE__, tolerance )
  #define test_string_is_equal( actual_value, expected_value ) testStringIsEqualStub( actual_value, expected_value, __FILE__, (""), __LINE__ )
  #define test_is_equal( actual_value, expected_value ) testIsEqualStub( actual_value, expected_value, __FILE__, (""), __LINE__ )

  #define test_non_error_( condition, msg ) testNonErrorStub( (condition), (#condition), __FILE__, (""), __LINE__, msg )
  #define test_is_true_( condition, msg ) testIsTrueStub( (condition), (#condition), __FILE__, (""), __LINE__, msg )
  #define test_int_is_equal_( actual_value, expected_value, msg ) testIntIsEqualStub( actual_value, expected_value, __FILE__, (""), __LINE__, msg )
  #define test_double_is_equal_( actual_value, expected_value, msg ) testDoubleIsEqualStub( actual_value, expected_value, __FILE__, (""), __LINE__, msg )
  #define double_is_within_tolerance_( actual_value, expected_value, tolerance, msg ) testDoubleIsEqualStub( actual_value, expected_value, __FILE__, (""), __LINE__, tolerance, msg )
  #define test_string_is_equal_( actual_value, expected_value, msg ) testStringIsEqualStub( actual_value, expected_value, __FILE__, (""), __LINE__, msg )
  #define test_is_equal_( actual_value, expected_value, msg ) testIsEqualStub( actual_value, expected_value, __FILE__, (""), __LINE__, msg )

#else // WIN32
  #define test_non_error( condition ) testNonErrorStub( (condition), (#condition), __FILE__, __PRETTY_FUNCTION__, __LINE__ )
  #define test_is_true( condition ) testIsTrueStub( (condition), (#condition), __FILE__, __PRETTY_FUNCTION__, __LINE__ )
  #define test_int_is_equal( actual_value, expected_value ) testIntIsEqualStub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__ )
  #define test_double_is_equal( actual_value, expected_value ) testDoubleIsEqualStub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__ )
  #define double_is_within_tolerance( actual_value, expected_value, tolerance ) testDoubleIsEqualStub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__, tolerance )
  #define test_string_is_equal( actual_value, expected_value ) testStringIsEqualStub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__ )
  #define test_is_equal( actual_value, expected_value ) testIsEqualStub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__ )

  #define test_non_error_( condition, msg ) testNonErrorStub( (condition), (#condition), __FILE__, __PRETTY_FUNCTION__, __LINE__, msg )
  #define test_is_true_( condition, msg ) testIsTrueStub( (condition), (#condition), __FILE__, __PRETTY_FUNCTION__, __LINE__, msg )
  #define test_int_is_equal_( actual_value, expected_value, msg ) testIntIsEqualStub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__, msg )
  #define test_double_is_equal_( actual_value, expected_value, msg ) testDoubleIsEqualStub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__, msg )
  #define double_is_within_tolerance_( actual_value, expected_value, tolerance, msg ) testDoubleIsEqualStub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__, tolerance, msg )
  #define test_string_is_equal_( actual_value, expected_value, msg ) testStringIsEqualStub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__ )
  #define test_is_equal_( actual_value, expected_value, msg ) testIsEqualStub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__, msg )

#endif // WIN32

#endif



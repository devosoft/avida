#ifndef TEST_CASE_H
#define TEST_CASE_H

#ifndef TEST_RESULT_H
#include "test_result.h"
#endif
#ifndef STRINGABLE_H
#include "stringable.h"
#endif
#include <vector>

#ifdef WIN32
#include <sstream>
using std::ostringstream;
#endif //WIN32

// $Id: test_case.h,v 1.2 2003/12/29 22:55:16 kaben Exp $
/*! A composite test case 
 * its atomic test can be redefined by overriding
 * test(); it can also serve as a suite of tests by adding test_cases
 * to it via the add_test_case function.  It functions as a composite
 * object (see the composite pattern in Design Patterns, p 163) without
 * methods for getting individual children.
 */
class test_case 
{
 protected:
  //private methods and data
  //!the result of the last test
  test_result m_last_result;
  //!the collection of subtests
  std::vector< test_case* >m_subtests;
  //!runs the subtests
  virtual void run_subtests( void );

 public:
  //construction/destruction
  test_case( void );
  virtual ~test_case( void );

  //composition methods
  //! adds a child to the test case
  virtual void adopt_test_case( test_case* new_case );
  //! number of child test cases
  int subtest_count( void ) const;
  
  //testing methods
  //! runs the test.  This is a template method (Design Patterns 325)
  // Made virtual so subclasses could redefine the template method.  RS 2/21/01
  // specificly I want a subclass that won't exit until it sees a QT signal
  virtual void run_test( void );
  //! tests to see if the supplied condition is true; called by the test_is_true macro
  void test_non_error_stub( bool condition, 
			    const std::string& condition_name,
			    const std::string& filename,
			    const std::string& functionname,
			    long line_number );
  //! tests to see if the supplied condition is true; called by the test_is_true macro
  void test_is_true_stub( bool condition, 
			  const std::string& condition_name,
			  const std::string& filename,
			  const std::string& functionname,
			  long line_number );
  //! tests to see if two longs are equal
  void test_int_is_equal_stub( long actual,
			   long expected,
			   const std::string& filename,
			   const std::string& functionname,
			   long line_number );
  //! tests to see if two doubles are within limits
  void test_double_is_equal_stub( double actual,
				  double expected,
				  const std::string& filename,
			          const std::string& functionname,
				  long line_number,
				  double tolerance = 0.005 );
  //! tests to see if two strings are equal
  void test_string_is_equal_stub( const std::string& actual,
				  const std::string& expected,
				  const std::string& filename,
			          const std::string& functionname,
				  long line_number );
  //! creates an "equality test failed" problem report
  test_problem create_equality_problem_report( const std::string& actual_message,
					       const std::string& expected_message,
					       const std::string& filename,
			                       const std::string& functionname,
					       long line_number );
  
				 
  template< class T >void test_is_equal_stub( const T& actual,
					      const T& expected,
					      const std::string& filename,
			                      const std::string& functionname,
					      long line_number )
    {
      if ( actual == expected ) 
	{
	  m_last_result.add_success();
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
	  m_last_result.add_failure( create_equality_problem_report( actual_message,
								     expected_message,
							             filename,
							             functionname,
								     line_number ) );
#endif // LINUX
        //will use strstreams to get generic i/o working
        //just make sure all components have an operator <<
#ifdef WIN32
	  ostringstream actual_message;
	  ostringstream expected_message; 

    actual_message << actual;
    expected_message << expected;
	  m_last_result.add_failure( create_equality_problem_report( actual_message.str(),
								     expected_message.str(),
							             filename,
							             functionname,
								     line_number ) );

#endif // WIN32
	}
    };
  //! check the last result for final test results
  const test_result& last_result( void ) const;

  //overrideables
  //! sets up the test.  Key method to override
  virtual void set_up( void );
  //! tears down after the test, and cleans up   Key method to override
  virtual void tear_down( void );
  //!the test itself.    Key method to override
  /*! It should either use the test_is_true/test_is_equal methods or 
   * append to the result itself
   */
  virtual void test( void );
};


//these macros are used instead of the code stubs in test_case
//in order to use preprocessor features to get filename/line number
#ifdef WIN32
  #define test_non_error( condition ) (this->test_non_error_stub( (condition), (#condition), __FILE__, (""), __LINE__ ))
  #define test_is_true( condition ) (this->test_is_true_stub( (condition), (#condition), __FILE__, (""), __LINE__ ))
  #define test_int_is_equal( actual_value, expected_value ) (this->test_int_is_equal_stub( actual_value, expected_value, __FILE__, (""), __LINE__ ))
  #define test_double_is_equal( actual_value, expected_value ) (this->test_double_is_equal_stub( actual_value, expected_value, __FILE__, (""), __LINE__ ))
  #define double_is_within_tolerance( actual_value, expected_value, tolerance ) ( this->test_double_is_equal_stub( actual_value, expected_value, __FILE__, (""), __LINE__, tolerance ))
  #define test_string_is_equal( actual_value, expected_value ) (this->test_string_is_equal_stub( actual_value, expected_value, __FILE__, (""), __LINE__ ))
  #define test_is_equal( actual_value, expected_value ) (this->test_is_equal_stub( actual_value, expected_value, __FILE__, (""), __LINE__ ))
#else // WIN32
  #define test_non_error( condition ) (this->test_non_error_stub( (condition), (#condition), __FILE__, __PRETTY_FUNCTION__, __LINE__ ))
  #define test_is_true( condition ) (this->test_is_true_stub( (condition), (#condition), __FILE__, __PRETTY_FUNCTION__, __LINE__ ))
  #define test_int_is_equal( actual_value, expected_value ) (this->test_int_is_equal_stub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__ ))
  #define test_double_is_equal( actual_value, expected_value ) (this->test_double_is_equal_stub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__ ))
  #define double_is_within_tolerance( actual_value, expected_value, tolerance ) ( this->test_double_is_equal_stub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__, tolerance ))
  #define test_string_is_equal( actual_value, expected_value ) (this->test_string_is_equal_stub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__ ))
  #define test_is_equal( actual_value, expected_value ) (this->test_is_equal_stub( actual_value, expected_value, __FILE__, __PRETTY_FUNCTION__, __LINE__ ))
#endif // WIN32

#endif



#include "test_case.h"

#include<math.h>
#include<stdio.h>

//adopts a test case; we use "adopt" because we intend to take over
//the destruction of the passed object
void
test_case::adopt_test_case( test_case* new_case )
{
  m_subtests.push_back( new_case );
}

//the number of subtests
int
test_case::subtest_count( void ) const
{
  return m_subtests.size();
}

//runs all the subtests
void
test_case::run_subtests( void ) 
{
  for ( std::vector< test_case* >::iterator iter = m_subtests.begin();
	iter != m_subtests.end();
	++iter )
    {
      (*iter)->run_test();
      m_last_result.add_result( (*iter)->last_result() );
    }
}

//actually runs the test
void
test_case::run_test( void )
{
  m_last_result.delete_contents();
  set_up();
  run_subtests();
  try
    {
      test();
    }
  catch ( std::exception& e )
    {
      test_problem new_error( "unknown file", -1, e.what() );
      m_last_result.add_error( new_error );
    }
  tear_down();
}

//tests to see if the supplied condition is true; called by the 
//test_non_error macro
void
test_case::test_non_error_stub( bool condition,
			        const std::string& condition_name,
			        const std::string& filename,
			        const std::string& functionname,
			        long line_number )
{
  if ( !condition )
    {
      std::string message = "\"";
      message.append( condition_name );
      message.append( std::string( "\" evaluated to false." ) );
      m_last_result.add_error( test_problem( filename,
					       functionname,
					       line_number,
					       message ));
    }
}


//tests to see if the supplied condition is true; called by the 
//test_is_true macro
void
test_case::test_is_true_stub( bool condition,
			      const std::string& condition_name,
			      const std::string& filename,
			      const std::string& functionname,
			      long line_number )
{
  if ( condition )
    {
      m_last_result.add_success();
    }
  else 
    {
      std::string message = "\"";
      message.append( condition_name );
      message.append( std::string( "\" evaluated to false." ) );
      m_last_result.add_failure( test_problem( filename,
					       functionname,
					       line_number,
					       message ));
    }
}


//creates a test problem representing an inequality
test_problem
test_case::create_equality_problem_report( const std::string& actual_message,
					   const std::string& expected_message,
					   const std::string& filename,
					   const std::string& functionname,
					   long line_number )
{
  std::string message = "Equality failed.  Got ";
  message.append( actual_message );
  message.append( ", expected " );
  message.append( expected_message );
  return test_problem( filename, functionname, line_number, message );
}


//checks to see if two longs are equal
//tests to see if two longs are equal
void
test_case::test_int_is_equal_stub( long actual,
			       long expected,
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
      char temp[100];
      sprintf( temp, "%ld", actual );
      std::string actual_message( temp );
      sprintf( temp, "%ld", expected );
      std::string expected_message( temp );
      m_last_result.add_failure( create_equality_problem_report( actual_message,
								 expected_message,
								 filename,
								 functionname,
								 line_number ) );
    }
}

void 
test_case::test_double_is_equal_stub( double actual,
				      double expected,
				      const std::string& filename,
				      const std::string& functionname,
				      long line_number,
// msvc didn't like the redeclaration of default parameter RS 2/14/2001
//				      double tolerance = 0.005 )
				      double tolerance)
{
  if ( fabs( actual - expected ) < fabs( tolerance ) )
    {
      m_last_result.add_success();
    }
  else
    {
      char temp[100];
      sprintf( temp, "%f", actual );
      std::string actual_message( temp );
      sprintf( temp, "%f", expected );
      std::string expected_message( temp );
      m_last_result.add_failure( create_equality_problem_report( actual_message,
								 expected_message,
								 filename,
								 functionname,
								 line_number ));
    }
}


//tests to see if two strings are equal
void 
test_case::test_string_is_equal_stub( const std::string& actual,
			   const std::string& expected,
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
      std::string actual_message = std::string( "\"" );
      actual_message.append( actual );
      actual_message.append( "\"" );
      std::string expected_message = std::string( "\"" );
      expected_message.append( expected );
      expected_message.append( "\"" );
      m_last_result.add_failure( create_equality_problem_report( actual_message,
								 expected_message,
								 filename,
								 functionname,
								 line_number ));
    }
}



//sets up the test (here, does nothing)
void
test_case::set_up( void ) 
{
}

//tears down the test (here, does nothing)
void
test_case::tear_down( void )
{
}

//the test itself; here, does nothing
void
test_case::test( void )
{
}

//construction
test_case::test_case( void ) 
{
  m_last_result.delete_contents();
  m_subtests.clear();
}

//destruction
test_case::~test_case( void )
{
  m_last_result.delete_contents();
  for ( std::vector< test_case* >::iterator iter = m_subtests.begin();
	iter != m_subtests.end();
	++iter )
    {
      delete( (*iter) );
      (*iter) = NULL;
    }
  m_subtests.clear();
}

//the last result after testing
const test_result&
test_case::last_result( void ) const
{
  return m_last_result;
}


#include "test_result.h"

//adds an error to the result
void
test_result::add_error( const test_problem& new_error )
{
  m_errors.push_back( new_error );
}

//adds a failure
void
test_result::add_failure( const test_problem& new_failure )
{
  m_failures.push_back( new_failure );
}

//adds a success
void
test_result::add_success( void ) 
{
  ++m_success_count;
}

//adds another result
void
test_result::add_result( const test_result& new_result )
{
  //add errors
  m_errors.insert( m_errors.end(), 
		   new_result.m_errors.begin(),
		   new_result.m_errors.end() );
  //add failures
  m_failures.insert( m_failures.end(),
		     new_result.m_failures.begin(),
		     new_result.m_failures.end() );
  //add successes
  m_success_count += new_result.m_success_count;
}

//deletes the contents of the test result
void
test_result::delete_contents( void )
{
  m_errors.clear();
  m_failures.clear();
  m_success_count = 0;
}

const std::vector<test_problem>& 
test_result::errors( void ) const
{
  return m_errors;
}

const std::vector<test_problem>& 
test_result::failures( void ) const
{
  return m_failures;
}

int 
test_result::success_count( void ) const
{
  return m_success_count;
}

int 
test_result::total_test_count( void ) const
{
  return m_success_count 
    + m_errors.size()
    + m_failures.size();
}



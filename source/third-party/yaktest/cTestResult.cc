#include "cTestResult.hh"

//adds an error to the result
cTestResult::cTestResult( const cTestResult& test_result )
: m_errors(test_result.m_errors)
, m_failures(test_result.m_failures)
, m_success_count(test_result.m_success_count)
{}

//adds an error to the result
void
cTestResult::addError( const cTestProblem& new_error )
{
  m_errors.push_back( new_error );
}

//adds a failure
void
cTestResult::addFailure( const cTestProblem& new_failure )
{
  m_failures.push_back( new_failure );
}

//adds a success
void
cTestResult::addSuccess( void ) 
{
  ++m_success_count;
}

//adds another result
void
cTestResult::addResult( const cTestResult& new_result )
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
cTestResult::deleteContents( void )
{
  m_errors.clear();
  m_failures.clear();
  m_success_count = 0;
}

const std::vector<cTestProblem>& 
cTestResult::errors( void ) const
{
  return m_errors;
}

const std::vector<cTestProblem>& 
cTestResult::failures( void ) const
{
  return m_failures;
}

int 
cTestResult::successCount( void ) const
{
  return m_success_count;
}

int 
cTestResult::totalTestCount( void ) const
{
  return m_success_count 
    + m_errors.size()
    + m_failures.size();
}



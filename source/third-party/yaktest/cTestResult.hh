#ifndef TEST_RESULT_H
#define TEST_RESULT_H

#ifndef TEST_PROBLEM_H
#include "cTestProblem.hh"
#endif

#include<vector>


class cTestResult
{
 protected:
  //protected data members
  //the accumulated errors
  std::vector< cTestProblem > m_errors;
  //the accumulated failures
  std::vector< cTestProblem > m_failures;
  //the number of successes
  int m_success_count;

 public:
  cTestResult(){}
  //copy constructor for accessibility from boost.python
  cTestResult(const cTestResult &test_result);
  //add errors and failures to the list
  //add an error (problem in testing or error in test framework)
  void addError( const cTestProblem& new_error );
  //add a failure (test for truth or equality failed)
  void addFailure( const cTestProblem& new_error );
  //add a success 
  void addSuccess( void );
  //add another result
  void addResult( const cTestResult& result );
  //delete the contents of the result
  void deleteContents( void );
  
  //accessorts for the errors and failures in the test result
  const std::vector<cTestProblem>& errors( void ) const;
  const std::vector<cTestProblem>& failures( void ) const;
  int successCount( void ) const;
  int totalTestCount( void ) const;
};


#endif

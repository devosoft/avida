#ifndef TEST_RESULT_H
#define TEST_RESULT_H

#ifndef TEST_PROBLEM_H
#include "test_problem.h"
#endif

#include<vector>
//$Id
//a class to accumulate the results of tests.  While you can add other
//test_results to it, it is not a composite; the added results
//are simply merged with the existing result

class test_result
{
 protected:
  //protected data members
  //the accumulated errors
  std::vector< test_problem > m_errors;
  //the accumulated failures
  std::vector< test_problem > m_failures;
  //the number of successes
  int m_success_count;

 public:
  //add errors and failures to the list
  //add an error (problem in testing or error in test framework)
  void add_error( const test_problem& new_error );
  //add a failure (test for truth or equality failed)
  void add_failure( const test_problem& new_error );
  //add a success 
  void add_success( void );
  //add another result
  void add_result( const test_result& result );
  //delete the contents of the result
  void delete_contents( void );
  
  //accessorts for the errors and failures in the test result
  const std::vector<test_problem>& errors( void ) const;
  const std::vector<test_problem>& failures( void ) const;
  int success_count( void ) const;
  int total_test_count( void ) const;
};


#endif

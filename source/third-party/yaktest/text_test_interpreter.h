#ifndef TEXT_TEST_INTERPRETER_H
#define TEXT_TEST_INTERPRETER_H

#ifndef TEST_RESULT_H
#include "test_result.h"
#endif
#include <string>

//a simple interpreter which outputs a result of testing
class text_test_interpreter
{
 protected:
  //partial interpretations
  std::string header( const test_result& result ) const;
  std::string footer( const test_result& result ) const;
  std::string successes( const test_result& result ) const;
  std::string failures( const test_result& result ) const;
  std::string errors( const test_result& result ) const;
  std::string problem_vector_string( const std::vector< test_problem >& vect ) const;
  std::string problem_string( const test_problem& problem ) const;
 public:
  //returns the interpretation of a test result
  std::string interpretation( const test_result& result_to_interpret ) const;
};

#endif

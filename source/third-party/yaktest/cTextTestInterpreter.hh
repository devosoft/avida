#ifndef TEXT_TEST_INTERPRETER_H
#define TEXT_TEST_INTERPRETER_H

#ifndef TEST_RESULT_H
#include "cTestResult.hh"
#endif
#include <string>

//a simple interpreter which outputs a result of testing
class cTextTestInterpreter
{
 protected:
  //partial interpretations
  std::string header( const cTestResult& result ) const;
  std::string footer( const cTestResult& result ) const;
  std::string successes( const cTestResult& result ) const;
  std::string failures( const cTestResult& result ) const;
  std::string errors( const cTestResult& result ) const;
  std::string problemVectorString( const std::vector< cTestProblem >& vect ) const;
  std::string problemString( const cTestProblem& problem ) const;
 public:
  cTextTestInterpreter(){}
  cTextTestInterpreter(const cTextTestInterpreter &){}
  //returns the interpretation of a test result
  std::string interpretation( const cTestResult& result_to_interpret ) const;
};

#endif

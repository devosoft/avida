#ifndef TEST_PROBLEM_H
#define TEST_PROBLEM_H

#include <string>

//this is a test "problem" -- a note when a test failed or erred
class cTestProblem
{
 protected:
  //protected data members
  //line number of the error, if appropriate
  long m_line_number;
  //function_name of the error, if appropriate
  std::string m_function_name;
  //file_name of the error, if appropriate
  std::string m_file_name;
  //additional message as appropriate
  std::string m_message;

 public:
  //construction
  cTestProblem( const std::string& file_name, const std::string& function_name, long line_number, const std::string& message );
  cTestProblem( const std::string& file_name, long line_number, const std::string& message );
  cTestProblem( const cTestProblem& rhs );
  cTestProblem& operator= (const cTestProblem& rhs);
  //data access
  long lineNumber( void ) const;
  const std::string& functionName( void ) const;
  const std::string& fileName( void ) const;
  const std::string& message( void ) const;

};

#endif

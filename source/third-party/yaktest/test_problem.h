#ifndef TEST_PROBLEM_H
#define TEST_PROBLEM_H

#include <string>

//$Id
//this is a test "problem" -- a note when a test failed or erred
class test_problem
{
 protected:
  //protected data members
  //line number of the error, if appropriate
  long m_line_number;
  //functionname of the error, if appropriate
  std::string m_functionname;
  //filename of the error, if appropriate
  std::string m_filename;
  //additional message as appropriate
  std::string m_message;

 public:
  //construction
  test_problem( const std::string& filename, const std::string& functionname, long line_number, const std::string& message );
  test_problem( const std::string& filename, long line_number, const std::string& message );
  test_problem( const test_problem& rhs );
  test_problem& operator= (const test_problem& rhs);
  //data access
  long line_number( void ) const;
  const std::string& functionname( void ) const;
  const std::string& filename( void ) const;
  const std::string& message( void ) const;

};

#endif

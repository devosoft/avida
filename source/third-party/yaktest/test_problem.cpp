#include "test_problem.h"

//constructor (dumb initializer) for test_problem
test_problem::test_problem(  const std::string& filename, 
			     const std::string& functionname,
			     long line_number, 
			     const std::string& message ) :
m_line_number( line_number ),
m_functionname( functionname ),
m_filename( filename ),
m_message( message )
{
}

//constructor (dumb initializer) for test_problem, sans functionname
test_problem::test_problem(  const std::string& filename, 
			     long line_number, 
			     const std::string& message ) :
m_line_number( line_number ),
m_functionname( "" ),
m_filename( filename ),
m_message( message )
{
}

//copy constructor
test_problem::test_problem( const test_problem& rhs ) :
m_line_number( rhs.m_line_number ),
m_functionname( rhs.m_functionname ),
m_filename( rhs.m_filename ),
m_message( rhs.m_message )
{
}

//copy operator
test_problem&
test_problem::operator=( const test_problem& rhs )
{
  m_line_number = rhs.m_line_number;
  m_functionname = rhs.m_functionname;
  m_filename = rhs.m_filename;
  m_message = rhs.m_message;
  return *this;
}

//data access
long
test_problem::line_number( void ) const
{
  return m_line_number;
}


//data access
const std::string&
test_problem::functionname( void ) const
{
  return m_functionname;
}

//data access
const std::string&
test_problem::filename( void ) const
{
  return m_filename;
}

//data access 
const std::string&
test_problem::message( void ) const
{
  return m_message;
}

#include "cTestProblem.hh"

//constructor (dumb initializer) for cTestProblem
cTestProblem::cTestProblem(  const std::string& file_name, 
			     const std::string& function_name,
			     long line_number, 
			     const std::string& message ) :
m_line_number( line_number ),
m_function_name( function_name ),
m_file_name( file_name ),
m_message( message )
{
}

//constructor (dumb initializer) for cTestProblem, sans function_name
cTestProblem::cTestProblem(  const std::string& file_name, 
			     long line_number, 
			     const std::string& message ) :
m_line_number( line_number ),
m_function_name( "" ),
m_file_name( file_name ),
m_message( message )
{
}

//copy constructor
cTestProblem::cTestProblem( const cTestProblem& rhs ) :
m_line_number( rhs.m_line_number ),
m_function_name( rhs.m_function_name ),
m_file_name( rhs.m_file_name ),
m_message( rhs.m_message )
{
}

//copy operator
cTestProblem&
cTestProblem::operator=( const cTestProblem& rhs )
{
  m_line_number = rhs.m_line_number;
  m_function_name = rhs.m_function_name;
  m_file_name = rhs.m_file_name;
  m_message = rhs.m_message;
  return *this;
}

//data access
long
cTestProblem::lineNumber( void ) const
{
  return m_line_number;
}


//data access
const std::string&
cTestProblem::functionName( void ) const
{
  return m_function_name;
}

//data access
const std::string&
cTestProblem::fileName( void ) const
{
  return m_file_name;
}

//data access 
const std::string&
cTestProblem::message( void ) const
{
  return m_message;
}

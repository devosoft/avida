#include "text_test_interpreter.h"

#include <stdio.h>

std::string _yak_itoa( long input )
{
  char buffer[ 100 ];
  sprintf( buffer, "%ld", input );
  return std::string( buffer );
}

//the header for the report
std::string
text_test_interpreter::header( const test_result& result ) const
{
  std::string Result = "Test output begins; number of tests: ";
  Result.append( _yak_itoa( result.total_test_count() ) );
  Result.append( "\n\n" );
  return Result;
}

std::string
text_test_interpreter::footer( const test_result& result ) const
{
  std::string Result = "Test output ends\n";
  return Result;
}

std::string
text_test_interpreter::successes( const test_result& result ) const
{
  std::string Result = "Successes: ";
  Result.append( _yak_itoa( result.success_count() ) );
  Result.append( "\n\n" );
  return Result;
}

std::string
text_test_interpreter::failures( const test_result& result ) const
{
  std::string Result = "Failures: ";
  Result.append( _yak_itoa( result.failures().size() ) );
  Result.append( "\n" );
  Result.append( problem_vector_string( result.failures() ) );
  Result.append( "\n" );
  return Result;
}

std::string
text_test_interpreter::errors( const test_result& result ) const
{
  std::string Result = "Errors: ";
  Result.append( _yak_itoa( result.errors().size() ) );
  Result.append( "\n" );
  Result.append( problem_vector_string( result.errors() ) );
  Result.append( "\n" );
  return Result;
}

//! takes a list (vector to be precise) of problems and creates a str for each one
std::string
text_test_interpreter::problem_vector_string( const std::vector< test_problem >& vect ) const
{
  std::string Result;
  for ( std::vector< test_problem >::const_iterator iter = vect.begin();
	iter != vect.end();
	++iter )
    {
      Result.append( problem_string( *iter ) );
    }
  return Result;
}

//! converts a single test problem to a descriptive emacs compilation buffer friendly string 
std::string
text_test_interpreter::problem_string( const test_problem& problem ) const
{
  std::string Result;
  Result.append( problem.filename() );
  Result.append( ":" );
  Result.append( _yak_itoa( problem.line_number() ) );
  Result.append( ":<" );
  Result.append( problem.functionname() );
  Result.append( "> " );
  Result.append( problem.message() );
  Result.append( "\n" );
  return Result;
}

std::string 
text_test_interpreter::interpretation( const test_result& result_to_interpret ) const
{
  std::string Result;
  Result.append( header( result_to_interpret ) );
  Result.append( successes( result_to_interpret ) );
  Result.append( failures( result_to_interpret ) );
  Result.append( errors( result_to_interpret ) );
  Result.append( footer( result_to_interpret ) );
  return Result;
}

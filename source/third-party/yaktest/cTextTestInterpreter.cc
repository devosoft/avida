#include "cTextTestInterpreter.hh"

#include <stdio.h>

std::string _yak_itoa( long input )
{
  char buffer[ 100 ];
  sprintf( buffer, "%ld", input );
  return std::string( buffer );
}

//the header for the report
std::string
cTextTestInterpreter::header( const cTestResult& result ) const
{
  std::string Result = "Test output begins; number of tests: ";
  Result.append( _yak_itoa( result.totalTestCount() ) );
  Result.append( "\n\n" );
  return Result;
}

std::string
cTextTestInterpreter::footer( const cTestResult& result ) const
{
  std::string Result = "Test output ends\n";
  return Result;
}

std::string
cTextTestInterpreter::successes( const cTestResult& result ) const
{
  std::string Result = "Successes: ";
  Result.append( _yak_itoa( result.successCount() ) );
  Result.append( "\n\n" );
  return Result;
}

std::string
cTextTestInterpreter::failures( const cTestResult& result ) const
{
  std::string Result = "Failures: ";
  Result.append( _yak_itoa( result.failures().size() ) );
  Result.append( "\n" );
  Result.append( problemVectorString( result.failures() ) );
  Result.append( "\n" );
  return Result;
}

std::string
cTextTestInterpreter::errors( const cTestResult& result ) const
{
  std::string Result = "Errors: ";
  Result.append( _yak_itoa( result.errors().size() ) );
  Result.append( "\n" );
  Result.append( problemVectorString( result.errors() ) );
  Result.append( "\n" );
  return Result;
}

//! takes a list (vector to be precise) of problems and creates a str for each one
std::string
cTextTestInterpreter::problemVectorString( const std::vector< cTestProblem >& vect ) const
{
  std::string Result;
  for ( std::vector< cTestProblem >::const_iterator iter = vect.begin();
	iter != vect.end();
	++iter )
    {
      Result.append( problemString( *iter ) );
    }
  return Result;
}

//! converts a single test problem to a descriptive emacs compilation buffer friendly string 
std::string
cTextTestInterpreter::problemString( const cTestProblem& problem ) const
{
  std::string Result;
  Result.append( problem.fileName() );
  Result.append( ":" );
  Result.append( _yak_itoa( problem.lineNumber() ) );
  Result.append( ":<" );
  Result.append( problem.functionName() );
  Result.append( "> " );
  Result.append( problem.message() );
  Result.append( "\n" );
  return Result;
}

std::string 
cTextTestInterpreter::interpretation( const cTestResult& result_to_interpret ) const
{
  std::string Result;
  Result.append( header( result_to_interpret ) );
  Result.append( successes( result_to_interpret ) );
  Result.append( failures( result_to_interpret ) );
  Result.append( errors( result_to_interpret ) );
  Result.append( footer( result_to_interpret ) );
  return Result;
}

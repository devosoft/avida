#include "cTestCase.hh"

#include<math.h>
#include<stdio.h>

cTestCase::cTestCase( const cTestCase& test_case)
: m_last_result(test_case.m_last_result)
, m_subtests(test_case.m_subtests)
{}

//adopts a test case; we use "adopt" because we intend to take over
//the destruction of the passed object
void
cTestCase::pyAdoptTestCase( boost::shared_ptr<cTestCase> new_case )
{
  m_subtests.push_back( new_case );
}

//adopts a test case; we use "adopt" because we intend to take over
//the destruction of the passed object
void
cTestCase::adoptTestCase( cTestCase *new_case )
{
  boost::shared_ptr<cTestCase> p_new_case(new_case);
  m_subtests.push_back( p_new_case );
}

//the number of subtests
int
cTestCase::subtestCount( void ) const
{
  return m_subtests.size();
}

//runs all the subtests
void
cTestCase::runSubtests( void ) 
{
  for ( std::vector< boost::shared_ptr<cTestCase> >::iterator iter = m_subtests.begin();
        iter != m_subtests.end();
        ++iter )
    {
      (*iter)->runTest();
      m_last_result.addResult( (*iter)->lastResult() );
    }
}

//actually runs the test
void
cTestCase::runTest( void )
{
  m_last_result.deleteContents();
  setUp();
  runSubtests();
  try
    {
      test();
    }
  catch ( std::exception& e )
    {
      cTestProblem new_error( "unknown file", -1, e.what() );
      m_last_result.addError( new_error );
    }
  tearDown();
}

//tests to see if the supplied condition is true; called by the 
//test_non_error macro
bool
cTestCase::testNonErrorStub( bool condition,
                             const std::string& condition_name,
                             const std::string& file_name,
                             const std::string& function_name,
                             long line_number,
                             const std::string& msg )
{
  if ( !condition )
    {
      std::string message = "\"";
      message.append( condition_name );
      message.append( std::string( "\" evaluated to false." ) );
      if ( !msg.empty() ) message.append( msg );
      m_last_result.addError( cTestProblem( file_name,
                                            function_name,
                                            line_number,
                                            message ));
    }
  return condition;
}


//tests to see if the supplied condition is true; called by the 
//test_is_true macro
bool
cTestCase::testIsTrueStub( bool condition,
                           const std::string& condition_name,
                           const std::string& file_name,
                           const std::string& function_name,
                           long line_number,
                           const std::string& msg )
{
  if ( condition )
    {
      m_last_result.addSuccess();
      return true;
    }
  else 
    {
      std::string message = "\"";
      message.append( condition_name );
      message.append( std::string( "\" evaluated to false." ) );
      if ( !msg.empty() ) message.append( msg );
      m_last_result.addFailure( cTestProblem( file_name,
                                              function_name,
                                              line_number,
                                              message ));
      return false;
    }
}


//creates a test problem representing an inequality
cTestProblem
cTestCase::createEqualityProblemReport( const std::string& actual_message,
                                        const std::string& expected_message,
                                        const std::string& file_name,
                                        const std::string& function_name,
                                        long line_number,
                                        const std::string& msg )
{
  std::string message = "Equality failed.  Got ";
  message.append( actual_message );
  message.append( ", expected " );
  message.append( expected_message );
      if ( !msg.empty() ) message.append( msg );
  return cTestProblem( file_name, function_name, line_number, message );
}


//checks to see if two longs are equal
//tests to see if two longs are equal
bool
cTestCase::testIntIsEqualStub( long actual,
                               long expected,
                               const std::string& file_name,
                               const std::string& function_name,
                               long line_number,
                               const std::string& msg )
{
  if ( actual == expected ) 
    {
      m_last_result.addSuccess();
      return true;
    }
  else
    {
      char temp[100];
      sprintf( temp, "%ld", actual );
      std::string actual_message( temp );
      sprintf( temp, "%ld", expected );
      std::string expected_message( temp );
      m_last_result.addFailure( createEqualityProblemReport( actual_message,
                                                             expected_message,
                                                             file_name,
                                                             function_name,
                                                             line_number,
                                                             msg ) );
      return false;
    }
}

bool 
cTestCase::testDoubleIsEqualStub( double actual,
                                  double expected,
                                  const std::string& file_name,
                                  const std::string& function_name,
                                  long line_number,
// msvc didn't like the redeclaration of default parameter RS 2/14/2001
//                                double tolerance = 0.005 )
                                  double tolerance,
                                  const std::string& msg )
{
  if ( fabs( actual - expected ) < fabs( tolerance ) )
    {
      m_last_result.addSuccess();
      return true;
    }
  else
    {
      char temp[100];
      sprintf( temp, "%f", actual );
      std::string actual_message( temp );
      sprintf( temp, "%f", expected );
      std::string expected_message( temp );
      m_last_result.addFailure( createEqualityProblemReport( actual_message,
                                                             expected_message,
                                                             file_name,
                                                             function_name,
                                                             line_number,
                                                             msg ));
      return false;
    }
}


//tests to see if two strings are equal
bool 
cTestCase::testStringIsEqualStub( const std::string& actual,
                                  const std::string& expected,
                                  const std::string& file_name,
                                  const std::string& function_name,
                                  long line_number,
                                  const std::string& msg )
{
  if ( actual == expected ) 
    {
      m_last_result.addSuccess();
      return true;
    }
  else
    {
      std::string actual_message = std::string( "\"" );
      actual_message.append( actual );
      actual_message.append( "\"" );
      std::string expected_message = std::string( "\"" );
      expected_message.append( expected );
      expected_message.append( "\"" );
      m_last_result.addFailure( createEqualityProblemReport( actual_message,
                                                             expected_message,
                                                             file_name,
                                                             function_name,
                                                             line_number,
                                                             msg ));
      return false;
    }
}



//sets up the test (here, does nothing)
void
cTestCase::setUp( void ) 
{
}

//tears down the test (here, does nothing)
void
cTestCase::tearDown( void )
{
}

//the test itself; here, does nothing
void
cTestCase::test( void )
{
}

//construction
cTestCase::cTestCase( void ) 
{
  m_last_result.deleteContents();
  m_subtests.clear();
}

//destruction
cTestCase::~cTestCase( void )
{
  m_last_result.deleteContents();
  m_subtests.clear();
}

//the last result after testing
const cTestResult&
cTestCase::lastResult( void ) const
{
  return m_last_result;
}


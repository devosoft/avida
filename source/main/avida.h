//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef avida_h
#define avida_h

class cEnvironment;
class cPopulationInterface;
class cString;

cString AvidaVersion();

/**
 * This function properly shuts down the Avida program.
 **/
void ExitAvida(int exit_code);

/**
 * This function does all the command line parsing etc., sets everything
 * up, and returns a reference to the environment that should be used.
 *
 * Besides the command line parameters, this function needs two more
 * variables. One is the environment the Avida run should use (the
 * environment is read in from disk and initialized in SetupAvida), the
 * other is the test population interface (also initialized in SetupAvida).
 **/
void SetupAvida(
		cEnvironment &environment,
		cPopulationInterface &test_interface );

#endif

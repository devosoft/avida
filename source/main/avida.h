//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef avida_h
#define avida_h

class cString;

cString AvidaVersion();

/**
 * This function properly shuts down the Avida program.
 **/
void ExitAvida(int exit_code);

#endif

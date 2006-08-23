/*
 *  avida.h
 *  Avida
 *
 *  Called "avida.hh" prior to 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 */

#ifndef avida_h
#define avida_h

class cString;

cString getAvidaVersion();
void printVersionBanner();

/**
 * This function properly shuts down the Avida program.
 **/
void ExitAvida(int exit_code);

#endif

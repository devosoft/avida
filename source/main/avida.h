/*
 *  avida.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 */

#ifndef avida_h
#define avida_h

class cString;

cString AvidaVersion();

/**
 * This function properly shuts down the Avida program.
 **/
void ExitAvida(int exit_code);

#endif

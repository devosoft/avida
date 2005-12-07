/*
 *  cAvidaDriver_Base.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cAvidaDriver_Base.h"

#include "cString.h"

#include <iostream>

using namespace std;


// Static variable definitions
cAvidaDriver_Base * cAvidaDriver_Base::main_driver = NULL;


cAvidaDriver_Base::cAvidaDriver_Base() : done_flag(false)
{
}

cAvidaDriver_Base::~cAvidaDriver_Base()
{
}

void cAvidaDriver_Base::ExitProgram(int exit_code)
{
  exit(exit_code);   // If nothing is setup, nothing needs to be shutdown.
}

void cAvidaDriver_Base::NotifyComment(const cString & in_string)
{
  cout << in_string << endl;  // Just output
}

void cAvidaDriver_Base::NotifyWarning(const cString & in_string)
{
  cerr << in_string << endl;  // Just output
}

void cAvidaDriver_Base::NotifyError(const cString & in_string)
{
  cerr << in_string << endl;  // Just output
}

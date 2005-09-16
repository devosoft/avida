//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVIDA_DRIVER_BASE_HH
#include "cAvidaDriver_Base.h"
#endif

#ifndef STRING_HH
#include "string.hh"
#endif

#include <iostream>

using namespace std;


// Static variable definitions
cAvidaDriver_Base * cAvidaDriver_Base::main_driver = NULL;

///////////////////////
//  cAvidaDriver_Base
///////////////////////

cAvidaDriver_Base::cAvidaDriver_Base()
  : done_flag(false)
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

/*
 *  cFallbackWorldDriver.cc
 *  Avida
 *
 *  Created by David on 12/10/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#include "cFallbackWorldDriver.h"

#include "cString.h"

#include <stdlib.h>
#include <iostream>

using namespace std;


void cFallbackWorldDriver::RaiseException(const cString& in_string)
{
  cerr << "Error: " << in_string << endl;
}

void cFallbackWorldDriver::RaiseFatalException(int exit_code, const cString& in_string)
{
  cerr << "Error: " << in_string << "  Exiting..." << endl;
  exit(exit_code);
}

void cFallbackWorldDriver::NotifyComment(const cString& in_string)
{
  cout << in_string << endl;
}

void cFallbackWorldDriver::NotifyWarning(const cString& in_string)
{
  cout << "Warning: " << in_string << endl;
}

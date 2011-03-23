/*
 *  cFallbackWorldDriver.cc
 *  Avida
 *
 *  Created by David on 12/10/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cFallbackWorldDriver.h"

#include "cString.h"

#include <cstdlib>
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

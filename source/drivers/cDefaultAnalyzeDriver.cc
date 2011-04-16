/*
 *  cDefaultAnalyzeDriver.cc
 *  Avida
 *
 *  Created by David on 12/11/05.
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

#include "cDefaultAnalyzeDriver.h"

#include "cAnalyze.h"
#include "cWorld.h"

#include <cstdlib>
#include <iostream>

using namespace std;


cDefaultAnalyzeDriver::cDefaultAnalyzeDriver(cWorld* world, bool inter)
  : m_world(world), m_interactive(inter)
{
  GlobalObjectManager::Register(this);
  world->SetDriver(this);
}

cDefaultAnalyzeDriver::~cDefaultAnalyzeDriver()
{
  GlobalObjectManager::Unregister(this);
  delete m_world;
}


void cDefaultAnalyzeDriver::Run()
{
  cout << "In analyze mode!!" << endl;
  cAnalyze& analyze = m_world->GetAnalyze();
  analyze.RunFile(m_world->GetConfig().ANALYZE_FILE.Get());
  if (m_interactive == true) analyze.RunInteractive();
}

void cDefaultAnalyzeDriver::RaiseException(const cString& in_string)
{
  cerr << "Error: " << in_string << endl;
}

void cDefaultAnalyzeDriver::RaiseFatalException(int exit_code, const cString& in_string)
{
  cerr << "Error: " << in_string << "  Exiting..." << endl;
  exit(exit_code);
}

void cDefaultAnalyzeDriver::NotifyComment(const cString& in_string)
{
  cout << in_string << endl;
}

void cDefaultAnalyzeDriver::NotifyWarning(const cString& in_string)
{
  cout << "Warning: " << in_string << endl;
}

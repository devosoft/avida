/*
 *  cDefaultAnalyzeDriver.cc
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#include "cDefaultAnalyzeDriver.h"

#include "cAnalyze.h"
#include "cDriverManager.h"
#include "cWorld.h"

#include <stdlib.h>
#include <iostream>

using namespace std;


cDefaultAnalyzeDriver::cDefaultAnalyzeDriver(cWorld* world, bool inter)
  : m_world(world), m_interactive(inter)
{
  cDriverManager::Register(static_cast<cAvidaDriver*>(this));
  world->SetDriver(this);
}

cDefaultAnalyzeDriver::~cDefaultAnalyzeDriver()
{
  cDriverManager::Unregister(static_cast<cAvidaDriver*>(this));
  delete m_world;
}


void cDefaultAnalyzeDriver::Run()
{
  cout << "In analyze mode!!" << endl;
  cAnalyze& analyze = m_world->GetAnalyze();
  if (m_interactive == true) {
    analyze.RunInteractive();
  }
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

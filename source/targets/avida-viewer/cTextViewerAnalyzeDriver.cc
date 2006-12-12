/*
 *  cTextViewerAnalyzeDriver.cc
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#include "cTextViewerAnalyzeDriver.h"

#include "cAnalyze.h"
#include "cAnalyzeView.h"
#include "cChangeList.h"
#include "cClassificationManager.h"
#include "cDriverManager.h"
#include "cGenotype.h"
#include "cHardwareBase.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cString.h"
#include "cWorld.h"

#include <stdlib.h>

using namespace std;


cTextViewerAnalyzeDriver::cTextViewerAnalyzeDriver(cWorld* world, bool inter)
  : cTextViewerDriver_Base(world), m_interactive(inter)
{
  m_view = new cAnalyzeView(world);

  cDriverManager::Register(static_cast<cAvidaDriver*>(this));
  world->SetDriver(this);
}

cTextViewerAnalyzeDriver::~cTextViewerAnalyzeDriver()
{
  cDriverManager::Unregister(static_cast<cAvidaDriver*>(this));

  if (m_view != NULL) EndProg(0);
}


void cTextViewerAnalyzeDriver::Run()
{
  // cAnalyze& analyze = m_world->GetAnalyze();
  // cAvidaContext ctx(m_world->GetRandom());

  while (!m_done) {
    // Setup the viewer for the new update.
    m_view->NotifyUpdate();
  }
}

void cTextViewerAnalyzeDriver::RaiseException(const cString& in_string)
{
  m_view->NotifyError(in_string);
}

void cTextViewerAnalyzeDriver::RaiseFatalException(int exit_code, const cString& in_string)
{
  m_view->NotifyError(in_string);
  exit(exit_code);
}

void cTextViewerAnalyzeDriver::NotifyComment(const cString& in_string)
{
  m_view->NotifyComment(in_string);
}

void cTextViewerAnalyzeDriver::NotifyWarning(const cString& in_string)
{
  m_view->NotifyWarning(in_string);
}

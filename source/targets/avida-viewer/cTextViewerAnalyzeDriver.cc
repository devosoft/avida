/*
 *  cTextViewerAnalyzeDriver.cc
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cTextViewerAnalyzeDriver.h"

#include "cAnalyze.h"
#include "cAnalyzeView.h"
#include "cChangeList.h"
#include "cClassificationManager.h"
#include "cDriverManager.h"
#include "cHardwareBase.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cString.h"
#include "cWorld.h"

#include <cstdlib>

using namespace std;


cTextViewerAnalyzeDriver::cTextViewerAnalyzeDriver(cWorld* world, bool inter)
  : cTextViewerDriver_Base(world), m_interactive(inter)
{
  m_view = new cAnalyzeView(world);

  cDriverManager::Register(this);
  world->SetDriver(this);
}

cTextViewerAnalyzeDriver::~cTextViewerAnalyzeDriver()
{
  cDriverManager::Unregister(this);

  if (m_view != NULL) EndProg(0);
}


void cTextViewerAnalyzeDriver::Run()
{
  // cAnalyze& analyze = m_world->GetAnalyze();

  while (!m_done) {
    // Setup the viewer for the new update.
    m_view->NotifyUpdate(m_world->GetDefaultContext());
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

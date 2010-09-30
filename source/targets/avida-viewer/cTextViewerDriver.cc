/*
 *  cTextViewerDriver.cc
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

#include "cTextViewerDriver.h"

#include "cChangeList.h"
#include "cClassificationManager.h"
#include "cDriverManager.h"
#include "cHardwareBase.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cString.h"
#include "cView.h"
#include "cWorld.h"

#include <cstdlib>

using namespace std;


cTextViewerDriver::cTextViewerDriver(cWorld* world)
  : cTextViewerDriver_Base(world), m_pause(false), m_firstupdate(true)
{
  m_view = new cView(world);
  m_view->SetViewMode(-1);    // Set the view mode to its default value.

  cDriverManager::Register(this);
  world->SetDriver(this);
}

cTextViewerDriver::~cTextViewerDriver()
{
  cDriverManager::Unregister(this);
  
  if (m_view != NULL) EndProg(0);
}


void cTextViewerDriver::Run()
{
  cPopulation& population = m_world->GetPopulation();
  cStats& stats = m_world->GetStats();
  
  const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
  const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get();
  
  cAvidaContext ctx(m_world, m_world->GetRandom());
  ctx.EnableOrgFaultReporting();
  
  while (!m_done) {
    if (cChangeList* change_list = population.GetChangeList()) {
      change_list->Reset();
    }
    
    m_world->GetEvents(ctx);
    if (m_done == true) break;
    
    // Increment the Update.
    stats.IncCurrentUpdate();
    
    // Handle all data collection for previous update.
    if (stats.GetUpdate() > 0) {
      // Tell the stats object to do update calculations and printing.
      stats.ProcessUpdate();
    }
    
    
    // Process the update.
    const int UD_size = ave_time_slice * population.GetNumOrganisms();
    const double step_size = 1.0 / (double) UD_size;
    
    if (m_pause) {
      m_view->Pause();
      m_pause = false;
      
      // This is needed to have the top bar drawn properly; I'm not sure why...
      if (m_firstupdate) {
        m_view->Refresh(ctx);
        m_firstupdate = false;
      }
    }
    
    // Are we stepping through an organism?
    if (m_view->GetStepOrganism() != -1) {  // Yes we are!
                                            // Keep the viewer informed about the organism we are stepping through...
      for (int i = 0; i < UD_size; i++) {
        const int next_id = population.ScheduleOrganism();
        if (next_id == m_view->GetStepOrganism()) {
          m_view->NotifyUpdate(ctx);
          m_view->NewUpdate(ctx);
          
          // This is needed to have the top bar drawn properly; I'm not sure why...
          if (m_firstupdate) {
            m_view->Refresh(ctx);
            m_firstupdate = false;
          }
        }
        population.ProcessStep(ctx, step_size, next_id);
      }
    }
    else {
      for (int i = 0; i < UD_size; i++) population.ProcessStep(ctx, step_size, population.ScheduleOrganism());
    }
    
    
    // end of update stats...
    population.ProcessPostUpdate(ctx);
    
    // Setup the viewer for the new update.
    if (m_view->GetStepOrganism() == -1) {
      m_view->NewUpdate(ctx);
 
      // This is needed to have the top bar drawn properly; I'm not sure why...
      if (m_firstupdate) {
        m_view->Refresh(ctx);
        m_firstupdate = false;
      }
    }
    
    
    // Do Point Mutations
    if (point_mut_prob > 0 ) {
      for (int i = 0; i < population.GetSize(); i++) {
        if (population.GetCell(i).IsOccupied()) {
          population.GetCell(i).GetOrganism()->GetHardware().PointMutate(ctx, point_mut_prob);
        }
      }
    }
    
    // Exit conditons...
    if (population.GetNumOrganisms() == 0) m_done = true;
  }
}

void cTextViewerDriver::RaiseException(const cString& in_string)
{
  m_view->NotifyError(in_string);
}

void cTextViewerDriver::RaiseFatalException(int exit_code, const cString& in_string)
{
  m_view->NotifyError(in_string);
  exit(exit_code);
}

void cTextViewerDriver::NotifyComment(const cString& in_string)
{
  m_view->NotifyComment(in_string);
}

void cTextViewerDriver::NotifyWarning(const cString& in_string)
{
  m_view->NotifyWarning(in_string);
}

void cTextViewerDriver::SignalBreakpoint()
{
  m_view->DoBreakpoint();
}

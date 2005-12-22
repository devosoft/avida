/*
 *  cTextViewerDriver.cc
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#include "cTextViewerDriver.h"

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
#include "cView.h"
#include "cWorld.h"

#include <stdlib.h>

using namespace std;


cTextViewerDriver::cTextViewerDriver(cWorld* world) : m_world(world), m_done(false), m_view(NULL)
{
  m_view = new cView(world);
  m_view->SetViewMode(world->GetConfig().VIEW_MODE.Get());

  cDriverManager::Register(static_cast<cAvidaDriver*>(this));
  world->SetDriver(this);
}

cTextViewerDriver::~cTextViewerDriver()
{
  cDriverManager::Unregister(static_cast<cAvidaDriver*>(this));
  delete m_world;
  
  if (m_view != NULL) EndProg(0);
}


void cTextViewerDriver::Run()
{
  cClassificationManager& classmgr = m_world->GetClassificationManager();
  cPopulation& population = m_world->GetPopulation();
  cStats& stats = m_world->GetStats();
  
  const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
  const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get();
  
  while (!m_done) {
    if (cChangeList* change_list = population.GetChangeList()) {
      change_list->Reset();
    }
    
    m_world->GetEvents();
    if (m_done == true) break;
    
    // Increment the Update.
    stats.IncCurrentUpdate();
    
    // Handle all data collection for previous update.
    if (stats.GetUpdate() > 0) {
      // Tell the stats object to do update calculations and printing.
      stats.ProcessUpdate();
      
      // Update all the genotypes for the end of this update.
      for (cGenotype * cur_genotype = classmgr.ResetThread(0);
           cur_genotype != NULL && cur_genotype->GetThreshold();
           cur_genotype = classmgr.NextGenotype(0)) {
        cur_genotype->UpdateReset();
      }
    }
    
    
    // Process the update.
    const int UD_size = ave_time_slice * population.GetNumOrganisms();
    const double step_size = 1.0 / (double) UD_size;
    

    // Are we stepping through an organism?
    if (m_view->GetStepOrganism() != -1) {  // Yes we are!
                                            // Keep the viewer informed about the organism we are stepping through...
      for (int i = 0; i < UD_size; i++) {
        const int next_id = population.ScheduleOrganism();
        if (next_id == m_view->GetStepOrganism()) {
          m_view->NotifyUpdate();
          m_view->NewUpdate();
          
          // This is needed to have the top bar drawn properly; I'm not sure why...
          static bool first_update = true;
          if (first_update) {
            m_view->Refresh();
            first_update = false;
          }
        }
        population.ProcessStep(step_size, next_id);
      }
    }
    else {
      for (int i = 0; i < UD_size; i++) population.ProcessStep(step_size);
    }
    
    
    // end of update stats...
    population.CalcUpdateStats();
    
    
    // Setup the viewer for the new update.
    if (m_view->GetStepOrganism() == -1) {
      m_view->NotifyUpdate();
      m_view->NewUpdate();
      
      // This is needed to have the top bar drawn properly; I'm not sure why...
      static bool first_update = true;
      if (first_update) {
        m_view->Refresh();
        first_update = false;
      }
    }
    
    
    // Do Point Mutations
    if (point_mut_prob > 0 ) {
      for (int i = 0; i < population.GetSize(); i++) {
        if (population.GetCell(i).IsOccupied()) {
          population.GetCell(i).GetOrganism()->GetHardware().PointMutate(point_mut_prob);
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

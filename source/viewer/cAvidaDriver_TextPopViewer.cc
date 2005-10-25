//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cAvidaDriver_TextPopViewer.h"

#include "cPopulation.h"
#include "cView.h"
#include "cWorld.h"

#include <iostream>

using namespace std;

cAvidaDriver_TextPopViewer::
cAvidaDriver_TextPopViewer(cWorld* world)
  : cAvidaDriver_Population(world)
  , viewer(NULL)
{
  cout << "Initializing Text Viewer... " << flush;

  viewer = new cView(world);
  viewer->SetViewMode(world->GetConfig().VIEW_MODE.Get());

  cout << " ...done" << endl;
}

cAvidaDriver_TextPopViewer::~cAvidaDriver_TextPopViewer()
{
  if (viewer != NULL) EndProg(0);
}

void cAvidaDriver_TextPopViewer::ProcessOrganisms()
{
  // Process the update.
  const int UD_size = m_world->GetConfig().AVE_TIME_SLICE.Get() * m_world->GetPopulation().GetNumOrganisms();
  const double step_size = 1.0 / (double) UD_size;

  // Are we stepping through an organism?
  if (viewer->GetStepOrganism() != -1) {  // Yes we are!
    // Keep the viewer informed about the organism we are stepping through...
    for (int i = 0; i < UD_size; i++) {
      const int next_id = m_world->GetPopulation().ScheduleOrganism();
      if (next_id == viewer->GetStepOrganism()) viewer->NotifyUpdate();
      m_world->GetPopulation().ProcessStep(step_size, next_id);
    }
  }
  else {
    for (int i = 0; i < UD_size; i++) m_world->GetPopulation().ProcessStep(step_size);
  }

  // end of update stats...
  m_world->GetPopulation().CalcUpdateStats();

  // Setup the viewer for the new update.
  if (viewer->GetStepOrganism() == -1) NotifyUpdate();
}

void cAvidaDriver_TextPopViewer::NotifyComment(const cString & in_string)
{
  // Send a commment to the user.
  viewer->NotifyComment(in_string);
}

void cAvidaDriver_TextPopViewer::NotifyWarning(const cString & in_string)
{
  // Send a warning to the user.
  viewer->NotifyWarning(in_string);
}

void cAvidaDriver_TextPopViewer::NotifyError(const cString & in_string)
{
  // Send an error to the user.
  viewer->NotifyError(in_string);
}

void cAvidaDriver_TextPopViewer::NotifyUpdate()
{
  viewer->NotifyUpdate();
  viewer->NewUpdate();

  // This is needed to have the top bar drawn properly; I'm not sure why...
  static bool first_update = true;
  if (first_update) {
    viewer->Refresh();
    first_update = false;
  }
}

void cAvidaDriver_TextPopViewer::SignalBreakpoint()
{
  viewer->DoBreakpoint();
}

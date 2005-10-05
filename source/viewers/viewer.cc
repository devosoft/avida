//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef VIEWER_HH
#include "viewer.hh"
#endif

#ifndef CONFIG_HH
#include "cConfig.h"
#endif
#ifndef ENVIRONMENT_HH
#include "cEnvironment.h"
#endif
#ifndef POPULATION_HH
#include "cPopulation.h"
#endif
#ifndef VIEW_HH
#include "../viewers/view.hh"          // class cView
#endif


using namespace std;


int main(int argc, char * argv[])
{
  // Catch Interrupt making sure to close appropriately
  signal(SIGINT, ExitAvida);

  // output copyright message
  cout << "Avida " << AVIDA_VERSION << endl;
  cout << "----------------------------------------------------------------------" << endl;
  cout << "Copyright (C) 1993-2003 California Institute of Technology." << endl;
  cout << "Copyright (C) 1999-2005 Michigan State University." << endl << endl;
  
  cout << "Avida comes with ABSOLUTELY NO WARRANTY." << endl;
  cout << "This is free software, and you are welcome to redistribute it" << endl;
  cout << "under certain conditions. See file COPYING for details." << endl << endl;
  
  // Initialize the configuration data...
  cConfig::InitGroupList();
  cConfig::Setup(argc, argv);
  cConfig::SetupMS();

  cEnvironment environment;
  cPopulationInterface test_interface;

  SetupAvida(environment, test_interface);

  if (cConfig::GetAnalyzeMode() == true) {
    cAvidaDriver_Base::main_driver = new cAvidaDriver_Analyze();
  }
  else {
    cAvidaDriver_Base::main_driver = new cAvidaDriver_TextPopViewer(environment);
  }

  cAvidaDriver_Base::main_driver->Run();

  // Exit Nicely
  ExitAvida(0);
}


////////////////////////////////
//  cAvidaDriver_TextPopViewer
////////////////////////////////

cAvidaDriver_TextPopViewer::
cAvidaDriver_TextPopViewer(cEnvironment & environment)
  : cAvidaDriver_Population(environment)
  , viewer(NULL)
{
  cout << "Initializing Text Viewer... " << flush;

  viewer = new cView(*population);
  viewer->SetViewMode(cConfig::GetViewMode());

  cout << " ...done" << endl;
}

cAvidaDriver_TextPopViewer::~cAvidaDriver_TextPopViewer()
{
  if (viewer != NULL) EndProg(0);
}

void cAvidaDriver_TextPopViewer::ProcessOrganisms()
{
  //  cout << "DEBUG: Running Update w/ Viewer" << endl;

  // Process the update.
  const int UD_size = cConfig::GetAveTimeslice()*population->GetNumOrganisms();
  const double step_size = 1.0 / (double) UD_size;

  // Are we stepping through an organism?
  if (viewer->GetStepOrganism() != -1) {  // Yes we are!
    // Keep the viewer informed about the organism we are stepping through...
    for (int i = 0; i < UD_size; i++) {
      const int next_id = population->ScheduleOrganism();
      if (next_id == viewer->GetStepOrganism()) viewer->NotifyUpdate();
      population->ProcessStep(step_size, next_id);
    }
  }
  else {
    for (int i = 0; i < UD_size; i++) population->ProcessStep(step_size);
  }

  // end of update stats...
  population->CalcUpdateStats();

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

#include "cCoreView_Info.h"

cCoreView_Info::cCoreView_Info(cPopulation & in_pop, int in_total_colors)
  : m_population(in_pop)
  , m_total_colors(in_total_colors)
  , m_threshold_colors(in_total_colors * 5 / 6)
  , m_pause_level(PAUSE_OFF)
  , m_step_organism_id(-1)
  , m_step_organism_thread(-1)
  , fp("debug.dat")
{
  // Redirect standard output...
  std::cout.rdbuf(m_cout_stream.rdbuf());
  std::cerr.rdbuf(m_cerr_stream.rdbuf());
}

cCoreView_Info::~cCoreView_Info()
{
}

///////////////////////
// Helper Methods...

void cCoreView_Info::FlushOut()
{
  m_cout_list.Load(m_cout_stream.str().c_str(), '\n');  // Load the stored stream...
  m_cout_stream.str("");  // Clear the streams.
}

void cCoreView_Info::FlushErr()
{
  m_cerr_list.Load(m_cerr_stream.str().c_str(), '\n');  // Load the stored stream...
  m_cerr_stream.str("");  // Clear the streams.
}


/////////////////////////
//  Other functions...

void cCoreView_Info::EnterStepMode(int org_id)
{
  SetPauseLevel(PAUSE_ADVANCE_INST);
  SetStepOrganism(org_id);
}

void cCoreView_Info::ExitStepMode()
{
  SetPauseLevel(PAUSE_ON);
  SetStepOrganism(-1);
}

bool cCoreView_Info::TogglePause()
{
  // If pause is off, turn it on.
  if (m_pause_level == PAUSE_OFF) {
    SetPauseLevel(PAUSE_ON);
    return true;
  }

  // Otherwise pause is on; carefully turn it off.
  if (m_pause_level == PAUSE_ADVANCE_INST) ExitStepMode();

  // Clean up any faults we may have been tracking in step mode.
//   if (info.GetActiveCell()->IsOccupied()) {
//     info.GetActiveCell()->GetOrganism()->GetPhenotype().SetFault("");
//   }
  SetPauseLevel(PAUSE_OFF);

  return false;
}

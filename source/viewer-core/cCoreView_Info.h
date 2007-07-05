#ifndef cCoreView_Info_h
#define cCoreView_Info_h

// The cCoreView_Info object is responsible for holding on to all of the general information about the state of
// a population in the view.  It does not process any of this information, but allows it to be easily shared by
// all of the sections of the viewer.

#include <iostream>
#include <sstream>

#include "cStringList.h"
#include "tList.h"

class cPopulation;
class cCoreView_Base;

class cCoreView_Info {
protected:
  cPopulation & m_population;

  // Setup streams to capture stdin and stdout so we can control them as needed.
  std::stringstream out_stream;
  std::stringstream err_stream;
  
  // And string lists to convert outputs to for easier management.
  cStringList cout_list;
  cStringList cerr_list;

  // Constant Inforation setup by specific viewer.
  const int m_total_colors;
  const int m_threshold_colors;

  // Variable information, changing modes based on user input.
  int m_pause_level;
  int m_step_organism;

  // Helper Methods
  void FlushOut();  // Move stored cout to string list 'out_list'
  void FlushErr();  // Move stored cerr to string list 'err_list'
  
public:
  // Constant Information across all viewers.
  enum ePause { PAUSE_ON, PAUSE_OFF, PAUSE_ADVANCE_INST, PAUSE_ADVANCE_UPDATE, PAUSE_ADVANCE_DIVIDE };

public:
  cCoreView_Info(cPopulation & in_pop, int in_total_colors);
  ~cCoreView_Info();

  cPopulation & GetPopulation() { return m_population; }
  const cPopulation & GetPopulation() const { return m_population; }

  // Accessors for variable information
  int GetPauseLevel() const { return m_pause_level; }
  int GetStepOrganism() const { return m_step_organism; }

  void SetPauseLevel(int in_level) { m_pause_level = in_level; }
  void SetStepOrganism(int in_id) { m_step_organism = in_id; }

  // Special accessors...
  cStringList & GetOutList() { FlushOut(); return cout_list; }
  cStringList & GetErrList() { FlushErr(); return cerr_list; }
};

#endif

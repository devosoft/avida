#include "cCoreView_Info.h"

cCoreView_Info::cCoreView_Info(cPopulation & in_pop, int in_total_colors)
  : m_population(in_pop)
  , m_total_colors(in_total_colors)
  , m_threshold_colors(in_total_colors * 5 / 6)
  , m_pause_level(PAUSE_OFF)
  , m_step_organism(-1)
{
  // Redirect standard output...
  std::cout.rdbuf(out_stream.rdbuf());
  std::cerr.rdbuf(err_stream.rdbuf());
}

cCoreView_Info::~cCoreView_Info()
{
}
